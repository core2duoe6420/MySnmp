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
	 * ��������Vb����Ϣ������Host��
	 * Ŀǰʵ�ֵ�����һ�λ�ȡ��Vbʱ�Ĵ����
	 * ��Ϊ���Զ��Vb��һ��Pdu�У�������һ��Pdu��
	 * ������Pdu�е�����Vb��������ͳһ�Ĵ����
	 * ���ܴ��������ĸ�Vb�����
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
		/* ��ָ����Ϊ��ϣ���ֵ���ڴ�й¶��Σ��
		 * �����Լ����ڴ渴�ƣ�lambda���ʽ�ô�����΢�ÿ���һ��
		 */
		SafeHashMap<std::string, VbExtended *> oidValues;
		Snmp_pp::IpAddress address;

	public:
		Host(int id, OidTree& oidtree, const Snmp_pp::IpAddress& address) :
			hostid(id), oidtree(oidtree), address(address),
			oidValues(new RWLock(), [](const std::string& oidStr, VbExtended* & vbe) {
			/* lambda���ʽ */
			delete vbe;
		}) {};

		virtual ~Host() {}

		/* @return:�����ϣ����û��Vb������true
				   �������Vb������ԭֵ������false
				   */
		bool AddOidValue(const Snmp_pp::Vb& vb, int lastSnmpErrStatus, int lastPduErrStatus);
		/* @return:Ŀǰ�ܷ���true */
		bool AddOidValue(const Snmp_pp::Vb * vb, int vbCount, int lastSnmpErrStatus, int lastPduErrStatus);
		/* @return:ͬAddOidValue(const Snmp_pp::Vb * vb) */
		bool AddOidValue(const char * oid, const char * value, int lastSnmpErrStatus, int lastPduErrStatus);

		VbExtended * GetOidValue(const char * oid) const {
			return *this->oidValues.Find(oid);
		}

		/* @return:�ҵ�ֵ��ɾ������true���޴�ֵ����false */
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