#include <MySnmp/View/Module.h>
#include <MySnmp/View/FrmListCtrlBase.h>
#include <MySnmp/Command/HostCommand.h>
#include <MySnmp/Command/SnmpRequestCommand.h>

#include <wx/wx.h>

#include <MySnmp/debug.h>

namespace mysnmp {
	class FrmInterface : public FrmSnmpTableBase {
	private:
		virtual void OnListDoubleClick(wxListEvent& event);
	public:
		FrmInterface(InterfaceModule * module) :
			FrmSnmpTableBase(module, "1.3.6.1.2.1.2.2", L" 接口信息", wxSize(800, 400)) {};
	};
}

using namespace mysnmp;

void InterfaceModule::OnMenuItemClick(wxCommandEvent& event) {
	FrmInterface * frmInterface = new FrmInterface(this);
	frmInterface->Show();
}

void FrmInterface::OnListDoubleClick(wxListEvent& event) {
	int rowIndex = event.GetIndex();
	wxString statusStr = list->GetItemText(rowIndex, 6);
	wxString choices[3];
	choices[0] = L"启动";
	choices[1] = L"关闭";
	choices[2] = L"测试中";
	wxSingleChoiceDialog * choiceDialog = new wxSingleChoiceDialog(this,
																   L"请选择新的管理状态",
																   L"修改管理状态",
																   3, choices);
	if (choiceDialog->ShowModal() == wxID_OK) {
		wxString chosenStr = choiceDialog->GetStringSelection();
		if (chosenStr != statusStr) {
			const char * oidpre = "1.3.6.1.2.1.2.2.1.7";
			wxString oidstr, valuestr;
			int ifIndex = wxAtoi(list->GetItemText(rowIndex, 0));
			oidstr.Printf("%s.%d", oidpre, ifIndex);
			int value = choiceDialog->GetSelection() + 1;
			valuestr.Printf("%d", value);
			SnmpRequestCommand command(SnmpType::SNMP_SET, chosenHost->GetHostId());
			command.AddVb(oidstr.mb_str(), valuestr.mb_str());
			command.Execute();
		}
	}
	choiceDialog->Destroy();
}