#ifndef __FRMLISTCTRL_BASE_H
#define __FRMLISTCTRL_BASE_H

#include <MySnmp/View/Module.h>
#include <wx/listctrl.h>
#include <wx/frame.h>
#include <wx/button.h>
#include <wx/timer.h>

namespace mysnmp {
	/* 这是所有以一个列表 刷新重发关闭按钮为布局的对话框的基类 */
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

	/* 这是SNMP请求一个表对象的对话框的基类 */
	class FrmSnmpTableBase : public FrmListCtrlBase {
	private:
		const char * oidpre;
	protected:
		int lastRequestId;
		TableModule * module;
		TopoHost * chosenHost;

		virtual void sendRequest();
		virtual void updateListCtrl();
	public:
		/* @oidpre：所请求的SNMP子树的OID */
		FrmSnmpTableBase(TableModule * module, const char * oidpre,
						 const wxString& caption, wxSize size);
	};
}

#endif 