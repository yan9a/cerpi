// File: svrRestThread.cpp
// Author: Yan Naing Aye
// Date: 2021-Nov-02
// References
//  http://cool-emerald.blogspot.com/2020/10/c-rest-sdk.html
//  https://github.com/Microsoft/cpprestsdk/wiki/Getting-Started-Tutorial
//  https://github.com/Microsoft/cpprestsdk/wiki/Programming-with-Tasks
//  https://github.com/Meenapintu/Restweb
#include <wx/wx.h>
#include <string>
#include <iostream>
#include <cpprest/http_listener.h>
#include <cpprest/filestream.h>
#include <cpprest/uri.h>
#include <cpprest/json.h>

#if defined (__WIN32__) || defined(_WIN32) || defined(WIN32) || defined(__WINDOWS__) || defined(__TOS_WIN__)
#define CEWIN
#endif

using namespace utility; // Common utilities like string conversions
using namespace web; // Common features like URIs.
using namespace web::http; // Common HTTP functionality
using namespace web::http::experimental::listener; // HTTP listener
using namespace concurrency::streams; // Asynchronous streams
using namespace std;

const int ID_THREAD = 101;

class MyApp : public wxAppConsole
{
  public:
    virtual bool OnInit();
	void OnHttpEvent(wxThreadEvent& event);	
};

class MyRestApi : public wxThread
{
public:
	// For console app, use wxAppConsole* instead of MyFrame*
	MyRestApi(const std::string& hostval, const std::string& pathval,
		const web::http::method& methodval, std::function<void(web::http::http_request)> handler);
	static void CreateRestApi(wxAppConsole* app);
private:
	static wxAppConsole* _app;
	std::string _host;
	std::string _path;
    web::http::method _method;
	std::function<void(web::http::http_request)> _handler;
	// thread execution starts here
	virtual void* Entry();
	// called when the thread exits - whether it terminates normally or is
	// stopped with Delete() (but not when it is Kill()ed!)
	virtual void OnExit();
	static void handle_get(http_request message);
	static void handle_post(http_request message);
	static void CreateListener(const std::string& hostval, const std::string& pathval,
		const web::http::method& methodval, std::function<void(web::http::http_request)> handler);
};
wxAppConsole* MyRestApi::_app = nullptr;

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
	Connect(ID_THREAD, wxEVT_THREAD, wxThreadEventHandler(MyApp::OnHttpEvent));
	cout<<("Starting multithreaded C++ REST SDK server ... \n")<<endl;
	MyRestApi::CreateRestApi(this);
    return true;
}

void MyApp::OnHttpEvent(wxThreadEvent& event)
{
	string str = event.GetString().ToStdString();
	cout << str << endl;
}

// --------------------------------------------------------
// MyRestApi
MyRestApi::MyRestApi(const std::string &hostval, const std::string &pathval, 
	const web::http::method &methodval, std::function<void (web::http::http_request)> handler)
	: wxThread(), _host(hostval), _path(pathval), _method(methodval), _handler(handler)
{
	
}

void MyRestApi::OnExit()
{
}

wxThread::ExitCode MyRestApi::Entry()
{
#ifndef CEWIN //CE_WINDOWS
    string uhost = _host;
    string upath = _path;
#else
    string_t uhost = utility::conversions::to_string_t(_host);
    string_t upath = utility::conversions::to_string_t(_path);
#endif // !CE_WINDOWS
	web::http::experimental::listener::http_listener 
 		listener(uhost+upath);
    listener.support(_method, _handler);
 	try{
 		listener.open()
 			.then([&listener](){
                //printf("\nStarting svrRest\n");
                })
 			    .wait();
 		while(true);
 	}
 	catch (const std::exception & e) {
 		printf("Error exception:%s\n", e.what());
 	}
	return NULL;
}

void MyRestApi::CreateListener(const std::string& hostval, const std::string& pathval,
	const web::http::method& methodval, std::function<void(web::http::http_request)> handler)
{
	MyRestApi* thread = new MyRestApi(hostval, pathval, methodval, handler);
	if (thread->Create() != wxTHREAD_NO_ERROR)
	{
		//printf("Can't create thread!");
		return;
	}
	thread->Run();
}

void MyRestApi::CreateRestApi(wxAppConsole* app)
{
	_app = app;
	MyRestApi::CreateListener("http://localhost:8080", "/cpprest/sg", methods::GET, MyRestApi::handle_get);
	MyRestApi::CreateListener("http://localhost:8080", "/cpprest/sp", methods::POST, MyRestApi::handle_post);
}

void MyRestApi::handle_get(http_request message) {
	//cout << "Handle get: " << message.to_string() << endl;
	json::value jsonObject;
	jsonObject[U("x")] = json::value::string(U("Getx"));
	jsonObject[U("y")] = json::value::string(U("Gety"));
	message.reply(status_codes::OK, jsonObject);
	string rv = "Rx Get";
	wxThreadEvent event(wxEVT_THREAD, ID_THREAD);
	event.SetString(rv);
	wxQueueEvent(_app, event.Clone());
}

void MyRestApi::handle_post(http_request message) {
	//cout << "Handle post: " << message.to_string() << endl;
	json::value jsonObject;
	try {
		message.extract_json()
			.then([&jsonObject](json::value jo) {
			//cout << "Val:" << jo.serialize() << endl;
			jsonObject = jo;
				})
			.wait();
	}
	catch (const std::exception & e) {
		printf("Error exception:%s\n", e.what());
	}
	jsonObject[U("z")] = json::value::string(U("Postz"));
	message.reply(status_codes::OK, jsonObject);
	string rv = "Rx Post";
	wxThreadEvent event(wxEVT_THREAD, ID_THREAD);
	event.SetString(rv);
	wxQueueEvent(_app, event.Clone());
}