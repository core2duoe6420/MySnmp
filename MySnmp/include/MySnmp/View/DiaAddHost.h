#ifndef __DIAADDHOST_H
#define __DIAADDHOST_H

#include <wx/wx.h>

namespace mysnmp {
	class DiaAddHost : public wxDialog {
	private:
		wxTextCtrl * txtName;
		wxTextCtrl * txtReadCommunity;
		wxTextCtrl * txtWriteCommunity;
		wxComboBox * cbRetry;
		wxComboBox * cbTimeout;
		wxTextCtrl * txtUDPPort;
		wxTextCtrl * txtIpAddress;
		wxComboBox * cbVersion;
		virtual bool TransferDataToWindow();
		virtual bool TransferDataFromWindow();
		void ShowErrorDialog(const wxString& text, const wxString& caption);

		wxString strName;
		wxString strReadCommunity;
		wxString strWriteCommunity;
		int intRetry;
		int intUDPPort;
		int intTimeout;
		wxString strIpAddress;
		wxString strVersion;

	public:
		DiaAddHost(wxWindow * parent, const wxString& title);

		wxString GetName() const { return strName; }
		wxString GetReadCommunity() const { return strReadCommunity; }
		wxString GetWriteCommunity() const { return strWriteCommunity; }
		wxString GetIpAddress() const { return strIpAddress; }
		wxString GetVersion() const { return strVersion; }
		int GetRetryTimes() const { return intRetry; }
		int GetUDPPort() const { return intUDPPort; }
		int GetTimeout() const { return intTimeout; }
	};
}

#endif