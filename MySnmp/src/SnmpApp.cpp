#include <libsnmp.h>
#include <snmp_pp/snmp_pp.h>

#include <MySnmp/View/SnmpApp.h>

#include <MySnmp/debug.h>

using namespace mysnmp;

wxIMPLEMENT_APP(MySnmpApp);

bool MySnmpApp::OnInit() {
#ifdef _WIN32
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	Snmp_pp::Snmp::socket_startup();
	wxImage::AddHandler(new wxPNGHandler);
	frmMain = new FrmMain("MySnmp", wxPoint(100, 100), wxSize(700, 500));
	frmMain->Show(true);
	return true;
}