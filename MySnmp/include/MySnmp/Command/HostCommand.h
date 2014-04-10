#ifndef __HOST_COMMAND_H
#define __HOST_COMMAND_H

#include <wx/string.h>
#include <MySnmp/Command/Command.h>

namespace mysnmp {

	class AddHostCommand : public Command {
	private:
		int timeout;
		int udpport;
		wxString readcommunity;
		wxString writecommunity;
		int retry;
		wxString version;
		wxString ipAddress;
		int type;
		int newHostId;

	public:
		AddHostCommand(int type = 0, const wxString& ipAddress = wxEmptyString,
					   int timeout = 300, int retry = 2,
					   const wxString& version = "version2c",
					   const wxString& wrtiecommunity = "public",
					   const wxString& readcommuity = "public",
					   int udpport = 161);

		virtual int Execute();

		void SetTimeout(int timeout) { this->timeout = timeout; }
		void SetUdpPort(int udpport) { this->udpport = udpport; }
		void SetReadCommunity(const wxString& readcommunity) { this->readcommunity = readcommunity; }
		void SetWriteCommunity(const wxString& writecommunity) { this->writecommunity = writecommunity; }
		void SetRetryTimes(int retry) { this->retry = retry; }
		void SetSnmpVersion(const wxString& version) { this->version = version; }
		void SetIpAddress(const wxString& ip) { this->ipAddress = ip; }
		void SetType(int type) { this->type = type; }
	};

	enum {
		GetHostInfo_SUCCESS = 0,
		GetHostInfo_NOTREACH = 1,
		GetHostInfo_PDUERR = 2,
		GetHostInfo_SNMPERR = 3,
		GetHostInfo_VBERR = 4,
	};

	class GetHostInfoCommand : public Command {
	private:
		int hostid;
		const char * oidstr;
		wxString result;
	public:
		GetHostInfoCommand(int hostid) : hostid(hostid), oidstr(NULL) {}
		virtual int Execute();

		wxString GetResult() { return result; }
		void SetOid(const char * oidstr) { this->oidstr = oidstr; }
	};

}

#endif