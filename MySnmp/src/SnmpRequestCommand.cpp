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
	case SnmpType::get:
		request = RequestManager::GetManager().CreateSnmpGetRequest(*host);
		break;
	case SnmpType::set:
		request = RequestManager::GetManager().CreateSnmpSetRequest(*host);
		break;
	default:
		break;
	}
}

void SnmpRequestCommand::SetBulkNonRepeater(int value) {
	if (!host || type != SnmpType::get)
		return;
	SnmpGetRequest * getRequest = dynamic_cast<SnmpGetRequest *>(request);
	getRequest->GetConfig().SetBulkNonRepeater(value);
}

void SnmpRequestCommand::SetBulkMaxRepeater(int value) {
	if (!host || type != SnmpType::get)
		return;
	SnmpGetRequest * getRequest = dynamic_cast<SnmpGetRequest *>(request);
	getRequest->GetConfig().SetBulkMaxRepeater(value);
}

void SnmpRequestCommand::AddOid(const char * oidstr) {
	if (!host || type != SnmpType::get)
		return;

	SnmpGetRequest * getRequest = dynamic_cast<SnmpGetRequest *>(request);
	getRequest->AddOid(oidstr);

}

void SnmpRequestCommand::AddVb(const char * oidstr, const wxString& value) {
	if (!host || type != SnmpType::set)
		return;

	SnmpSetRequest * setRequest = dynamic_cast<SnmpSetRequest *>(request);
	setRequest->AddVb(oidstr, value.mb_str());
}

int SnmpRequestCommand::Execute() {
	if (!host)
		return -1;
	RequestManager::GetManager().AddRequestToQueue(request);
	return 0;
}