// File: getClient.cpp
// Author: Yan Naing Aye
// Date: 2020-Aug-14
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

using namespace utility; // Common utilities like string conversions
using namespace web; // Common features like URIs.
using namespace web::http; // Common HTTP functionality
using namespace web::http::client; // HTTP client features
using namespace concurrency::streams; // Asynchronous streams
using namespace std;
int main(int argc, char* argv[])
{
	// http client
	web::http::client::http_client client(U("https://yan9a.github.io"));
	// web::http::client::http_client client(U("http://localhost:8080"));

	// the pplx task 'resp' gives a placeholder for a value 
	// that will be available in the future
	pplx::task<web::http::http_response> resp = client.request(
		web::http::methods::GET, U("/rpi/iot/cpprest/getClient/j1.json"));
		// web::http::methods::GET, U("/cpprest/svrRest"));

	// attached a handler to be invoked when resp.is_done() is true
	pplx::task<json::value> jv = resp.then([=]
	(pplx::task<web::http::http_response> task) {
		web::http::http_response response = task.get();

		// Check the status code.
		if (response.status_code() != 200) {
			throw std::runtime_error("Returned " + 
				std::to_string(response.status_code()));
		}
		std::cout << "Returned " + 
			std::to_string(response.status_code())<<endl;

		// Convert the response body to JSON object.
		pplx::task<json::value> jsonObject = response.extract_json();
		return jsonObject;
	});

	// handler to be invoked when json has been extracted
	jv.then([=](json::value jo){
		cout<<"Val:"<<jo.serialize() << endl;
	});

	// Wait until json value is ready
	try {
		jv.wait();
	}
	catch (const std::exception & e) {
		printf("Error exception:%s\n", e.what());
	}
	return 0;
}