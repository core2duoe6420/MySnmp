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
			FrmSnmpTableBase(module, "1.3.6.1.2.1.6.13", L" TCP连接表", wxSize(600, 400)) {}
	};

	class FrmTcpBasic : public FrmListCtrlBase {
	private:
		int lastRequestId;
		TcpBasicModule * module;
		TopoHost * chosenHost;

		virtual void sendRequest();
		virtual void updateListCtrl();

		virtual void OnListDoubleClick(wxListEvent& event);

	public:
		FrmTcpBasic(TcpBasicModule * module);
	};
}

using namespace mysnmp;

//TcpConnTableModule & FrmTcpConnTable

void TcpConnTableModule::OnMenuItemClick(wxCommandEvent& event) {
	FrmTcpConnTable * frmTcpConnTable = new FrmTcpConnTable(this);
	frmTcpConnTable->Show();
}

void FrmTcpConnTable::OnListDoubleClick(wxListEvent& event) {}


//TcpBasicModule & FrmTcpBasic

void TcpBasicModule::OnMenuItemClick(wxCommandEvent& event) {
	FrmTcpBasic * frmTcpBasic = new FrmTcpBasic(this);
	frmTcpBasic->Show();
}

FrmTcpBasic::FrmTcpBasic(TcpBasicModule * module) :
FrmListCtrlBase(module->GetCanvas()->GetParent(), "", wxSize(450, 400)), module(module) {
	chosenHost = module->GetCanvas()->GetChosenHost();
	this->SetTitle(chosenHost->GetName() + L" TCP基本信息");

	sendRequest();

	list->InsertColumn(0, L"项目");
	list->InsertColumn(1, L"值");
	list->SetColumnWidth(0, 200);
	list->SetColumnWidth(1, 200);

	XMLColumnCollection * columns = module->GetColumnCollection();
	for (int i = 0; i < columns->GetColumnCount(); i++)
		list->InsertItem(i, columns->GetColumn(i)->GetName(), -1);

	updateListCtrl();
}

void FrmTcpBasic::OnListDoubleClick(wxListEvent& event) {}

void FrmTcpBasic::sendRequest() {
	SnmpRequestCommand command(SnmpType::SNMP_GET, chosenHost->GetHostId());
	Snmp_pp::Oid oid("1.3.6.1.2.1.6.1.0");
	for (int i = 1; i < 13; i++) {
		oid[7] = i;
		command.AddOid(oid.get_printable());
	}
	command.Execute();
}

void FrmTcpBasic::updateListCtrl() {
	GetHostOidCommand command(chosenHost->GetHostId());
	Snmp_pp::Oid oid("1.3.6.1.2.1.6.1.0");

	for (int i = 0; i < 12; i++) {
		oid[7] = i + 1;
		command.SetOid(oid.get_printable());
		command.Execute();
		wxString value = command.GetResult();
		const ColumnInfo * column = module->GetColumnCollection()->GetColumn(i);
		if (column->HasValueMap())
			value = column->MapValueToString(wxAtoi(value));

		list->SetItem(i, 1, value);
	}
}