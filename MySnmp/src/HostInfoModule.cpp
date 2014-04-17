#include <MySnmp/View/Module.h>
#include <MySnmp/View/FrmListCtrlBase.h>
#include <MySnmp/Command/SnmpRequestCommand.h>
#include <MySnmp/Command/HostCommand.h>

#include <wx/wx.h>
#include <wx/listctrl.h>

#include <MySnmp/debug.h>

namespace mysnmp {
	class FrmHostInfo : public FrmListCtrlBase {
	private:
		Module * module;
		TopoHost * chosenHost;

		virtual void sendRequest();
		virtual void updateListCtrl();

		virtual void OnListDoubleClick(wxListEvent& event);

	public:
		FrmHostInfo(HostInfoModule * module);
	};
}

using namespace mysnmp;

void HostInfoModule::OnMenuItemClick(wxCommandEvent& event) {
	FrmHostInfo * frmHostInfo = new FrmHostInfo(this);
	frmHostInfo->Show();
}

FrmHostInfo::FrmHostInfo(HostInfoModule * module) :
FrmListCtrlBase(module->GetCanvas()->GetParent(), "", wxSize(500, 300)), module(module) {
	chosenHost = module->GetCanvas()->GetChosenHost();
	this->SetTitle(chosenHost->GetName() + L" 主机信息");

	list->InsertColumn(0, L"项目");
	list->InsertColumn(1, L"值");

	list->InsertItem(0, L"系统信息", -1);
	list->InsertItem(1, L"生产商授权标识符", -1);
	list->InsertItem(2, L"系统启动时间", -1);
	list->InsertItem(3, L"联系人", -1);
	list->InsertItem(4, L"系统名字", -1);
	list->InsertItem(5, L"系统物理地点", -1);
	list->InsertItem(6, L"系统服务", -1);
	list->SetColumnWidth(0, 150);
	list->SetColumnWidth(1, wxLIST_AUTOSIZE);

	sendRequest();
	updateListCtrl();
}

void FrmHostInfo::OnListDoubleClick(wxListEvent& event) {
	int index = event.GetIndex();
	wxString name = list->GetItemText(index, 0);
	wxString value = list->GetItemText(index, 1);
	if (name == L"系统名字") {
		wxTextEntryDialog * dialog = new wxTextEntryDialog(this, L"输入新的系统名字",
														   L"修改系统名字信息", value, wxOK | wxCANCEL);
		if (dialog->ShowModal() == wxID_OK) {
			if (dialog->GetValue() != value) {
				SnmpRequestCommand command(SnmpType::SNMP_SET, chosenHost->GetHostId());
				command.AddVb("1.3.6.1.2.1.1.5.0", dialog->GetValue());
				command.Execute();
			}
		}
	} else {
		wxMessageBox(value, name);
	}
}

void FrmHostInfo::sendRequest() {
	SnmpRequestCommand command(SnmpType::SNMP_GET, chosenHost->GetHostId());
	char oidstr[] = "1.3.6.1.2.1.1.1.0";
	int pos = strlen(oidstr) - 3;
	for (int i = 1; i < 8; i++) {
		oidstr[pos] = i + '0';
		command.AddOid(oidstr);
	}
	command.Execute();
}

void FrmHostInfo::updateListCtrl() {
	GetHostOidCommand command(chosenHost->GetHostId());
	char oidstr[] = "1.3.6.1.2.1.1.1.0";
	/* 倒数第三个字符用于在system下切换 */
	int pos = strlen(oidstr) - 3;

	for (int i = 0; i < 7; i++) {
		oidstr[pos] = i + 1 + '0';
		command.SetOid(oidstr);
		command.Execute();
		list->SetItem(i, 1, command.GetResult());
	}
	list->SetColumnWidth(1, wxLIST_AUTOSIZE);
}