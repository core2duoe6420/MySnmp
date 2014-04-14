#ifndef __MODULE_H
#define __MODULE_H

#include <vector>
#include <unordered_map>

#include <wx/wx.h>

#include <MySnmp/View/TopoCanvas.h>

namespace mysnmp {

	class ColumnInfo {
		friend class XMLColumnCollection;
	private:
		std::unordered_map<int, wxString> valueMap;
		wxString columnName;
		int columnSize;
		bool hasValueMap;

	public:
		ColumnInfo(const wxString& name, int size) :
			columnName(name), columnSize(size), hasValueMap(false) {}
		
		wxString MapValueToString(int value) const {
			std::unordered_map<int, wxString>::const_iterator iter = valueMap.find(value);
			if (iter == valueMap.end())
				return wxEmptyString;
			return iter->second;
		}

		bool HasValueMap() const { return hasValueMap; }
		wxString GetName() const { return columnName; }
		int GetSize() const { return columnSize; }
	};

	class XMLColumnCollection {
	private:
		std::vector<ColumnInfo *> columns;
	public:
		XMLColumnCollection(const char * xmlpath, int& ret);
		~XMLColumnCollection();
		int GetColumnCount() const { return columns.size(); }
		const ColumnInfo * GetColumn(int index) const { return columns[index]; }
	};

	class Module : public wxObject {
	private:
		/* 显示在TopoCanvas弹出菜单中的项
		 * menuItem由包含它的wxMenu析构，
		 * 不需要调用delete
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

	class HostInfoModule : public Module {
	private:
	public:
		HostInfoModule(const wxString& menuLabel) :
			Module(menuLabel) {}

		virtual void OnMenuItemClick(wxCommandEvent& event);
	};

	class ModifyHostModule : public Module {
	private:
	public:
		ModifyHostModule(const wxString& menuLabel) :
			Module(menuLabel) {}

		virtual void OnMenuItemClick(wxCommandEvent& event);
	};

	class DeleteHostModule : public Module {
	private:
	public:
		DeleteHostModule(const wxString& menuLabel) :
			Module(menuLabel) {}

		virtual void OnMenuItemClick(wxCommandEvent& event);
	};

	class InterfaceModule : public Module {
	private:
		XMLColumnCollection * columnInfos;
	public:
		InterfaceModule(const wxString& menuLabel);
		~InterfaceModule();
		virtual void OnMenuItemClick(wxCommandEvent& event);
		XMLColumnCollection * GetColumnCollection() const { return columnInfos; }
	};
}

#endif