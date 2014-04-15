#ifndef __DIAADDHOST_H
#define __DIAADDHOST_H

#include <wx/window.h>
#include <wx/dialog.h>
#include <wx/textctrl.h>
#include <wx/combobox.h>


namespace mysnmp {

	class DiaHostInfo : public wxDialog {
	public:
		enum {
			HOST_ADD = 1,
			HOST_MODIFY = 2,
		};
	private:
		int type;
		int hostid;	/* 仅type==HOST_MODIFY时有效 */
		wxString hostName; /* 仅type==HOST_MODIFY时有效 */
		wxString originIp; /* 仅type==HOST_MODIFY时有效 */
		wxTextCtrl * txtName;
		wxTextCtrl * txtReadCommunity;
		wxTextCtrl * txtWriteCommunity;
		wxComboBox * cbRetry;
		wxComboBox * cbTimeout;
		wxTextCtrl * txtUDPPort;
		wxTextCtrl * txtIpAddress;
		wxComboBox * cbVersion;

		void ShowErrorDialog(const wxString& text, const wxString& caption);
		virtual bool TransferDataToWindow();
		virtual bool TransferDataFromWindow();

		wxString strName;
		wxString strReadCommunity;
		wxString strWriteCommunity;
		int intRetry;
		int intUDPPort;
		int intTimeout;
		wxString strIpAddress;
		wxString strVersion;

	public:
		DiaHostInfo(int type, wxWindow * parent, const wxString& title,
					const wxString& hostName = wxEmptyString, int hostid = -1);

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