#ifndef __TOPOCANVAS_H
#define __TOPOCANVAS_H

#include <wx/wx.h>
#include <vector>
#include <wx/graphics.h>
#include "wx/dcgraph.h" 
namespace mysnmp {
	class TopoHost;

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

	class TopoHost : public wxObject {
	private:
		wxBitmap bitmap;
		wxPoint point;
		wxString name;
		
		std::vector<TopoHost *> connectedHost;
	public:
		TopoHost(const wxBitmap& bitmap, const wxPoint& point,
			TopoCanvas * canvas, const wxString& name = wxEmptyString) :
			bitmap(bitmap), point(point), name(name) {
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

		inline void SetName(const wxString& name) {
			this->name = name;
		}

		inline wxString& GetName() {
			return name;
		}

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
}
#endif