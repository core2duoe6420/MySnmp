#include <MySnmp/debug.h>

#include <MySnmp/HostConfig.h>

using namespace mysnmp;

void HostConfig::SetSnmpVersion(Snmp_pp::snmp_version snmpversion) {
	const char * value = NULL;
	switch (snmpversion) {
	case Snmp_pp::snmp_version::version1:
		value = "v1"; break;
	case Snmp_pp::snmp_version::version2c:
		value = "v2c"; break;
#ifdef _SNMPv3
	case Snmp_pp::snmp_version::version3:
		value = "v3"; break;
#endif
	}
	Config::setConfigItemValue(SNMPVERSION, value);
}

Snmp_pp::snmp_version HostConfig::GetSnmpVersion() const {
	const char * value = Config::GetConfigItem(SNMPVERSION);
	if (strcmp(value, "v1") == 0)
		return Snmp_pp::snmp_version::version1;
	if (strcmp(value, "v2c") == 0)
		return Snmp_pp::snmp_version::version2c;
#ifdef _SNMPv3
	if (strcmp(value, "v3") == 0)
		return Snmp_pp::snmp_version::version3;
#endif
}