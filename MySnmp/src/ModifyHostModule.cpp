#include <MySnmp/View/Module.h>
#include <MySnmp/View/DiaHostInfo.h>
#include <MySnmp/Command/HostCommand.h>

#include <MySnmp/debug.h>

using namespace mysnmp;

void ModifyHostModule::OnMenuItemClick(wxCommandEvent& event) {
	TopoHost * topoHost = canvas->GetChosenHost();
	DiaHostInfo * dialog = new DiaHostInfo(DiaHostInfo::HOST_MODIFY, canvas->GetParent(),
										   L"修改 " + topoHost->GetName() + L" 主机信息",
										   topoHost->GetName(), topoHost->GetHostId());
	if (dialog->ShowModal() == wxID_OK) {
		HostInfoCommand command(topoHost->GetHostId(), HostInfoCommand::COMMAND_WRITE);
		command.SetRetryTimes(dialog->GetRetryTimes());
		command.SetTimeout(dialog->GetTimeout());
		command.SetIpAddress(dialog->GetIpAddress());
		command.SetReadCommunity(dialog->GetReadCommunity());
		command.SetWriteCommunity(dialog->GetWriteCommunity());
		command.SetUdpPort(dialog->GetUDPPort());
		command.SetSnmpVersion(dialog->GetVersion());
		command.Execute();
		topoHost->SetIpAddress(dialog->GetIpAddress());
		if (dialog->GetName() != topoHost->GetName()) {
			topoHost->SetName(dialog->GetName());
			canvas->RefreshCanvas();
		}
		canvas->SetChanged();
	}
}