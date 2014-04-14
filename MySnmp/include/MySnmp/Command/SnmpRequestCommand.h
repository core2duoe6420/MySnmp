#ifndef __SNMP_REQEUST_COMMAND_H
#define __SNMP_REQEUST_COMMAND_H

#include <wx/string.h>
#include <MySnmp/Command/Command.h>

namespace mysnmp {

#ifndef SNMPTYPE
#define SNMPTYPE
	enum SnmpType {
		get, set, trap, infrom
	};
#endif

	class SnmpRequest;
	class Host;
	class SnmpRequestCommand : public Command {

	private:
		SnmpRequest * request;
		int hostId;
		SnmpType type;
		Host * host;
	public:
		SnmpRequestCommand(SnmpType type, int hostId);

		void SetBulkNonRepeater(int value);
		void SetBulkMaxRepeater(int value);
		void AddOid(const char * oidstr);
		void AddVb(const char * oidstr, const wxString& value);
		virtual int Execute();
	};
}

#endif