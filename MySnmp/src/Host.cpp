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

/* 这段代码糟糕到了一定境界
 * 但是一来SafeType没有其他办法遍历
 * 二来不知为何lambda表达式不能引用局部变量
 * 只能暂时这样了。
 * 我实在太弱了
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
		/* 这是一个lambda表达式作函数指针的参数，下面是个函数 */
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
	}, /*这是第二个参数，是传给lambda表达式的参数，权宜之计 */ (void *)&param);
	return ret;
}