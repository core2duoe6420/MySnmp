#include <MySnmp/debug.h>

#include <MySnmp/SnmpRequest.h>
#include <MySnmp/SnmpResult.h>
#include <MySnmp/RequestManager.h>
#include <MySnmp/OidTree.h>
#include <MySnmp/OidNode.h>

using namespace mysnmp;
using namespace Snmp_pp;

bool SnmpGetRequest::AddOid(const char * oidstr) {
	Oid oid(oidstr);
	return AddOid(oid);
}

bool SnmpGetRequest::AddOid(const Snmp_pp::Oid& oid) {
	const OidNode * node = host.GetOidTree().GetOidNode(oid);
	if (node == NULL) {
		this->errMsg = "no such oid";
		return false;
	}
	AddOid(node);
	return true;
}

bool SnmpGetRequest::AddOid(const OidNode * oid) {
	switch (oid->GetTypeEnum()) {
	case OidTypeEnum::node:
	case OidTypeEnum::scalar:
		this->getNextVector.push_back(oid->ToOid());
		break;
	case OidTypeEnum::table:
	case OidTypeEnum::entry:
	case OidTypeEnum::column:
		this->getBulkVector.push_back(oid->ToOid());
		break;
	case OidTypeEnum::value:
		this->getVector.push_back(oid->ToOid());
		break;
	}
	this->oidTotalCount++;
	return true;
}

void * SnmpGetRequest::Run(void * data) {
	RequestHolder * holder = (RequestHolder *)data;
	SnmpGetRequest * request = dynamic_cast<SnmpGetRequest *>(holder->GetSnmpRequest());
	RequestManager * manager = request->manager;
	SnmpResult * result = holder->GetSnmpResult();

	if (request->oidTotalCount == 0)
		result->SetErrMsg("Oid list is empty");

	if (result->GetErrMsg() == NULL) {
		request->host.Lock();
		CTarget target(request->host.GetAddress());
		target.set_retry(request->host.GetConfig().GetRetryTimes());
		target.set_timeout(request->host.GetConfig().GetTimeout());
		target.set_readcommunity(request->host.GetConfig().GetReadCommunity());
		target.set_writecommunity(request->host.GetConfig().GetWriteCommunity());
		target.set_version(request->host.GetConfig().GetSnmpVersion());
		request->host.UnLock();

		int status;
		Snmp snmp(status);

		if (status != SNMP_CLASS_SUCCESS)
			result->SetErrMsg((char *)snmp.error_msg(status));

		if (result->GetErrMsg() == NULL) {
			Pdu pdu;
			request->handleVector(request->getVector, snmp, target, pdu, result);
			request->handleVector(request->getNextVector, snmp, target, pdu, result);
			request->handleVector(request->getBulkVector, snmp, target, pdu, result);
		}
	}

	manager->AddResultToQueue(holder);
	return NULL;
}

void SnmpGetRequest::handleVector(std::vector<Oid>& vector,
								  Snmp& snmp, SnmpTarget& target, Pdu& pdu, SnmpResult * result) {
	pdu.clear();
	int len = vector.size();
	if (len == 0)
		return;

	Vb * vblist = new Vb[len];
	for (int i = 0; i < len; i++)
		vblist[i].set_oid(vector[i]);
	pdu.set_vblist(vblist, len);

	int snmpErrStatus = 0, pduErrStatus = 0, pduErrIndex = -1;

	if (vector == this->getVector)
		snmpErrStatus = snmp.get(pdu, target);
	else if (vector == this->getNextVector)
		snmpErrStatus = snmp.get_next(pdu, target);
	else if (vector == this->getBulkVector) {
		int nonRepeater = this->config.GetBulkNonRepeater();
		int maxRepeater = this->config.GetBulkMaxRepeater();
		snmpErrStatus = snmp.get_bulk(pdu, target, nonRepeater, maxRepeater);
	}
	delete[] vblist;

	if (snmpErrStatus == SNMP_CLASS_ERR_STATUS_SET) {
		pduErrStatus = pdu.get_error_status();
		pduErrIndex = pdu.get_error_index();
	}

	int vbCount = pdu.get_vb_count();
	Vb * resultVb = new Vb[vbCount];
	pdu.get_vblist(resultVb, vbCount);
	for (int i = 0; i < vbCount; i++) {
		result->AddVb(resultVb[i]);
		result->AddSnmpErrStatus(snmpErrStatus);
		if (pduErrStatus != 0 && i == pduErrIndex)
			result->AddPduErrStatus(pduErrStatus);
		else
			result->AddPduErrStatus(0);
	}
	delete[] resultVb;

}