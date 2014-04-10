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
		std::vector<int> snmpErrStatus;
		std::vector<int> pduErrStatus;

	public:
		SnmpResult(SnmpType type, Host& host, int requestId) :
			type(type), host(host), requestId(requestId), errMsg(NULL) {}

		~SnmpResult() {}

		void SetErrMsg(const char * err) { this->errMsg = err; }
		const char * GetErrMsg() { return this->errMsg; }
		void AddPduErrStatus(int err) { this->pduErrStatus.push_back(err); }
		void AddSnmpErrStatus(int err) { this->snmpErrStatus.push_back(err); }
		int GetRequestId() const { return requestId; }
		void AddVb(Snmp_pp::Vb& vb) { vblist.push_back(vb); }
		const std::vector<int>& GetSnmpErrStatus() const { return snmpErrStatus; }
		const std::vector<int>& GetPduErrStatus() const { return pduErrStatus; }
		const std::vector<Snmp_pp::Vb>& GetVbList() const { return vblist; }
		Host& GetHost() const { return host; }
	};
}

#endif