#include <MySnmp/Command/SnmpRequestCommand.h>
#include <MySnmp/HostManager.h>
#include <MySnmp/RequestManager.h>

#include <MySnmp/debug.h>

using namespace mysnmp;

SnmpRequestCommand::SnmpRequestCommand(SnmpType type, int hostId) : type(type) {
	host = HostManager::GetHost(hostId);
	if (!host)
		return;
	request = RequestManager::GetManager().CreateSnmpGetRequest(*host);
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
	if (!host)
		return;
	if (type == SnmpType::get) {
		SnmpGetRequest * getRequest = dynamic_cast<SnmpGetRequest *>(request);
		getRequest->AddOid(oidstr);
	}
}

int SnmpRequestCommand::Execute() {
	if (!host)
		return -1;
	RequestManager::GetManager().AddRequestToQueue(request);
	return 0;
}