#include <MySnmp/View/TopoCanvas.h>

#include <MySnmp/debug.h>
#include <wx/dcbuffer.h>

using namespace mysnmp;

BEGIN_EVENT_TABLE(TopoCanvas, wxScrolledCanvas)
EVT_MOUSE_EVENTS(TopoCanvas::OnMouseEvents)
EVT_PAINT(TopoCanvas::OnPaint)
EVT_SIZE(TopoCanvas::OnSize)
END_EVENT_TABLE()


TopoCanvas::TopoCanvas(wxWindow * parent, const wxSize& virtualSize, int scrollRate) :
wxScrolledCanvas(parent), scrollRate(scrollRate), dragStatus(0){
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

void TopoCanvas::DrawBitmap(const wxBitmap& host, const wxPoint& point) {
	TopoHost * topoHost = new TopoHost(host, point, this, "这是一台新主机");
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