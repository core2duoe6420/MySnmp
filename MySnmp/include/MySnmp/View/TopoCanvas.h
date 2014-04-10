#ifndef __TOPOCANVAS_H
#define __TOPOCANVAS_H

#include <wx/wx.h>
#include <vector>

namespace mysnmp {
	class TopoHost;

	class TopoCanvas : public wxScrolledCanvas {
	public:
		TopoCanvas(wxWindow * parent, const wxSize& virtualSize, int scrollRate);
		virtual ~TopoCanvas();
		void DrawBitmap(int hostId, const wxBitmap& host, const wxPoint& point,
						const wxString& ipAddress, const wxString& name);
		TopoHost * GetHost(const wxString& ipAddress);

		TopoHost * GetChosenHost() { return chosenTopoHost; }
		wxMenu * GetPopMenu() { return menuPop; }

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

		wxMenu * menuPop;
		TopoHost * chosenTopoHost;

		int dragStatus;
		TopoHost * draggedTopoHost;
		wxPoint dragStartLogicalPoint;

		DECLARE_EVENT_TABLE();
	};

	class TopoHost : public wxObject {
		friend class TopoCanvas;
	private:
		wxBitmap bitmap;
		wxPoint point;
		wxString name;
		wxString ipAddress;
		int hostId;
		std::vector<TopoHost *> connectedHost;

		wxRect GetRect() const {
			return wxRect(point.x, point.y, bitmap.GetWidth(), bitmap.GetHeight());
		}

		bool IsChosen(const wxPoint& pt) {
			wxRect rect(this->GetRect());
			return rect.Contains(pt.x, pt.y);
		}

		bool IsOverlap(const TopoHost& other) {
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

	public:
		TopoHost(int hostId, const wxBitmap& bitmap, const wxPoint& point,
				 TopoCanvas * canvas, const wxString& ipAddress, const wxString& name = wxEmptyString);

		int GetHostId() const { return hostId; }
		void SetName(const wxString& name) { this->name = name; }
		wxString& GetName() { return name; }
		wxBitmap& GetBitmap() { return bitmap; }
		wxPoint& GetPoint() { return point; }
		wxString& GetIpAddress() { return ipAddress; }
		void SetPoint(const wxPoint& pt) { this->point = pt; }

	};
}
#endif