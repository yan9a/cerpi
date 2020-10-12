// File: postClient.cpp
// Author: Yan Naing Aye
// Date: 2020-Oct-01
// References
//  https://github.com/Microsoft/cpprestsdk/wiki/Getting-Started-Tutorial
//  http://www.atakansarioglu.com/easy-quick-start-cplusplus-rest-client-example-cpprest-tutorial/
//  https://github.com/Microsoft/cpprestsdk/wiki/Programming-with-Tasks

#include <iostream>
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include <cpprest/uri.h>
#include <cpprest/json.h>

using namespace utility;                    // Common utilities like string conversions
using namespace web;                        // Common features like URIs.
using namespace web::http;                  // Common HTTP functionality
using namespace web::http::client;          // HTTP client features
using namespace concurrency::streams;       // Asynchronous streams
using namespace std;

int main(int argc, char* argv[])
{
	json::value jsonObject;
	jsonObject[U("apple")] = json::value::string(U("A"));
	jsonObject[U("banana")] = json::value::string(U("B"));

	// Make a POST request.
	// auto requestJson = http_client(U("http://localhost:80"))
		// .request(methods::POST, uri_builder(U("cpprestsdk/cpprest/postClient/j2.php")).to_string(), 
	auto requestJson = http_client(U("http://localhost:8080"))
		.request(methods::POST, uri_builder(U("/cpprestsdk/cpprest/svrRest")).to_string(),
			jsonObject.serialize(), U("application/json"))
		// Get the response.
		.then([](http_response response) {
				// Check the status code.
				if (response.status_code() != 200) {
					throw std::runtime_error("Returned " + std::to_string(response.status_code()));
				}
				std::cout << "Returned " + std::to_string(response.status_code())<<endl;
				// Convert the response body to JSON object.
				return response.extract_json();
		})
		// Get the data field.
		.then([](json::value jsonObject) {
            cout<<"Val:"<<jsonObject.serialize() << endl;
			return jsonObject;
		});

		// Wait for the concurrent tasks to finish.
		try {
			requestJson.wait();
		}
		catch (const std::exception & e) {
			printf("Error exception:%s\n", e.what());
		}
	return 0;
}