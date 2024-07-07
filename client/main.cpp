#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <nlohmann/json.hpp>

#include <boost/asio.hpp>

namespace asio = boost::asio;
using boost::asio::ip::tcp;
using nlohmann::json;

void readTrackData(const std::string& trackFile, tcp::socket& socket)
{
    // Create an input filestream
    std::ifstream myFile(trackFile);

    // Make sure the file is open
    if(!myFile.is_open()) throw std::runtime_error("Could not open file");

    std::string line;
    std::string token;

    bool firstLine = true;

    // Read data, line by line
    while(std::getline(myFile, line))
    {
        // skiping header row
        if (firstLine) {
            firstLine = false;
            continue;
        }

        // remove possible line-ending leftovers
        line.erase(std::remove(line.begin(), line.end(), '\r' ), line.end());

        // simple CSV parsing 
        std::stringstream ss(line);
        
        std::getline(ss, token, ',');
        std::string driverId = token;

        std::getline(ss, token, ',');
        std::string timeMark = token;

        json j;
        j["method"] = "lap_event";
        j["driver_id"] = driverId;
        j["time"] = timeMark;

        std::string message = j.dump() + '\n';

        std::cout << "Sending data to the server: " << j.dump() << std::endl;
        asio::write(socket, asio::buffer(message, message.length()));

    }
}


int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cout << "Usage: ml_client <host> <port> <data_path>" << std::endl;
        return 1;
    }

    asio::io_context io_context;
    
    tcp::socket socket(io_context);
    tcp::resolver resolver(io_context);

    asio::connect(socket, resolver.resolve(argv[1], argv[2]));
   
    readTrackData(argv[3], socket);
    

    boost::system::error_code ec;
    socket.shutdown(tcp::socket::shutdown_both, ec);
    socket.close();
}
