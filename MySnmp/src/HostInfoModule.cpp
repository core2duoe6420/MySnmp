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
	this->SetTitle(chosenHost->GetName() + L" ������Ϣ");

	list->InsertColumn(0, L"��Ŀ");
	list->InsertColumn(1, L"ֵ");

	list->InsertItem(0, L"ϵͳ��Ϣ", -1);
	list->InsertItem(1, L"��������Ȩ��ʶ��", -1);
	list->InsertItem(2, L"ϵͳ����ʱ��", -1);
	list->InsertItem(3, L"��ϵ��", -1);
	list->InsertItem(4, L"ϵͳ����", -1);
	list->InsertItem(5, L"ϵͳ����ص�", -1);
	list->InsertItem(6, L"ϵͳ����", -1);
	list->SetColumnWidth(0, 150);
	list->SetColumnWidth(1, wxLIST_AUTOSIZE);

	sendRequest();
	updateListCtrl();
}

void FrmHostInfo::OnListDoubleClick(wxListEvent& event) {
	int index = event.GetIndex();
	wxString name = list->GetItemText(index, 0);
	wxString value = list->GetItemText(index, 1);
	if (name == L"ϵͳ����") {
		wxTextEntryDialog * dialog = new wxTextEntryDialog(this, L"�����µ�ϵͳ����",
														   L"�޸�ϵͳ������Ϣ", value, wxOK | wxCANCEL);
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
	/* �����������ַ�������system���л� */
	int pos = strlen(oidstr) - 3;

	for (int i = 0; i < 7; i++) {
		oidstr[pos] = i + 1 + '0';
		command.SetOid(oidstr);
		command.Execute();
		list->SetItem(i, 1, command.GetResult());
	}
	list->SetColumnWidth(1, wxLIST_AUTOSIZE);
}