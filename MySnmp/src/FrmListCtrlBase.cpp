#include <MySnmp/View/FrmListCtrlBase.h>

#include <wx/wx.h>

#include <MySnmp/debug.h>

using namespace mysnmp;

FrmListCtrlBase::FrmListCtrlBase(wxWindow * parent, const wxString& title, wxSize size) :
wxFrame(parent, wxID_ANY, title, wxDefaultPosition, size, wxMINIMIZE_BOX | wxCLOSE_BOX | wxCAPTION) {
	
	this->Center();

	this->SetBackgroundColour(*wxWHITE);
	wxBoxSizer * topSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer * buttonSizer = new wxBoxSizer(wxHORIZONTAL);
	list = new wxListCtrl(this, wxID_ANY, wxDefaultPosition,
							wxDefaultSize, wxLC_REPORT);
	topSizer->Add(list, 1, wxEXPAND);
	topSizer->Add(buttonSizer, 0, wxFIXED_MINSIZE | wxALIGN_RIGHT);

	btnRefresh = new wxButton(this, wxID_ANY, L"刷新");
	buttonSizer->Add(btnRefresh, 0, wxALL, 10);
	btnResend = new wxButton(this, wxID_ANY, L"重发");
	buttonSizer->Add(btnResend, 0, wxALL, 10);
	btnClose = new wxButton(this, wxID_CANCEL, L"关闭");
	buttonSizer->Add(btnClose, 0, wxALL, 10);

	timer = new wxTimer(this, wxID_ANY);
	timer->Start(2000);
	this->SetSizer(topSizer);
	btnRefresh->SetDefault();
	this->eventInitialize();
}

void FrmListCtrlBase::eventInitialize() {
	this->Bind(wxEVT_BUTTON, &FrmListCtrlBase::OnCloseClick, this, btnClose->GetId());
	this->Bind(wxEVT_BUTTON, &FrmListCtrlBase::OnRefreshClick, this, btnRefresh->GetId());
	this->Bind(wxEVT_BUTTON, &FrmListCtrlBase::OnResendClick, this, btnResend->GetId());
	this->Bind(wxEVT_TIMER, &FrmListCtrlBase::OnTimer, this, timer->GetId());
	this->Bind(wxEVT_CLOSE_WINDOW, &FrmListCtrlBase::OnClose, this);
	this->Bind(wxEVT_LIST_ITEM_ACTIVATED, &FrmListCtrlBase::OnListDoubleClick, this, list->GetId());
}

void FrmListCtrlBase::OnResendClick(wxCommandEvent& event) {
	this->sendRequest();
}

void FrmListCtrlBase::OnCloseClick(wxCommandEvent& event) {
	this->Close();
}

void FrmListCtrlBase::OnRefreshClick(wxCommandEvent& event) {
	this->updateListCtrl();
}

void FrmListCtrlBase::OnTimer(wxTimerEvent& event) {
	this->updateListCtrl();
}

void FrmListCtrlBase::OnClose(wxCloseEvent& event) {
	timer->Stop();
	delete timer;
	this->Destroy();
}

void FrmListCtrlBase::OnListDoubleClick(wxListEvent& event) {}