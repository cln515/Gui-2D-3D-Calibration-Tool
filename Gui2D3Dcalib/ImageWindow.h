#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "wx/colordlg.h"
#include "wx/image.h"
#include "wx/artprov.h"
#include "wx/dcbuffer.h"
#include "wx/dcgraph.h"
#include "wx/overlay.h"
#include "wx/graphics.h"
#include "wx/filename.h"
#include "wx/metafile.h"
#include "wx/settings.h"
#include "wx\string.h"

#include <opencv2\opencv.hpp>

using namespace std;

class MyCanvas;
typedef map<int,cv::Point2f> I_MAP;

class DrawFrame: public wxFrame{
public:
	DrawFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
	I_MAP getImap();


	   MyCanvas   *m_canvas;
	   int         m_backgroundMode;
    int         m_textureBackground;
    wxMappingMode m_mapMode;
    double      m_xUserScale;
    double      m_yUserScale;
    int         m_xLogicalOrigin;
    int         m_yLogicalOrigin;
    bool        m_xAxisReversed,
                m_yAxisReversed;
    wxColour    m_colourForeground,    // these are _text_ colours
                m_colourBackground;
    wxBrush     m_backgroundBrush;
};


class MyCanvas: public wxScrolledWindow
{
public:
	MyCanvas( DrawFrame *parent );
	void OnPaint(wxPaintEvent &event);
	void OnMouseEvent(wxMouseEvent& event);
	void Draw(wxDC& dc);
	void DrawDefault(wxDC& dc);
	void DrawMovie(wxDC& dc);
	void OnIdle(wxIdleEvent& event);
	void setShowMode(int m_show_){m_show=m_show_;};
	bool ReadImage(string filePath){
		newImagePath=filePath;
		clearImap();
		mat=cv::imread(newImagePath);
		if(mat.cols>0)m_bNewImage=true;
		return m_bNewImage;
	};
	I_MAP getImap_(){return pt;};
	void clearImap(){pt.clear();};
	cv::Size getImageSize(){return cv::Size(mat.cols,mat.rows);};
	cv::Mat getImageClone() { return mat.clone(); };
private:
	DrawFrame *m_owner;
	wxGraphicsRenderer* m_renderer;
    bool         m_useBuffer;
	int          m_show;
    wxBitmap     m_smile_bmp;
    wxIcon       m_std_icon;
    bool         m_clip;
    wxOverlay    m_overlay;
    bool         m_rubberBand;
    wxPoint      m_anchorpoint;
    wxPoint      m_currentpoint;
	bool		m_bNewImage;
	string newImagePath;
	cv::Mat mat;
	wxBitmap image;
	int im_x,im_y;
	double im_rate;
	I_MAP pt;
	//vector<cv::Point2f> proj;
	//vector<cv::Point2f> feat;
	wxDECLARE_EVENT_TABLE();

};