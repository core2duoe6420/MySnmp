#include <MySnmp/View/Module.h>
#include <MySnmp/View/FrmListCtrlBase.h>
#include <MySnmp/Command/HostCommand.h>
#include <MySnmp/Command/SnmpRequestCommand.h>

#include <wx/wx.h>

#include <MySnmp/debug.h>

namespace mysnmp {
	class FrmIpRouteTable : public FrmListCtrlBase {
	private:
		int lastRequestId;
		IpRouteTableModule * module;
		TopoHost * chosenHost;

		virtual void sendRequest();
		virtual void updateListCtrl();

		virtual void OnListDoubleClick(wxListEvent& event);

	public:
		FrmIpRouteTable(IpRouteTableModule * module);
	};
}

using namespace mysnmp;

void IpRouteTableModule::OnMenuItemClick(wxCommandEvent& event) {
	FrmIpRouteTable * frmIpRouteTable = new FrmIpRouteTable(this);
	frmIpRouteTable->Show();
}

IpRouteTableModule::IpRouteTableModule(const wxString& menuLabel) : Module(menuLabel) {
	int ret = 0;
	this->columnInfos = new XMLColumnCollection("xml/modules/ipRouteTable.xml", ret);
}

IpRouteTableModule::~IpRouteTableModule() {
	delete columnInfos;
}

FrmIpRouteTable::FrmIpRouteTable(IpRouteTableModule * module) :
FrmListCtrlBase(module->GetCanvas()->GetParent(), "", wxSize(800, 400)), module(module) {
	chosenHost = module->GetCanvas()->GetChosenHost();
	this->SetTitle(chosenHost->GetName() + L" IP·�ɱ�");

	sendRequest();

	XMLColumnCollection * columns = module->GetColumnCollection();
	for (int i = 0; i < columns->GetColumnCount(); i++) {
		list->InsertColumn(i, columns->GetColumn(i)->GetName());
		list->SetColumnWidth(i, columns->GetColumn(i)->GetSize());
	}


	updateListCtrl();
}

void FrmIpRouteTable::OnListDoubleClick(wxListEvent& event) {}

void FrmIpRouteTable::sendRequest() {
	SnmpRequestCommand command(SnmpType::SNMP_WALK, chosenHost->GetHostId());
	command.AddOid("1.3.6.1.2.1.4.21");
	lastRequestId = command.Execute();
}

void FrmIpRouteTable::updateListCtrl() {
	GetOidSubtreeCommand command(chosenHost->GetHostId(), lastRequestId);
	command.SetOid("1.3.6.1.2.1.4.21.1");
	command.Execute();
	const std::vector<Snmp_pp::Vb>& vbs = command.GetResults();

	/* �Ȼ����������Ϊ�еĴ���ֻ���ؼ����ж���
	 * �����޷�ͨ��mib�е������ж�����
	 */
	int rows = 0, columns;
	for (; rows < vbs.size(); rows++) {
		/* oid�ĵ�10λ���к� */
		if (vbs[rows].get_oid()[9] != 1)
			break;
	}
	if (rows != 0)
		columns = vbs.size() / rows;
	/* ������ԭ��������������ȫ��ɾ������� */
	if (rows != list->GetItemCount()) {
		list->DeleteAllItems();
		for (int i = 0; i < rows; i++)
			list->InsertItem(i, "");
	}

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < columns; j++) {
			wxString value = vbs[i + rows*j].get_printable_value();
			int colidx = vbs[i + rows*j].get_oid()[9] - 1;
			const ColumnInfo * column = module->GetColumnCollection()->GetColumn(colidx);
			if (column->HasValueMap() && value != "")
				value = column->MapValueToString(wxAtoi(value));

			if (list->GetItemText(i, colidx) != value)
				list->SetItem(i, colidx, value);
		}
	}
}