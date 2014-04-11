#include <MySnmp/Command/HostCommand.h>
#include <MySnmp/HostManager.h>
#include <MySnmp/OidTree.h>

#include <MySnmp/debug.h>

using namespace mysnmp;

AddHostCommand::AddHostCommand(int type, const wxString& ipAddress,
							   int timeout, int retry,
							   const wxString& version,
							   const wxString& wrtiecommunity,
							   const wxString& readcommuity,
							   int udpport) :
							   type(type), timeout(timeout), udpport(udpport), retry(retry),
							   ipAddress(ipAddress), readcommunity(readcommuity), writecommunity(writecommunity),
							   version(version) {}

int AddHostCommand::Execute() {
	if (this->ipAddress == wxEmptyString)
		return -1;
	Snmp_pp::IpAddress ip(ipAddress.mb_str());
	if (!ip.valid())
		return -1;

	Host * host = HostManager::CreateHost(OidTree::GetDefaultOidTree(), ip);
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

int GetHostInfoCommand::Execute() {
	if (!oidstr)
		return -1;
	Host * host = HostManager::GetHost(hostid);
	VbExtended * vbe = host->GetOidValue(oidstr);
	if (!vbe) {
		result = L"请求结果未抵达";
		return GetHostInfo_NOTREACH;
	}
	if (vbe->GetLastPduErrStatus() != 0) {
		result = Snmp_pp::Snmp::error_msg(vbe->GetLastPduErrStatus());
		return GetHostInfo_PDUERR;
	}
	if (vbe->GetLastSnmpErrStatus() != 0) {
		result = Snmp_pp::Snmp::error_msg(vbe->GetLastSnmpErrStatus());
		return GetHostInfo_SNMPERR;
	}

	Snmp_pp::Vb& vb = vbe->GetVb();
	if ((vb.get_syntax() == sNMP_SYNTAX_ENDOFMIBVIEW)) {
		result = L"已抵达MIB末尾";
		return GetHostInfo_VBERR;
	} else if (vb.get_syntax() == sNMP_SYNTAX_NOSUCHINSTANCE) {
		result = L"代理报告无此实例";
		return GetHostInfo_VBERR;
	} else if (vb.get_syntax() == sNMP_SYNTAX_NOSUCHOBJECT) {
		result = L"代理报告无此对象";
		return GetHostInfo_VBERR;
	}

	result = vb.get_printable_value();
	return GetHostInfo_SUCCESS;
}

int DeleteHostCommand::Execute() {
	Host * host = HostManager::GetHost(hostid);
	if (host) {
		host->SetDelFlag(true);
		if (host->GetReferenceCount() == 0) {
			HostManager::RemoveHost(hostid);
			return 0;
		}
		return 1;
	}
	return 2;
}
