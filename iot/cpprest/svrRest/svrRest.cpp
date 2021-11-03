// File: svrRest.cpp
// Author: Yan Naing Aye
// Date: 2020-Oct-06
// References
//  http://cool-emerald.blogspot.com/2020/10/c-rest-sdk.html
//  https://github.com/Microsoft/cpprestsdk/wiki/Getting-Started-Tutorial
//  https://github.com/Microsoft/cpprestsdk/wiki/Programming-with-Tasks
//  https://github.com/Meenapintu/Restweb

#include <iostream>
#include <cpprest/http_listener.h>
#include <cpprest/filestream.h>
#include <cpprest/uri.h>
#include <cpprest/json.h>

using namespace utility; // Common utilities like string conversions
using namespace web; // Common features like URIs.
using namespace web::http; // Common HTTP functionality
using namespace web::http::experimental::listener; // HTTP listener
using namespace concurrency::streams; // Asynchronous streams
using namespace std;

void handle_get(http_request message){
	cout<<"Handle get: "<<message.to_string()<<endl;
	json::value jsonObject;
	jsonObject[U("apple")] = json::value::string(U("A"));
	jsonObject[U("banana")] = json::value::string(U("B"));
	message.reply(status_codes::OK,jsonObject);
}

void handle_post(http_request message){
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

void handle_put(http_request message){
	cout<<"Handle post: "<<message.to_string()<<endl;
	string rep = U("PUT handled");
	message.reply(status_codes::OK,rep);
}

void handle_del(http_request message){
	cout<<"Handle delete: "<<message.to_string()<<endl;
	string rep = U("DELETE handled");
	message.reply(status_codes::OK,rep);
}

int main(int argc, char* argv[])
{
	web::http::experimental::listener::http_listener 
		listener(U("http://localhost:8080/cpprest/svrRest"));
	listener.support(methods::GET,handle_get);
	listener.support(methods::POST,handle_post);
	listener.support(methods::PUT,handle_put);
	listener.support(methods::DEL,handle_del);
	try{
		listener.open()
			.then([&listener](){printf("\nStarting svrRest\n");})
			.wait();
		while(true);
	}
	catch (const std::exception & e) {
		printf("Error exception:%s\n", e.what());
	}
	return 0;
}