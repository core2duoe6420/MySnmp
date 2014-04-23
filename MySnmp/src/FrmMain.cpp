#include <MySnmp/Host.h>

#include <MySnmp/View/FrmMain.h>
#include <MySnmp/View/TopoCanvas.h>
#include <MySnmp/View/DiaHostInfo.h>
#include <MySnmp/View/Module.h>
#include <MySnmp/Command/HostCommand.h>

#include <wx/filedlg.h>
#include <wx/sizer.h>
#include <wx/msgdlg.h>

#include <MySnmp/debug.h>

using namespace mysnmp;

int FrmMain::showAskSaveDialog() {
	wxMessageDialog askDialog(this, L"拓扑图已修改，是否保存？", L"是否保存",
							  wxYES_NO | wxCANCEL | wxYES_DEFAULT | wxICON_INFORMATION);
	return askDialog.ShowModal();
}

void FrmMain::OnClose(wxCloseEvent& event) {
	if (canvas->IsChanged()) {
		switch (showAskSaveDialog()) {
		case wxID_CANCEL:
			event.Veto();
			return;
		case wxID_YES:
			if (canvas->HasSavedToFile() == false)
				saveTopoCanvas();
			else
				canvas->SaveToXml(canvas->GetXmlFile());
			break;
		case wxID_NO:
			break;
		}
	}
	this->Destroy();
}

#include <icon.xpm>

FrmMain::FrmMain(const wxString& title, const wxPoint& pos, const wxSize& size) :
wxFrame(NULL, wxID_ANY, title, pos, size), ID_tbAddHost(wxNewId()), ID_tbSaveCanvas(wxNewId()),
ID_tbLoadCanvas(wxNewId()) {
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
	menuProgram->AppendSubMenu(menuHost, L"主机(&H)");
	menuAddHost = new wxMenuItem(menuHost, wxID_ANY, L"添加(&A)");
	menuExit = new wxMenuItem(menuProgram, wxID_ANY, L"退出(&E)");
	menuSaveCanvas = new wxMenuItem(menuProgram, wxID_ANY, L"保存拓扑(&S)");
	menuSaveCanvasAs = new wxMenuItem(menuProgram, wxID_ANY, L"另存为拓扑(&D)");
	menuLoadCanvas = new wxMenuItem(menuProgram, wxID_ANY, L"打开拓扑(&O)");
	menuHost->Append(menuAddHost);
	menuProgram->AppendSeparator();
	menuProgram->Append(menuSaveCanvas);
	menuProgram->Append(menuSaveCanvasAs);
	menuProgram->Append(menuLoadCanvas);
	menuProgram->AppendSeparator();
	menuProgram->Append(menuExit);
	menuBar->Append(menuProgram, L"程序(&P)");
	menuBar->Append(menuTool, L"工具(&T)");
	menuBar->Append(menuHelp, L"帮助(&H)");
	this->SetMenuBar(menuBar);
}

void FrmMain::eventInitialize() {
	//this->Bind(wxEVT_SIZE, &FrmMain::OnSize, this);
	this->Bind(wxEVT_CLOSE_WINDOW, &FrmMain::OnClose, this);
	this->Bind(wxEVT_MENU, &FrmMain::OnMenuExitClick, this, menuExit->GetId());
	this->Bind(wxEVT_MENU, &FrmMain::OnAddHostClick, this, menuAddHost->GetId());
	this->Bind(wxEVT_TOOL, &FrmMain::OnAddHostClick, this, ID_tbAddHost);
	this->Bind(wxEVT_TOOL, &FrmMain::OnSaveCanvasClick, this, ID_tbSaveCanvas);
	this->Bind(wxEVT_TOOL, &FrmMain::OnLoadCanvasClick, this, ID_tbLoadCanvas);
	this->Bind(wxEVT_MENU, &FrmMain::OnSaveCanvasClick, this, menuSaveCanvas->GetId());
	this->Bind(wxEVT_MENU, &FrmMain::OnLoadCanvasClick, this, menuLoadCanvas->GetId());
	this->Bind(wxEVT_MENU, &FrmMain::OnSaveCanvasAsClick, this, menuSaveCanvasAs->GetId());
}

