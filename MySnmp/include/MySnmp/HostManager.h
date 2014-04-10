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

		static Host * GetHost(int hostid) {
			return *singleton.hosts.Find(hostid);
		}

		static Host * CreateHost(OidTree& oidtree, Snmp_pp::IpAddress ip) {
			int nextId = nextHostId.GetAndInc();
			Host * host = new Host(nextId, oidtree, ip);
			singleton.hosts.Insert(nextId, host);
			return host;
		}

		static void RemoveHost(int hostid) {
			singleton.hosts.Delete(hostid, [](const int& key, Host* & value) {
				delete value;
			});
		}
	};
}

#endif