#include <string_view>
#include <memory>
#include <boost/asio.hpp>
#include "track.hpp"

namespace asio = boost::asio;
using boost::asio::ip::tcp;

class MYLEventHandler {
public:
    explicit MYLEventHandler(std::shared_ptr<Track> trackData);
    ~MYLEventHandler() = default;

    void HandleEvent(std::string_view messages);

private:
    std::shared_ptr<Track> m_trackData;
};

class MYLServer
{
public:
    // we need an io_context and a port where we listen to 
    MYLServer(asio::io_context& io_context, short port); 
    void SetTrackDataHandler(std::shared_ptr<MYLEventHandler> trackDataHandler) {
        m_trackDataHandler = trackDataHandler;
    }

private:
    void DoAccept();

private: 
    tcp::acceptor m_acceptor;
    std::shared_ptr<MYLEventHandler> m_trackDataHandler;
};

class MYLSession : public std::enable_shared_from_this<MYLSession>
{
public:
    // our session holds the socket
    MYLSession(tcp::socket socket, std::shared_ptr<MYLEventHandler> eventHandler);  
    virtual ~MYLSession() = default;

    // and run was already called in our server, where we just wait for requests
    void Run();

private:
    void WaitForRequest();
    void ProcessRecievedData();
    
private:
    tcp::socket m_socket;
    asio::streambuf m_buffer;
    std::string m_bufferData;
    std::shared_ptr<MYLEventHandler> m_trackDataHandler;
};

