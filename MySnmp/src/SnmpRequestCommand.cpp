#include <MySnmp/Command/SnmpRequestCommand.h>
#include <MySnmp/HostManager.h>
#include <MySnmp/RequestManager.h>

#include <MySnmp/debug.h>

using namespace mysnmp;

SnmpRequestCommand::SnmpRequestCommand(SnmpType type, int hostId) : type(type) {
	host = HostManager::GetHost(hostId);
	request = RequestManager::GetManager().CreateSnmpGetRequest(*host);
}

void SnmpRequestCommand::AddOid(const char * oidstr) {
	if (type == SnmpType::get) {
		SnmpGetRequest * getRequest = dynamic_cast<SnmpGetRequest *>(request);
		getRequest->AddOid(oidstr);
	}
}

int SnmpRequestCommand::Execute() {
	RequestManager::GetManager().AddRequestToQueue(request);
	return 0;
}