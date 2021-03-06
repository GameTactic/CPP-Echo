#include <set>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <iostream>
#include "ThorsHammer/hammer.h"

struct connection_data {
    int session;
    std::string room;
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


// This class will be heart of our app.
class EchoServer
{
public:
    EchoServer(int port = 80)
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

            // Tell something :)
            std::cout << "Starting GameTactic CPP Echo server...\n";
            std::cout << "--------------------------------------------------------\n";
            std::cout << "|     Copyright 2019 Niko Granö <niko@ironlions.fi>    |\n";
            std::cout << "|                 Licensed under GPLv3.                |\n";
            std::cout << "|                 https://gametactic.eu                |\n";
            std::cout << "--------------------------------------------------------\n\n";
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

        // Check if client is requesting to join.
        if (msg->get_payload().substr(0,5) == "join:") {
            std::string room = msg->get_payload().substr(5);
            ptr->room = room;
            std::stringstream ss;
            ss << "{\"success\":\"Room " << room << " selected.\"}";
            ptr->send(ss.str());

            return;
        }

        if (ptr->room.empty()) {
            ptr->send("{\"error\":\"No room selected.\"}");

            return;
        }

        try {
            for (auto c : _connections) {
                connection_ptr _ptr = _server.get_con_from_hdl(c);
                if (_ptr->room == ptr->room) {
                    _server.send(c,msg);
                }
            }
        } catch (websocketpp::exception const &e) {
            std::cout << "Error: " << "(" << e.what() << ")" << std::endl;
        }
    }

    // Callback to handle new connections
    void onOpen(connection_hdl conn) {
        connection_ptr ptr = _server.get_con_from_hdl(conn);
        ptr->session = _session++;
        _connections.insert(conn);
    }

    // Callback to handle closing connection
    void onClose(connection_hdl conn) {
        connection_ptr ptr = _server.get_con_from_hdl(conn);
        _connections.erase(conn);
    }

private:
    typedef std::set<connection_hdl,std::owner_less<connection_hdl>> con_list;

    // Create a server and list for connections.
    server _server;
    con_list _connections;

    // Every client has own id to help identify.
    int _session;
};

// Basic signal hander
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

    // default values
    int port = 80;

    // Parse command line arguments
    using ThorsAnvil::Utils::OptionsParser;
    OptionsParser   options({{"port", 'p', "Provide an alternative port number (default 80)", [&port](char const* arg){port = std::atoi(arg);return true;}}});
    std::vector<std::string>    files = options.parse(argc, argv);
    if (files.size() != 0) {
        options.displayHelp();
    }

    // Start app.
    EchoServer srv(port);
    srv.run();
}
