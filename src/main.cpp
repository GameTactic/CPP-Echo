#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

#include <iostream>

typedef websocketpp::server<websocketpp::config::asio> server;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

// pull out the type of messages sent by our config
typedef server::message_ptr message_ptr;

// Define a callback to handle incoming messages
void on_message(server* s, websocketpp::connection_hdl hdl, message_ptr msg) {
    try {
        s->send(hdl, msg->get_payload(), msg->get_opcode());
    } catch (websocketpp::exception const & e) {
        std::cout << "Error: "
                  << "(" << e.what() << ")" << std::endl;
    }
}

void signalHandler( int signum ) {
   std::cout << "Interrupt signal (" << signum << ") received.\n";
   exit(signum);
}


int main() {
    // Handle signals.
    signal(SIGINT, signalHandler);

    // Create a server endpoint
    server _server;

    try {
        // Set logging settings
        _server.set_access_channels(websocketpp::log::alevel::all);
        _server.clear_access_channels(websocketpp::log::alevel::frame_payload);
        _server.init_asio();

        // Message handler
        _server.set_message_handler(bind(&on_message,&_server,::_1,::_2));
        _server.listen(80);

        // Start the server.
        // First start itself, second start asio.
        _server.start_accept();
        _server.run();
    } catch (websocketpp::exception const & e) {
        std::cout << e.what() << std::endl;
    } catch (...) {
        std::cout << "Error: " << std::endl;
    }
}
