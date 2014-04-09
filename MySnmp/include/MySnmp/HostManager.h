#ifndef __HOST_MANAGER_H
#define __HOST_MANAGER_H

#include <MySnmp/Host.h>

namespace mysnmp {
	class HostManager {
	private:
		static SafeInteger nextHostId;
		static HostManager singleton;
		SafeHashMap<int, Host *> hosts;

		HostManager();
		HostManager(const HostManager& other) : hosts(NULL) {}
	public:

		static HostManager& GetHostManager() {
			return singleton;
		}

		Host * GetHost(int hostid) const {
			return *this->hosts.Find(hostid);
		}

		Host * CreateHost(OidTree& oidtree, Snmp_pp::IpAddress ip) {
			int nextId = nextHostId.GetAndInc();
			Host * host = new Host(nextId, oidtree, ip);
			this->hosts.Insert(nextId, host);
			return host;
		}

		void RemoveHost(int hostid) {
			this->hosts.Delete(hostid, [](const int& key, Host* & value) {
				delete value;
			});
		}
	};
}

#endif