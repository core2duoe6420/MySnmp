#include <MySnmp/View/TopoCanvas.h>

#include <wx/wx.h>
#include <wx/dcgraph.h>
#include <wx/dcbuffer.h>

#include <MySnmp/debug.h>

using namespace mysnmp;

BEGIN_EVENT_TABLE(TopoCanvas, wxScrolledCanvas)
EVT_MOUSE_EVENTS(TopoCanvas::OnMouseEvents)
EVT_PAINT(TopoCanvas::OnPaint)
EVT_SIZE(TopoCanvas::OnSize)
END_EVENT_TABLE()

void TopoHost::drawNameOnBitmap() {
	/* lblName��������������ִ�С */
	wxStaticBox * lblName = lblName = new wxStaticBox(canvas, wxID_ANY, name);
	wxSize lblSize = lblName->GetEffectiveMinSize();
	/* �������� */
	lblName->Destroy();
	int lblx, lbly;
	/* ��һЩС���� */
	/* Ubuntu����������С */
#ifdef _WIN32
	int x_modvalue = 7;
#else
	int x_modvalue = 4;
#endif
	lblx = (originBitmap.GetSize().GetWidth() - lblSize.GetWidth()) / 2 + x_modvalue;
	lbly = lblSize.GetHeight() - 5;
	if (lblx < 0)
		lblx = 0;

	wxImage image = originBitmap.ConvertToImage();
	/* Ubuntu��͸��bitmap��GCDC��Ȼ�޷�������
	* Windows����GCDCЧ�����һ����Ϳ
	* ����ȫ��֪����ν����Щ����
	*/
#ifdef _WIN32
	image.Resize(originBitmap.GetSize() + wxSize(0, lbly), wxPoint(0, lbly));
#else
	image.Resize(originBitmap.GetSize() + wxSize(0, lbly), wxPoint(0, lbly), 255, 255, 255);
#endif
	this->bitmap = wxBitmap(image);

	wxMemoryDC memDC(this->bitmap);
	wxGCDC gcdc(memDC);
	wxFont font(9, wxFONTFAMILY_TELETYPE, wxNORMAL, wxBOLD);
	gcdc.SetFont(font);
	gcdc.DrawText(name, lblx, 0);
	memDC.SelectObject(wxNullBitmap);
}

TopoHost::TopoHost(int hostId, const wxBitmap& bitmap, const wxPoint& point,
				   TopoCanvas * canvas, const wxString& ipAddress, const wxString& name) :
				   canvas(canvas), hostId(hostId), originBitmap(bitmap),
				   point(point), ipAddress(ipAddress), name(name) {
	drawNameOnBitmap();
}

TopoCanvas::TopoCanvas(wxWindow * parent, const wxSize& virtualSize, int scrollRate) :
wxScrolledCanvas(parent), scrollRate(scrollRate), dragStatus(0), chosenTopoHost(NULL),
xmlfile(wxEmptyString), isChanged(false) {
	this->SetVirtualSize(virtualSize);
	this->SetScrollRate(scrollRate, scrollRate);

	menuPop = new wxMenu();
}

TopoCanvas::~TopoCanvas() {
	delete menuPop;
	wxList::compatibility_iterator node = topoHosts.GetFirst();
	while (node) {
		TopoHost * topoHost = (TopoHost*)node->GetData();
		delete topoHost;
		node = node->GetNext();
	}
}

TopoHost * TopoCanvas::GetHost(const wxString& ipAddress) {
	wxList::compatibility_iterator node = topoHosts.GetFirst();
	while (node) {
		TopoHost * topoHost = (TopoHost*)node->GetData();
		if (topoHost->GetIpAddress() == ipAddress)
			return topoHost;
		node = node->GetNext();
	}
	return NULL;
}

bool TopoCanvas::RemoveHost(int hostId) {
	TopoHost * host;
	wxList::compatibility_iterator node = topoHosts.GetFirst();
	while (node) {
		TopoHost * topoHost = (TopoHost*)node->GetData();
		if (topoHost->GetHostId() == hostId) {
			host = topoHost;
			break;
		}
		node = node->GetNext();
	}
	if (node) {
		topoHosts.DeleteNode(node);
		delete host;
		this->SetChanged();
		return true;
	}
	return false;
}

