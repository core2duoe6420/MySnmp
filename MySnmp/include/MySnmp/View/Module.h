#ifndef __MODULE_H
#define __MODULE_H

#include <vector>
#include <unordered_map>

#include <wx/string.h>
#include <wx/list.h>
#include <wx/menu.h>
#include <wx/menuitem.h>
#include <wx/scrolwin.h>

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
	public:
		HostInfoModule(const wxString& menuLabel) :
			Module(menuLabel) {}

		virtual void OnMenuItemClick(wxCommandEvent& event);
	};

	class ModifyHostModule : public Module {
	public:
		ModifyHostModule(const wxString& menuLabel) :
			Module(menuLabel) {}

		virtual void OnMenuItemClick(wxCommandEvent& event);
	};

	class DeleteHostModule : public Module {
	public:
		DeleteHostModule(const wxString& menuLabel) :
			Module(menuLabel) {}

		virtual void OnMenuItemClick(wxCommandEvent& event);
	};

	/* 显示并请求一个SNMP表对象的模块的基类 */
	class TableModule : public Module {
	private:
		XMLColumnCollection * columnInfos;
	public:
		TableModule(const char * xmlColumnsPath, const wxString& menuLabel) : Module(menuLabel) {
			int ret = 0;
			columnInfos = new XMLColumnCollection(xmlColumnsPath, ret);
		}
		virtual ~TableModule() { delete columnInfos; }
		XMLColumnCollection * GetColumnCollection() const { return columnInfos; }
	};

	class InterfaceModule : public TableModule {
	public:
		InterfaceModule(const wxString& menuLabel) :
			TableModule("xml/modules/interface.xml", menuLabel) {}
		virtual void OnMenuItemClick(wxCommandEvent& event);
	};

	class IpRouteTableModule : public TableModule {
	public:
		IpRouteTableModule(const wxString& menuLabel) :
			TableModule("xml/modules/ipRouteTable.xml", menuLabel) {}
		virtual void OnMenuItemClick(wxCommandEvent& event);
	};

	class IpAddrTableModule : public TableModule {
	public:
		IpAddrTableModule(const wxString& menuLabel) :
			TableModule("xml/modules/ipAddrTable.xml", menuLabel) {}
		virtual void OnMenuItemClick(wxCommandEvent& event);
	};

	class IpTransTableModule : public TableModule {
	public:
		IpTransTableModule(const wxString& menuLabel) :
			TableModule("xml/modules/ipTransTable.xml", menuLabel) {}
		virtual void OnMenuItemClick(wxCommandEvent& event);
	};

	class IpBasicModule : public TableModule {
	public:
		IpBasicModule(const wxString& menuLabel) :
			TableModule("xml/modules/ipBasic.xml", menuLabel) {}
		virtual void OnMenuItemClick(wxCommandEvent& event);
	};

	class TcpConnTableModule : public TableModule {
	public:
		TcpConnTableModule(const wxString& menuLabel) :
			TableModule("xml/modules/tcpConnTable.xml", menuLabel) {}
		virtual void OnMenuItemClick(wxCommandEvent& event);
	};

}

#endif