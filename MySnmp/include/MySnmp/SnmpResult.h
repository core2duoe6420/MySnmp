#ifndef __SNMP_RESULT_H
#define __SNMP_RESULT_H

#include <vector>
#include <MySnmp/Lock.h>
#include <MySnmp/SnmpRequest.h>

namespace mysnmp {
	class Host;

	class SnmpResult {
	private:
		Lock * lock;
		SnmpType type;
		Host& host;
		std::vector<Snmp_pp::Vb> vblist;
		int requestId;
		const char * errMsg;
		/* ������ƻ��ܲ����� */
		std::vector<int> snmpErrStatus;
		std::vector<int> pduErrStatus;

	public:
		SnmpResult(SnmpType type, Host& host, int requestId, Lock * lock = NULL) :
			type(type), host(host), requestId(requestId), errMsg(NULL), lock(lock) {
			host.Refer();
		}

		~SnmpResult() {
			host.UnRefer();
			if (lock)
				delete lock;
		}

		/* Ϊ����Ϸ����״̬��RequestHolder���뵽������������
		 * Result�¼���������Hostһ������⣬�����ɵ����߸��������ͽ���
		 * ͨ������vblistд��Ͷ�ȡʱ��Ҫʹ����
		 */
		void Lock(int type = 0) { if (lock) lock->Enter(type); }
		void UnLock() { if (lock) lock->Exit(); }
		void SetErrMsg(const char * err) { this->errMsg = err; }
		const char * GetErrMsg() { return this->errMsg; }
		void AddPduErrStatus(int err) { this->pduErrStatus.push_back(err); }
		void AddSnmpErrStatus(int err) { this->snmpErrStatus.push_back(err); }
		int GetRequestId() const { return requestId; }
		SnmpType GetType() const { return type; }
		void AddVb(Snmp_pp::Vb& vb) { vblist.push_back(vb); }
		void ClearVb() { Lock(); vblist.clear(); UnLock(); }
		const std::vector<int>& GetSnmpErrStatus() const { return snmpErrStatus; }
		const std::vector<int>& GetPduErrStatus() const { return pduErrStatus; }
		const std::vector<Snmp_pp::Vb>& GetVbList() const { return vblist; }
		Host& GetHost() const { return host; }
	};
}

#endif