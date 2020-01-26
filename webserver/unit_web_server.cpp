//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

//------------------------------------------------------------------------------
//
// Example: HTTP server, asynchronous
//
//------------------------------------------------------------------------------

#include <algorithm>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/config.hpp>
#include <boost/container/flat_map.hpp>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <streambuf>
#include <string>
#include <thread>
#include <vector>

#include "units/units.hpp"

namespace beast = boost::beast; // from <boost/beast.hpp>
namespace http = beast::http; // from <boost/beast/http.hpp>
namespace net = boost::asio; // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp; // from <boost/asio/ip/tcp.hpp>

static std::string as_string(double val)
{
    std::ostringstream str;
    str << std::setprecision(6) << val;
    return str.str();
}

static std::string loadFile(const std::string& fileName)
{
    std::ifstream t(fileName);
    return std::string((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
}

static const std::string index_page = loadFile("index.html");
static const std::string response_page = loadFile("convert.html");
static const std::string response_json =
    "{\n\"measurement\":\"$M1$\",\n\"units\":\"$U1$\",\n\"value\":\"$VALUE$\"\n}";

//decode a uri to clean up a string, convert character codes in a uri to the original character
static std::string uri_decode(beast::string_view str)
{
    std::string ret;
    size_t len = str.length();

    for (size_t ii = 0; ii < len; ii++) {
        if (str[ii] != '%') {
            if (str[ii] == '+')
                ret.push_back(' ');
            else
                ret.push_back(str[ii]);
        } else {
            unsigned int spchar;
            auto converted = sscanf(std::string(str.substr(ii + 1, 2)).c_str(), "%x", &spchar);
            if (converted == 1) {
                ret.push_back(static_cast<char>(spchar));
                ii = ii + 2;
            } else {
                ret.push_back(str[ii]);
            }
        }
    }
    return ret;
}

// function to extract the request parameters and clean up the target
static std::pair<beast::string_view, boost::container::flat_map<beast::string_view, std::string>>
    process_request_parameters(beast::string_view target, beast::string_view body)
{
    std::pair<beast::string_view, boost::container::flat_map<beast::string_view, std::string>>
        results;
    auto param_mark = target.find('?');
    if (param_mark != beast::string_view::npos) {
        results.first = target.substr(1, param_mark - 1);
        target = target.substr(param_mark + 1);
    } else {
        results.first = target;
        target.clear();
    }
    if (results.first.starts_with('/')) {
        results.first.remove_prefix(1);
    }

    std::vector<beast::string_view> parameters;
    if (!target.empty()) {
        auto splitloc = target.find_first_of('&');
        while (splitloc != beast::string_view::npos) {
            parameters.push_back(target.substr(0, splitloc));
            target = target.substr(splitloc + 1);
            splitloc = target.find_first_of('&');
        }
        if (!target.empty()) {
            parameters.push_back(target);
        }
    }
    if (!body.empty()) {
        auto splitloc = body.find_first_of('&');
        while (splitloc != beast::string_view::npos) {
            parameters.push_back(body.substr(0, splitloc));
            body = body.substr(splitloc + 1);
            splitloc = body.find_first_of('&');
        }
        if (!body.empty()) {
            parameters.push_back(body);
        }
    }

    for (auto& param : parameters) {
        auto eq_loc = param.find_first_of('=');
        results.second[param.substr(0, eq_loc)] = uri_decode(param.substr(eq_loc + 1));
    }
    return results;
}

// This function produces an HTTP response for the given
// request. The type of the response object depends on the
// contents of the request, so the interface requires the
// caller to pass a generic lambda for receiving the response.
template<class Body, class Allocator, class Send>
void handle_request(http::request<Body, http::basic_fields<Allocator>>&& req, Send&& send)
{
    // Returns a bad request response
    auto const bad_request = [&req](beast::string_view why) {
        http::response<http::string_body> res{http::status::bad_request, req.version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, "text/html");
        res.keep_alive(req.keep_alive());
        res.body() = std::string(why);
        res.prepare_payload();
        return res;
    };

    // Returns a not found response
    auto const not_found = [&req](beast::string_view target) {
        http::response<http::string_body> res{http::status::not_found, req.version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, "text/html");
        res.keep_alive(req.keep_alive());
        res.body() = std::string(target) + "' was not found.";
        res.prepare_payload();
        return res;
    };

    // Returns a server error response
    /*
    auto const server_error = [&req](beast::string_view what) {
        http::response<http::string_body> res{http::status::internal_server_error, req.version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, "text/html");
        res.keep_alive(req.keep_alive());
        res.body() = "An error occurred: '" + std::string(what) + "'";
        res.prepare_payload();
        return res;
    };
	*/
    // generate the main page
    auto const main_page = [&req]() {
        http::response<http::string_body> res{http::status::ok, req.version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, "text/html");
        res.keep_alive(req.keep_alive());
        if (req.method() != http::verb::head) {
            res.body() = index_page;
            res.prepare_payload();
        } else {
            res.set(http::field::content_length, index_page.size());
        }
        return res;
    };

    // generate a conversion response
    auto const conversion_response =
        [&req](const std::string& value, const std::string& M1, const std::string& U1) {
            http::response<http::string_body> res{http::status::ok, req.version()};
            res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
            res.set(http::field::content_type, "text/html");
            res.keep_alive(req.keep_alive());
            auto resp = response_page;
            auto v = resp.find("$M1$");
            while (v != std::string::npos) {
                resp.replace(v, 4, M1);
                v = resp.find("$M1$");
            }

            v = resp.find("$U1$", v + 4);
            while (v != std::string::npos) {
                resp.replace(v, 4, U1);
                v = resp.find("$U1$");
            }
            v = resp.find("$VALUE$", v + 4);
            while (v != std::string::npos) {
                resp.replace(v, 7, value);
                v = resp.find("$VALUE$");
            }

            if (req.method() != http::verb::head) {
                res.body() = resp;
                res.prepare_payload();
            } else {
                res.set(http::field::content_length, resp.size());
            }
            return res;
        };
    // generate a conversion response
    auto const conversion_response_trivial = [&req](const std::string& value) {
        http::response<http::string_body> res{http::status::ok, req.version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, "text/plain");
        res.keep_alive(req.keep_alive());
        if (req.method() != http::verb::head) {
            res.body() = value;
            res.prepare_payload();
        } else {
            res.set(http::field::content_length, value.size());
        }
        return res;
    };

    // generate a conversion response
    auto const conversion_response_json =
        [&req](const std::string& value, const std::string& M1, const std::string& U1) {
            http::response<http::string_body> res{http::status::ok, req.version()};
            res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
            res.set(http::field::content_type, "application/json");
            res.keep_alive(req.keep_alive());
            auto resp = response_json;
            auto v = resp.find("$M1$");
            resp.replace(v, 4, M1);
            v = resp.find("$U1$", v + 4);
            resp.replace(v, 4, U1);
            v = resp.find("$VALUE$", v + 4);
            resp.replace(v, 7, value);

            if (req.method() != http::verb::head) {
                res.body() = resp;
                res.prepare_payload();
            } else {
                res.set(http::field::content_length, resp.size());
            }

            return res;
        };

    switch (req.method()) {
        case http::verb::head:
        case http::verb::post:
        case http::verb::get:
            break;
        default:
            return send(bad_request("Unknown HTTP-method"));
    }
    beast::string_view target(req.target());
    if (target == "/" || target == "/index.html") {
        return send(main_page());
    }

    if (target.compare(0, 8, "/convert") != 0) {
        return send(not_found(target));
    }

    auto reqpr = process_request_parameters(target, req.body());
    std::string measurement;
    std::string toUnits;
    auto& fields = reqpr.second;
    if (fields.find("measurement") != fields.end()) {
        measurement = fields["measurement"];
        if (measurement.size() > 256) {
            return send(bad_request("measurement string size exceeds limits of 256 characters"));
        }
    }
    if (fields.find("units") != fields.end()) {
        toUnits = fields["units"];
        if (toUnits.size() > 256) {
            return send(
                bad_request("conversion units string size exceeds limits of 256 characters"));
        }
    }
    if (reqpr.first == "convert") {
        auto meas = units::measurement_from_string(measurement);
        auto u2 = units::unit_from_string(toUnits);
        double V = meas.value_as(u2);
        return send(conversion_response(as_string(V), measurement, toUnits));
    } else if (reqpr.first == "convert_trivial") {
        auto meas = units::measurement_from_string(measurement);
        auto u2 = units::unit_from_string(toUnits);
        double V = meas.value_as(u2);
        return send(conversion_response_trivial(as_string(V)));
    } else if (reqpr.first == "convert_json") {
        auto meas = units::measurement_from_string(measurement);
        auto u2 = units::unit_from_string(toUnits);
        double V = meas.value_as(u2);
        return send(conversion_response_json(as_string(V), measurement, toUnits));
    }
    return send(bad_request("#unknown"));
}

//------------------------------------------------------------------------------

// Report a failure
void fail(beast::error_code ec, char const* what)
{
    std::cerr << what << ": " << ec.message() << "\n";
}

// Handles an HTTP server connection
class session : public std::enable_shared_from_this<session> {
    // This is the C++11 equivalent of a generic lambda.
    // The function object is used to send an HTTP message.
    struct send_lambda {
        session& self_;

        explicit send_lambda(session& self) : self_(self) {}

        template<bool isRequest, class Body, class Fields>
        void operator()(http::message<isRequest, Body, Fields>&& msg) const
        {
            // The lifetime of the message has to extend
            // for the duration of the async operation so
            // we use a shared_ptr to manage it.
            auto sp = std::make_shared<http::message<isRequest, Body, Fields>>(std::move(msg));

            // Store a type-erased version of the shared
            // pointer in the class to keep it alive.
            self_.res_ = sp;

            // Write the response
            http::async_write(
                self_.stream_,
                *sp,
                beast::bind_front_handler(
                    &session::on_write, self_.shared_from_this(), sp->need_eof()));
        }
    };

    beast::tcp_stream stream_;
    beast::flat_buffer buffer_;
    http::request<http::string_body> req_;
    std::shared_ptr<void> res_;
    send_lambda lambda_;

  public:
    // Take ownership of the stream
    explicit session(tcp::socket&& socket) : stream_(std::move(socket)), lambda_(*this) {}

    // Start the asynchronous operation
    void run() { do_read(); }

    void do_read()
    {
        // Make the request empty before reading,
        // otherwise the operation behavior is undefined.
        req_ = {};

        // Set the timeout.
        stream_.expires_after(std::chrono::seconds(30));

        // Read a request
        http::async_read(
            stream_,
            buffer_,
            req_,
            beast::bind_front_handler(&session::on_read, shared_from_this()));
    }

    void on_read(beast::error_code ec, std::size_t bytes_transferred)
    {
        boost::ignore_unused(bytes_transferred);

        // This means they closed the connection
        if (ec == http::error::end_of_stream) return do_close();

        if (ec) {
            if (beast::error::timeout != ec) {
                fail(ec, "read");
            }
            return;
        }

        // Send the response
        handle_request(std::move(req_), lambda_);
    }

    void on_write(bool close, beast::error_code ec, std::size_t bytes_transferred)
    {
        boost::ignore_unused(bytes_transferred);

        if (ec) return fail(ec, "write");

        if (close) {
            // This means we should close the connection, usually because
            // the response indicated the "Connection: close" semantic.
            return do_close();
        }

        // We're done with the response so delete it
        res_ = nullptr;

        // Read another request
        do_read();
    }

    void do_close()
    {
        // Send a TCP shutdown
        beast::error_code ec;
        stream_.socket().shutdown(tcp::socket::shutdown_send, ec);

        // At this point the connection is closed gracefully
    }
};

//------------------------------------------------------------------------------

// Accepts incoming connections and launches the sessions
class listener : public std::enable_shared_from_this<listener> {
    net::io_context& ioc_;
    tcp::acceptor acceptor_;

  public:
    listener(net::io_context& ioc, tcp::endpoint endpoint) :
        ioc_(ioc), acceptor_(net::make_strand(ioc))
    {
        beast::error_code ec;

        // Open the acceptor
        acceptor_.open(endpoint.protocol(), ec);
        if (ec) {
            fail(ec, "open");
            return;
        }

        // Allow address reuse
        acceptor_.set_option(net::socket_base::reuse_address(true), ec);
        if (ec) {
            fail(ec, "set_option");
            return;
        }

        // Bind to the server address
        acceptor_.bind(endpoint, ec);
        if (ec) {
            fail(ec, "bind");
            return;
        }

        // Start listening for connections
        acceptor_.listen(net::socket_base::max_listen_connections, ec);
        if (ec) {
            fail(ec, "listen");
            return;
        }
    }

    // Start accepting incoming connections
    void run() { do_accept(); }

  private:
    void do_accept()
    {
        // The new connection gets its own strand
        acceptor_.async_accept(
            net::make_strand(ioc_),
            beast::bind_front_handler(&listener::on_accept, shared_from_this()));
    }

    void on_accept(beast::error_code ec, tcp::socket socket)
    {
        if (ec) {
            fail(ec, "accept");
        } else {
            // Create the session and run it
            std::make_shared<session>(std::move(socket))->run();
        }

        // Accept another connection
        do_accept();
    }
};

//------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    // Check command line arguments.
    if (argc < 3) {
        std::cerr << "Usage: unit_web_server <address> <port>\n"
                  << "Example:\n"
                  << "    unit_web_server 0.0.0.0 80\n";
        return EXIT_FAILURE;
    }
    auto const address = net::ip::make_address(argv[1]);
    auto const port = static_cast<unsigned short>(std::atoi(argv[2]));

    // The io_context is required for all I/O
    net::io_context ioc{1};

    // Create and launch a listening port
    std::make_shared<listener>(ioc, tcp::endpoint{address, port})->run();

    // Run the I/O service
    ioc.run();

    return EXIT_SUCCESS;
}