void FrmMain::toolbarInitialize() {
	toolBar = this->CreateToolBar();
	toolBar->AddTool(ID_tbAddHost, L"创建新主机",
					 wxBitmap("image/AddButton.png", wxBITMAP_TYPE_PNG), L"创建新主机");
	toolBar->AddTool(ID_tbLoadCanvas, L"打开拓扑",
					 wxBitmap("image/OpenButton.png", wxBITMAP_TYPE_PNG), L"打开拓扑");
	toolBar->AddTool(ID_tbSaveCanvas, L"保存拓扑",
					 wxBitmap("image/SaveButton.png", wxBITMAP_TYPE_PNG), L"保存拓扑");

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
	HostInfoModule * hostInfoModule = new HostInfoModule(L"显示主机信息");
	ModuleManager::RegisterModule(hostInfoModule);
	InterfaceModule * interfaceModule = new InterfaceModule(L"接口信息");
	ModuleManager::RegisterModule(interfaceModule);
	IpBasicModule * ipBasicModule = new IpBasicModule(L"IP基本信息");
	ModuleManager::RegisterModule(ipBasicModule);
	IpAddrTableModule * ipAddrTableModule = new IpAddrTableModule(L"IP地址表");
	ModuleManager::RegisterModule(ipAddrTableModule);
	IpRouteTableModule * ipRouteTableModule = new IpRouteTableModule(L"IP路由表");
	ModuleManager::RegisterModule(ipRouteTableModule);
	IpTransTableModule * ipTransTableModule = new IpTransTableModule(L"IP转换表");
	ModuleManager::RegisterModule(ipTransTableModule);
	TcpBasicModule * tcpBasicModule = new TcpBasicModule(L"TCP统计数据");
	ModuleManager::RegisterModule(tcpBasicModule);
	TcpConnTableModule * tcpConnTableModule = new TcpConnTableModule(L"TCP连接表");
	ModuleManager::RegisterModule(tcpConnTableModule);
	ModifyHostModule * modifyInfoModule = new ModifyHostModule(L"编辑主机");
	ModuleManager::RegisterModule(modifyInfoModule);
	DeleteHostModule * deleteInfoModule = new DeleteHostModule(L"删除主机");
	ModuleManager::RegisterModule(deleteInfoModule);
}

void FrmMain::OnAddHostClick(wxCommandEvent& event) {
	DiaHostInfo * diaHostInfo = new DiaHostInfo(DiaHostInfo::HOST_ADD, this, L"添加新主机");
	if (diaHostInfo->ShowModal() == wxID_OK) {
		AddHostCommand command;
		command.SetRetryTimes(diaHostInfo->GetRetryTimes());
		command.SetTimeout(diaHostInfo->GetTimeout());
		command.SetIpAddress(diaHostInfo->GetIpAddress());
		command.SetReadCommunity(diaHostInfo->GetReadCommunity());
		command.SetWriteCommunity(diaHostInfo->GetWriteCommunity());
		command.SetUdpPort(diaHostInfo->GetUDPPort());
		command.SetSnmpVersion(diaHostInfo->GetVersion());
		int hostId = command.Execute();
		this->canvas->DrawBitmap(hostId, wxBitmap("image/Host.png", wxBITMAP_TYPE_PNG),
								 wxPoint(50, 50), diaHostInfo->GetIpAddress(), diaHostInfo->GetName());
	}
	diaHostInfo->Destroy();
}

void FrmMain::OnMenuExitClick(wxCommandEvent& event) {
	this->Close();
}

void FrmMain::OnSaveCanvasClick(wxCommandEvent& event) {
	/* 没有保存过，相当于另存为 */
	if (canvas->HasSavedToFile() == false) {
		saveTopoCanvas();
		return;
	}
	/* 保存过了，但没变动过，直接返回 */
	if (canvas->IsChanged() == false)
		return;

	/* 保存过了并且变动过，直接保存为原文件 */
	canvas->SaveToXml(canvas->GetXmlFile());
}

void FrmMain::OnSaveCanvasAsClick(wxCommandEvent& event) {
	saveTopoCanvas();
}

#ifdef _WIN32
#define SLASH "\\"
#else
#define SLASH "/"
#endif

void FrmMain::saveTopoCanvas() {
	wxFileDialog dialog(this, L"保存拓扑文件", wxEmptyString, "topo.xml",
						L"拓扑XML文件（*.xml)|*.xml", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	if (dialog.ShowModal() == wxID_OK) {
		wxString xmlpath = dialog.GetDirectory() + SLASH + dialog.GetFilename();
		canvas->SaveToXml(xmlpath.mb_str());
		this->SetTitle("MySnmp - " + xmlpath);
	}
}

void FrmMain::OnLoadCanvasClick(wxCommandEvent& event) {
	if (canvas->IsChanged()) {
		switch (showAskSaveDialog()) {
		case wxID_CANCEL:return;
		case wxID_YES:
			if (canvas->HasSavedToFile() == false)
				saveTopoCanvas();
			else
				canvas->SaveToXml(canvas->GetXmlFile());
			break;
		case wxID_NO:
			break;
		}
	}
	wxFileDialog dialog(this, L"打开拓扑文件", wxEmptyString, "topo.xml",
						L"拓扑XML文件（*.xml)|*.xml", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	if (dialog.ShowModal() == wxID_OK) {
		wxString xmlpath = dialog.GetDirectory() + SLASH + dialog.GetFilename();
		canvas->LoadFromXml(xmlpath.mb_str());
		this->SetTitle("MySnmp - " + xmlpath);
	}
}