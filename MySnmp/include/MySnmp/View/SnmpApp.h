#ifndef __SNMP_APP_H
#define __SNMP_APP_H

#include <MySnmp/View/FrmMain.h>

namespace mysnmp {

	class MySnmpApp : public wxApp {
	private:
		wxFrame * frmMain;
	public:
		virtual bool OnInit();
		wxFrame * GetFrmMain() { return frmMain; }
	};

};

#endif