void TopoCanvas::OnPaint(wxPaintEvent& event) {
	wxBitmap bitmap(this->GetVirtualSize());
	wxBufferedPaintDC dc(this, bitmap);
	DoPrepareDC(dc);
	dc.SetBackground(*wxWHITE_BRUSH);
	dc.Clear();

	wxList::compatibility_iterator node = topoHosts.GetFirst();
	while (node) {
		TopoHost * topoHost = (TopoHost*)node->GetData();
		dc.DrawBitmap(topoHost->GetBitmap(), topoHost->GetPoint());
		node = node->GetNext();
	}
}

#define TEST_DRAG_NONE     0
#define TEST_DRAG_START    1
#define TEST_DRAG_DRAGGING 2

void TopoCanvas::OnMouseEvents(wxMouseEvent& event) {
	if (event.RightDown()) {
		/* �Ҽ������˵� */
		TopoHost * topoHost = this->findTopoHost(CalcUnscrolledPosition(event.GetPosition()));
		if (topoHost) {
			this->chosenTopoHost = topoHost;
			this->PopupMenu(menuPop, event.GetPosition());
		}
	} else if (event.LeftDown()) {
		TopoHost * topoHost = this->findTopoHost(CalcUnscrolledPosition(event.GetPosition()));
		if (topoHost) {
			dragStatus = TEST_DRAG_START;
			dragStartLogicalPoint = CalcUnscrolledPosition(event.GetPosition()) - topoHost->GetPoint();
			draggedTopoHost = topoHost;
		}
	} else if (event.LeftUp() && dragStatus != TEST_DRAG_NONE) {
		dragStatus = TEST_DRAG_NONE;

		if (!draggedTopoHost)
			return;

		draggedTopoHost->SetPoint(CalcUnscrolledPosition(event.GetPosition()) - dragStartLogicalPoint);
		TopoHost * overlappedHost = NULL;
		while ((overlappedHost = findOverlappedHost(draggedTopoHost))) {
			wxPoint newPoint;
			newPoint.x = overlappedHost->GetPoint().x + overlappedHost->GetBitmap().GetSize().x + 1;
			newPoint.y = draggedTopoHost->GetPoint().y;
			draggedTopoHost->SetPoint(newPoint);
		}
		RefreshCanvas();
	} else if (event.Dragging() && dragStatus != TEST_DRAG_NONE) {
		this->SetChanged();
		if (dragStatus == TEST_DRAG_START) {
			int tolerance = 2;
			wxPoint eventLogicalPoint = CalcUnscrolledPosition(event.GetPosition());
			int dx = abs(eventLogicalPoint.x - dragStartLogicalPoint.x);
			int dy = abs(eventLogicalPoint.y - dragStartLogicalPoint.y);
			if (dx <= tolerance && dy <= tolerance)
				return;

			dragStatus = TEST_DRAG_DRAGGING;
			draggedTopoHost->SetPoint(CalcUnscrolledPosition(event.GetPosition()) - dragStartLogicalPoint);
			RefreshCanvas();
		} else if (dragStatus == TEST_DRAG_DRAGGING) {
			wxPoint eventLogicalPoint = CalcUnscrolledPosition(event.GetPosition());
			draggedTopoHost->SetPoint(eventLogicalPoint - dragStartLogicalPoint);

			OnClientEdgeAndScroll(event.GetPosition(), 30);
			OnVirtualEdgeAndExpand(event.GetPosition(), 30, 100);
			RefreshCanvas();
		}
	}
}

void TopoCanvas::OnClientEdgeAndScroll(const wxPoint& eventPoint, int threshold) {
	wxSize clientSize = this->GetClientSize();
	int horizonalScrollUnit = 0, verticalScrollUnit = 0;
	if (eventPoint.x >= clientSize.GetWidth() - threshold)
		horizonalScrollUnit = 1;
	else if (eventPoint.x <= threshold)
		horizonalScrollUnit = -1;
	if (eventPoint.y >= clientSize.GetHeight() - threshold)
		verticalScrollUnit = 1;
	else if (eventPoint.y <= threshold)
		verticalScrollUnit = -1;
	this->Scroll(GetViewStart() + wxPoint(horizonalScrollUnit, verticalScrollUnit));
}

