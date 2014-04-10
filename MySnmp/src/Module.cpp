#include <MySnmp/View/Module.h>

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