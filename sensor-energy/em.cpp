// File: em.cpp
// Description: Sampler for energy monitor
// WebSite: http://cool-emerald.blogspot.com
// MIT License (https://opensource.org/licenses/MIT)
// Copyright (c) 2018 Yan Naing Aye

#include <wx/wx.h>
#include <string>
#include <fstream>
#include "ce_spi.h"
class MyFrame : public wxFrame
{
public:
    MyFrame(const wxString& title);
	void OnTimer(wxTimerEvent& event);
	int Write(string mes);
private:
	int m_data[60];
	wxStaticText *m_lbl;
	wxTextCtrl *m_txt;
	CE_SPI *m_ai;
	wxTimer m_timer;
	wxDECLARE_EVENT_TABLE();
};
const int ID_LABEL = 102;
const int ID_TXT = 103;
const int ID_TIMER = 104;
wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
EVT_TIMER(ID_TIMER, MyFrame::OnTimer)
wxEND_EVENT_TABLE()
MyFrame::MyFrame(const wxString& title)
       : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(250, 150))
		, m_timer(this, ID_TIMER)
{
	m_lbl = new wxStaticText(this, ID_LABEL, wxT("Analog input:"), wxPoint(20,15), wxSize(100,25));
	m_txt = new wxTextCtrl(this, ID_TXT, wxT("0"), wxPoint(140,10), wxSize(50,25));	
	m_ai = new CE_SPI();
	m_ai->Begin();
	Centre();
	for (int i = 0; i < 60; i++) m_data[i] = 0;
	m_timer.Start(1000);
}

class MyApp : public wxApp
{
  public:
    virtual bool OnInit();
};

IMPLEMENT_APP(MyApp)
bool MyApp::OnInit()
{
    MyFrame *myFrame = new MyFrame(wxT("Energy Monitor"));
    myFrame->Show(true);

    return true;
}
void MyFrame::OnTimer(wxTimerEvent& WXUNUSED(event))
{
	static int i = 0;
	i=++i%60;	

	int x=0;
	m_ai->tx[0]=0x01;//Send start bit - 0000 0001
	m_ai->tx[1]=0x80;//read channel 0  - | s/~d | d2 d1 d0 | x x x x |
	// b7= single/~differential = 1 for single ended
	// d2 d1 d0 = 0 for channel 0
	m_ai->tx[2]=0x00; //  don't care = x x x x x x x x
	m_ai->Transfer(3);
	x = m_ai->rx[1]<<8 | m_ai->rx[2]; //read channel 0
	x&=0x03FF;//mask out invalid bits

	float a = 205; // offset = 1023.0 / 5 = 205 N/V;
	float k = 28;  // scale factor = (20 * 230) / (0.8*205) => 28 VA/N
	float val = x;
	float w = k*(val - a);

	m_data[i] = int(w);
	string str = "";
	for (int j = 0, k = i+1;; j++,k++) {
		k %= 60;
		str += "[" + to_string(j + 1) + "," + to_string(m_data[k]) + "]";
		if (j >= 59) break;
		str += ",";
	}
	m_txt->Clear();
	m_txt->AppendText(wxString::Format(wxT("%d"), m_data[i]));
	this->Write(str);
}
int MyFrame::Write(string mes)
{
	ofstream wfile;
	int r = -1;
	string fpath = "./eseconds.php";
	wfile.open(fpath.c_str(), std::fstream::out);
	if (wfile.is_open()) {
		wfile << mes << endl;
		r = 0;
	}
	wfile.close();
	return r;
}