void TopoCanvas::OnVirtualEdgeAndExpand(const wxPoint& eventPoint, int threshold, int delta) {
	wxSize virtualSize = this->GetVirtualSize();
	wxPoint eventLogicalPoint(CalcUnscrolledPosition(eventPoint));
	int horizonalDelta = 0, verticalDelta = 0;
	if (eventLogicalPoint.x >= virtualSize.GetWidth() - threshold)
		horizonalDelta = delta;
	else if (eventLogicalPoint.x <= threshold)
		horizonalDelta = -delta;
	if (eventLogicalPoint.y >= virtualSize.GetHeight() - threshold)
		verticalDelta = delta;
	else if (eventLogicalPoint.y <= threshold)
		verticalDelta = -delta;

	this->SetVirtualSize(this->GetVirtualSize() + wxSize(abs(horizonalDelta), abs(verticalDelta)));
	if (horizonalDelta < 0 || verticalDelta < 0) {
		this->Scroll(GetViewStart() + wxPoint(abs(horizonalDelta) / scrollRate, abs(verticalDelta) / scrollRate));
		wxList::compatibility_iterator node = topoHosts.GetFirst();
		while (node) {
			TopoHost * topoHost = (TopoHost*)node->GetData();
			topoHost->SetPoint(topoHost->GetPoint() + wxPoint(abs(horizonalDelta), abs(verticalDelta)));
			node = node->GetNext();
		}
	}
}

void TopoCanvas::DrawBitmap(int hostId, const wxBitmap& host,
							const wxPoint& point, const wxString& ipAddress, const wxString& name) {
	TopoHost * topoHost = new TopoHost(hostId, host, point, this, ipAddress, name);
	topoHosts.Append(topoHost);
	RefreshCanvas();
	this->SetChanged();
}

void TopoCanvas::OnSize(wxSizeEvent& event) {

	this->Update();
	event.Skip();
}

TopoHost * TopoCanvas::findTopoHost(const wxPoint& pt) {
	wxList::compatibility_iterator node = topoHosts.GetFirst();
	while (node) {
		TopoHost * topoHost = (TopoHost*)node->GetData();
		if (topoHost->IsChosen(pt))
			return topoHost;
		node = node->GetNext();
	}
	return NULL;
}

TopoHost * TopoCanvas::findOverlappedHost(TopoHost * host) {
	wxList::compatibility_iterator node = topoHosts.GetFirst();
	while (node) {
		TopoHost * topoHost = (TopoHost*)node->GetData();
		if (topoHost == host) {
			node = node->GetNext();
			continue;
		}
		if (topoHost->IsOverlap(*host))
			return topoHost;
		node = node->GetNext();
	}
	return NULL;
}

void TopoCanvas::RefreshCanvas() {
	wxClientDC dc(this);
	wxBufferedDC bufferedDC(&dc, this->GetVirtualSize());
	bufferedDC.SetBackground(*wxWHITE_BRUSH);
	bufferedDC.Clear();
	//DoPrepareDC(dc);
	wxList::compatibility_iterator node = topoHosts.GetFirst();
	while (node) {
		TopoHost * topoHost = (TopoHost*)node->GetData();
		bufferedDC.DrawBitmap(topoHost->GetBitmap(), CalcScrolledPosition(topoHost->GetPoint()));
		node = node->GetNext();
	}
	//this->Update();
}

/* ����Ͷ�ȡ���� */

#include <MySnmp/Command/HostCommand.h>
#include <wx/xml/xml.h>

