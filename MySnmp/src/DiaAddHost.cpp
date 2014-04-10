#include <libsnmp.h>
#include <snmp_pp/snmp_pp.h>
#include <MySnmp/View/FrmMain.h>
#include <MySnmp/View/TopoCanvas.h>
#include <MySnmp/View/DiaAddHost.h>

#include <MySnmp/debug.h>

using namespace mysnmp;

DiaAddHost::DiaAddHost(wxWindow * parent, const wxString& title) :
wxDialog(parent, wxID_ANY, title, wxDefaultPosition, wxSize(600, 430)) {
	this->Center();
	wxBoxSizer * topSizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer * rightSizer = new wxBoxSizer(wxVERTICAL);
	wxStaticBoxSizer * labelSizer = new wxStaticBoxSizer(wxHORIZONTAL, this, L"主机信息");
	wxFlexGridSizer * infoSizer = new wxFlexGridSizer(8, 2, 10, 10);
	wxBoxSizer * buttonSizer = new wxBoxSizer(wxHORIZONTAL);

	wxStaticBitmap * hostBitmap = new wxStaticBitmap(this, wxID_ANY, wxBitmap("image/Host.png", wxBITMAP_TYPE_PNG));
	topSizer->Add(hostBitmap, 0, wxLEFT | wxTOP, 30);
	topSizer->Add(rightSizer, 1, wxALL | wxEXPAND, 20);
	rightSizer->Add(labelSizer, wxEXPAND);
	labelSizer->Add(infoSizer, 1, wxALL | wxEXPAND, 5);

	//infoSizer->AddGrowableCol(1, 1);

	/* Labels */
	wxStaticText * lblName = new wxStaticText(this, wxID_ANY, L"主机名称：");
	wxFont font = lblName->GetFont();
	font.SetPointSize(12);
	wxStaticText * lblReadCommunity = new wxStaticText(this, wxID_ANY, L"读取共同体：");
	wxStaticText * lblWriteCommunity = new wxStaticText(this, wxID_ANY, L"写入共同体：");
	wxStaticText * lblRetryTimes = new wxStaticText(this, wxID_ANY, L"重试次数：");
	wxStaticText * lblVersion = new wxStaticText(this, wxID_ANY, L"SNMP版本：");
	wxStaticText * lblTimeout = new wxStaticText(this, wxID_ANY, L"超时：");
	wxStaticText * lblUDPPort = new wxStaticText(this, wxID_ANY, L"UDP端口：");
	wxStaticText * lblIpAddress = new wxStaticText(this, wxID_ANY, L"IP地址：");
	lblName->SetFont(font);
	lblWriteCommunity->SetFont(font);
	lblReadCommunity->SetFont(font);
	lblRetryTimes->SetFont(font);
	lblVersion->SetFont(font);
	lblTimeout->SetFont(font);
	lblUDPPort->SetFont(font);
	lblIpAddress->SetFont(font);

	wxArrayString strsTimeout;
	strsTimeout.Add(L"1秒");
	strsTimeout.Add(L"2秒");
	strsTimeout.Add(L"3秒");
	strsTimeout.Add(L"4秒");
	strsTimeout.Add(L"5秒");

	wxArrayString strsRetry;
	strsRetry.Add("0");
	strsRetry.Add("1");
	strsRetry.Add("2");
	strsRetry.Add("3");
	strsRetry.Add("4");

	wxArrayString strsVersion;
	strsVersion.Add("version1");
	strsVersion.Add("version2c");
	//strsVersion.Add("version3");

	cbRetry = new wxComboBox(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(250, 25), strsRetry, wxCB_READONLY);
	cbTimeout = new wxComboBox(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(250, 25), strsTimeout, wxCB_READONLY);
	cbVersion = new wxComboBox(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(250, 25), strsVersion, wxCB_READONLY);
	/* TextBox */
	txtName = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(250, 25));
	txtReadCommunity = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(250, 25));
	txtWriteCommunity = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(250, 25));
	txtUDPPort = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(250, 25));
	txtIpAddress = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(250, 25));

	infoSizer->Add(lblName);
	infoSizer->Add(txtName);
	infoSizer->Add(lblIpAddress);
	infoSizer->Add(txtIpAddress);
	infoSizer->Add(lblUDPPort);
	infoSizer->Add(txtUDPPort);
	infoSizer->Add(lblVersion);
	infoSizer->Add(cbVersion);
	infoSizer->Add(lblTimeout);
	infoSizer->Add(cbTimeout);
	infoSizer->Add(lblRetryTimes);
	infoSizer->Add(cbRetry);
	infoSizer->Add(lblReadCommunity);
	infoSizer->Add(txtReadCommunity);
	infoSizer->Add(lblWriteCommunity);
	infoSizer->Add(txtWriteCommunity);


	rightSizer->Add(buttonSizer, 0, wxALIGN_RIGHT | wxRIGHT, 30);
	wxButton * okButton = new wxButton(this, wxID_OK, L"确定");
	okButton->SetDefault();
	buttonSizer->Add(okButton, 0, wxALL, 10);
	buttonSizer->Add(new wxButton(this, wxID_CANCEL, L"取消"), 0, wxALL, 10);

	this->SetSizer(topSizer);
}

bool DiaAddHost::TransferDataToWindow() {
	cbRetry->SetSelection(1);
	cbTimeout->SetSelection(1);
	cbVersion->SetSelection(1);
	/* UDP 端口目前不能设置*/
	txtUDPPort->SetValue("161");
	txtUDPPort->SetEditable(false);
	txtReadCommunity->SetValue("public");
	txtWriteCommunity->SetValue("public");
	txtIpAddress->SetValue("192.168.157.138");
	return true;
}

bool DiaAddHost::TransferDataFromWindow() {
	wxString wrongCaption(L"信息有误");
	wxString ipstr = txtIpAddress->GetValue();
	Snmp_pp::IpAddress ipaddress(ipstr.mb_str());
	if (ipstr == "" || !ipaddress.valid()) {
		ShowErrorDialog(L"IP地址不合法", wrongCaption);
		return false;
	}
	if (dynamic_cast<FrmMain*>(this->GetParent())->GetCanvas()->GetHost(ipstr)) {
		ShowErrorDialog(L"已存在该IP地址的主机", wrongCaption);
		return false;
	}

	if (txtReadCommunity->GetValue() == "") {
		ShowErrorDialog(L"读取共同体不能为空", wrongCaption);
		return false;
	}
	if (txtWriteCommunity->GetValue() == "") {
		ShowErrorDialog(L"写入共同体不能为空", wrongCaption);
		return false;
	}

	/* 数据验证结束 */
	if (txtName->GetValue() == "")
		strName = ipstr;
	else
		strName = txtName->GetValue();
	strIpAddress = ipstr;
	strReadCommunity = txtReadCommunity->GetValue();
	strWriteCommunity = txtWriteCommunity->GetValue();
	strVersion = cbVersion->GetValue();
	intRetry = wxAtoi(cbRetry->GetValue());
	intUDPPort = wxAtoi(txtUDPPort->GetValue());
	intTimeout = wxAtoi(cbTimeout->GetValue().Mid(0, 1)) * 100;

	return true;
}

void DiaAddHost::ShowErrorDialog(const wxString& text, const wxString& caption) {
	wxMessageDialog * errorDialog = new wxMessageDialog(this, text, caption, wxOK | wxICON_ERROR);
	errorDialog->ShowModal();
	errorDialog->Destroy();
}