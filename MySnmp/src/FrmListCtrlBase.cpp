#include <MySnmp/View/FrmListCtrlBase.h>

#include <wx/wx.h>

#include <MySnmp/debug.h>

using namespace mysnmp;

//FrmListCtrlBase

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
	timer->Start(1000);
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

//FrmSnmpTableBase
#include <MySnmp/Command/SnmpRequestCommand.h>
#include <MySnmp/Command/HostCommand.h>

FrmSnmpTableBase::FrmSnmpTableBase(TableModule * module, const char * oidpre, const wxString& caption, wxSize size) :
FrmListCtrlBase(module->GetCanvas()->GetParent(), "", size), module(module), oidpre(oidpre) {
	chosenHost = module->GetCanvas()->GetChosenHost();
	this->SetTitle(chosenHost->GetName() + caption);

	XMLColumnCollection * columns = module->GetColumnCollection();
	for (int i = 0; i < columns->GetColumnCount(); i++) {
		list->InsertColumn(i, columns->GetColumn(i)->GetName());
		list->SetColumnWidth(i, columns->GetColumn(i)->GetSize());
	}
	sendRequest();
	updateListCtrl();
}

void FrmSnmpTableBase::sendRequest() {
	SnmpRequestCommand command(SnmpType::SNMP_WALK, chosenHost->GetHostId());
	command.AddOid(oidpre);
	lastRequestId = command.Execute();
}

void FrmSnmpTableBase::updateListCtrl() {
	GetOidSubtreeCommand command(chosenHost->GetHostId(), lastRequestId);
	command.SetOid(oidpre);
	command.Execute();
	const std::vector<Snmp_pp::Vb>& vbs = command.GetResults();

	int rows = 0, columns;
	for (; rows < vbs.size(); rows++) {
		if (vbs[rows].get_oid()[9] != 1)
			break;
	}
	if (rows != 0)
		columns = vbs.size() / rows;

	if (rows != list->GetItemCount()) {
		list->DeleteAllItems();
		for (int i = 0; i < rows; i++)
			list->InsertItem(i, "");
	}

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < columns; j++) {
			wxString value = vbs[i + rows*j].get_printable_value();
			int colidx = vbs[i + rows*j].get_oid()[9] - 1;
			const ColumnInfo * column = module->GetColumnCollection()->GetColumn(colidx);
			if (column->HasValueMap() && value != "") {
				value = column->MapValueToString(wxAtoi(value));
			} else	if (column->GetName() == L"物理地址") {
				//SNMP++的bug，物理地址会显示多余的奇怪字符串
				value = value.Mid(0, 19);
			} else if (column->GetName() == L"速率") {
				int v = wxAtoi(value) / 1000000;
				value.Printf("%dMbps", v);
			}

			if (list->GetItemText(i, colidx) != value)
				list->SetItem(i, colidx, value);
		}
	}
}