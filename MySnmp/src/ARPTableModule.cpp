#include <MySnmp/View/Module.h>

#include <MySnmp/Command/SnmpRequestCommand.h>
#include <MySnmp/Command/HostCommand.h>

#include <wx/wx.h>
#include <wx/listctrl.h>

#include <MySnmp/debug.h>

namespace mysnmp {
	class FrmARPTable : public wxFrame {
	private:
		int lastRequestId;

		Module * module;
		TopoHost * chosenHost;

		wxListCtrl * arptable;
		wxButton * btnRefresh;
		wxButton * btnClose;
		wxTimer * timer;

		void sendARPTableRequest();
		void updateListCtrl();
		void eventInitialize();

		void OnCloseClick(wxCommandEvent& event);
		void OnRefreshClick(wxCommandEvent& event);
		void OnTimer(wxTimerEvent& event);
		void OnClose(wxCloseEvent& event);

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
wxFrame(module->GetCanvas()->GetParent(), wxID_ANY, "", wxDefaultPosition, wxSize(500, 400),
wxMINIMIZE_BOX | wxCLOSE_BOX | wxCAPTION),
module(module) {
	this->Center();
	chosenHost = module->GetCanvas()->GetChosenHost();
	this->SetTitle(chosenHost->GetName() + L" ARP表");

	sendARPTableRequest();
	this->SetBackgroundColour(*wxWHITE);
	wxBoxSizer * topSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer * buttonSizer = new wxBoxSizer(wxHORIZONTAL);
	arptable = new wxListCtrl(this, wxID_ANY, wxDefaultPosition,
							  wxDefaultSize, wxLC_REPORT);
	topSizer->Add(arptable, 1, wxEXPAND);
	topSizer->Add(buttonSizer, 0, wxFIXED_MINSIZE | wxALIGN_RIGHT);

	btnRefresh = new wxButton(this, wxID_ANY, L"刷新");
	buttonSizer->Add(btnRefresh, 0, wxALL, 10);
	btnClose = new wxButton(this, wxID_CANCEL, L"关闭");
	buttonSizer->Add(btnClose, 0, wxALL, 10);

	arptable->InsertColumn(0, L"相应接口索引");
	arptable->InsertColumn(1, L"物理地址");
	arptable->InsertColumn(2, L"网络地址");

	arptable->SetColumnWidth(0, 100);
	arptable->SetColumnWidth(1, 160);
	arptable->SetColumnWidth(2, 160);
	updateListCtrl();

	timer = new wxTimer(this, wxID_ANY);
	timer->Start(1000);
	this->SetSizer(topSizer);
	btnRefresh->SetDefault();
	this->eventInitialize();
}

void FrmARPTable::OnCloseClick(wxCommandEvent& event) {
	this->Close();
}

void FrmARPTable::OnRefreshClick(wxCommandEvent& event) {
	this->sendARPTableRequest();
}

void FrmARPTable::OnTimer(wxTimerEvent& event) {
	this->updateListCtrl();
}

void FrmARPTable::OnClose(wxCloseEvent& event) {
	timer->Stop();
	delete timer;
	this->Destroy();
}

void FrmARPTable::eventInitialize() {
	this->Bind(wxEVT_BUTTON, &FrmARPTable::OnCloseClick, this, btnClose->GetId());
	this->Bind(wxEVT_BUTTON, &FrmARPTable::OnRefreshClick, this, btnRefresh->GetId());
	this->Bind(wxEVT_TIMER, &FrmARPTable::OnTimer, this, timer->GetId());
	this->Bind(wxEVT_CLOSE_WINDOW, &FrmARPTable::OnClose, this);
}

void FrmARPTable::sendARPTableRequest() {
	SnmpRequestCommand command(SnmpType::SNMP_WALK, chosenHost->GetHostId());
	command.AddOid("1.3.6.1.2.1.3");
	lastRequestId = command.Execute();
}

void FrmARPTable::updateListCtrl() {
	arptable->DeleteAllItems();
	GetOidSubtreeCommand command(chosenHost->GetHostId(), lastRequestId);
	command.SetOid("1.3.6.1.2.1.3.1.1");
	command.Execute();
	const std::vector<Snmp_pp::Vb>& vbs = command.GetResults();

	int rows = vbs.size() / 3;
	for (int i = 0; i < rows; i++) {
		arptable->InsertItem(i, vbs[i].get_printable_value());
		arptable->SetItem(i, 1, wxString(vbs[i + rows].get_printable_value()).Mid(0, 19));
		arptable->SetItem(i, 2, vbs[i + rows * 2].get_printable_value());
	}
}
