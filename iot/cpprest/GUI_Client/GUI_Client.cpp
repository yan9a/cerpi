// File: GUI_Client.cpp
// Author: Yan Naing Aye
// Date: 2021-Nov-01
// References
//  http://cool-emerald.blogspot.com/2020/10/c-rest-sdk.html
//  https://github.com/Microsoft/cpprestsdk/wiki/Getting-Started-Tutorial
//  http://www.atakansarioglu.com
//      /easy-quick-start-cplusplus-rest-client-example-cpprest-tutorial/
//  https://github.com/Microsoft/cpprestsdk/wiki/Programming-with-Tasks

#include <iostream>
#include <cpprest/http_client.h>
#include <cpprest/uri.h>
#include <cpprest/json.h>
#include <wx/wx.h>

#if defined (__WIN32__) || defined(_WIN32) || defined(WIN32) || defined(__WINDOWS__) || defined(__TOS_WIN__)
#define CEWIN
#endif

using namespace utility; // Common utilities like string conversions
using namespace web; // Common features like URIs.
using namespace web::http; // Common HTTP functionality
using namespace web::http::client; // HTTP client features
using namespace concurrency::streams; // Asynchronous streams
using namespace std;

class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};

class MyFrame : public wxFrame
{
public:
    MyFrame(const wxString& title);
    wxButton* btnSend;
    wxTextCtrl* txtSend;
    wxTextCtrl* txtRx;
    wxButton* btnClear;
	wxTextCtrl* txtRemotehost;
	wxTextCtrl* txtPath;
	wxChoice* cboMethod;
    void OnSend(wxCommandEvent& event);
    void OnClearText(wxCommandEvent& event);
    void OnWorkerEvent(wxThreadEvent& event);
	void OnCboConn(wxCommandEvent& event);
private:
	uint8_t _connType;
	std::string GetHost();
	std::string GetContent();
	std::string GetPath();
};
// IDs for the controls and the menu commands
const int ID_BTNSEND = 101;
const int ID_TXTSEND = 102;
const int ID_TXTRX = 103;
const int ID_BTNCLEAR = 104;
const int ID_THREAD = 105;
const int ID_CBO_METHOD = 106;
const int ID_TXT_REMOTEHOST = 107;
const int ID_TXT_PATH = 108;

class MyWorkerThread : public wxThread
{
public:
	// For console app, use wxAppConsole* instead of MyFrame*
    MyWorkerThread(MyFrame* frame, std::string remotehost, std::string path, std::string data = "");
    // thread execution starts here
    virtual void* Entry();

    // called when the thread exits - whether it terminates normally or is
    // stopped with Delete() (but not when it is Kill()ed!)
    virtual void OnExit();
    bool RestGet(std::string remotehost, std::string path, std::string& retVal);
    bool RestPost(std::string remotehost, std::string path, std::string data, std::string& retVal);
private:
    MyFrame* _frame;
    std::string _remotehost;
    std::string _path;
    std::string _data;
};


IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
    if (!wxApp::OnInit())
        return false;

    MyFrame *frame = new MyFrame(wxT("C++ REST Client"));
    frame->Show(true);

    return true;
}

MyFrame::MyFrame(const wxString& title)
    : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(640, 480),
        wxDEFAULT_FRAME_STYLE^ wxRESIZE_BORDER)
{
    btnSend = new wxButton(this, ID_BTNSEND, wxT("Send"), wxPoint(10, 50), wxSize(100, 25));
    txtSend = new wxTextCtrl(this, ID_TXTSEND, wxT("{\"apple\":\"A\",\"banana\":\"B\"}"), wxPoint(130, 50), wxSize(480, 25));
	txtSend->SetToolTip(wxT("Content to post"));
    txtRx = new wxTextCtrl(this, ID_TXTRX, wxT(""), wxPoint(10, 85), wxSize(600, 315), wxTE_MULTILINE);
    btnClear = new wxButton(this, ID_BTNCLEAR, wxT("Clear"), wxPoint(10, 410), wxSize(100, 25));
    Connect(ID_BTNSEND, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MyFrame::OnSend));
    Connect(ID_BTNCLEAR, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MyFrame::OnClearText));	
	Connect(ID_THREAD, wxEVT_THREAD, wxThreadEventHandler(MyFrame::OnWorkerEvent));
	txtRemotehost = new wxTextCtrl(this, ID_TXT_REMOTEHOST, wxT("http://localhost:8080"), wxPoint(130, 10), wxSize(240, 25));
	txtRemotehost->SetToolTip(wxT("Remotehost"));
	txtPath = new wxTextCtrl(this, ID_TXT_PATH, wxT("/cpprest/svrRest"), wxPoint(380, 10), wxSize(240, 25));
	txtPath->SetToolTip(wxT("Path"));
	//wxStaticText* lblRemote = new wxStaticText(this, -1, wxT("HOST:"));
	wxArrayString connList;
	connList.Add("GET");
	connList.Add("POST");
	cboMethod = new wxChoice(this, ID_CBO_METHOD, wxPoint(10, 10), wxSize(100, 25), connList, wxCB_SORT);
	cboMethod->SetSelection(0);
	this->_connType = 0;
	Connect(ID_CBO_METHOD, wxEVT_CHOICE, wxCommandEventHandler(MyFrame::OnCboConn));
	if (this->_connType == 0) txtSend->Disable();
    Centre();
}

