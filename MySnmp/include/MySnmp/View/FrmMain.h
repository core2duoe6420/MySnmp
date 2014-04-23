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
		void OnClose(wxCloseEvent& event);
		void OnAddHostClick(wxCommandEvent& event);
		void OnMenuExitClick(wxCommandEvent& event);
		void OnSaveCanvasClick(wxCommandEvent& event);
		void OnSaveCanvasAsClick(wxCommandEvent& event);
		void OnLoadCanvasClick(wxCommandEvent& event);

		wxMenuItem * menuAddHost;
		wxMenuItem * menuSaveCanvas;
		wxMenuItem * menuSaveCanvasAs;
		wxMenuItem * menuLoadCanvas;
		wxMenuItem * menuExit;

		wxToolBar * toolBar;

		TopoCanvas * canvas;

		void saveTopoCanvas();
		int showAskSaveDialog();

		void toolbarInitialize();
		void menuInitialize();
		void canvasInitialize();
		void eventInitialize();
		/* 必须在canvasInitialize之后运行 */
		void modulesInitialize();

		const int ID_tbAddHost;
		const int ID_tbSaveCanvas;
		const int ID_tbLoadCanvas;
	};

}

#endif