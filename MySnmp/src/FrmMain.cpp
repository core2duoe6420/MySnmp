#include <MySnmp/Host.h>

#include <MySnmp/View/FrmMain.h>
#include <MySnmp/View/TopoCanvas.h>
#include <MySnmp/debug.h>

using namespace mysnmp;

void FrmMain::OnExit(wxCloseEvent& event) {
	this->Destroy();
}

#include <icon.xpm>

FrmMain::FrmMain(const wxString& title, const wxPoint& pos, const wxSize& size) :
	wxFrame(NULL, wxID_ANY, title, pos, size), 
	ID_menuAddHost(wxNewId()), ID_menuExit(wxNewId()), ID_tbAddHost(wxNewId()) {
	menuInitialize();
	toolbarInitialize();
	canvasInitialize();
	eventInitialize();
	this->SetIcon(wxIcon(icon_xpm));
}

void FrmMain::menuInitialize() {
	menuBar = new wxMenuBar();
	menuProgram = new wxMenu();
	menuTool = new wxMenu();
	menuHelp = new wxMenu();
	menuHost = new wxMenu();
	menuProgram->AppendSubMenu(menuHost, L"主机(&H)");
	menuAddHost = new wxMenuItem(menuHost, ID_menuAddHost, L"添加(&A)");
	menuExit = new wxMenuItem(menuProgram, ID_menuExit, L"退出(&E)");
	menuHost->Append(menuAddHost);
	menuProgram->AppendSeparator();
	menuProgram->Append(menuExit);
	menuBar->Append(menuProgram, L"程序(&P)");
	menuBar->Append(menuTool, L"工具(&T)");
	menuBar->Append(menuHelp, L"帮助(&H)");
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
	toolBar->AddTool(ID_tbAddHost, L"创建新主机",
		wxBitmap("image/AddButton.png", wxBITMAP_TYPE_PNG), L"创建新主机");
	toolBar->Realize();
	this->SetToolBar(toolBar);
}

void FrmMain::canvasInitialize() {
	wxBoxSizer * sizer = new wxBoxSizer(wxVERTICAL);
	this->canvas = new TopoCanvas(this, wxSize(1000,1000), 20);
	sizer->Add(canvas, 1, wxGROW, 0);
	this->SetSizer(sizer);
}

void FrmMain::OnAddHostClick(wxCommandEvent& event) {
	this->canvas->DrawBitmap(wxBitmap("image/Host.png", wxBITMAP_TYPE_PNG), wxPoint(50, 50));
}

void FrmMain::OnMenuExitClick(wxCommandEvent& event) {
	this->Close();
}
