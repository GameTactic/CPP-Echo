#include <set>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <iostream>
#include <nlohmann/json.hpp>
#include <jwt-cpp/jwt.h>
#include "ThorsHammer/hammer.h"

struct connection_data {
    int session;
    std::string room;
    std::string username;
    std::string uid;
};

struct custom_config : public websocketpp::config::asio {
    // Default settings from our core config
    typedef websocketpp::config::asio core;

    typedef core::concurrency_type concurrency_type;
    typedef core::request_type request_type;
    typedef core::response_type response_type;
    typedef core::message_type message_type;
    typedef core::con_msg_manager_type con_msg_manager_type;
    typedef core::endpoint_msg_manager_type endpoint_msg_manager_type;
    typedef core::alog_type alog_type;
    typedef core::elog_type elog_type;
    typedef core::rng_type rng_type;
    typedef core::transport_type transport_type;
    typedef core::endpoint_base endpoint_base;

    // Set a custom connection_base class to handle indetification.
    typedef connection_data connection_base;
};

typedef websocketpp::server<custom_config> server;
typedef server::connection_ptr connection_ptr;
typedef server::message_ptr message_ptr;
typedef server::connection_ptr connection_ptr;

using websocketpp::connection_hdl;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;
using json = nlohmann::json;

// This class will be heart of our app.
class EchoServer
{
public:
    EchoServer(int port = 80, bool debug = false, std::string jwt = "")
    {
        // Try to not fail. Please. I have faith on you.
            // Set logging settings
            _server.set_access_channels(websocketpp::log::alevel::none);
            _server.clear_access_channels(websocketpp::log::alevel::none);
            _server.init_asio();

            // Message handler
            _server.set_open_handler(bind(&EchoServer::onOpen,this,::_1));
            _server.set_close_handler(bind(&EchoServer::onClose,this,::_1));
            _server.set_message_handler(bind(&EchoServer::onMessage,this,::_1,::_2));
            _server.listen(port);
            _debug = debug;

            // Tell something :)
            std::cout << "Up & Running GameTactic CPP Echo server...\n";
            std::cout << "--------------------------------------------------------\n";
            std::cout << "|     Copyright 2019 Niko Granö <niko@ironlions.fi>    |\n";
            std::cout << "|                 Licensed under GPLv3.                |\n";
            std::cout << "|                 https://gametactic.eu                |\n";
            std::cout << "--------------------------------------------------------\n";
            if (debug == true) {
                std::cout << "[NOTICE]: Debug output enabled!!!\n";
            }
            if (jwt == "") {
                _jwt = false;
                std::cout << "[NOTICE]: Insecure server!!! Please specify JWT private key.\n";
            } else {
                _jwt_private_key = jwt;
                _jwt = true;
            }
            std::cout << "\n";
    }
    void run()
    {
        try
        {
            // Start the server.
            _server.start_accept();
            _server.run();
        }
        catch (std::exception const & e) {
            // catch and report all exceptions.
            // Note: websocketpp::exception is derived from std::exception so it covers this situation.
            std::cout << "Exception: " << e.what() << "\n";

            // Re-Throw the exception so application exits and reports the exception.
            throw;
        }
        catch (...) {
            // catch all other types of exceptions that are not real exceptions.
            std::cout << "Exception: Unknown\n";

            // Re-Throw the exception so application exits and reports the exception.
            throw;
        }
    }

