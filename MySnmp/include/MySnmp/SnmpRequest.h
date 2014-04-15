#ifndef __SNMP_REQUEST_H
#define __SNMP_REQUEST_H


#include <libsnmp.h>
#include <snmp_pp/snmp_pp.h>

#include <vector>
#include <MySnmp/Host.h>

namespace mysnmp {
	class RequestManager;
	class SnmpResult;
	class OidNode;

#ifndef SNMPTYPE
#define SNMPTYPE
	enum SnmpType {
		SNMP_GET, SNMP_GETNEXT, SNMP_GETBULK, SNMP_WALK,
		SNMP_SET, SNMP_TRAP, SNMP_INFORM
	};
#endif

	class SnmpRequest {
	protected:
		SnmpType type;
		int requestId;
		Host& host;
		RequestManager * manager;
		const char * errMsg;

	public:
		SnmpRequest(int requestId, SnmpType type, Host& host, RequestManager * manager) :
			requestId(requestId), host(host), type(type), manager(manager), errMsg(NULL) {
			host.Refer();
		};

		virtual ~SnmpRequest() {
			host.UnRefer();
		}

		const char * GetErrMsg() const { return errMsg; }
		SnmpType GetType() { return type; }
		RequestManager * GetManager() const { return manager; }
		Host& GetHost() const { return host; }
		int GetRequestId() const { return requestId; }

	};

	class SnmpSetRequest : public SnmpRequest {
	private:
		std::vector<Snmp_pp::Vb> vbs;

	public:
		SnmpSetRequest(int requestId, Host& host, RequestManager * manager) :
			SnmpRequest(requestId, SnmpType::SNMP_SET, host, manager) {}

		virtual ~SnmpSetRequest() {}

		bool AddVb(const Snmp_pp::Oid& oid, const char * value);
		bool AddVb(const char * oidstr, const char * value);
		bool AddVb(const OidNode * oidnode, const char * oidstr, const char * value);

		static void * Run(void * data);
	};

	class SnmpGetRequestBase : public SnmpRequest {
	protected:
		std::vector<Snmp_pp::Oid> oids;
		SnmpGetRequestBase(int requestId, SnmpType type, Host& host, RequestManager * manager) :
			SnmpRequest(requestId, type, host, manager) {}

	public:
		bool AddOid(const char * oidstr);
		bool AddOid(const Snmp_pp::Oid& oid);
		bool AddOid(const OidNode * oid);
		virtual ~SnmpGetRequestBase() {}
		std::vector<Snmp_pp::Oid>& GetOids() { return oids; }
	};

	class SnmpGetRequest : public SnmpGetRequestBase {
	public:
		SnmpGetRequest(int requestId, Host& host, RequestManager * manager) :
			SnmpGetRequestBase(requestId, SnmpType::SNMP_GET, host, manager) {}

		virtual ~SnmpGetRequest() {}

		static void * Run(void * data);
	};

	class SnmpGetNextRequest : public SnmpGetRequestBase {
	public:
		SnmpGetNextRequest(int requestId, Host& host, RequestManager * manager) :
			SnmpGetRequestBase(requestId, SnmpType::SNMP_GETNEXT, host, manager) {}

		virtual ~SnmpGetNextRequest() {}

		static void * Run(void * data);
	};

	class SnmpGetBulkRequest : public SnmpGetRequestBase {
	private:
		int maxReapter, nonRepeater;
	public:
		SnmpGetBulkRequest(int requestId, Host& host, RequestManager * manager) :
			SnmpGetRequestBase(requestId, SnmpType::SNMP_GETBULK, host, manager),
			maxReapter(0), nonRepeater(0) {}

		virtual ~SnmpGetBulkRequest() {}

		void SetMaxReapter(int value) { this->maxReapter = value; }
		void SetNonReapter(int value) { this->nonRepeater = value; }
		int GetMaxReapter() { return maxReapter; }
		int GetNonReapter() { return nonRepeater; }

		static void * Run(void * data);
	};

	class SnmpWalkRequest : public SnmpGetRequestBase {
	public:
		SnmpWalkRequest(int requestId, Host& host, RequestManager * manager) :
			SnmpGetRequestBase(requestId, SnmpType::SNMP_WALK, host, manager) {}

		virtual ~SnmpWalkRequest() {}

		static void * Run(void * data);
	};
}

#endif