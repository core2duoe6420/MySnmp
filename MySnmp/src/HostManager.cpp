#include <MySnmp/HostManager.h>

using namespace mysnmp;

SafeInteger HostManager::nextHostId(NULL);
HostManager HostManager::singleton;

/* �ȼ��赥�̷߳��ʲ���Ҫ�� */
HostManager::HostManager() : hosts(NULL, [](const int& key, Host *& value) {
	delete value;
}) {}