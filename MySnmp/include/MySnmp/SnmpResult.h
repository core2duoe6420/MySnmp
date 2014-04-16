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
		/* 错误机制还很不成熟 */
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

		/* 为了配合非完成状态的RequestHolder加入到结果处理队列中
		 * Result新加了锁，与Host一样很糟糕，必须由调用者负责上锁和解锁
		 * 通常是往vblist写入和读取时需要使用锁
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