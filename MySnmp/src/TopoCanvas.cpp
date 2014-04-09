#include <MySnmp/View/TopoCanvas.h>

#include <MySnmp/debug.h>
#include <wx/dcbuffer.h>
#include <wx/dcgraph.h>

using namespace mysnmp;

BEGIN_EVENT_TABLE(TopoCanvas, wxScrolledCanvas)
EVT_MOUSE_EVENTS(TopoCanvas::OnMouseEvents)
EVT_PAINT(TopoCanvas::OnPaint)
EVT_SIZE(TopoCanvas::OnSize)
END_EVENT_TABLE()

TopoHost::TopoHost(int hostId, const wxBitmap& bitmap, const wxPoint& point,
TopoCanvas * canvas, const wxString& ipAddress, const wxString& name) :
hostId(hostId), bitmap(bitmap), point(point), ipAddress(ipAddress), name(name) {
	/* lblName仅仅用来获得文字大小 */
	wxStaticBox * lblName = lblName = new wxStaticBox(canvas, wxID_ANY, name);
	wxSize lblSize = lblName->GetEffectiveMinSize();
	/* 功成身退 */
	lblName->Destroy();

	int lblx, lbly;
	/* 做一些小修正 */
	/* Ubuntu下修正量较小 */
#ifdef _WIN32
	int x_modvalue = 7;
#else
	int x_modvalue = 4;
#endif
	lblx = (bitmap.GetSize().GetWidth() - lblSize.GetWidth()) / 2 + x_modvalue;
	lbly = lblSize.GetHeight() - 5;
	if (lblx < 0)
		lblx = 0;

	wxImage image = bitmap.ConvertToImage();
	/* Ubuntu下透明bitmap用GCDC仍然无法绘制字
	* Windows下用GCDC效果差的一塌糊涂
	* 我完全不知道如何解决这些问题
	*/
#ifdef _WIN32
	image.Resize(bitmap.GetSize() + wxSize(0, lbly), wxPoint(0, lbly));
#else
	image.Resize(bitmap.GetSize() + wxSize(0, lbly), wxPoint(0, lbly), 255, 255, 255);
#endif
	this->bitmap = wxBitmap(image);

	wxMemoryDC memDC(this->bitmap);
	wxGCDC gcdc(memDC);
	wxFont font(9, wxFONTFAMILY_TELETYPE, wxNORMAL, wxBOLD);
	gcdc.SetFont(font);
	gcdc.DrawText(name, lblx, 0);
	memDC.SelectObject(wxNullBitmap);
}

TopoCanvas::TopoCanvas(wxWindow * parent, const wxSize& virtualSize, int scrollRate) :
wxScrolledCanvas(parent), scrollRate(scrollRate), dragStatus(0) {
	this->SetVirtualSize(virtualSize);
	this->SetScrollRate(scrollRate, scrollRate);
}

TopoCanvas::~TopoCanvas() {
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
	if (event.LeftDown()) {
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
		refreshCanvas();
	} else if (event.Dragging() && dragStatus != TEST_DRAG_NONE) {
		if (dragStatus == TEST_DRAG_START) {
			int tolerance = 2;
			wxPoint eventLogicalPoint = CalcUnscrolledPosition(event.GetPosition());
			int dx = abs(eventLogicalPoint.x - dragStartLogicalPoint.x);
			int dy = abs(eventLogicalPoint.y - dragStartLogicalPoint.y);
			if (dx <= tolerance && dy <= tolerance)
				return;

			dragStatus = TEST_DRAG_DRAGGING;
			draggedTopoHost->SetPoint(CalcUnscrolledPosition(event.GetPosition()) - dragStartLogicalPoint);
			refreshCanvas();
		} else if (dragStatus == TEST_DRAG_DRAGGING) {
			wxPoint eventLogicalPoint = CalcUnscrolledPosition(event.GetPosition());
			draggedTopoHost->SetPoint(eventLogicalPoint - dragStartLogicalPoint);

			OnClientEdgeAndScroll(event.GetPosition(), 30);
			OnVirtualEdgeAndExpand(event.GetPosition(), 30, 100);
			refreshCanvas();
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
	refreshCanvas();
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

void TopoCanvas::refreshCanvas() {
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