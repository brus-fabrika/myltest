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

void displayPromt() {
    std::cout << "Select command: " << std::endl;
    std::cout << "AddEvent <DriverId> <Time>" << std::endl;
    std::cout << "GetDriverStat <DriverId>" << std::endl;
    std::cout << "GetTotalStat" << std::endl;
    std::cout << "Exit (or Ctrl-C)" << std::endl << std::endl;
}

void handleUserInput(tcp::socket& socket, const std::string& cmdLine) {
    static const std::string getTotalStatCmd = "GetTotalStat";
    static const std::string getDriverStatCmd = "GetDriverStat";
    static const std::string addEventCmd = "AddEvent";

    if (cmdLine.compare(0, getTotalStatCmd.length(), getTotalStatCmd) == 0) {
        json j;
        j["method"] = "ranking";
        std::string message = j.dump() + '\n';

        std::cout << "Sending data to the server: " << j.dump() << std::endl;
        asio::write(socket, asio::buffer(message, message.length()));
        asio::streambuf responseBuffer;
        asio::read_until(socket, responseBuffer, "\n");
        std::string responseData{
            std::istreambuf_iterator<char>(&responseBuffer), 
            std::istreambuf_iterator<char>() 
        };
        std::cout << "Response: " << responseData << std::endl;
    }
    else if (cmdLine.compare(0, getDriverStatCmd.length(), getDriverStatCmd) == 0) {
        // we count everything after driver stat cmd as driver's id
        std::string driverId;
        std::cin >> driverId;

        json j;
        j["method"] = "ranking";
        j["driver_id"] = driverId;
        std::string message = j.dump() + '\n';

        std::cout << "Sending data to the server: " << j.dump() << std::endl;
        asio::write(socket, asio::buffer(message, message.length()));
        asio::streambuf responseBuffer;
        asio::read_until(socket, responseBuffer, "\n");
        std::string responseData{
            std::istreambuf_iterator<char>(&responseBuffer), 
            std::istreambuf_iterator<char>() 
        };
        std::cout << "Response: " << responseData << std::endl;
    }
    else if (cmdLine.compare(0, addEventCmd.length(), addEventCmd) == 0) {
        std::string driverId, timeMark;
        std::cin >> driverId >> timeMark;

        json j;
        j["method"] = "lap_event";
        j["driver_id"] = driverId;
        j["time"] = timeMark;

        std::string message = j.dump() + '\n';

        std::cout << "Sending data to the server: " << j.dump() << std::endl;
        asio::write(socket, asio::buffer(message, message.length()));
    }
    else {
        std::cout << "Invalid command, try again (or Ctrl-C)" << std::endl;
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

    bool doExit = false;
    while (!doExit) {
        displayPromt();
        std::string cmdLine;
        std::cin >> cmdLine;
        if (cmdLine.compare("Exit") == 0) {
            doExit = true;
        }
        else {
            handleUserInput(socket, cmdLine);
        }
    }

    boost::system::error_code ec;
    socket.shutdown(tcp::socket::shutdown_both, ec);
    socket.close();
}
