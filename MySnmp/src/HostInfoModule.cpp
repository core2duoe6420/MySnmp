#include <MySnmp/View/Module.h>
#include <MySnmp/Command/SnmpRequestCommand.h>
#include <MySnmp/Command/HostCommand.h>

#include <wx/listctrl.h>

#include <MySnmp/debug.h>

namespace mysnmp {
	class FrmHostInfo : public wxFrame {
	private:
		Module * module;
		TopoHost * chosenHost;

		wxListCtrl * infolist;
		wxButton * btnRefresh;
		wxButton * btnClose;
		wxTimer * timer;

		void sendHostInfoRequest();
		void updateListCtrl();
		void eventInitialize();

		void OnCloseClick(wxCommandEvent& event);
		void OnRefreshClick(wxCommandEvent& event);
		void OnTimer(wxTimerEvent& event);
		void OnClose(wxCloseEvent& event);
		void OnListDoubleClick(wxListEvent& event);

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
wxFrame(module->GetCanvas()->GetParent(), wxID_ANY, "", wxDefaultPosition, wxSize(500, 300),
wxMINIMIZE_BOX | wxCLOSE_BOX | wxCAPTION),
module(module) {
	this->Center();
	chosenHost = module->GetCanvas()->GetChosenHost();
	this->SetTitle(chosenHost->GetName() + L" 主机信息");
	
	/* 先发送请求说不定在界面布局完成后结果已经抵达 */
	sendHostInfoRequest();
	this->SetBackgroundColour(*wxWHITE);
	wxBoxSizer * topSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer * buttonSizer = new wxBoxSizer(wxHORIZONTAL);
	infolist = new wxListCtrl(this, wxID_ANY, wxDefaultPosition,
							  wxDefaultSize, wxLC_REPORT);
	topSizer->Add(infolist, 1, wxEXPAND);
	topSizer->Add(buttonSizer, 0, wxFIXED_MINSIZE | wxALIGN_RIGHT);

	btnRefresh = new wxButton(this, wxID_ANY, L"刷新");
	buttonSizer->Add(btnRefresh, 0, wxALL, 10);
	btnClose = new wxButton(this, wxID_CANCEL, L"关闭");
	buttonSizer->Add(btnClose, 0, wxALL, 10);

	infolist->InsertColumn(0, L"项目");
	infolist->InsertColumn(1, L"值");

	infolist->InsertItem(0, L"系统信息", -1);
	infolist->InsertItem(1, L"生产商授权标识符", -1);
	infolist->InsertItem(2, L"系统启动时间", -1);
	infolist->InsertItem(3, L"联系人", -1);
	infolist->InsertItem(4, L"系统名字", -1);
	infolist->InsertItem(5, L"系统物理地点", -1);
	infolist->InsertItem(6, L"系统服务", -1);
	infolist->SetColumnWidth(0, 150);
	infolist->SetColumnWidth(1, wxLIST_AUTOSIZE);
	updateListCtrl();

	timer = new wxTimer(this, wxID_ANY);
	timer->Start(1000);
	this->SetSizer(topSizer);
	btnRefresh->SetDefault();
	this->eventInitialize();
}

void FrmHostInfo::OnListDoubleClick(wxListEvent& event) {
	int index = event.GetIndex();
	wxString name = infolist->GetItemText(index, 0);
	wxString value = infolist->GetItemText(index, 1);
	wxMessageBox(value, name);
}

void FrmHostInfo::OnCloseClick(wxCommandEvent& event) {
	this->Close();
}

void FrmHostInfo::OnRefreshClick(wxCommandEvent& event) {
	this->sendHostInfoRequest();
}

void FrmHostInfo::OnTimer(wxTimerEvent& event) {
	this->updateListCtrl();
}

void FrmHostInfo::OnClose(wxCloseEvent& event) {
	timer->Stop();
	delete timer;
	this->Destroy();
}


void FrmHostInfo::eventInitialize() {
	this->Bind(wxEVT_BUTTON, &FrmHostInfo::OnCloseClick, this, btnClose->GetId());
	this->Bind(wxEVT_BUTTON, &FrmHostInfo::OnRefreshClick, this, btnRefresh->GetId());
	this->Bind(wxEVT_TIMER, &FrmHostInfo::OnTimer, this, timer->GetId());
	this->Bind(wxEVT_CLOSE_WINDOW, &FrmHostInfo::OnClose, this);
	this->Bind(wxEVT_LIST_ITEM_ACTIVATED, &FrmHostInfo::OnListDoubleClick, this, infolist->GetId());
}

void FrmHostInfo::sendHostInfoRequest() {
	SnmpRequestCommand command(SnmpType::get, chosenHost->GetHostId());
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
		infolist->SetItem(i, 1, command.GetResult());
	}
	infolist->SetColumnWidth(1, wxLIST_AUTOSIZE);
}