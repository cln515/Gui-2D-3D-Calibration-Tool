#include "ImageWindow.h"

DrawFrame::DrawFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
       : wxFrame((wxFrame *)NULL, wxID_ANY, title, pos, size,
                 wxDEFAULT_FRAME_STYLE | wxNO_FULL_REPAINT_ON_RESIZE)
{
	CreateStatusBar(2);
    SetStatusText(wxT(""));
	m_canvas = new MyCanvas( this );
    //m_canvas->SetScrollbars( 10, 10, 280, 140 );
}


wxBEGIN_EVENT_TABLE(MyCanvas, wxScrolledWindow)
    EVT_PAINT  (MyCanvas::OnPaint)
	EVT_MOUSE_EVENTS(MyCanvas::OnMouseEvent)
	EVT_IDLE(MyCanvas::OnIdle)
wxEND_EVENT_TABLE()

MyCanvas::MyCanvas(DrawFrame *parent)
        : wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                           wxHSCROLL | wxVSCROLL | wxNO_FULL_REPAINT_ON_RESIZE)
{
	
    m_owner = parent;
    m_show = 0;
//    m_smile_bmp = wxBitmap(smile_xpm);
    m_std_icon = wxArtProvider::GetIcon(wxART_INFORMATION);
    m_clip = false;
    m_rubberBand = false;
    m_renderer = NULL;
	m_bNewImage=false;
    m_useBuffer = true;
	im_x=0;
	im_y=0;
	im_rate=1.0;
	mat=cv::imread("..\\assets\\1461460139457.jpg");

	if (mat.channels() == 3)
	   cv::cvtColor(mat, mat, CV_BGR2RGB);
	
	  // cv::Mat‚Ì‰æ‘fî•ñ‚ðwxImage‚É“n‚µ‚ÄwxBitmap‚É•ÏŠ·‚·‚é
	  wxImage tmp = wxImage(mat.cols, mat.rows, mat.data, TRUE);
	  image = wxBitmap(tmp);
}



void MyCanvas::OnPaint(wxPaintEvent &WXUNUSED(event))
{
    if ( m_useBuffer )
    {
        wxBufferedPaintDC bpdc(this);
        Draw(bpdc);
    }
    else
    {
        wxPaintDC pdc(this);
        Draw(pdc);
    }
}

void MyCanvas::Draw(wxDC& pdc)
{
#if wxUSE_GRAPHICS_CONTEXT
    wxGCDC gdc;

    if ( m_renderer )
    {
        wxGraphicsContext* context;
        if ( wxPaintDC *paintdc = wxDynamicCast(&pdc, wxPaintDC) )
        {
            context = m_renderer->CreateContext(*paintdc);
        }
        else if ( wxMemoryDC *memdc = wxDynamicCast(&pdc, wxMemoryDC) )
        {
            context = m_renderer->CreateContext(*memdc);
        }
#if wxUSE_METAFILE && defined(wxMETAFILE_IS_ENH)
        else if ( wxMetafileDC *metadc = wxDynamicCast(&pdc, wxMetafileDC) )
        {
            context = m_renderer->CreateContext(*metadc);
        }
#endif
        else
        {
            wxFAIL_MSG( "Unknown wxDC kind" );
            return;
        }

        gdc.SetGraphicsContext(context);
    }

    wxDC &dc = m_renderer ? (wxDC&) gdc : (wxDC&) pdc ;
#else
    wxDC &dc = pdc ;
#endif

    PrepareDC(dc);

    m_owner->PrepareDC(dc);

    dc.SetBackgroundMode(true);
    if ( m_owner->m_backgroundBrush.IsOk() )
        dc.SetBackground( m_owner->m_backgroundBrush );
    if ( m_owner->m_colourForeground.IsOk() )
        dc.SetTextForeground( m_owner->m_colourForeground );
    if ( m_owner->m_colourBackground.IsOk() )
        dc.SetTextBackground( m_owner->m_colourBackground );

    if ( m_owner->m_textureBackground) {
        if ( ! m_owner->m_backgroundBrush.IsOk() ) {
            dc.SetBackground(wxBrush(wxColour(0, 128, 0)));
        }
    }

    if ( m_clip )
        dc.SetClippingRegion(100, 100, 100, 100);

    dc.Clear();

    if ( m_owner->m_textureBackground )
    {
        dc.SetPen(*wxMEDIUM_GREY_PEN);
        for ( int i = 0; i < 200; i++ )
            dc.DrawLine(0, i*10, i*10, 0);
    }
	if(m_bNewImage){
		

	if (mat.channels() == 3)
	   cv::cvtColor(mat, mat, CV_BGR2RGB);
	
	  // cv::Mat‚Ì‰æ‘fî•ñ‚ðwxImage‚É“n‚µ‚ÄwxBitmap‚É•ÏŠ·‚·‚é
	  wxImage tmp = wxImage(mat.cols, mat.rows, mat.data, TRUE);
	  image = wxBitmap(tmp);
	  m_bNewImage=false;
	}
    switch ( m_show )
    {
        case 0:
            DrawDefault(dc);
            break;
		case 1:case 2:
            DrawMovie(dc);
            break;
        default:
            break;
    }
}

