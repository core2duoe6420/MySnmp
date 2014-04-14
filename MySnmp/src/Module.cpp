#include <MySnmp/View/Module.h>

#include <wx/xml/xml.h>

#include <MySnmp/debug.h>

using namespace mysnmp;

ModuleManager ModuleManager::singleton;

ModuleManager::~ModuleManager() {
	wxList::compatibility_iterator node = modules.GetFirst();
	while (node) {
		Module * module = (Module*)node->GetData();
		delete module;
		node = node->GetNext();
	}
}

XMLColumnCollection::XMLColumnCollection(const char * xmlpath, int& ret) {
	wxXmlDocument doc;
	if (!doc.Load(xmlpath)) {
		ret = 1;
		return;
	}
	int index = 0;
	wxXmlNode * root = doc.GetRoot();
	wxXmlNode * node = root->GetChildren();
	while (node) {
		if (node->GetType() == wxXML_ELEMENT_NODE && node->GetName() == "column") {
			wxString name = node->GetAttribute("name");
			int size = wxAtoi(node->GetAttribute("size"));
			ColumnInfo * info = new ColumnInfo(name, size);
			wxXmlNode * mapnode = node->GetChildren();
			while (mapnode) {
				if (mapnode->GetType() == wxXML_ELEMENT_NODE && mapnode->GetName() == "map") {
					info->hasValueMap = true;
					int key = wxAtoi(mapnode->GetAttribute("key"));
					wxString value = mapnode->GetAttribute("value");
					info->valueMap.insert(std::unordered_map<int, wxString>::value_type(key, value));
				}
				mapnode = mapnode->GetNext();
			}
			this->columns.push_back(info);
		}
		node = node->GetNext();
	}
	ret = 0;
}

XMLColumnCollection::~XMLColumnCollection() {
	for (int i = 0; i < columns.size(); i++)
		delete columns[i];
}