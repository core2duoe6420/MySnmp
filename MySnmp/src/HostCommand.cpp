#include <MySnmp/HostManager.h>
#include <MySnmp/OidTree.h>
#include <MySnmp/Command/HostCommand.h>

#include <algorithm>

#include <MySnmp/debug.h>

using namespace mysnmp;

void HostCommand::SetConfig(Host * host) {
	HostConfig& config = host->GetConfig();
	config.SetTimeout(timeout * 100);
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
}


int AddHostCommand::Execute() {
	if (this->ipAddress == wxEmptyString)
		return -1;
	Snmp_pp::IpAddress ip(ipAddress.mb_str());
	if (!ip.valid())
		return -1;

	Host * host = HostManager::CreateHost(OidTree::GetDefaultOidTree(), ip);
	this->hostid = host->GetId();
	//这里Host刚创建，应该没有同时访问Host的可能，就不上锁了
	HostCommand::SetConfig(host);
	return hostid;
}

int GetHostOidCommand::Execute() {
	if (!oidstr)
		return GetHostInfo_NOOID;
	Host * host = HostManager::GetHost(hostid);
	if (!host) {
		result = L"无此主机";
		return GetHostInfo_NOHOST;
	}
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
	if (!host)
		return -1;

	host->SetDelFlag(true);
	if (host->GetReferenceCount() == 0) {
		HostManager::RemoveHost(hostid);
		return 0;
	}
	return 1;
}

HostInfoCommand::HostInfoCommand(int hostid, int commandtype) :
HostCommand(hostid), commandtype(commandtype) {
	if (commandtype == COMMAND_READ) {
		Host * host = HostManager::GetHost(hostid);
		host->Lock();
		this->ipAddress = host->GetAddress().get_printable();
		this->retry = host->GetConfig().GetRetryTimes();
		this->timeout = host->GetConfig().GetTimeout() / 100;
		this->udpport = host->GetConfig().GetUDPPort();
		this->readcommunity = host->GetConfig().GetReadCommunity();
		this->writecommunity = host->GetConfig().GetWriteCommunity();
		Snmp_pp::snmp_version version = host->GetConfig().GetSnmpVersion();
		switch (version) {
		case Snmp_pp::snmp_version::version1:
			this->version = "version1"; break;
		case Snmp_pp::snmp_version::version2c:
			this->version = "version2c"; break;
		case Snmp_pp::snmp_version::version3:
			this->version = "version3"; break;
		}
		host->UnLock();
	}
}

int HostInfoCommand::Execute() {
	if (this->commandtype == COMMAND_READ)
		return -2;
	Host * host = HostManager::GetHost(hostid);
	if (!host)
		return -1;

	host->Lock();
	host->SetAddress(ipAddress.mb_str());
	HostCommand::SetConfig(host);
	host->UnLock();
	return 0;
}

int GetOidSubtreeCommand::Execute() {
	Host * host = HostManager::GetHost(hostid);
	if (!host || !oidstr)
		return -1;

	results.clear();
	std::vector<VbExtended *> * vbes = host->GetOidSubtree(oidstr, requestId);
	for (int i = 0; i < vbes->size(); i++)
		results.push_back(((*vbes)[i])->GetVb());

	delete vbes;
	sort(results.begin(), results.end(), [](const Snmp_pp::Vb& vb1, const Snmp_pp::Vb& vb2) {
		return vb1.get_oid() < vb2.get_oid();
	});
	return results.size();
}