bool TopoCanvas::LoadFromXml(const wxString& xmlfile) {
	/* ��ȡ����ǰ��ɾ�����е�Host */
	wxList::compatibility_iterator node = topoHosts.GetFirst();
	while (node) {
		TopoHost * topoHost = (TopoHost*)node->GetData();
		DeleteHostCommand command(topoHost->GetHostId());
		command.Execute();
		delete topoHost;
		node = node->GetNext();
	}
	topoHosts.Clear();
	RefreshCanvas();

	wxXmlDocument xmlDoc(xmlfile);
	wxXmlNode * root = xmlDoc.GetRoot();
	if (root->GetName() != "topo")
		return false;
	/* �������⻭����С */
	wxSize size;
	size.SetWidth(wxAtoi(root->GetAttribute("width")));
	size.SetHeight(wxAtoi(root->GetAttribute("height")));
	this->SetVirtualSize(size);
	this->Scroll(0, 0);
	/* ������� */
	wxXmlNode * xmlnode = root->GetChildren();
	while (xmlnode) {
		if (xmlnode->GetType() == wxXML_ELEMENT_NODE && xmlnode->GetName() == "host") {
			wxString ipaddress = xmlnode->GetAttribute("ip");
			AddHostCommand command;
			command.SetIpAddress(ipaddress);
			command.SetReadCommunity(xmlnode->GetAttribute("rcommunity"));
			command.SetWriteCommunity(xmlnode->GetAttribute("wcommunity"));
			command.SetRetryTimes(wxAtoi(xmlnode->GetAttribute("retry")));
			command.SetTimeout(wxAtoi(xmlnode->GetAttribute("timeout")));
			command.SetUdpPort(wxAtoi(xmlnode->GetAttribute("udpport")));
			command.SetSnmpVersion(xmlnode->GetAttribute("version"));
			int hostid = command.Execute();

			wxPoint point;
			point.x = wxAtoi(xmlnode->GetAttribute("pointx"));
			point.y = wxAtoi(xmlnode->GetAttribute("pointy"));
			wxString name = xmlnode->GetAttribute("name");
			this->DrawBitmap(hostid, wxBitmap("image/Host.png", wxBITMAP_TYPE_PNG), point, ipaddress, name);
		}
		xmlnode = xmlnode->GetNext();
	}

	this->xmlfile = xmlfile;
	this->isChanged = false;
	return true;
}

bool TopoCanvas::SaveToXml(const wxString& xmlfile) {
	wxString value;
	wxXmlDocument xmlDoc;
	wxXmlNode * docNode = new wxXmlNode(wxXML_DOCUMENT_NODE, "");
	xmlDoc.SetDocumentNode(docNode);
	wxXmlNode * root = new wxXmlNode(docNode, wxXML_ELEMENT_NODE, "topo");
	value.Printf("%d", this->GetVirtualSize().GetWidth());
	root->AddAttribute("width", value);
	value.Printf("%d", this->GetVirtualSize().GetHeight());
	root->AddAttribute("height", value);

	wxList::compatibility_iterator node = topoHosts.GetFirst();

	while (node) {
		TopoHost * topoHost = (TopoHost*)node->GetData();
		wxXmlNode * hostXmlNode = new wxXmlNode(root, wxXML_ELEMENT_NODE, "host");
		value.Printf("%d", topoHost->GetHostId());
		hostXmlNode->AddAttribute("id", value);
		hostXmlNode->AddAttribute("name", topoHost->GetName());
		value.Printf("%d", topoHost->point.x);
		hostXmlNode->AddAttribute("pointx", value);
		value.Printf("%d", topoHost->point.y);
		hostXmlNode->AddAttribute("pointy", value);
		HostInfoCommand command(topoHost->GetHostId(), HostInfoCommand::COMMAND_READ);
		value.Printf("%d", command.GetUdpPort());
		hostXmlNode->AddAttribute("udpport", value);
		value.Printf("%d", command.GetTimeout());
		hostXmlNode->AddAttribute("timeout", value);
		value.Printf("%d", command.GetRetryTimes());
		hostXmlNode->AddAttribute("retry", value);
		hostXmlNode->AddAttribute("ip", command.GetIpAddress());
		hostXmlNode->AddAttribute("rcommunity", command.GetReadCommunity());
		hostXmlNode->AddAttribute("wcommunity", command.GetWriteCommunity());
		hostXmlNode->AddAttribute("version", command.GetSnmpVersion());
		node = node->GetNext();
	}
	xmlDoc.Save(xmlfile);
	this->xmlfile = xmlfile;
	this->isChanged = false;
	return true;
}