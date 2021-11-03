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
	void OnWorkerEvent(wxThreadEvent& event);
	void CreateListener(std::string hostval, std::string pathval, web::http::method methodval);
};

class MyWorkerThread : public wxThread
{
public:
	// For console app, use wxAppConsole* instead of MyFrame*
	MyWorkerThread(wxAppConsole* app, std::string hostval, std::string pathval, web::http::method methodval);
	// thread execution starts here
	virtual void* Entry();

	// called when the thread exits - whether it terminates normally or is
	// stopped with Delete() (but not when it is Kill()ed!)
	virtual void OnExit();
	static void handle_get(http_request message);
	static void handle_post(http_request message);
private:
	static wxAppConsole* _app;
	std::string _host;
	std::string _path;
    web::http::method _method;
};
wxAppConsole* MyWorkerThread::_app = nullptr;

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
	Connect(ID_THREAD, wxEVT_THREAD, wxThreadEventHandler(MyApp::OnWorkerEvent));
	cout<<("Hello svr rest thread")<<endl;
    this->CreateListener("http://localhost:8080", "/cpprest/svrpost", methods::POST);
	this->CreateListener("http://localhost:8080", "/cpprest/svrget", methods::GET);
    
    return true;
}

void MyApp::CreateListener(std::string hostval, std::string pathval, web::http::method methodval)
{
	MyWorkerThread* thread = new MyWorkerThread(this, hostval, pathval, methodval);
	if (thread->Create() != wxTHREAD_NO_ERROR)
	{
		//printf("Can't create thread!");
		return;
	}
	thread->Run();
}
// --------------------------------------------------------
// MyWorkerThread
MyWorkerThread::MyWorkerThread(wxAppConsole* app, std::string hostval, std::string pathval, web::http::method methodval)
	: wxThread(), _host(hostval), _path(pathval), _method(methodval)
{
	_app = app;
}

void MyWorkerThread::OnExit()
{
}

wxThread::ExitCode MyWorkerThread::Entry()
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
    if (_method == methods::GET) {
        listener.support(_method, handle_get);
    }
    else {
        listener.support(_method, handle_post);
    }
  //  listener.support(methods::GET, handle_get);
 	//listener.support(methods::POST,handle_post);
 	//listener.support(methods::PUT,handle_put);
 	//listener.support(methods::DEL,handle_del);
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

void MyApp::OnWorkerEvent(wxThreadEvent& event)
{
	string str = event.GetString().ToStdString();
	cout << str << endl;
}

 void MyWorkerThread::handle_get(http_request message){
 	cout<<"Handle get: "<<message.to_string()<<endl;
 	json::value jsonObject;
 	jsonObject[U("x")] = json::value::string(U("GetA"));
 	jsonObject[U("y")] = json::value::string(U("GetB"));
 	message.reply(status_codes::OK,jsonObject);
    string rv;
    rv = "get entry";
    wxThreadEvent event(wxEVT_THREAD, ID_THREAD);
    event.SetString(rv);
    wxQueueEvent(_app, event.Clone());
 }

 void MyWorkerThread::handle_post(http_request message){
 	cout<<"Handle post: "<<message.to_string()<<endl;
 	json::value jsonObject;
 	try{
 		message.extract_json()
 			.then([&jsonObject](json::value jo){
 				cout<<"Val:"<<jo.serialize() << endl;
 				jsonObject = jo;
 			})
 			.wait();
 	}
 	catch (const std::exception & e) {
 		printf("Error exception:%s\n", e.what());
 	}
 	jsonObject[U("cherry")] = json::value::string(U("C"));
 	message.reply(status_codes::OK,jsonObject);
 }

// void handle_put(http_request message){
// 	cout<<"Handle post: "<<message.to_string()<<endl;
// 	string rep = U("PUT handled");
// 	message.reply(status_codes::OK,rep);
// }

// void handle_del(http_request message){
// 	cout<<"Handle delete: "<<message.to_string()<<endl;
// 	string rep = U("DELETE handled");
// 	message.reply(status_codes::OK,rep);
// }

// int main(int argc, char* argv[])
// {
// 	web::http::experimental::listener::http_listener 
// 		listener(U("http://localhost:8080/cpprest/svrRest"));
// 	listener.support(methods::GET,handle_get);
// 	listener.support(methods::POST,handle_post);
// 	listener.support(methods::PUT,handle_put);
// 	listener.support(methods::DEL,handle_del);
// 	try{
// 		listener.open()
// 			.then([&listener](){printf("\nStarting svrRest\n");})
// 			.wait();
// 		while(true);
// 	}
// 	catch (const std::exception & e) {
// 		printf("Error exception:%s\n", e.what());
// 	}
// 	return 0;
// }