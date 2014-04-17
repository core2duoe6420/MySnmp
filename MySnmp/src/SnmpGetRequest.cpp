#include <MySnmp/debug.h>

#include <MySnmp/SnmpRequest.h>
#include <MySnmp/SnmpResult.h>
#include <MySnmp/RequestManager.h>
#include <MySnmp/OidTree.h>
#include <MySnmp/OidNode.h>

using namespace mysnmp;
using namespace Snmp_pp;

bool SnmpGetRequestBase::AddOid(const char * oidstr) {
	Oid oid(oidstr);
	return AddOid(oid);
}

bool SnmpGetRequestBase::AddOid(const Snmp_pp::Oid& oid) {
	const OidNode * node = host.GetOidTree().GetOidNode(oid);
	if (node == NULL) {
		this->errMsg = "no such oid";
		return false;
	}
	AddOid(node);
	return true;
}

bool SnmpGetRequestBase::AddOid(const OidNode * oid) {
	this->oids.push_back(oid->ToOid());
	return true;
}

static void handleVector(std::vector<Oid>& vector, Snmp& snmp,
						 SnmpTarget& target, Pdu& pdu,
						 SnmpRequest * request, SnmpResult * result) {
	pdu.clear();
	int len = vector.size();
	if (len == 0)
		return;

	Vb * vblist = new Vb[len];
	for (int i = 0; i < len; i++)
		vblist[i].set_oid(vector[i]);
	pdu.set_vblist(vblist, len);

	int snmpErrStatus = 0, pduErrStatus = 0, pduErrIndex = -1;

	switch (request->GetType()) {
	case SNMP_GET:
		snmpErrStatus = snmp.get(pdu, target);
		break;
	case SNMP_GETNEXT:
		snmpErrStatus = snmp.get_next(pdu, target);
		break;
	case SNMP_GETBULK:
		SnmpGetBulkRequest * bulkRequest = dynamic_cast<SnmpGetBulkRequest *>(request);
		int nonRepeater = bulkRequest->GetNonReapter();
		int maxRepeater = bulkRequest->GetMaxReapter();
		snmpErrStatus = snmp.get_bulk(pdu, target, nonRepeater, maxRepeater);
		break;
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

static void * GetRequestRunBase(void * data) {
	RequestHolder * holder = (RequestHolder *)data;
	SnmpGetRequestBase * request = dynamic_cast<SnmpGetRequestBase *>(holder->GetSnmpRequest());
	RequestManager * manager = request->GetManager();
	SnmpResult * result = holder->GetSnmpResult();

	if (request->GetOids().size() == 0)
		result->SetErrMsg("Oid list is empty");

	if (result->GetErrMsg() == NULL) {
		Host& host = request->GetHost();
		host.Lock();
		CTarget target(host.GetAddress());
		target.set_retry(host.GetConfig().GetRetryTimes());
		target.set_timeout(host.GetConfig().GetTimeout());
		target.set_readcommunity(host.GetConfig().GetReadCommunity());
		target.set_writecommunity(host.GetConfig().GetWriteCommunity());
		target.set_version(host.GetConfig().GetSnmpVersion());
		host.UnLock();

		int status;
		Snmp snmp(status);

		if (status != SNMP_CLASS_SUCCESS)
			result->SetErrMsg((char *)snmp.error_msg(status));

		if (result->GetErrMsg() == NULL) {
			Pdu pdu;
			handleVector(request->GetOids(), snmp, target, pdu, request, result);
		}
	}
	holder->SetComplete();
	manager->AddResultToQueue(holder);
	return NULL;
}

void * SnmpGetRequest::Run(void * data) {
	return GetRequestRunBase(data);
}

void * SnmpGetNextRequest::Run(void * data) {
	return GetRequestRunBase(data);
}

void * SnmpGetBulkRequest::Run(void * data) {
	return GetRequestRunBase(data);
}

/* 这段有大量重复代码 */
void * SnmpWalkRequest::Run(void * data) {
	RequestHolder * holder = (RequestHolder *)data;
	SnmpWalkRequest * request = dynamic_cast<SnmpWalkRequest *>(holder->GetSnmpRequest());
	RequestManager * manager = request->GetManager();
	SnmpResult * result = holder->GetSnmpResult();

	if (request->GetOids().size() == 0)
		result->SetErrMsg("Oid list is empty");

	if (result->GetErrMsg() == NULL) {
		Host& host = request->GetHost();
		host.Lock();
		CTarget target(host.GetAddress());
		target.set_retry(host.GetConfig().GetRetryTimes());
		target.set_timeout(host.GetConfig().GetTimeout());
		target.set_readcommunity(host.GetConfig().GetReadCommunity());
		target.set_writecommunity(host.GetConfig().GetWriteCommunity());
		target.set_version(host.GetConfig().GetSnmpVersion());
		host.UnLock();

		int status;
		Snmp snmp(status);

		if (status != SNMP_CLASS_SUCCESS)
			result->SetErrMsg((char *)snmp.error_msg(status));

		if (result->GetErrMsg() == NULL) {
			Oid oid = request->GetOids()[0];
			Pdu pdu;
			Vb vb(oid);
			pdu.set_vb(vb, 0);
			int snmpErrStatus = 0;
			while ((snmpErrStatus = snmp.get_bulk(pdu, target, 0, 20)) == SNMP_CLASS_SUCCESS) {
				bool end = false;
				result->ClearVb();
				for (int i = 0; i < pdu.get_vb_count(); i++) {
					pdu.get_vb(vb, i);
					Oid tmp;
					vb.get_oid(tmp);
					if (oid.nCompare(oid.len(), tmp) != 0) {
						end = true;
						break;
					}
					if (vb.get_syntax() != sNMP_SYNTAX_ENDOFMIBVIEW) {
						result->Lock();
						result->AddVb(vb);
						result->AddSnmpErrStatus(snmpErrStatus);
						result->AddPduErrStatus(0);
						result->UnLock();
					}
				}
				if (end)
					break;
				pdu.set_vblist(&vb, 1);
				manager->AddResultToQueue(holder);
			}
		}
	}
	holder->SetComplete();
	manager->AddResultToQueue(holder);
	return NULL;
}