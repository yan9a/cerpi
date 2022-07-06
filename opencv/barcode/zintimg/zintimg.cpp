//File: zintimg.cpp
//Description: to manipulate barcode image using Zint, OpenCV, and wxWidgets
//Author: Yan Naing Aye
//Date: 2022 Jun 28
//MIT License - Copyright (c) 2022 Yan Naing Aye

#include <wx/wx.h>
#include <opencv2/opencv.hpp>
#include <string>
#include <zint.h>
#include <stdio.h>
using namespace std;
using namespace cv;

//-----------------------------------------------------------------------------
//Description: Functions to convert wxImage and OpenCV Mat
wxImage wx_from_mat(Mat &img) {
    Mat im2;
    if(img.channels()==1){cvtColor(img,im2,COLOR_GRAY2RGB);}
	else if (img.channels() == 4) { cvtColor(img, im2, COLOR_BGRA2RGB);}
    else {cvtColor(img,im2,COLOR_BGR2RGB);}
	long imsize = im2.rows*im2.cols*im2.channels();
    wxImage wx(im2.cols, im2.rows,(unsigned char*)malloc(imsize), false);
	unsigned char* s=im2.data;
	unsigned char* d=wx.GetData();
	for (long i = 0; i < imsize; i++) { d[i] = s[i];}
    return wx;
}

Mat mat_from_wx(wxImage &wx) {
    Mat im2(Size(wx.GetWidth(),wx.GetHeight()),CV_8UC3,wx.GetData());
    cvtColor(im2,im2,COLOR_RGB2BGR);
    return im2;
}

//-----------------------------------------------------------------------------

class MyFrame : public wxFrame
{
	wxStaticBitmap *wximg;
public:
	MyFrame(const wxString& title);

};
MyFrame::MyFrame(const wxString& title)
	: wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(512, 600))
{
	Centre();
	Mat imcv1(400,600, CV_8UC3, CV_RGB(128, 128, 128));	
	//From opencv to wx
	string str = "Channels:" + to_string(imcv1.channels());
	putText(imcv1, str, Point(100, 100), FONT_HERSHEY_PLAIN, 4.0, CV_RGB(128, 0, 128), 4.0);
	wxBitmap imwx1 = wx_from_mat(imcv1);

	// wximg = new wxStaticBitmap(this, wxID_ANY, wxBitmap(wxT("./wximg.png"), wxBITMAP_TYPE_PNG), wxPoint(256, 0), wxSize(512,512));
	wximg = new wxStaticBitmap(this, wxID_ANY, imwx1, wxPoint(256, 0), wxSize(512,512));

	struct zint_symbol *my_symbol;
	my_symbol = ZBarcode_Create();
	if(my_symbol != NULL){
		// printf("Symbol successfully created!\n");
	}
	ZBarcode_Encode(my_symbol, (unsigned char*)"87654321",0);
	ZBarcode_Buffer(my_symbol,0);

	int height = my_symbol->bitmap_height;
	int width = my_symbol->bitmap_width;
	char * img = my_symbol->bitmap;
	Mat imcv2(height,width, CV_8UC3, img);	
	wxBitmap imwx2 = wx_from_mat(imcv2);
	wximg->SetBitmap(imwx2);
	ZBarcode_Delete(my_symbol);

}

class MyApp : public wxApp
{
public:
	virtual bool OnInit();
};

IMPLEMENT_APP(MyApp)
bool MyApp::OnInit()
{
	if (!wxApp::OnInit())
		return false;
	wxInitAllImageHandlers();
	MyFrame *frame = new MyFrame(wxT("Barcode Image Manipulation"));
	frame->Show(true);

	return true;
}