void MyCanvas::DrawMovie(wxDC& dc)
{
 
	  // cv::Mat‚Ì‰æ‘fî•ñ‚ðwxImage‚É“n‚µ‚ÄwxBitmap‚É•ÏŠ·‚·‚é
	  //wxImage tmp = wxImage(mat.cols, mat.rows, mat.data, TRUE);
	  //image = wxBitmap(tmp);
	  
	dc.SetUserScale(1350/2700.0,1350/2700.0);
	  dc.DrawBitmap(image,0,0);
	dc.SetPen(*wxRED_PEN);
	dc.SetBrush(*wxTRANSPARENT_BRUSH);
	 
	  
}

void MyCanvas::DrawDefault(wxDC& dc)
{
    // Draw circle centered at the origin, then flood fill it with a different
    // color. Done with a wxMemoryDC because Blit (used by generic
    // wxDoFloodFill) from a window that is being painted gives unpredictable
    // results on wxGTK
    //{
    //    wxImage img(21, 21, false);
    //    img.Clear(1);
    //    wxBitmap bmp(img);
    //    {
    //        wxMemoryDC mdc(bmp);
    //        mdc.SetBrush(dc.GetBrush());
    //        mdc.SetPen(dc.GetPen());
    //        mdc.DrawCircle(10, 10, 10);
    //        wxColour c;
    //        if (mdc.GetPixel(11, 11, &c))
    //        {
    //            mdc.SetBrush(wxColour(128, 128, 0));
    //            mdc.FloodFill(11, 11, c, wxFLOOD_SURFACE);
    //        }
    //    }
    //    bmp.SetMask(new wxMask(bmp, wxColour(1, 1, 1)));
    //    dc.DrawBitmap(bmp, -10, -10, true);
    //}
	
	dc.SetUserScale(im_rate,im_rate);
	dc.DrawBitmap(image,im_x/im_rate,im_y/im_rate);
	wxBrush brush;
	int unit=64;

	for(auto itr=pt.begin();itr!=pt.end();itr++){
		int i=itr->first;
		int r=i/16;
		int g=(i%16)/4;
		int b=(i%4);
		dc.SetPen(*wxTRANSPARENT_PEN);
		brush.SetColour(r*unit+63,g*unit+63,b*unit+63);
		dc.SetBrush(brush);
		dc.DrawCircle(itr->second.x+im_x/im_rate,itr->second.y+im_y/im_rate,7.0/im_rate);
	}

	/*dc.SetPen(*wxRED_PEN);
	dc.SetBrush(*wxTRANSPARENT_BRUSH);
	dc.DrawCircle(100.0,100.0,18.0);*/
//    dc.DrawCheckMark(5, 80, 15, 15);
//    dc.DrawCheckMark(25, 80, 30, 30);
//    dc.DrawCheckMark(60, 80, 60, 60);
//
//    // this is the test for "blitting bitmap into DC damages selected brush" bug
//    wxCoord rectSize = m_std_icon.GetWidth() + 10;
//    wxCoord x = 100;
//    dc.SetPen(*wxTRANSPARENT_PEN);
//    dc.SetBrush( *wxGREEN_BRUSH );
//    dc.DrawRectangle(x, 10, rectSize, rectSize);
//    dc.DrawBitmap(m_std_icon, x + 5, 15, true);
//    x += rectSize + 10;
//    dc.DrawRectangle(x, 10, rectSize, rectSize);
//    dc.DrawIcon(m_std_icon, x + 5, 15);
//    x += rectSize + 10;
//    dc.DrawRectangle(x, 10, rectSize, rectSize);
//
//    // test for "transparent" bitmap drawing (it intersects with the last
//    // rectangle above)
//    //dc.SetBrush( *wxTRANSPARENT_BRUSH );
//
//    if (m_smile_bmp.IsOk())
//        dc.DrawBitmap(m_smile_bmp, x + rectSize - 20, rectSize - 10, true);
//
//    dc.SetBrush( *wxBLACK_BRUSH );
//    dc.DrawRectangle( 0, 160, 1000, 300 );
//
//    // draw lines
//    wxBitmap bitmap(20,70);
//    wxMemoryDC memdc;
//    memdc.SelectObject( bitmap );
//    memdc.SetBrush( *wxBLACK_BRUSH );
//    memdc.SetPen( *wxWHITE_PEN );
//    memdc.DrawRectangle(0,0,20,70);
//    memdc.DrawLine( 10,0,10,70 );
//
//    // to the right
//    wxPen pen = *wxRED_PEN;
//    memdc.SetPen(pen);
//    memdc.DrawLine( 10, 5,10, 5 );
//    memdc.DrawLine( 10,10,11,10 );
//    memdc.DrawLine( 10,15,12,15 );
//    memdc.DrawLine( 10,20,13,20 );
//
///*
//    memdc.SetPen(*wxRED_PEN);
//    memdc.DrawLine( 12, 5,12, 5 );
//    memdc.DrawLine( 12,10,13,10 );
//    memdc.DrawLine( 12,15,14,15 );
//    memdc.DrawLine( 12,20,15,20 );
//*/
//
//    // same to the left
//    memdc.DrawLine( 10,25,10,25 );
//    memdc.DrawLine( 10,30, 9,30 );
//    memdc.DrawLine( 10,35, 8,35 );
//    memdc.DrawLine( 10,40, 7,40 );
//
//    // XOR draw lines
//    dc.SetPen(*wxWHITE_PEN);
//    memdc.SetLogicalFunction( wxINVERT );
//    memdc.SetPen( *wxWHITE_PEN );
//    memdc.DrawLine( 10,50,10,50 );
//    memdc.DrawLine( 10,55,11,55 );
//    memdc.DrawLine( 10,60,12,60 );
//    memdc.DrawLine( 10,65,13,65 );
//
//    memdc.DrawLine( 12,50,12,50 );
//    memdc.DrawLine( 12,55,13,55 );
//    memdc.DrawLine( 12,60,14,60 );
//    memdc.DrawLine( 12,65,15,65 );
//
//    memdc.SelectObject( wxNullBitmap );
//    dc.DrawBitmap( bitmap, 10, 170 );
//    wxImage image = bitmap.ConvertToImage();
//    image.Rescale( 60,210 );
//    bitmap = wxBitmap(image);
//    dc.DrawBitmap( bitmap, 50, 170 );
//
//    // test the rectangle outline drawing - there should be one pixel between
//    // the rect and the lines
//    dc.SetPen(*wxWHITE_PEN);
//    dc.SetBrush( *wxTRANSPARENT_BRUSH );
//    dc.DrawRectangle(150, 170, 49, 29);
//    dc.DrawRectangle(200, 170, 49, 29);
//    dc.SetPen(*wxWHITE_PEN);
//    dc.DrawLine(250, 210, 250, 170);
//    dc.DrawLine(260, 200, 150, 200);
//
//    // test the rectangle filled drawing - there should be one pixel between
//    // the rect and the lines
//    dc.SetPen(*wxTRANSPARENT_PEN);
//    dc.SetBrush( *wxWHITE_BRUSH );
//    dc.DrawRectangle(300, 170, 49, 29);
//    dc.DrawRectangle(350, 170, 49, 29);
//    dc.SetPen(*wxWHITE_PEN);
//    dc.DrawLine(400, 170, 400, 210);
//    dc.DrawLine(300, 200, 410, 200);
//
//    // a few more tests of this kind
//    dc.SetPen(*wxRED_PEN);
//    dc.SetBrush( *wxWHITE_BRUSH );
//    dc.DrawRectangle(300, 220, 1, 1);
//    dc.DrawRectangle(310, 220, 2, 2);
//    dc.DrawRectangle(320, 220, 3, 3);
//    dc.DrawRectangle(330, 220, 4, 4);
//
//    dc.SetPen(*wxTRANSPARENT_PEN);
//    dc.SetBrush( *wxWHITE_BRUSH );
//    dc.DrawRectangle(300, 230, 1, 1);
//    dc.DrawRectangle(310, 230, 2, 2);
//    dc.DrawRectangle(320, 230, 3, 3);
//    dc.DrawRectangle(330, 230, 4, 4);
//
//    // and now for filled rect with outline
//    dc.SetPen(*wxRED_PEN);
//    dc.SetBrush( *wxWHITE_BRUSH );
//    dc.DrawRectangle(500, 170, 49, 29);
//    dc.DrawRectangle(550, 170, 49, 29);
//    dc.SetPen(*wxWHITE_PEN);
//    dc.DrawLine(600, 170, 600, 210);
//    dc.DrawLine(500, 200, 610, 200);
//
//    // test the rectangle outline drawing - there should be one pixel between
//    // the rect and the lines
//    dc.SetPen(*wxWHITE_PEN);
//    dc.SetBrush( *wxTRANSPARENT_BRUSH );
//    dc.DrawRoundedRectangle(150, 270, 49, 29, 6);
//    dc.DrawRoundedRectangle(200, 270, 49, 29, 6);
//    dc.SetPen(*wxWHITE_PEN);
//    dc.DrawLine(250, 270, 250, 310);
//    dc.DrawLine(150, 300, 260, 300);
//
//    // test the rectangle filled drawing - there should be one pixel between
//    // the rect and the lines
//    dc.SetPen(*wxTRANSPARENT_PEN);
//    dc.SetBrush( *wxWHITE_BRUSH );
//    dc.DrawRoundedRectangle(300, 270, 49, 29, 6);
//    dc.DrawRoundedRectangle(350, 270, 49, 29, 6);
//    dc.SetPen(*wxWHITE_PEN);
//    dc.DrawLine(400, 270, 400, 310);
//    dc.DrawLine(300, 300, 410, 300);
//
//    // Added by JACS to demonstrate bizarre behaviour.
//    // With a size of 70, we get a missing red RHS,
//    // and the height is too small, so we get yellow
//    // showing. With a size of 40, it draws as expected:
//    // it just shows a white rectangle with red outline.
//    int totalWidth = 70;
//    int totalHeight = 70;
//    wxBitmap bitmap2(totalWidth, totalHeight);
//
//    wxMemoryDC memdc2;
//    memdc2.SelectObject(bitmap2);
//
//    memdc2.SetBackground(*wxYELLOW_BRUSH);
//    memdc2.Clear();
//
//    // Now draw a white rectangle with red outline. It should
//    // entirely eclipse the yellow background.
//    memdc2.SetPen(*wxRED_PEN);
//    memdc2.SetBrush(*wxWHITE_BRUSH);
//
//    memdc2.DrawRectangle(0, 0, totalWidth, totalHeight);
//
//    memdc2.SetPen(wxNullPen);
//    memdc2.SetBrush(wxNullBrush);
//    memdc2.SelectObject(wxNullBitmap);
//
//    dc.DrawBitmap(bitmap2, 500, 270);
//
//    // Repeat, but draw directly on dc
//    // Draw a yellow rectangle filling the bitmap
//
//    x = 600; int y = 270;
//    dc.SetPen(*wxYELLOW_PEN);
//    dc.SetBrush(*wxYELLOW_BRUSH);
//    dc.DrawRectangle(x, y, totalWidth, totalHeight);
//
//    // Now draw a white rectangle with red outline. It should
//    // entirely eclipse the yellow background.
//    dc.SetPen(*wxRED_PEN);
//    dc.SetBrush(*wxWHITE_BRUSH);
//
//    dc.DrawRectangle(x, y, totalWidth, totalHeight);
}

