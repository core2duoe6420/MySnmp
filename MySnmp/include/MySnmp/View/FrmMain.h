#ifndef __FRMMAIN_H
#define __FRMMAIN_H

#include <wx/frame.h>
#include <wx/toolbar.h>

namespace mysnmp {
	class TopoCanvas;

	class FrmMain : public wxFrame {
	public:
		FrmMain(const wxString& title, const wxPoint& pos, const wxSize& size);
		TopoCanvas * GetCanvas() { return canvas; }

	private:
		void OnExit(wxCloseEvent& event);
		void OnAddHostClick(wxCommandEvent& event);
		void OnMenuExitClick(wxCommandEvent& event);

		wxMenuItem * menuAddHost;
		wxMenuItem * menuExit;

		wxToolBar * toolBar;
		wxToolBarToolBase * tbAddHost;

		TopoCanvas * canvas;

		void toolbarInitialize();
		void menuInitialize();
		void canvasInitialize();
		void eventInitialize();
		/* 必须在canvasInitialize之后运行 */
		void modulesInitialize();

		const int ID_menuAddHost;
		const int ID_menuExit;
		const int ID_tbAddHost;
	};

}

#endif