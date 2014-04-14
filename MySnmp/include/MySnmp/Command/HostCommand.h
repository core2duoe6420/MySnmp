#ifndef __HOST_COMMAND_H
#define __HOST_COMMAND_H

#include <wx/string.h>
#include <MySnmp/Command/Command.h>

namespace mysnmp {
	class Host;

	class HostCommand : public Command {
	protected:
		int timeout;
		int udpport;
		wxString readcommunity;
		wxString writecommunity;
		int retry;
		wxString version;
		wxString ipAddress;
		int type;
		int hostid;

		void SetConfig(Host * host);

		HostCommand(int hostid) : hostid(hostid) {}
		HostCommand() {}
	public:
		void SetTimeout(int timeout) { this->timeout = timeout; }
		void SetUdpPort(int udpport) { this->udpport = udpport; }
		void SetReadCommunity(const wxString& readcommunity) { this->readcommunity = readcommunity; }
		void SetWriteCommunity(const wxString& writecommunity) { this->writecommunity = writecommunity; }
		void SetRetryTimes(int retry) { this->retry = retry; }
		void SetSnmpVersion(const wxString& version) { this->version = version; }
		void SetIpAddress(const wxString& ip) { this->ipAddress = ip; }
		void SetType(int type) { this->type = type; }
		int GetTimeout() { return this->timeout; }
		int GetUdpPort() { return this->udpport; }
		wxString GetReadCommunity() { return this->readcommunity; }
		wxString GetWriteCommunity() { return this->writecommunity; }
		int GetRetryTimes() { return this->retry; }
		wxString GetSnmpVersion() { return this->version; }
		wxString GetIpAddress() { return this->ipAddress; }
	};

	class AddHostCommand : public HostCommand {
	public:
		AddHostCommand() {}

		virtual int Execute();
	};

	enum {
		GetHostInfo_SUCCESS = 0,
		GetHostInfo_NOTREACH = -1,
		GetHostInfo_PDUERR = -2,
		GetHostInfo_SNMPERR = -3,
		GetHostInfo_VBERR = -4,
		GetHostInfo_NOHOST = -5,
		GetHostInfo_NOOID = -6,
	};

	class GetHostOidCommand : public Command {
	private:
		int hostid;
		const char * oidstr;
		wxString result;
	public:
		GetHostOidCommand(int hostid) : hostid(hostid), oidstr(NULL) {}
		virtual int Execute();

		wxString GetResult() { return result; }
		void SetOid(const char * oidstr) { this->oidstr = oidstr; }
	};

	class HostInfoCommand : public HostCommand {
	public:
		enum {
			COMMAND_READ = 1,
			COMMAND_WRITE = 2,
		};
	private:
		int commandtype;

	public:
		HostInfoCommand(int hostid, int commandtype);

		virtual int Execute();
	};

	class DeleteHostCommand : public Command {
	private:
		int hostid;
	public:
		DeleteHostCommand(int hostid) : hostid(hostid) {}

		/* @return：0表示成功删除
		 *			1表示设置了删除标志但还存在引用没有实际删除
		 *			-1表示没有找到id
		 */
		virtual int Execute();
	};
}

#endif