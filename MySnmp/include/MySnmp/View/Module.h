#ifndef __MODULE_H
#define __MODULE_H

#include <wx/wx.h>
#include <MySnmp/View/TopoCanvas.h>

namespace mysnmp {
	class Module : public wxObject {
	private:
		/* ��ʾ��TopoCanvas�����˵��е���
		 * menuItem�ɰ�������wxMenu������
		 * ����Ҫ����delete
		 */
		wxMenuItem * menuItem;

	protected:
		TopoCanvas * canvas;
	public:
		Module(const wxString& menuLabel) : canvas(NULL) {
			menuItem = new wxMenuItem(NULL, wxID_ANY, menuLabel);
		}

		virtual void OnMenuItemClick(wxCommandEvent& event) = 0;
		virtual ~Module() {}
		wxMenuItem * GetMenuItem() { return menuItem; }
		TopoCanvas * GetCanvas() const { return canvas; }
		void SetCanvas(TopoCanvas * canvas) { this->canvas = canvas; }
	};

	class ModuleManager {
	private:
		static ModuleManager singleton;

		wxList modules;
		TopoCanvas * canvas;

		ModuleManager() : canvas(NULL) {}
		ModuleManager(const ModuleManager& other) {}
		~ModuleManager();
	public:
		//ModuleManager& GetModuleManager() { return singleton; }
		static wxList * GetModuleList() { return &singleton.modules; }
		static void SetTopoCanvas(TopoCanvas * canvas) { singleton.canvas = canvas; }

		static void RegisterModule(Module * module) {
			if (!singleton.canvas)
				return;
			singleton.modules.Append(module);

			wxMenu * canvasPopMenu = singleton.canvas->GetPopMenu();
			wxMenuItem * moduleMenuItem = module->GetMenuItem();
			moduleMenuItem->SetMenu(canvasPopMenu);
			singleton.canvas->Bind(wxEVT_MENU, &Module::OnMenuItemClick,
								   module, moduleMenuItem->GetId());

			module->SetCanvas(singleton.canvas);
			canvasPopMenu->Append(moduleMenuItem);

		}

	};
}

#endif