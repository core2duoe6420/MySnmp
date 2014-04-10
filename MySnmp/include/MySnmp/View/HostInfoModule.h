#ifndef __HOSTINFO_MODULE_H
#define __HOSTINFO_MODULE_H

#include <MySnmp/View/Module.h>

namespace mysnmp {

	class HostInfoModule : public Module {
	private:
	public:
		HostInfoModule(const wxString& menuLabel) :
			Module(menuLabel) {}

		virtual void OnMenuItemClick(wxCommandEvent& event);
	};
}

#endif