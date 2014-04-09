#ifndef __HOST_H
#define __HOST_H

#include <libsnmp.h>
#include <snmp_pp/snmp_pp.h>

#include <MySnmp/SafeType.h>
#include <MySnmp/HostConfig.h>


namespace mysnmp {
	class SnmpResult;
	class OidTree;

	/* VbExtended class
	 * 用来扩充Vb的信息，放在Host中
	 * 目前实现的有上一次获取该Vb时的错误号
	 * 因为可以多个Vb在一个Pdu中，所以在一个Pdu中
	 * 发生该Pdu中的所有Vb都会设置统一的错误号
	 * 不管错误是由哪个Vb引起的
	 */
	class VbExtended {
	private:
		Snmp_pp::Vb vb;
		int lastSnmpErrStatus;
		int lastPduErrStatus;

	public:
		VbExtended(const Snmp_pp::Vb& vb, int lastSnmpErrStatus = 0, int lastPduErrStatus = 0) :
			vb(vb), lastSnmpErrStatus(lastSnmpErrStatus), lastPduErrStatus(lastPduErrStatus) {}

		Snmp_pp::Vb& GetVb() { return vb; }
		int GetLastSnmpErrStatus() const { return lastSnmpErrStatus; }
		int GetLastPduErrStatus() const { return lastPduErrStatus; }
	};

	class Host {
	private:
		int hostid;
		HostConfig config;
		const OidTree& oidtree;
		/* 用指针作为哈希表的值有内存泄露的危险
		 * 但可以减少内存复制，lambda表达式让代码稍微好看了一下
		 */
		SafeHashMap<std::string, VbExtended *> oidValues;
		Snmp_pp::IpAddress address;

	public:
		Host(int id, OidTree& oidtree, const Snmp_pp::IpAddress& address) :
			hostid(id), oidtree(oidtree), address(address),
			oidValues(new RWLock(), [](const std::string& oidStr, VbExtended* & vbe) {
			/* lambda表达式 */
			delete vbe;
		}) {};

		virtual ~Host() {}

		/* @return:如果哈希表中没有Vb，返回true
				   如过已有Vb，更新原值，返回false
				   */
		bool AddOidValue(const Snmp_pp::Vb& vb, int lastSnmpErrStatus, int lastPduErrStatus);
		/* @return:目前总返回true */
		bool AddOidValue(const Snmp_pp::Vb * vb, int vbCount, int lastSnmpErrStatus, int lastPduErrStatus);
		/* @return:同AddOidValue(const Snmp_pp::Vb * vb) */
		bool AddOidValue(const char * oid, const char * value, int lastSnmpErrStatus, int lastPduErrStatus);

		VbExtended * GetOidValue(const char * oid) const {
			return *this->oidValues.Find(oid);
		}

		/* @return:找到值并删除返回true，无此值返回false */
		bool RemoveOidValue(const Snmp_pp::Vb& vb) {
			const char * oid = vb.get_printable_oid();
			return RemoveOidValue(oid);
		}

		bool RemoveOidValue(const char * oid) {
			return this->oidValues.Delete(oid);
		}

		int GetId() const { return hostid; }
		const Snmp_pp::IpAddress& GetAddress() const { return address; }
		const OidTree& GetOidTree() const { return oidtree; }
		HostConfig& GetConfig() { return config; }

		static void AddOidValueFromSnmpResult(SnmpResult * result);
	};
}

#endif