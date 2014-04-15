#include <MySnmp/Command/SnmpRequestCommand.h>
#include <MySnmp/HostManager.h>
#include <MySnmp/RequestManager.h>

#include <MySnmp/debug.h>

using namespace mysnmp;

SnmpRequestCommand::SnmpRequestCommand(SnmpType type, int hostId) : type(type) {
	host = HostManager::GetHost(hostId);
	if (!host)
		return;
	switch (this->type) {
	case SnmpType::SNMP_GET:
	case SnmpType::SNMP_GETNEXT:
	case SnmpType::SNMP_GETBULK:
	case SnmpType::SNMP_WALK:
		request = RequestManager::GetManager().CreateSnmpGetRequest(*host, this->type);
		break;
	case SnmpType::SNMP_SET:
		request = RequestManager::GetManager().CreateSnmpSetRequest(*host);
		break;
	default:
		break;
	}
	this->requestId = request->GetRequestId();
}

void SnmpRequestCommand::SetBulkNonRepeater(int value) {
	if (!host || type != SnmpType::SNMP_GETBULK)
		return;
	SnmpGetBulkRequest * bulkRequest = dynamic_cast<SnmpGetBulkRequest *>(request);
	bulkRequest->SetNonReapter(value);
}

void SnmpRequestCommand::SetBulkMaxRepeater(int value) {
	if (!host || type != SnmpType::SNMP_GETBULK)
		return;
	SnmpGetBulkRequest * bulkRequest = dynamic_cast<SnmpGetBulkRequest *>(request);
	bulkRequest->SetMaxReapter(value);
}

void SnmpRequestCommand::AddOid(const char * oidstr) {
	if (!host || (type != SnmpType::SNMP_GET &&
		type != SnmpType::SNMP_GETNEXT &&
		type != SnmpType::SNMP_GETBULK &&
		type != SnmpType::SNMP_WALK))
		return;

	SnmpGetRequestBase * getRequest = dynamic_cast<SnmpGetRequestBase *>(request);
	getRequest->AddOid(oidstr);
}

void SnmpRequestCommand::AddVb(const char * oidstr, const wxString& value) {
	if (!host || type != SnmpType::SNMP_SET)
		return;

	SnmpSetRequest * setRequest = dynamic_cast<SnmpSetRequest *>(request);
	setRequest->AddVb(oidstr, value.mb_str());
}

int SnmpRequestCommand::Execute() {
	if (!host)
		return -1;
	RequestManager::GetManager().AddRequestToQueue(request);
	return requestId;
}