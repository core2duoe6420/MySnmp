#include <MySnmp/Host.h>
#include <MySnmp/SnmpResult.h>
#include <vector>

#include <MySnmp/debug.h>

using namespace mysnmp;

bool Host::AddOidValue(int requestId, const Snmp_pp::Vb& vb, int lastSnmpErrStatus = 0, int lastPduErrStatus = 0) {
	VbExtended * vbe = new VbExtended(requestId, vb, lastSnmpErrStatus, lastPduErrStatus);
	std::string oid(vb.get_printable_oid());
	return this->oidValues.Insert(oid, vbe, true, [](const std::string& oidStr, VbExtended* & vbe) {
		delete vbe;
	});
}

void Host::AddOidValueFromSnmpResult(SnmpResult * result) {
	Host& host = result->GetHost();
	const std::vector<Snmp_pp::Vb>& vblist = result->GetVbList();
	const std::vector<int>& snmpErrList = result->GetSnmpErrStatus();
	const std::vector<int>& pduErrList = result->GetPduErrStatus();
	switch (result->GetType()) {
	default:
		for (int i = 0; i < vblist.size(); i++)
			host.AddOidValue(result->GetRequestId(), vblist[i], snmpErrList[i], pduErrList[i]);
		break;

	}
}

VbExtended * Host::GetOidValue(const char * oid) const {
	VbExtended ** retval = NULL;
	if ((retval = this->oidValues.Find(oid)) == NULL)
		return NULL;
	return *retval;
}

/* ��δ�����⵽��һ������
 * ����һ��SafeTypeû�������취����
 * ������֪Ϊ��lambda���ʽ�������þֲ�����
 * ֻ����ʱ�����ˡ�
 * ��ʵ��̫����
 */
struct Param {
	const char * oidstr;
	int requestId;
};

std::vector<VbExtended *> * Host::GetOidSubtree(const char * oidstr, int requestId) const {
	std::vector<VbExtended *> * ret;
	Param param;
	param.requestId = requestId;
	param.oidstr = oidstr;
	ret = (vector<VbExtended *> *)((Host*)this)->oidValues.DoSafeWork(
		/* ����һ��lambda���ʽ������ָ��Ĳ����������Ǹ����� */
		[](SafeHashMap<std::string, VbExtended *>::HashMap * hashmap, void * data) {
		Snmp_pp::Oid oid(((Param*)data)->oidstr);
		int len = oid.len();
		std::vector<VbExtended *> * vector = new std::vector<VbExtended *>();
		SafeHashMap<std::string, VbExtended *>::HashMap::iterator iter = hashmap->begin();
		for (; iter != hashmap->end(); iter++) {
			if (oid.nCompare(len, iter->second->GetVb().get_oid()) == 0 &&
				iter->second->GetLastRequestId() == ((Param*)data)->requestId)
				vector->push_back(iter->second);
		}
		return (void*)vector;
	}, /*���ǵڶ����������Ǵ���lambda���ʽ�Ĳ�����Ȩ��֮�� */ (void *)&param);
	return ret;
}