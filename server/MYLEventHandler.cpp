#include <iostream>
#include <sstream>
#include <nlohmann/json.hpp>
#include <chrono>
#include "MYLEventHandler.hpp"

using json = nlohmann::ordered_json;

std::chrono::system_clock::time_point convertFromString(const std::string& timeString, const std::string& timeFormat) {
    tm tm;
    std::istringstream ss(timeString);
    ss >> std::get_time(&tm, timeFormat.c_str());
    tm.tm_mday = 1; tm.tm_mon = 0; tm.tm_year = 100; // this is to prevent random dates generated...

    return std::chrono::system_clock::from_time_t(mktime(&tm));
}

json toJson(const DriverStats& ds) {
    return json {
            {"driver_id", ds.driverId},
            {"total_time", ds.totalTime},
            {"average_lap", ds.averageLap},
            {"best_lap", ds.bestLap},
            {"total_laps", ds.lapsTotal},
            {"best_lap_diff", ds.winLapDiff}
    };
}

json toJson(const TotalDriverStats& tds) {
    json j = json::array();
    
    for (auto driverStats: tds) {
        j.emplace_back(toJson(driverStats));
    }
    return j;
}

MYLEventHandler::MYLEventHandler(std::shared_ptr<Track> trackData)
    : m_trackData(trackData) {
}

void MYLEventHandler::handleEvent(std::string_view messages)
{
    // parsing every new line as a separate json event message
    std::string line;
    std::istringstream input;
    input.str(std::string(messages));
    while(std::getline(input, line)) {
        std::cout << "non-parsed line: " << line << std::endl;
        json j;

        try
        {
            j = json::parse(line);
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            continue;
        }
        
        if (j.contains("method")){
            if (j["method"] == "lap_event") {
                std::cout << "New lap event for driver " << j["driver_id"] << " and time " << j["time"] << std::endl;
                
                Detection oneDetect = {j["driver_id"], j["time"], convertFromString(j["time"], "%T")};

                m_trackData->addDetection(oneDetect);
            }
            else if (j["method"] == "ranking") {
                if (j.contains("driver_id")) {
                    std::cout << "Get lap stats for driver " << j["driver_id"] << std::endl;
                    auto lapsData = m_trackData->getDriverStats(j["driver_id"]);
                    json reply;
                    reply["method"] = j["method"];
                    reply["result"] = toJson(lapsData);
                    std::cout << reply.dump() << std::endl;
                }
                else {
                    std::cout << "Get total lap stats" << std::endl;
                    auto totalLapsData = m_trackData->getTotalStats();
                    json reply;
                    reply["method"] = j["method"];
                    reply["result"] = toJson(totalLapsData);
                    std::cout << reply.dump() << std::endl;
                }
            }
        }
        else {
            std::cout << "Incorrect message format" << std::endl;
        }
    }
}

