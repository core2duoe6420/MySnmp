#include <MySnmp/Host.h>

#include <MySnmp/View/FrmMain.h>
#include <MySnmp/View/TopoCanvas.h>
#include <MySnmp/View/DiaAddHost.h>
#include <MySnmp/View/HostInfoModule.h>
#include <MySnmp/Command/HostCommand.h>

#include <MySnmp/debug.h>

using namespace mysnmp;

void FrmMain::OnExit(wxCloseEvent& event) {
	this->Destroy();
}

#include <icon.xpm>

FrmMain::FrmMain(const wxString& title, const wxPoint& pos, const wxSize& size) :
wxFrame(NULL, wxID_ANY, title, pos, size),
ID_menuAddHost(wxNewId()), ID_menuExit(wxNewId()), ID_tbAddHost(wxNewId()) {
	this->Center();
	menuInitialize();
	toolbarInitialize();
	canvasInitialize();
	eventInitialize();
	modulesInitialize();
	this->SetIcon(wxIcon(icon_xpm));
}

void FrmMain::menuInitialize() {
	wxMenuBar * menuBar = new wxMenuBar();
	wxMenu * menuProgram = new wxMenu();
	wxMenu * menuHost = new wxMenu();
	wxMenu * menuTool = new wxMenu();
	wxMenu * menuHelp = new wxMenu();
	menuProgram->AppendSubMenu(menuHost, L"����(&H)");
	menuAddHost = new wxMenuItem(menuHost, ID_menuAddHost, L"���(&A)");
	menuExit = new wxMenuItem(menuProgram, ID_menuExit, L"�˳�(&E)");
	menuHost->Append(menuAddHost);
	menuProgram->AppendSeparator();
	menuProgram->Append(menuExit);
	menuBar->Append(menuProgram, L"����(&P)");
	menuBar->Append(menuTool, L"����(&T)");
	menuBar->Append(menuHelp, L"����(&H)");
	this->SetMenuBar(menuBar);
}

void FrmMain::eventInitialize() {
	//this->Bind(wxEVT_SIZE, &FrmMain::OnSize, this);
	this->Bind(wxEVT_CLOSE_WINDOW, &FrmMain::OnExit, this);
	this->Bind(wxEVT_MENU, &FrmMain::OnMenuExitClick, this, this->ID_menuExit);
	this->Bind(wxEVT_MENU, &FrmMain::OnAddHostClick, this, this->ID_menuAddHost);
	this->Bind(wxEVT_TOOL, &FrmMain::OnAddHostClick, this, this->ID_tbAddHost);
}

void FrmMain::toolbarInitialize() {
	toolBar = this->CreateToolBar();
	toolBar->AddTool(ID_tbAddHost, L"����������",
					 wxBitmap("image/AddButton.png", wxBITMAP_TYPE_PNG), L"����������");
	toolBar->Realize();
	this->SetToolBar(toolBar);
}

void FrmMain::canvasInitialize() {
	wxBoxSizer * sizer = new wxBoxSizer(wxVERTICAL);
	this->canvas = new TopoCanvas(this, wxSize(1000, 1000), 20);
	sizer->Add(canvas, 1, wxGROW, 0);
	this->SetSizer(sizer);
}

void FrmMain::modulesInitialize() {
	ModuleManager::SetTopoCanvas(canvas);
	HostInfoModule * hostInfoModule = new HostInfoModule(L"��ʾ������Ϣ");
	ModuleManager::RegisterModule(hostInfoModule);
}

void FrmMain::OnAddHostClick(wxCommandEvent& event) {
	DiaAddHost * diaAddHost = new DiaAddHost(this, L"���������");
	if (diaAddHost->ShowModal() == wxID_OK) {
		AddHostCommand command;
		command.SetRetryTimes(diaAddHost->GetRetryTimes());
		command.SetTimeout(diaAddHost->GetTimeout());
		command.SetIpAddress(diaAddHost->GetIpAddress());
		command.SetReadCommunity(diaAddHost->GetReadCommunity());
		command.SetWriteCommunity(diaAddHost->GetWriteCommunity());
		command.SetUdpPort(diaAddHost->GetUDPPort());
		command.SetSnmpVersion(diaAddHost->GetVersion());
		int hostId = command.Execute();
		this->canvas->DrawBitmap(hostId, wxBitmap("image/Host.png", wxBITMAP_TYPE_PNG),
								 wxPoint(50, 50), diaAddHost->GetIpAddress(), diaAddHost->GetName());
	}

	diaAddHost->Destroy();

}

void FrmMain::OnMenuExitClick(wxCommandEvent& event) {
	this->Close();
}
