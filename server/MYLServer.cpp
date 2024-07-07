#include <iostream>
#include "MYLServer.hpp"
#include "MYLEventHandler.hpp"

MYLServer::MYLServer(boost::asio::io_context& io_context, short port) 
    : m_acceptor(io_context, tcp::endpoint(tcp::v4(), port)) {
    // wait for clients
    DoAccept();
}

void MYLServer::DoAccept() {
    // this is an async accept which means the lambda function is 
    // executed, when a client connects
    m_acceptor.async_accept([this](boost::system::error_code ec, tcp::socket socket) {
        if (!ec) {
            // let's see where we created our session
            std::cout << "creating session on: " 
                << socket.remote_endpoint().address().to_string() 
                << ":" << socket.remote_endpoint().port() << '\n';
            // create a session where we immediately call the run function
            // the socket is passed to the lambda here
            auto newConnection = std::make_shared<MYLSession>(std::move(socket), m_trackDataHandler);
            newConnection->Run();
        } else {
            std::cout << "error: " << ec.message() << std::endl;
        }
        // since we want multiple clients to connnect, wait for the next one by calling doAccept()
        DoAccept();
    });
}

MYLSession::MYLSession(tcp::socket socket, std::shared_ptr<MYLEventHandler> eventHandler)
    : m_socket(std::move(socket)), m_trackDataHandler(eventHandler) {
        
}

void MYLSession::Run() {
    WaitForRequest();
}

void MYLSession::WaitForRequest() {
    // since we capture `this` in the callback, we need to call shared_from_this()
    auto self(shared_from_this());
    // and now call the lambda once data arrives
    // we read a string until the null termination character
    asio::async_read_until(m_socket, m_buffer, "\n", 
        [this, self](boost::system::error_code ec, std::size_t length) {
            // if there was no error, everything went well and for this demo
            // we print the data to stdout and wait for the next request
            if (!ec)  {
                std::cout << "New message recieved" << std::endl;

                std::string data{
                    std::istreambuf_iterator<char>(&m_buffer), 
                    std::istreambuf_iterator<char>() 
                };

                // append read data from socket to internal persistent buffer
                m_bufferData += data;

                std::cout << m_bufferData << std::endl;

                if (data[data.length() - 1] != '\n')
                {
                    std::cout << "partial message recieved :( with length " << length << std::endl;
                }

                ProcessRecievedData();

                WaitForRequest();
            } else {
                std::cout << "error: " << ec << std::endl;;
            }
        }
    );
}

void MYLSession::ProcessRecievedData()
{
    // we process all data from persistent buffer while there are full lines delimeted with '\n'
    // what is left will be processed on the next read iterations

    size_t start_pos = 0;
    size_t to = 0;
    
    while ((to = m_bufferData.find('\n', start_pos)) != std::string::npos)
    {
        std::string line = m_bufferData.substr(start_pos, to - start_pos);
        start_pos = to + 1;

        std::cout << "Processing line:" << std::endl;
        std::cout << line << std::endl;

        m_trackDataHandler->HandleEvent(line);
    }

    m_bufferData.erase(0, start_pos);   
}