    // Callback to handle incoming messages
    void onMessage(connection_hdl conn, message_ptr msg) {
        // Get connection
        connection_ptr ptr = _server.get_con_from_hdl(conn);

        // Try parse JSON input. Return error if required.
        json input;
        json output;
        try {
            input = json::parse(msg->get_payload());
        } catch (json::exception&) {
            output["success"] = false;
            output["error"] = "Invalid JSON Syntax.";
            std::string stringOutput = output.dump();
            std::stringstream debugOutput;
            debugOutput << "Invalid json given. Sending: " << stringOutput << ".";
            logDebug(debugOutput.str());
            ptr->send(stringOutput);

            return;
        }

        std::string room = input.value("join_room", "");
        std::string inputJwt = input.value("jwt", "");
        if (room != "") {
            if (_jwt) { // If JWT Authentication required.

            } else {
                std::stringstream username_uid;
                username_uid << "Anonymous_" << random_string();
                ptr->room = room;
                ptr->username = username_uid.str();
                ptr->uid = username_uid.str();
            }

            output["success"] = true;
            std::string stringOutput = output.dump();
            std::stringstream debugOutput;
            debugOutput << "Joining room " << room << ". Sending: " << stringOutput << ".";
            logDebug(debugOutput.str());
            ptr->send(stringOutput);

            return;
        }

        if (ptr->room.empty()) {
            output["success"] = false;
            output["error"] = "No room selected.";
            std::string stringOutput = output.dump();
            std::stringstream debugOutput;
            debugOutput << "No room selected. Sending: " << stringOutput << ".";
            logDebug(debugOutput.str());
            ptr->send(stringOutput);

            return;
        }

        // Build message as defined in https://github.com/GameTactic/CPP-Echo/issues/20
        output["payload"] = input;
        output["meta"]["username"] = ptr->username;
        output["meta"]["uid"] = ptr->uid;
        std::string stringOutput = output.dump();
        std::stringstream debugOutput;
        debugOutput << "Sending " << stringOutput << " to room " << ptr->room << ".";
        logDebug(debugOutput.str());

        try {
            for (auto c : _connections) {
                connection_ptr _ptr = _server.get_con_from_hdl(c);
                if (_ptr->room == ptr->room && ptr->session != _ptr->session) {
                    _ptr->send(stringOutput);
                }
            }

            // Send status to client.
            output.clear();
            json output;
            output["success"] = true;
            std::string stringOutput = output.dump();
            ptr->send(stringOutput);

        } catch (websocketpp::exception const &e) {
            output["success"] = false;
            output["error"] = "Internal Server Error";
            ptr->send(output.dump());
            std::cout << "Error: " << "(" << e.what() << ")" << std::endl;
        }
    }

    // Callback to handle new connections
    void onOpen(connection_hdl conn) {
        connection_ptr ptr = _server.get_con_from_hdl(conn);
        ptr->session = _session++;
        _connections.insert(conn);
        logDebug("Websocket connected.");
    }

    // Callback to handle closing connection
    void onClose(connection_hdl conn) {
        connection_ptr ptr = _server.get_con_from_hdl(conn);
        _connections.erase(conn);
        logDebug("Websocket disconnected.");
    }

    // Echo debug, if it's enabled
    void logDebug(std::string msg) {
        if (_debug == true) {
            std::cout << "[DEBUG]: " << msg << "\n";
        }
    }

    // Generate random string. Used in uid and username, if not JWT present.
    std::string random_string(size_t length = 6)
    {
        auto randchar = []() -> char
        {
            const char charset[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
            const size_t max_index = (sizeof(charset) - 1);
            return charset[ rand() % max_index ];
        };
        std::string str(length,0);
        std::generate_n(str.begin(), length, randchar);
        return str;
    }

private:
    typedef std::set<connection_hdl,std::owner_less<connection_hdl>> con_list;

    // Create a server and list for connections.
    server _server;
    con_list _connections;

    // We need to have JWT public key to verify the key. Bool is used to check if JWT is present.
    std::string _jwt_private_key;
    bool _jwt;

    // Every client has own id to help identify.
    int _session;

    // If debug is true, tell more about data passing trough.
    bool _debug;
};

// Basic signal handler.
void signalHandler( int signum ) {
    static char message[] = "Interrupt signal ( XX ) received.\n";
    int hi = signum / 10;
    int lo = signum % 10;
    message[19] = (hi == 0 ) ? ' ' : char('0' + hi);
    message[20] = char('0' + lo);
    write(STDOUT_FILENO, message, sizeof(message));
    exit(signum);
}

int main(int argc, char* argv[]) {
    // Handle signals.
    signal(SIGINT, signalHandler);

    // Default values.
    int port = 80;
    bool debug = false;
    std::string jwt = "";

    // Parse command line arguments.
    using ThorsAnvil::Utils::OptionsParser;
    OptionsParser options(
    {
        {"port", 'p', "Provide an alternative port number (default 80)", [&port](char const* arg){port = std::atoi(arg);return true;}},
        {"debug",'d', "Show debug output in stdout (default false)", [&debug](char const*){debug = true;return false;}},
        {"jwt", 'j', "Use JWT for identifying user. Path to public pem. (default empty)", [&jwt](char const* arg){jwt = arg;return true;}}
    });
    std::vector<std::string> files = options.parse(argc, argv);
    if (files.size() != 0) {
        options.displayHelp();
    }

    // Start app.
    EchoServer srv(port, debug);
    srv.run();
}
