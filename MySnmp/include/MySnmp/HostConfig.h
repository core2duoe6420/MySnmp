#ifndef __HOST_CONFIG_H
#define __HOST_CONFIG_H

#include <libsnmp.h>
#include <snmp_pp/snmp_pp.h>

#include <MySnmp/Config.h>

namespace mysnmp {
	class HostConfig : public Config {
	private:
		const char * UDPPORT = "udpport";
		const char * RETRYTIMES = "retrytimes";
		const char * TIMEOUT = "timeout";
		const char * READCOMMUNITY = "readcommunity";
		const char * WRITECOMMUNITY = "writecommuinty";
		const char * SNMPVERSION = "snmp_version";
	public:
		HostConfig() {
			Config::addConfigItem(UDPPORT, "161");
			Config::addConfigItem(RETRYTIMES, "3");
			Config::addConfigItem(TIMEOUT, "300");
			Config::addConfigItem(READCOMMUNITY, "public");
			Config::addConfigItem(WRITECOMMUNITY, "public");
			Config::addConfigItem(SNMPVERSION, "v2c");
		}

		virtual ~HostConfig() {}

		void SetRetryTimes(int retry) {
			Config::setConfigItemValue(RETRYTIMES, retry);
		}

		int GetRetryTimes() const {
			return Config::GetIntConfigItem(RETRYTIMES);
		}

		void SetUDPPort(int udpport) {
			Config::setConfigItemValue(UDPPORT, udpport);
		}

		int GetUDPPort() const {
			return Config::GetIntConfigItem(UDPPORT);
		}

		void SetTimeout(int timeout) {
			Config::setConfigItemValue(TIMEOUT, timeout);
		}

		int GetTimeout() const {
			return Config::GetIntConfigItem(TIMEOUT);
		}

		void SetReadCommunity(const char * community) {
			Config::setConfigItemValue(READCOMMUNITY, community);
		}

		const char * GetReadCommunity() const {
			return Config::GetConfigItem(READCOMMUNITY);
		}

		void SetWriteCommunity(const char * community) {
			Config::setConfigItemValue(WRITECOMMUNITY, community);
		}

		const char * GetWriteCommunity() const {
			return Config::GetConfigItem(WRITECOMMUNITY);
		}

		void SetSnmpVersion(Snmp_pp::snmp_version snmpversion);
		Snmp_pp::snmp_version GetSnmpVersion() const;

	};
}

#endif