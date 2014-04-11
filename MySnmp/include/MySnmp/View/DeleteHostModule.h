#ifndef __DELETE_HOST_MODULE_H
#define __DELETE_HOST_MODULE_H

#include <MySnmp/View/Module.h>

namespace mysnmp {

	class DeleteHostModule : public Module {
	private:
	public:
		DeleteHostModule(const wxString& menuLabel) :
			Module(menuLabel) {}

		virtual void OnMenuItemClick(wxCommandEvent& event);
	};
}

#endif