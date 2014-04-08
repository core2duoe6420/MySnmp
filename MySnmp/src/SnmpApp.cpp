#include <MySnmp/View/FrmMain.h>
#include <MySnmp/debug.h>

namespace mysnmp {

	class MySnmpApp : public wxApp {
	public:
		virtual bool OnInit();
	};

};

using namespace mysnmp;

wxIMPLEMENT_APP(MySnmpApp);

bool MySnmpApp::OnInit() {
#ifdef _WIN32
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	wxImage::AddHandler(new wxPNGHandler);
	FrmMain * frmMain = new FrmMain("MySnmp", wxPoint(300, 300), wxSize(500, 400));
	frmMain->Show(true);
	return true;
}