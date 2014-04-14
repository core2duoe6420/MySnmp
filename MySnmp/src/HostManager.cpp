#include <MySnmp/HostManager.h>

using namespace mysnmp;

SafeInteger HostManager::nextHostId(NULL);
HostManager HostManager::singleton;

/* 先假设单线程访问不需要锁 */
HostManager::HostManager() : hosts(NULL, [](const int& key, Host *& value) {
	delete value;
}) {}