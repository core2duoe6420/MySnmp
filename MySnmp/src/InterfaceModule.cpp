#include <MySnmp/View/Module.h>
#include <MySnmp/Command/HostCommand.h>
#include <MySnmp/Command/SnmpRequestCommand.h>

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/busyinfo.h>

#include <MySnmp/debug.h>

namespace mysnmp {
	class FrmInterface : public wxFrame {
	private:
		int lastRequestId;
		InterfaceModule * module;
		TopoHost * chosenHost;

		wxListCtrl * iflist;
		wxButton * btnRefresh;
		wxButton * btnResend;
		wxButton * btnClose;
		wxTimer * timer;

		int ifNumber;
		//这个函数会死循环至获得ifNumber的值
		int getIfNumber();
		void sendInterfaceRequest();
		void updateListCtrl();
		void eventInitialize();

		void OnResendClick(wxCommandEvent& event);
		void OnCloseClick(wxCommandEvent& event);
		void OnRefreshClick(wxCommandEvent& event);
		void OnTimer(wxTimerEvent& event);
		void OnClose(wxCloseEvent& event);
		void OnListDoubleClick(wxListEvent& event);

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
wxFrame(module->GetCanvas()->GetParent(), wxID_ANY, "", wxDefaultPosition, wxSize(800, 400),
wxMINIMIZE_BOX | wxCLOSE_BOX | wxCAPTION),
module(module) {
	this->Center();
	chosenHost = module->GetCanvas()->GetChosenHost();
	this->SetTitle(chosenHost->GetName() + L" 接口信息");

	this->SetBackgroundColour(*wxWHITE);
	wxBoxSizer * topSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer * buttonSizer = new wxBoxSizer(wxHORIZONTAL);
	iflist = new wxListCtrl(this, wxID_ANY, wxDefaultPosition,
							wxDefaultSize, wxLC_REPORT);
	topSizer->Add(iflist, 1, wxEXPAND);
	topSizer->Add(buttonSizer, 0, wxFIXED_MINSIZE | wxALIGN_RIGHT);

	btnRefresh = new wxButton(this, wxID_ANY, L"刷新");
	buttonSizer->Add(btnRefresh, 0, wxALL, 10);
	btnResend = new wxButton(this, wxID_ANY, L"重发");
	buttonSizer->Add(btnResend, 0, wxALL, 10);
	btnClose = new wxButton(this, wxID_CANCEL, L"关闭");
	buttonSizer->Add(btnClose, 0, wxALL, 10);

	int ret;
	XMLColumnCollection * columns = module->GetColumnCollection();
	for (int i = 0; i < columns->GetColumnCount(); i++) {
		iflist->InsertColumn(i, columns->GetColumn(i)->GetName());
		iflist->SetColumnWidth(i, columns->GetColumn(i)->GetSize());
	}

	timer = new wxTimer(this, wxID_ANY);
	timer->Start(2000);
	this->SetSizer(topSizer);
	btnRefresh->SetDefault();
	this->eventInitialize();

	//先获取接口数量
	ifNumber = this->getIfNumber();
	if (ifNumber < 0) {
		wxMessageDialog errorDialog(this, L"无法获取接口数量信息，可能是因为主机未联通，或没有相应权限",
									L"获取接口数量出错", wxOK | wxICON_ERROR);
		errorDialog.ShowModal();
		this->Close();
	}
	for (int i = 0; i < ifNumber; i++)
		iflist->InsertItem(i, "");
	sendInterfaceRequest();
	updateListCtrl();
}

void FrmInterface::OnListDoubleClick(wxListEvent& event) {
	int ifIndex = event.GetIndex();
	wxString statusStr = iflist->GetItemText(ifIndex, 6);
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

void FrmInterface::OnCloseClick(wxCommandEvent& event) {
	this->Close();
}

void FrmInterface::OnRefreshClick(wxCommandEvent& event) {
	this->updateListCtrl();
}

void FrmInterface::OnResendClick(wxCommandEvent& event) {
	this->sendInterfaceRequest();
}

void FrmInterface::OnTimer(wxTimerEvent& event) {
	this->updateListCtrl();
}

void FrmInterface::OnClose(wxCloseEvent& event) {
	timer->Stop();
	delete timer;
	this->Destroy();
}

void FrmInterface::eventInitialize() {
	this->Bind(wxEVT_BUTTON, &FrmInterface::OnCloseClick, this, btnClose->GetId());
	this->Bind(wxEVT_BUTTON, &FrmInterface::OnRefreshClick, this, btnRefresh->GetId());
	this->Bind(wxEVT_BUTTON, &FrmInterface::OnResendClick, this, btnResend->GetId());
	this->Bind(wxEVT_TIMER, &FrmInterface::OnTimer, this, timer->GetId());
	this->Bind(wxEVT_CLOSE_WINDOW, &FrmInterface::OnClose, this);
	this->Bind(wxEVT_LIST_ITEM_ACTIVATED, &FrmInterface::OnListDoubleClick, this, iflist->GetId());
}

int FrmInterface::getIfNumber() {
	/* 显示一个提示信息让用户等待
	 * 函数退出后会自动关闭
	 * 写在这里而且不是由调用者负责
	 */
	wxBusyInfo busyInfo(L"正在获取接口数量信息，请稍后");
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

void FrmInterface::sendInterfaceRequest() {
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
	/* 有的代理接口索引不是从1开始的，而是自己设定的值
	 * 所以先拿到索引值
	 */
	const char * indexTree = "1.3.6.1.2.1.2.2.1.1";
	GetOidSubtreeCommand indexCommand(chosenHost->GetHostId(), lastRequestId);
	indexCommand.SetOid(oidpre);
	const std::vector<Snmp_pp::Vb>& vbs = indexCommand.GetResults();
	std::vector<int> indics;
	for (int i = 0; i < ifNumber; i++) {
		/* 先设置一个值，防止vbs可能为空 */
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
				//做一些调整
				if (column->GetName() == L"物理地址") {
					//SNMP++的bug，物理地址会显示多余的奇怪字符串
					value = value.Mid(0, 19);
				} else if (column->GetName() == L"速率") {
					int v = wxAtoi(command.GetResult()) / 1000000;
					value.Printf("%dMbps", v);
				}
			}
			if (iflist->GetItemText(i, j) != value)
				iflist->SetItem(i, j, value);
		}
	}
}