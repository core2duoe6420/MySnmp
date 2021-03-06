#ifndef __TOPOCANVAS_H
#define __TOPOCANVAS_H

#include <wx/scrolwin.h>
#include <wx/bitmap.h>

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
		bool RemoveHost(int hostId);
		void RefreshCanvas();
		TopoHost * GetChosenHost() { return chosenTopoHost; }

		wxMenu * GetPopMenu() { return menuPop; }

		bool LoadFromXml(const wxString& xmlfile);
		bool SaveToXml(const wxString& xmlfile);

		bool IsChanged() const { return isChanged; }
		bool HasSavedToFile() const { return xmlfile != wxEmptyString; }
		const wxString& GetXmlFile() const { return xmlfile; }
		void SetChanged() { isChanged = true; }

	private:
		/* 下面两个字段用于保存和读取拓扑 */
		bool isChanged;
		wxString xmlfile;

		wxList topoHosts;

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
		wxBitmap originBitmap;
		TopoCanvas * canvas;
		wxPoint point;
		wxString name;
		wxString ipAddress;
		int hostId;
		std::vector<TopoHost *> connectedHost;

		void drawNameOnBitmap();

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
		void SetName(const wxString& name) { this->name = name; drawNameOnBitmap(); }
		wxString& GetName() { return name; }
		wxBitmap& GetBitmap() { return bitmap; }
		wxPoint& GetPoint() { return point; }
		wxString& GetIpAddress() { return ipAddress; }
		void SetIpAddress(const wxString& ip) { this->ipAddress = ip; }
		void SetPoint(const wxPoint& pt) { this->point = pt; }

	};
}
#endif