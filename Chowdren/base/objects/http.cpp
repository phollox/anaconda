// Copyright (c) Mathias Kaerlev 2012-2015.
//
// This file is part of Anaconda.
//
// Anaconda is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Anaconda is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Anaconda.  If not, see <http://www.gnu.org/licenses/>.

#include "http.h"
#include <iostream>
#include "thread.h"

// HTTPObject

#ifdef CHOWDREN_IS_DESKTOP

#include "staticlibs/happyhttp/happyhttp.cpp"
#include "types.h"

static const char* http_headers[] = 
{
    "Connection", "close",
    "Content-type", "application/x-www-form-urlencoded",
    "Accept", "text/plain",
    0
};

struct HTTPRequest
{
    std::string host, path, args;
};

static happyhttp::Connection conn;
static Thread http_thread;
static volatile bool has_http_thread = false;

static volatile int current_id = 0;
static volatile int thread_id = 0;
static volatile bool has_request = false;
static HTTPRequest http_request;
static Mutex request_mutex;

static volatile bool has_response = false;
static std::string http_response;
static Mutex response_mutex;

static volatile bool http_running = false;
static std::string http_data;

#define HTTP_PREFIX "http://"

static void on_data(const happyhttp::Response * r, void * userdata,
                    const unsigned char * data, int n)
{
    http_data.append((const char *)data, n);
}

static void on_done(const happyhttp::Response * r, void * userdata)
{
    if (thread_id != current_id)
        return;
    response_mutex.lock();
    http_response = http_data;
    has_response = true;
    response_mutex.unlock();
    http_data.clear();
}

int http_thread_func(void * data)
{
    while (http_running) {
        request_mutex.lock();
        if (!has_request) {
            request_mutex.unlock();
            // XXX should use condition variable
            platform_sleep(0.25);
            continue;
        }

        HTTPRequest request = http_request;
        has_request = false;
        thread_id = current_id;
        request_mutex.unlock();

		std::cout << "Got request" << std::endl;
        conn.close();
        conn.set_host(request.host, 80);
        conn.setcallbacks(NULL, on_data, on_done, NULL);
        conn.request("POST", request.path.c_str(), http_headers,
                     (const unsigned char*)&request.args[0],
                     request.args.size());
        while (conn.outstanding())
            conn.pump();
    }
    has_http_thread = false;
    return 0;
}

HTTPObject::HTTPObject(int x, int y, int type_id)
: FrameObject(x, y, type_id), done(false)
{
    http_running = true;
}

HTTPObject::~HTTPObject()
{
    http_running = false;
}

void HTTPObject::add_post(const std::string & name, const std::string & value)
{
    if (args.empty())
        args += name + "=" + value;
    else
        args += "&" + name + "=" + value;
}

void HTTPObject::get(const std::string & url)
{
    int start = 0;
	if (url.compare(0, sizeof(HTTP_PREFIX) - 1,
                    HTTP_PREFIX, sizeof(HTTP_PREFIX) - 1) == 0)
        start += sizeof(HTTP_PREFIX)-1;

    size_t end = url.find_first_of('/', start);
    if (end == std::string::npos)
        end = url.size();

    std::string host = url.substr(start, end-start);
    std::string path = url.substr(end);
    if (path.empty())
        path = "/";

    if (!has_http_thread) {
        http_thread.start(http_thread_func, NULL);
        http_thread.detach();
        has_http_thread = true;
    }

    request_mutex.lock();
    current_id++;
    http_request.host = host;
    http_request.path = path;
    http_request.args = args;
    has_request = true;
    request_mutex.unlock();

    args.clear();

}

void HTTPObject::update()
{
    done = false;

    response_mutex.lock();
    if (has_response) {
        done = true;
        has_response = false;
        value = http_response;
    }
    response_mutex.unlock();
}

#else

HTTPObject::HTTPObject(int x, int y, int type_id)
: FrameObject(x, y, type_id), done(false)
{
}

HTTPObject::~HTTPObject()
{
}

void HTTPObject::add_post(const std::string & name, const std::string & value)
{
}


void HTTPObject::get(const std::string & url)
{
}

void HTTPObject::update()
{
}

#endif
