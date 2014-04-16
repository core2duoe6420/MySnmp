#include <MySnmp/SnmpRequest.h>
#include <MySnmp/RequestManager.h>
#include <MySnmp/SnmpResult.h>
#include <MySnmp/OidNode.h>
#include <MySnmp/OidTree.h>

#include <MySnmp/debug.h>

using namespace mysnmp;
using namespace Snmp_pp;

bool SnmpSetRequest::AddVb(const char * oidstr, const char * value) {
	const OidNode * oidnode = host.GetOidTree().GetOidNode(oidstr);
	if (oidnode == NULL) {
		this->errMsg = "no such oid";
		return false;
	}
	return AddVb(oidnode, oidstr, value);
}

bool SnmpSetRequest::AddVb(const Snmp_pp::Oid& oid, const char * value) {
	const OidNode * oidnode = host.GetOidTree().GetOidNode(oid);
	if (oidnode == NULL) {
		this->errMsg = "no such oid";
		return false;
	}
	return AddVb(oidnode, oid.get_printable(), value);
}

bool SnmpSetRequest::AddVb(const OidNode * oidnode, const char * oidstr, const char * value) {
	Vb vb;
	Oid oid(oidstr);
	vb.set_oid(oid);
	switch (oidnode->GetSyntax()) {
	case SYNTAX_COUNTER:
	{
						   Counter32 counter(atol(value));
						   vb.set_value(counter);
						   break;
	}
	case SYNTAX_INTEGER:
	{
						   int integer = atoi(value);
						   vb.set_value(integer);
						   break;
	}
	case SYNTAX_OID:
	{
					   Oid oidvalue(value);
					   vb.set_value(oidvalue);
					   break;
	}
	case SYNTAX_STRING:
	{
						  OctetStr valuestr(value);
						  vb.set_value(valuestr);
						  break;
	}
	case SYNTAX_TIMETICKS:
	{
							 TimeTicks timeticks(atol(value));
							 vb.set_value(timeticks);
							 break;
	}
	case SYNTAX_GAUGE:
	{
						 Gauge32 gauge(atol(value));
						 vb.set_value(gauge);
						 break;
	}
	default:
		return false;
	}
	this->vbs.push_back(vb);
	return true;
}

void * SnmpSetRequest::Run(void * data) {
	RequestHolder * holder = (RequestHolder *)data;
	SnmpSetRequest * request = dynamic_cast<SnmpSetRequest *>(holder->GetSnmpRequest());
	RequestManager * manager = request->manager;
	SnmpResult * result = holder->GetSnmpResult();

	if (request->vbs.size() == 0)
		result->SetErrMsg("Vb list is empty");

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
			for (int i = 0; i < request->vbs.size(); i++)
				pdu += request->vbs[i];

			int snmpErrStatus = snmp.set(pdu, target);

			int pduErrStatus = 0, pduErrIndex = -1;
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
	}
	holder->SetComplete();
	manager->AddResultToQueue(holder);
	return NULL;
}