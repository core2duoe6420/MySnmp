#include <MySnmp/Host.h>
#include <MySnmp/SnmpResult.h>
#include <vector>

#include <MySnmp/debug.h>

using namespace mysnmp;

bool Host::AddOidValue(const Snmp_pp::Vb& vb, int lastSnmpErrStatus = 0, int lastPduErrStatus = 0) {
	VbExtended * vbe = new VbExtended(vb, lastSnmpErrStatus, lastPduErrStatus);
	std::string oid(vb.get_printable_oid());
	return this->oidValues.Insert(oid, vbe, true, [](const std::string& oidStr, VbExtended* & vbe) {
		delete vbe;
	});
}

bool Host::AddOidValue(const Snmp_pp::Vb * vb, int vbCount, int lastSnmpErrStatus = 0, int lastPduErrStatus = 0) {
	for (int i = 0; i < vbCount; i++)
		AddOidValue(vb[i], lastSnmpErrStatus, lastPduErrStatus);
	return true;
}

bool Host::AddOidValue(const char * oid, const char * value, int lastSnmpErrStatus = 0, int lastPduErrStatus = 0) {
	Snmp_pp::Vb vb;
	vb.set_value(value);
	Snmp_pp::Oid newoid(oid);
	vb.set_oid(newoid);
	return AddOidValue(vb, lastSnmpErrStatus, lastPduErrStatus);
}

void Host::AddOidValueFromSnmpResult(SnmpResult * result) {
	Host& host = result->GetHost();
	const std::vector<Snmp_pp::Vb>& vblist = result->GetVbList();
	const std::vector<int>& snmpErrList = result->GetSnmpErrStatus();
	const std::vector<int>& pduErrList = result->GetPduErrStatus();
	for (int i = 0; i < vblist.size(); i++)
		host.AddOidValue(vblist[i], snmpErrList[i], pduErrList[i]);
}

//void main() {
//	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
//	OidTree oidtree("oid.xml");
//	Host host(oidtree, (Snmp_pp::IpAddress)"1.1.1.1");
//	host.AddOidValue("1.1.1", "123");
//	Snmp_pp::Vb vb;
//	vb.set_oid("1.2.3.4");
//	vb.set_value("999");
//	host.AddOidValue(&vb);
//	const Snmp_pp::Vb * vbstr;
//	vbstr = host.GetOidValue("1.2.3.4");
//	std::cout << vbstr->get_printable_value();
//	std::cout << host.AddOidValue("1.1.1", "234");
//	std::cout << host.GetOidValue("1.1.1")->get_printable_value();
//	std::cout << host.RemoveOidValue(vbstr);
//	std::cout << host.RemoveOidValue("1.1.1");
//	std::cout << host.RemoveOidValue("1.2.3");
//}