void MyCanvas::OnIdle(wxIdleEvent& event)
{
	event.RequestMore(true);
//	this->SetCurrent();
	this->Refresh(false);
}

void MyCanvas::OnMouseEvent(wxMouseEvent& event){
	static int dragging = 0;
    static float last_x, last_y;
    // Allow default processing to happen, or else the canvas cannot gain focus
    // (for key events).
    event.Skip();

	if (event.LeftIsDown())//Trans
    {
        if (!dragging)
        {
            dragging = 1;
        }
        else
        {
            double m_trax = (event.GetX() - last_x);
            double m_tray = (event.GetY() - last_y);
			im_x+=m_trax;
			im_y+=m_tray;
        }
        last_x = event.GetX();
        last_y = event.GetY();
    }
	//else if (event.RightIsDown())//Rot
 //   {
 //       if (!dragging)
 //       {
 //           dragging = 1;
 //       }
 //       else
 //       {
 //           double m_roty = (event.GetX() - last_x)*0.01;
 //           double m_rotx = (event.GetY() - last_y)*0.01;
	//		Matrix3d dR=axisRot2R(-m_rotx,-m_roty,0);
	//		R=dR*R;
	//		if(!bOriginCenter)T=dR*T;//rotation by centering T
 //           //Refresh(false);
 //       }
 //       last_x = event.GetX();
 //       last_y = event.GetY();
	//}
	else if (event.RightIsDown())//Rot
    {
		if (!dragging)
        {
            dragging = 1;
			double px=event.GetX()-im_x;
			double py=event.GetY()-im_y;
			px/=im_rate;
	        py/=im_rate;
			if(px>=0&&px<mat.cols&&py>=0&&py<mat.rows){
				bool flag=false;
				for(auto itr=pt.begin();itr!=pt.end();itr++){
					double dx=itr->second.x-px;
					double dy=itr->second.y-py;
					if(dx*dx+dy*dy<4*4){
						pt.erase(itr);
						flag=true;
						break;
					}
				}
				if(!flag){
					int i=0;
					while(pt.find(i)!=pt.end())i++;
					pt.insert(I_MAP::value_type(i,cv::Point2f(px,py)));
				}
			}
        }
	}
	else
    {
        dragging = 0;
    }
	if(event.GetWheelRotation()!=0){
			double m_traz = (event.GetWheelRotation())*0.0001;
			im_rate+=m_traz;
			if(im_rate<0.05)im_rate=0.05;
	}
};

I_MAP DrawFrame::getImap(){I_MAP imap=m_canvas->getImap_();return imap;};