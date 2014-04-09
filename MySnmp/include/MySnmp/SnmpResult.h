#ifndef __SNMP_RESULT_H
#define __SNMP_RESULT_H

#include <vector>
#include <MySnmp/SnmpRequest.h>

namespace mysnmp {
	class Host;

	class SnmpResult {
	private:
		SnmpType type;
		Host& host;
		std::vector<Snmp_pp::Vb> vblist;
		int requestId;
		const char * errMsg;
		/* 错误机制还很不成熟 */
		int snmpErrStatus;
		int pduErrStatus;

	public:
		SnmpResult(SnmpType type, Host& host, int requestId) :
			type(type), host(host), requestId(requestId),
			snmpErrStatus(0), pduErrStatus(0), errMsg(NULL) {}

		~SnmpResult() {}

		void SetErrMsg(const char * err) { this->errMsg = err; }
		const char * GetErrMsg() { return this->errMsg; }
		void SetPduErrStatus(int err) { this->pduErrStatus = err; }
		int GetPduErrStatus() { return this->pduErrStatus; }
		void SetSnmpErrStatus(int err) { this->snmpErrStatus = err; }
		int GetSnmpErrStatus() { return this->snmpErrStatus; }
		int GetRequestId() const { return requestId; }
		void AddVb(Snmp_pp::Vb& vb) { vblist.push_back(vb); }

		void AddVb(Snmp_pp::Vb * vbs, int count) {
			for (int i = 0; i < count; i++)
				vblist.push_back(vbs[i]);
		}

		const std::vector<Snmp_pp::Vb>& GetVbList() const { return vblist; }
		Host& GetHost() const { return host; }
	};
}

#endif