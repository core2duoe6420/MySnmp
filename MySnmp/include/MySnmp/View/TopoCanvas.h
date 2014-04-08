#ifndef __TOPOCANVAS_H
#define __TOPOCANVAS_H

#include <wx/wx.h>
#include <vector>
#include <wx/dragimag.h>

namespace mysnmp {
	class TopoHost : public wxObject {
	private:
		wxBitmap bitmap;
		wxPoint point;
		std::vector<TopoHost *> connectedHost;

	public:
		TopoHost(const wxBitmap& bitmap, const wxPoint& point) :
			bitmap(bitmap), point(point) {}

		inline wxRect GetRect() const {
			return wxRect(point.x, point.y, bitmap.GetWidth(), bitmap.GetHeight());
		}

		inline wxBitmap& GetBitmap() {
			return bitmap;
		}

		inline wxPoint& GetPoint() {
			return point;
		}

		inline void SetPoint(const wxPoint& pt) {
			this->point = pt;
		}

		inline bool IsChosen(const wxPoint& pt) {
			wxRect rect(this->GetRect());
			return rect.Contains(pt.x, pt.y);
		}

		inline bool IsOverlap(const TopoHost& other) {
			wxRect thisRect = this->GetRect();
			wxRect otherRect = other.GetRect();
			if (thisRect.Contains(otherRect.GetTopLeft()) ||
				thisRect.Contains(otherRect.GetTopRight()) ||
				thisRect.Contains(otherRect.GetBottomLeft()) ||
				thisRect.Contains(otherRect.GetBottomRight())) {
				return true;
			}
			return false;
		}
	};


	class TopoCanvas : public wxScrolledCanvas {
	public:
		TopoCanvas(wxWindow * parent, const wxSize& virtualSize, int scrollRate);
		virtual ~TopoCanvas();
		void DrawBitmap(const wxBitmap& host, const wxPoint& point);
		
	private:

		wxList topoHosts;
		void refreshCanvas();
		TopoHost * findTopoHost(const wxPoint& pt);
		TopoHost * findOverlappedHost(TopoHost * host);
		void OnClientEdgeAndScroll(const wxPoint& eventPoint, int threshold);
		void OnVirtualEdgeAndExpand(const wxPoint& eventPoint, int threshold, int delta);
		/* events */
		void OnMouseEvents(wxMouseEvent& event);
		void OnPaint(wxPaintEvent& event);
		void OnSize(wxSizeEvent& event);

		int scrollRate;
		int dragStatus;
		TopoHost * draggedTopoHost;
		wxPoint dragStartLogicalPoint;

		DECLARE_EVENT_TABLE();
	};

}
#endif