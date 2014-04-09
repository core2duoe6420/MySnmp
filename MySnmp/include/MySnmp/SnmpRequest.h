#ifndef __SNMP_REQUEST_H
#define __SNMP_REQUEST_H


#include <libsnmp.h>
#include <snmp_pp/snmp_pp.h>

#include <vector>
#include <MySnmp/Host.h>
#include <MySnmp/SnmpRequestConfig.h>

namespace mysnmp {
	class RequestManager;
	class SnmpResult;
	class OidNode;

	enum SnmpType {
		get, set, trap, infrom
	};

	class SnmpRequest {
	protected:
		SnmpType type;
		int requestId;
		Host& host;
		const char * errMsg;

	public:
		SnmpRequest(int requestId, Host& host, SnmpType type) :
			requestId(requestId), host(host), type(type), errMsg(NULL) {};

		virtual ~SnmpRequest() {}

		const char * GetErrMsg() const { return errMsg; }
		SnmpType GetType() { return type; }
		int GetRequestId() const { return requestId; }

	};

	class SnmpGetRequest : public SnmpRequest {
	private:
		RequestManager * manager;
		SnmpGetRequestConfig config;
		std::vector<Snmp_pp::Oid> getVector;
		std::vector<Snmp_pp::Oid> getNextVector;
		std::vector<Snmp_pp::Oid> getBulkVector;
		int oidTotalCount;

		void handleVector(std::vector<Snmp_pp::Oid>& vector,
			Snmp_pp::Snmp& snmp, Snmp_pp::SnmpTarget& target,
			Snmp_pp::Pdu& pdu, SnmpResult * result);

	public:
		SnmpGetRequest(int requestId, Host& host, RequestManager * manager) :
			SnmpRequest(requestId, host, SnmpType::get), oidTotalCount(0), manager(manager) {};

		virtual ~SnmpGetRequest() {}

		bool AddOid(const Snmp_pp::Oid& oid);
		bool AddOid(const char * oidstr);
		bool AddOid(const OidNode * oid);

		SnmpGetRequestConfig& GetConfig() { return config; }

		static void * Run(void * data);

	};
}

#endif