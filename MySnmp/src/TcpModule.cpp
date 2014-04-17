#include <MySnmp/View/Module.h>
#include <MySnmp/View/FrmListCtrlBase.h>
#include <MySnmp/Command/HostCommand.h>
#include <MySnmp/Command/SnmpRequestCommand.h>

#include <wx/wx.h>

#include <MySnmp/debug.h>

namespace mysnmp {
	class FrmTcpConnTable : public FrmSnmpTableBase {
	private:
		virtual void OnListDoubleClick(wxListEvent& event);
	public:
		FrmTcpConnTable(TcpConnTableModule * module) :
			FrmSnmpTableBase(module, "1.3.6.1.2.1.6.13", L" TCPÁ¬½Ó±í", wxSize(600, 400)) {}
	};
}

using namespace mysnmp;

//TcpConnTableModule & FrmTcpConnTable

void TcpConnTableModule::OnMenuItemClick(wxCommandEvent& event) {
	FrmTcpConnTable * frmTcpConnTable = new FrmTcpConnTable(this);
	frmTcpConnTable->Show();
}

void FrmTcpConnTable::OnListDoubleClick(wxListEvent& event) {}