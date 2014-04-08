#ifndef __FRMMAIN_H
#define __FRMMAIN_H

#include <wx/wx.h>

namespace mysnmp {
	class TopoCanvas;

	class FrmMain : public wxFrame {
	public:
		FrmMain(const wxString& title, const wxPoint& pos, const wxSize& size);

	private:
		void OnExit(wxCloseEvent& event);
		void OnAddHostClick(wxCommandEvent& event);
		void OnMenuExitClick(wxCommandEvent& event);

		wxMenuBar * menuBar;
		wxMenu * menuProgram;
		wxMenu * menuHost;
		wxMenuItem * menuAddHost;
		wxMenu * menuTool;
		wxMenu * menuHelp;
		wxMenuItem * menuExit;

		wxToolBar * toolBar;
		wxToolBarToolBase * tbAddHost;

		TopoCanvas * canvas;

		void toolbarInitialize();
		void menuInitialize();
		void canvasInitialize();
		void eventInitialize();

		const int ID_menuAddHost;
		const int ID_menuExit;
		const int ID_tbAddHost;
	};

}

#endif