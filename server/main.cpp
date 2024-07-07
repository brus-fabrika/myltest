#include <iostream>
#include <boost/asio.hpp>

#include "MYLServer.hpp"
#include "MYLEventHandler.hpp"

namespace asio = boost::asio;

class TrackDataHandler {
public:
    
};

int main() {
    asio::io_context io_context;
    MYLServer server(io_context, 25000);
    
    auto trackData = std::make_shared<Track>();
    auto eventHandler = std::make_shared<MYLEventHandler>(trackData);

    server.setTrackDataHandler(eventHandler);

    std::cout << "Starting the server" << std::endl;

    // and we run until our server is alive
    io_context.run();
}
