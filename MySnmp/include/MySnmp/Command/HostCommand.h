#ifndef __HOST_COMMAND_H
#define __HOST_COMMAND_H

#include <wx/string.h>
#include <MySnmp/Command/Command.h>
#include <MySnmp/HostManager.h>
#include <MySnmp/OidTree.h>


namespace mysnmp {

	class AddHostCommand : public Command {
	private:
		int timeout;
		int udpport;
		wxString readcommunity;
		wxString writecommunity;
		int retry;
		wxString version;
		wxString ipAddress;
		int type;
		int newHostId;

	public:
		AddHostCommand(int type = 0, const wxString& ipAddress = wxEmptyString,
			int timeout = 300, int retry = 2,
			const wxString& version = "version2c",
			const wxString& wrtiecommunity = "public",
			const wxString& readcommuity = "public",
			int udpport = 161) :
			type(type), timeout(timeout), udpport(udpport), retry(retry),
			ipAddress(ipAddress), readcommunity(readcommuity), writecommunity(writecommunity),
			version(version) {}

		virtual int Execute() {
			if (this->ipAddress == wxEmptyString)
				return -1;
			Snmp_pp::IpAddress ip(ipAddress.mb_str());
			if (!ip.valid())
				return -1;

			Host * host = HostManager::GetHostManager().CreateHost(OidTree::GetDefaultOidTree(), ip);
			this->newHostId = host->GetId();
			HostConfig& config = host->GetConfig();
			config.SetTimeout(timeout);
			config.SetRetryTimes(retry);
			config.SetUDPPort(udpport);
			config.SetReadCommunity(readcommunity.mb_str());
			config.SetWriteCommunity(writecommunity.mb_str());
			Snmp_pp::snmp_version snmpVersion;
			if (version == "version1")
				snmpVersion = Snmp_pp::snmp_version::version1;
			else if (version == "version2c")
				snmpVersion = Snmp_pp::snmp_version::version2c;
			else if (version == "version3")
				snmpVersion = Snmp_pp::snmp_version::version3;
			config.SetSnmpVersion(snmpVersion);
			return newHostId;
		}

		void SetTimeout(int timeout) { this->timeout = timeout; }
		void SetUdpPort(int udpport) { this->udpport = udpport; }
		void SetReadCommunity(const wxString& readcommunity) { this->readcommunity = readcommunity; }
		void SetWriteCommunity(const wxString& writecommunity) { this->writecommunity = writecommunity; }
		void SetRetryTimes(int retry) { this->retry = retry; }
		void SetSnmpVersion(const wxString& version) { this->version = version; }
		void SetIpAddress(const wxString& ip) { this->ipAddress = ip; }
		void SetType(int type) { this->type = type; }
	};

}

#endif