void MyFrame::OnSend(wxCommandEvent& WXUNUSED(event))
{
	string remotehost = GetHost();
	string path = GetPath();
	string data = "";
	if(this->_connType) data = GetContent();
	this->txtRx->AppendText(wxString::Format(wxT("HTTP %s:  %s%s > %s\n"), string(_connType ? "POST" : "GET"),remotehost,path,data));
	MyWorkerThread* thread = new MyWorkerThread(this, remotehost,path, data);
	if (thread->Create() != wxTHREAD_NO_ERROR)
	{
		//printf("Can't create thread!");
		return;
	}
	thread->Run();
}

void MyFrame::OnClearText(wxCommandEvent& WXUNUSED(event))
{
    txtRx->Clear();
}

void MyFrame::OnWorkerEvent(wxThreadEvent& event)
{
	string str = event.GetString().ToStdString();
	this->txtRx->AppendText(wxString::Format(wxT("HTTP Rx:  %s\n"), str));
}

void MyFrame::OnCboConn(wxCommandEvent& event)
{
	_connType = (uint8_t)cboMethod->GetSelection();
	this->txtRx->AppendText(string("HTTP method: ") + (_connType ? "POST\n" : "GET\n"));
	if (this->_connType==0) {
		txtSend->Disable();
	}
	else {
		txtSend->Enable();
	}
}

std::string MyFrame::GetHost()
{
	wxString ws = this->txtRemotehost->GetValue();
	std::string str = ws.ToStdString();
	str.erase(remove_if(str.begin(), str.end(), [](char ch) {return !(ch >= 0x21 && ch <= 0x7E); }), str.end());
	return str;
}

std::string MyFrame::GetContent()
{
	wxString ws = this->txtSend->GetValue();
	std::string str = ws.ToStdString();
	str.erase(remove_if(str.begin(), str.end(), [](char ch) {return !(ch >= 0x20 && ch <= 0x7E); }), str.end());
	return str;
}

std::string MyFrame::GetPath()
{
	wxString ws = this->txtPath->GetValue();
	std::string str = ws.ToStdString();
	str.erase(remove_if(str.begin(), str.end(), [](char ch) {return !(ch >= 0x20 && ch <= 0x7E); }), str.end());
	return str;
}
// --------------------------------------------------------
// MyWorkerThread

bool MyWorkerThread::RestGet(std::string remotehost, std::string path, std::string& retVal)
{
	bool r = false;
#ifndef CEWIN // CE_WINDOWS
	string host = remotehost;
	string upath = path;
#else
	string_t host = utility::conversions::to_string_t(remotehost);
	string_t upath = utility::conversions::to_string_t(path);
#endif // !CE_WINDOWS
	json::value j1;

	auto requestJson = http_client(host)
		.request(methods::GET, uri_builder(upath).to_string())
		// Get the response.
		.then([](http_response response) {
		// Check the status code.
		if (response.status_code() != 200) {
			throw std::runtime_error("Returned " +
				std::to_string(response.status_code()));
		}
		// std::cout << "Returned " + std::to_string(response.status_code()) << endl;
		// Convert the response body to JSON object.
		return response.extract_json();
			})
		// Get the data field.
				.then([&j1](json::value jsonObject) {
				j1 = jsonObject;
				return jsonObject;
					});

			// Wait for the concurrent tasks to finish.
			try {
				requestJson.wait();
				retVal = utility::conversions::to_utf8string(j1.serialize());
				r = true;
			}
			catch (const std::exception& e) {
				string em = e.what();
				retVal = em;
			}
			return r;
}

bool MyWorkerThread::RestPost(std::string remotehost, std::string path, std::string data, std::string& retVal)
{
	bool r = false;
#ifndef CEWIN //CE_WINDOWS
	string host = remotehost;
	string upath = path;
	string jstr = data;
#else
	string_t host = utility::conversions::to_string_t(remotehost);
	string_t upath = utility::conversions::to_string_t(path);
	string_t jstr = utility::conversions::to_string_t(data);
#endif // !CE_WINDOWS
	//utility::stringstream_t s;
	//s << data;
	json::value jpost = json::value::parse(jstr);
	json::value j1;

	auto requestJson = http_client(host)
		.request(methods::POST, uri_builder(upath).to_string(),
			jpost.serialize(), U("application/json"))
		// Get the response.
		.then([](http_response response) {
		// Check the status code.
		if (response.status_code() != 200) {
			throw std::runtime_error("Returned " +
				std::to_string(response.status_code()));
		}
		// std::cout << "Returned " + std::to_string(response.status_code()) << endl;
		// Convert the response body to JSON object.
		return response.extract_json();
			})
		// Get the data field.
				.then([&j1](json::value jsonObject) {
				j1 = jsonObject;
				return jsonObject;
					});

			// Wait for the concurrent tasks to finish.
			try {
				requestJson.wait();
				retVal = utility::conversions::to_utf8string(j1.serialize());
				r = true;
			}
			catch (const std::exception& e) {
				string em = e.what();
				retVal = em;
			}
			return r;
}

MyWorkerThread::MyWorkerThread(MyFrame* frame, std::string remotehost, std::string path, std::string data)
	: wxThread(), _frame(frame), _remotehost(remotehost), _path(path), _data(data)
{

}

void MyWorkerThread::OnExit()
{
}

wxThread::ExitCode MyWorkerThread::Entry()
{
	string rv;
	if (_data == "") {
		this->RestGet(_remotehost, _path, rv);
	}
	else {
		this->RestPost(_remotehost, _path, _data, rv);
	}
	wxThreadEvent event(wxEVT_THREAD, ID_THREAD);
	event.SetString(rv);
	wxQueueEvent(_frame, event.Clone());
	return NULL;
}
