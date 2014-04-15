#include <MySnmp/View/Module.h>
#include <MySnmp/View/TopoCanvas.h>
#include <MySnmp/Command/HostCommand.h>

#include <wx/wx.h>

#include <MySnmp/debug.h>

using namespace mysnmp;

void DeleteHostModule::OnMenuItemClick(wxCommandEvent& event) {
	TopoHost * topoHost = canvas->GetChosenHost();
	int hostId = topoHost->GetHostId();
	wxMessageDialog * dialog = new wxMessageDialog(canvas->GetParent(), L"ȷ��ɾ������" + topoHost->GetName() + L"��",
												   L"ȷ��ɾ������", wxICON_INFORMATION | wxYES_NO);
	if (dialog->ShowModal() == wxID_YES) {
		canvas->RemoveHost(hostId);
		canvas->RefreshCanvas();
		DeleteHostCommand command(hostId);
		command.Execute();
	}
	dialog->Destroy();
}