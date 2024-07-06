#include <iostream>
#include <boost/asio.hpp>

#include "MYLServer.hpp"

namespace asio = boost::asio;

int main() {
    // here we create the io_context
    asio::io_context io_context;
    
    // we'll just use an arbitrary port here 
    MYLServer server(io_context, 25000);
    
    // and we run until our server is alive
    io_context.run();
}
