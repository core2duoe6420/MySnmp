#include <MySnmp/View/Module.h>
#include <MySnmp/View/FrmListCtrlBase.h>
#include <MySnmp/Command/SnmpRequestCommand.h>
#include <MySnmp/Command/HostCommand.h>

#include <wx/wx.h>

#include <MySnmp/debug.h>

namespace mysnmp {
	class FrmARPTable : public FrmListCtrlBase {
	private:
		int lastRequestId;

		Module * module;
		TopoHost * chosenHost;

		virtual void sendRequest();
		virtual void updateListCtrl();

	public:
		FrmARPTable(ARPTableModule * module);
	};
}

using namespace mysnmp;

void ARPTableModule::OnMenuItemClick(wxCommandEvent& event) {
	FrmARPTable * frmARPTable = new FrmARPTable(this);
	frmARPTable->Show();
}

FrmARPTable::FrmARPTable(ARPTableModule * module) :
FrmListCtrlBase(module->GetCanvas()->GetParent(), "", wxSize(500, 400)), module(module) {
	chosenHost = module->GetCanvas()->GetChosenHost();
	this->SetTitle(chosenHost->GetName() + L" ARP��");

	list->InsertColumn(0, L"��Ӧ�ӿ�����");
	list->InsertColumn(1, L"�����ַ");
	list->InsertColumn(2, L"�����ַ");

	list->SetColumnWidth(0, 100);
	list->SetColumnWidth(1, 160);
	list->SetColumnWidth(2, 160);

	sendRequest();
	updateListCtrl();
}

void FrmARPTable::sendRequest() {
	SnmpRequestCommand command(SnmpType::SNMP_WALK, chosenHost->GetHostId());
	command.AddOid("1.3.6.1.2.1.3");
	lastRequestId = command.Execute();
}

void FrmARPTable::updateListCtrl() {
	GetOidSubtreeCommand command(chosenHost->GetHostId(), lastRequestId);
	command.SetOid("1.3.6.1.2.1.3.1.1");
	command.Execute();
	const std::vector<Snmp_pp::Vb>& vbs = command.GetResults();

	int rows = vbs.size() / 3;
	/* ������ԭ��������������ȫ��ɾ������� */
	if (rows != list->GetItemCount()) {
		list->DeleteAllItems();
		for (int i = 0; i < rows; i++)
			list->InsertItem(i, "");
	}

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < 3; j++) {
			wxString value = vbs[i + rows*j].get_printable_value();
			/* MAC��ַ��Ҫ�����ַ�����Ӧ����SNMP++��bug */
			if (j == 1)
				value = value.Mid(0, 19);
			if (list->GetItemText(i, j) != value)
				list->SetItem(i, j, value);
		}
	}
}
