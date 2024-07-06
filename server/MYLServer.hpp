#include <boost/asio.hpp>

namespace asio = boost::asio;
using boost::asio::ip::tcp;

class MYLServer
{
public:
    // we need an io_context and a port where we listen to 
    MYLServer(asio::io_context& io_context, short port); 

private:
    void DoAccept();

private: 
    tcp::acceptor m_acceptor;
};

class MYLSession : public std::enable_shared_from_this<MYLSession>
{
public:
    // our session holds the socket
    MYLSession(tcp::socket socket);  
    
    // and run was already called in our server, where we just wait for requests
    void Run();

private:
    void WaitForRequest();

private:
    tcp::socket m_socket;
    asio::streambuf m_buffer;
};
