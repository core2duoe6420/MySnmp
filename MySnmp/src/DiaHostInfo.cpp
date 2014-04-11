#include <libsnmp.h>
#include <snmp_pp/snmp_pp.h>
#include <MySnmp/View/FrmMain.h>
#include <MySnmp/View/TopoCanvas.h>
#include <MySnmp/View/DiaHostInfo.h>
#include <MySnmp/Command/HostCommand.h>

#include <MySnmp/debug.h>

using namespace mysnmp;

DiaHostInfo::DiaHostInfo(int type, wxWindow * parent, const wxString& title, const wxString& hostName, int hostid) :
wxDialog(parent, wxID_ANY, title, wxDefaultPosition, wxSize(600, 430)), type(type), hostName(hostName), hostid(hostid) {
	this->Center();
	wxBoxSizer * topSizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer * rightSizer = new wxBoxSizer(wxVERTICAL);
	wxStaticBoxSizer * labelSizer = new wxStaticBoxSizer(wxHORIZONTAL, this, L"������Ϣ");
	wxFlexGridSizer * infoSizer = new wxFlexGridSizer(8, 2, 10, 10);
	wxBoxSizer * buttonSizer = new wxBoxSizer(wxHORIZONTAL);

	wxStaticBitmap * hostBitmap = new wxStaticBitmap(this, wxID_ANY, wxBitmap("image/Host.png", wxBITMAP_TYPE_PNG));
	topSizer->Add(hostBitmap, 0, wxLEFT | wxTOP, 30);
	topSizer->Add(rightSizer, 1, wxALL | wxEXPAND, 20);
	rightSizer->Add(labelSizer, wxEXPAND);
	labelSizer->Add(infoSizer, 1, wxALL | wxEXPAND, 5);

	//infoSizer->AddGrowableCol(1, 1);

	/* Labels */
	wxStaticText * lblName = new wxStaticText(this, wxID_ANY, L"�������ƣ�");
	wxFont font = lblName->GetFont();
	font.SetPointSize(12);
	wxStaticText * lblReadCommunity = new wxStaticText(this, wxID_ANY, L"��ȡ��ͬ�壺");
	wxStaticText * lblWriteCommunity = new wxStaticText(this, wxID_ANY, L"д�빲ͬ�壺");
	wxStaticText * lblRetryTimes = new wxStaticText(this, wxID_ANY, L"���Դ�����");
	wxStaticText * lblVersion = new wxStaticText(this, wxID_ANY, L"SNMP�汾��");
	wxStaticText * lblTimeout = new wxStaticText(this, wxID_ANY, L"��ʱ��");
	wxStaticText * lblUDPPort = new wxStaticText(this, wxID_ANY, L"UDP�˿ڣ�");
	wxStaticText * lblIpAddress = new wxStaticText(this, wxID_ANY, L"IP��ַ��");
	lblName->SetFont(font);
	lblWriteCommunity->SetFont(font);
	lblReadCommunity->SetFont(font);
	lblRetryTimes->SetFont(font);
	lblVersion->SetFont(font);
	lblTimeout->SetFont(font);
	lblUDPPort->SetFont(font);
	lblIpAddress->SetFont(font);

	wxArrayString strsTimeout;
	strsTimeout.Add(L"1��");
	strsTimeout.Add(L"2��");
	strsTimeout.Add(L"3��");
	strsTimeout.Add(L"4��");
	strsTimeout.Add(L"5��");

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
	wxButton * okButton = new wxButton(this, wxID_OK, L"ȷ��");
	okButton->SetDefault();
	buttonSizer->Add(okButton, 0, wxALL, 10);
	buttonSizer->Add(new wxButton(this, wxID_CANCEL, L"ȡ��"), 0, wxALL, 10);

	this->SetSizer(topSizer);
}

bool DiaHostInfo::TransferDataToWindow() {
	if (type == HOST_ADD) {
		cbRetry->SetSelection(1);
		cbTimeout->SetSelection(1);
		cbVersion->SetSelection(1);
		/* UDP �˿�Ŀǰ��������*/
		txtUDPPort->SetValue("161");
		txtUDPPort->SetEditable(false);
		txtReadCommunity->SetValue("public");
		txtWriteCommunity->SetValue("public");
		txtIpAddress->SetValue("192.168.157.138");
	} else if (type == HOST_MODIFY) {
		wxString value;
		HostInfoCommand command(hostid, HostInfoCommand::COMMAND_READ);
		value.Printf("%d", command.GetUdpPort());
		txtUDPPort->SetValue(value);
		txtUDPPort->SetEditable(false);
		cbRetry->SetSelection(command.GetRetryTimes());
		cbTimeout->SetSelection(command.GetTimeout() - 1);
		txtReadCommunity->SetValue(command.GetReadCommunity());
		txtWriteCommunity->SetValue(command.GetWriteCommunity());
		originIp = command.GetIpAddress();
		txtIpAddress->SetValue(command.GetIpAddress());
		txtName->SetValue(this->hostName);
		value = command.GetSnmpVersion();
		if (value == "version1")
			cbVersion->SetSelection(0);
		else if (value == "version2c")
			cbVersion->SetSelection(1);
		else if (value == "version3")
			cbVersion->SetSelection(2);
	}
	return true;
}

bool DiaHostInfo::TransferDataFromWindow() {
	wxString wrongCaption(L"��Ϣ����");
	wxString ipstr = txtIpAddress->GetValue();
	Snmp_pp::IpAddress ipaddress(ipstr.mb_str());
	if (ipstr == "" || !ipaddress.valid()) {
		ShowErrorDialog(L"IP��ַ���Ϸ�", wrongCaption);
		return false;
	}
	if (dynamic_cast<FrmMain*>(this->GetParent())->GetCanvas()->GetHost(ipstr)) {
		if (type == HOST_ADD || ipstr != originIp) {
			ShowErrorDialog(L"�Ѵ��ڸ�IP��ַ������", wrongCaption);
			return false;
		}
	}

	if (txtReadCommunity->GetValue() == "") {
		ShowErrorDialog(L"��ȡ��ͬ�岻��Ϊ��", wrongCaption);
		return false;
	}
	if (txtWriteCommunity->GetValue() == "") {
		ShowErrorDialog(L"д�빲ͬ�岻��Ϊ��", wrongCaption);
		return false;
	}

	/* ������֤���� */
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

void DiaHostInfo::ShowErrorDialog(const wxString& text, const wxString& caption) {
	wxMessageDialog * errorDialog = new wxMessageDialog(this, text, caption, wxOK | wxICON_ERROR);
	errorDialog->ShowModal();
	errorDialog->Destroy();
}