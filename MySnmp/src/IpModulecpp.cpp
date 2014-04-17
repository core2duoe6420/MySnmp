#include <MySnmp/View/Module.h>
#include <MySnmp/View/FrmListCtrlBase.h>
#include <MySnmp/Command/HostCommand.h>
#include <MySnmp/Command/SnmpRequestCommand.h>

#include <wx/wx.h>

#include <MySnmp/debug.h>

namespace mysnmp {
	class FrmIpRouteTable : public FrmSnmpTableBase {
	private:
		virtual void OnListDoubleClick(wxListEvent& event);
	public:
		FrmIpRouteTable(IpRouteTableModule * module) :
			FrmSnmpTableBase(module, "1.3.6.1.2.1.4.21", L" IP路由表", wxSize(800, 400)) {}
	};

	class FrmIpAddrTable : public FrmSnmpTableBase {
	private:
		virtual void OnListDoubleClick(wxListEvent& event);
	public:
		FrmIpAddrTable(IpAddrTableModule * module) :
			FrmSnmpTableBase(module, "1.3.6.1.2.1.4.20", L" IP地址表", wxSize(450, 400)) {}
	};

	class FrmIpTransTable : public FrmSnmpTableBase {
	private:
		virtual void OnListDoubleClick(wxListEvent& event);
	public:
		FrmIpTransTable(IpTransTableModule * module) :
			FrmSnmpTableBase(module, "1.3.6.1.2.1.4.22", L" IP转换表", wxSize(450, 400)) {}
	};

	class FrmIpBasic : public FrmListCtrlBase {
	private:
		int lastRequestId;
		IpBasicModule * module;
		TopoHost * chosenHost;

		virtual void sendRequest();
		virtual void updateListCtrl();

		virtual void OnListDoubleClick(wxListEvent& event);

	public:
		FrmIpBasic(IpBasicModule * module);
	};
}

using namespace mysnmp;

//IpRouteTableModule & FrmIpRouteTable

void IpRouteTableModule::OnMenuItemClick(wxCommandEvent& event) {
	FrmIpRouteTable * frmIpRouteTable = new FrmIpRouteTable(this);
	frmIpRouteTable->Show();
}

void FrmIpRouteTable::OnListDoubleClick(wxListEvent& event) {}

//IpAddrModule & FrmIpAddrTable

void IpAddrTableModule::OnMenuItemClick(wxCommandEvent& event) {
	FrmIpAddrTable * frmIpAddrTable = new FrmIpAddrTable(this);
	frmIpAddrTable->Show();
}

void FrmIpAddrTable::OnListDoubleClick(wxListEvent& event) {}

//IpTransTableModule & FrmIpTransTable

void IpTransTableModule::OnMenuItemClick(wxCommandEvent& event) {
	FrmIpTransTable * frmIpTransTable = new FrmIpTransTable(this);
	frmIpTransTable->Show();
}

void FrmIpTransTable::OnListDoubleClick(wxListEvent& event) {}

//IpBasicModule & FrmIpBasic

void IpBasicModule::OnMenuItemClick(wxCommandEvent& event) {
	FrmIpBasic * frmIpBasic = new FrmIpBasic(this);
	frmIpBasic->Show();
}

FrmIpBasic::FrmIpBasic(IpBasicModule * module) :
FrmListCtrlBase(module->GetCanvas()->GetParent(), "", wxSize(450, 550)), module(module) {
	chosenHost = module->GetCanvas()->GetChosenHost();
	this->SetTitle(chosenHost->GetName() + L" IP基本信息");

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

void FrmIpBasic::OnListDoubleClick(wxListEvent& event) {}

void FrmIpBasic::sendRequest() {
	SnmpRequestCommand command(SnmpType::SNMP_GET, chosenHost->GetHostId());
	Snmp_pp::Oid oid("1.3.6.1.2.1.4.1.0");
	for (int i = 1; i < 20; i++) {
		oid[7] = i;
		command.AddOid(oid.get_printable());
	}
	command.Execute();
}

void FrmIpBasic::updateListCtrl() {
	GetHostOidCommand command(chosenHost->GetHostId());
	Snmp_pp::Oid oid("1.3.6.1.2.1.4.1.0");

	for (int i = 0; i < 19; i++) {
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