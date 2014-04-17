#ifndef __FRMLISTCTRL_BASE_H
#define __FRMLISTCTRL_BASE_H

#include <wx/listctrl.h>
#include <wx/frame.h>
#include <wx/button.h>
#include <wx/timer.h>

namespace mysnmp {
	class FrmListCtrlBase : public wxFrame {
	protected:
		wxListCtrl * list;
		wxButton * btnRefresh;
		wxButton * btnResend;
		wxButton * btnClose;
		wxTimer * timer;

		virtual void sendRequest() = 0;
		virtual void updateListCtrl() = 0;

		virtual void eventInitialize();

		virtual void OnResendClick(wxCommandEvent& event);
		virtual void OnCloseClick(wxCommandEvent& event);
		virtual void OnRefreshClick(wxCommandEvent& event);
		virtual void OnTimer(wxTimerEvent& event);
		virtual void OnClose(wxCloseEvent& event);
		virtual void OnListDoubleClick(wxListEvent& event);

		FrmListCtrlBase(wxWindow * parent, const wxString& title, wxSize size);
	};
}

#endif 