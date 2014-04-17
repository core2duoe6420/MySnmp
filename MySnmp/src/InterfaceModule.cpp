#include <MySnmp/View/Module.h>
#include <MySnmp/View/FrmListCtrlBase.h>
#include <MySnmp/Command/HostCommand.h>
#include <MySnmp/Command/SnmpRequestCommand.h>

#include <wx/wx.h>
#include <wx/busyinfo.h>

#include <MySnmp/debug.h>

namespace mysnmp {
	class FrmInterface : public FrmListCtrlBase {
	private:
		int lastRequestId;
		InterfaceModule * module;
		TopoHost * chosenHost;

		int ifNumber;
		//�����������ѭ�������ifNumber��ֵ
		int getIfNumber();
		virtual void sendRequest();
		virtual void updateListCtrl();

		virtual void OnListDoubleClick(wxListEvent& event);

	public:
		FrmInterface(InterfaceModule * module);
	};
}

using namespace mysnmp;

void InterfaceModule::OnMenuItemClick(wxCommandEvent& event) {
	FrmInterface * frmInterface = new FrmInterface(this);
	frmInterface->Show();
}

InterfaceModule::InterfaceModule(const wxString& menuLabel) : Module(menuLabel) {
	int ret = 0;
	this->columnInfos = new XMLColumnCollection("xml/modules/interface.xml", ret);
}

InterfaceModule::~InterfaceModule() {
	delete columnInfos;
}

FrmInterface::FrmInterface(InterfaceModule * module) :
FrmListCtrlBase(module->GetCanvas()->GetParent(), "", wxSize(800, 400)), module(module) {
	chosenHost = module->GetCanvas()->GetChosenHost();
	this->SetTitle(chosenHost->GetName() + L" �ӿ���Ϣ");

	XMLColumnCollection * columns = module->GetColumnCollection();
	for (int i = 0; i < columns->GetColumnCount(); i++) {
		list->InsertColumn(i, columns->GetColumn(i)->GetName());
		list->SetColumnWidth(i, columns->GetColumn(i)->GetSize());
	}

	//�Ȼ�ȡ�ӿ�����
	ifNumber = this->getIfNumber();
	if (ifNumber < 0) {
		wxMessageDialog errorDialog(this, L"�޷���ȡ�ӿ�������Ϣ����������Ϊ����δ��ͨ����û����ӦȨ��",
									L"��ȡ�ӿ���������", wxOK | wxICON_ERROR);
		errorDialog.ShowModal();
		this->Close();
	}
	for (int i = 0; i < ifNumber; i++)
		list->InsertItem(i, "");

	sendRequest();
	updateListCtrl();
}

void FrmInterface::OnListDoubleClick(wxListEvent& event) {
	int ifIndex = event.GetIndex();
	wxString statusStr = list->GetItemText(ifIndex, 6);
	wxString choices[3];
	choices[0] = L"����";
	choices[1] = L"�ر�";
	choices[2] = L"������";
	wxSingleChoiceDialog * choiceDialog = new wxSingleChoiceDialog(this,
																   L"��ѡ���µĹ���״̬",
																   L"�޸Ĺ���״̬",
																   3, choices);
	if (choiceDialog->ShowModal() == wxID_OK) {
		wxString chosenStr = choiceDialog->GetStringSelection();
		if (chosenStr != statusStr) {
			const char * oidpre = "1.3.6.1.2.1.2.2.1.7";
			wxString oidstr, valuestr;
			oidstr.Printf("%s.%d", oidpre, ifIndex + 1);
			int value = choiceDialog->GetSelection() + 1;
			valuestr.Printf("%d", value);
			SnmpRequestCommand command(SnmpType::SNMP_SET, chosenHost->GetHostId());
			command.AddVb(oidstr.mb_str(), valuestr.mb_str());
			command.Execute();
		}
	}
	choiceDialog->Destroy();
}

int FrmInterface::getIfNumber() {
	/* ��ʾһ����ʾ��Ϣ���û��ȴ�
	 * �����˳�����Զ��ر�
	 * д��������Ҳ����ɵ����߸���
	 */
	wxBusyInfo busyInfo(L"���ڻ�ȡ�ӿ�������Ϣ�����Ժ�");
	SnmpRequestCommand requestCommand(SnmpType::SNMP_GET, chosenHost->GetHostId());
	const char * oidstr = "1.3.6.1.2.1.2.1.0";
	requestCommand.AddOid(oidstr);
	requestCommand.Execute();

	GetHostOidCommand resultCommand(chosenHost->GetHostId());
	resultCommand.SetOid(oidstr);

	while (1) {
		int err = resultCommand.Execute();
		if (err == GetHostInfo_NOTREACH) {
			wxSleep(1);
			continue;
		} else if (err != GetHostInfo_SUCCESS) {
			return err;
		}
		return wxAtoi(resultCommand.GetResult());
	}
}

void FrmInterface::sendRequest() {
	SnmpRequestCommand command(SnmpType::SNMP_WALK, chosenHost->GetHostId());
	const char * oidstr = "1.3.6.1.2.1.2.2.1";
	command.AddOid(oidstr);
	//command.SetBulkNonRepeater(0);
	//command.SetBulkMaxRepeater(22 * ifNumber);
	lastRequestId = command.Execute();
}

void FrmInterface::updateListCtrl() {
	GetHostOidCommand command(chosenHost->GetHostId());
	const char * oidpre = "1.3.6.1.2.1.2.2.1";
	/* �еĴ���ӿ��������Ǵ�1��ʼ�ģ������Լ��趨��ֵ
	 * �������õ�����ֵ
	 */
	const char * indexTree = "1.3.6.1.2.1.2.2.1.1";
	GetOidSubtreeCommand indexCommand(chosenHost->GetHostId(), lastRequestId);
	indexCommand.SetOid(oidpre);
	indexCommand.Execute();
	const std::vector<Snmp_pp::Vb>& vbs = indexCommand.GetResults();
	std::vector<int> indics;
	for (int i = 0; i < ifNumber; i++) {
		/* ������һ��ֵ����ֹvbs����Ϊ�� */
		indics.push_back(i + 1);
		if (vbs.size() > i) {
			Snmp_pp::Oid oid = vbs[i].get_oid();
			indics[i] = oid[oid.len() - 1];
		}
	}

	wxString oidstr;
	for (int i = 0; i < ifNumber; i++) {
		for (int j = 0; j < module->GetColumnCollection()->GetColumnCount(); j++) {
			const ColumnInfo * column = module->GetColumnCollection()->GetColumn(j);
			oidstr.Printf("%s.%d.%d", oidpre, j + 1, indics[i]);
			command.SetOid(oidstr);
			int err = command.Execute();
			wxString value;
			if (err == GetHostInfo_SUCCESS && column->HasValueMap())
				value = column->MapValueToString(wxAtoi(command.GetResult()));
			else
				value = command.GetResult();

			if (err == GetHostInfo_SUCCESS) {
				//��һЩ����
				if (column->GetName() == L"�����ַ") {
					//SNMP++��bug�������ַ����ʾ���������ַ���
					value = value.Mid(0, 19);
				} else if (column->GetName() == L"����") {
					int v = wxAtoi(command.GetResult()) / 1000000;
					value.Printf("%dMbps", v);
				}
			}
			if (list->GetItemText(i, j) != value)
				list->SetItem(i, j, value);
		}
	}
}