#include <iostream>
#include <sstream>
#include <nlohmann/json.hpp>
#include "MYLEventHandler.hpp"

MYLEventHandler::MYLEventHandler(std::shared_ptr<Track> trackData)
    : m_trackData(trackData) {

}

void MYLEventHandler::HandleEvent(std::string_view messages)
{
    // parsing every new line as a separate json event message
    using json = nlohmann::json;

    std::string line;
    std::istringstream input;
    input.str(std::string(messages));
    while(std::getline(input, line)) {
        std::cout << "non-parsed line: " << line << std::endl;
        json j = json::parse(line);
        if (j.contains("method")){
            if (j["method"] == "lap_event") {
                std::cout << "New lap event for driver " << j["driver_id"] << " and time " << j["time"] << std::endl;
                Detection oneDetect = {j["driver_id"], j["time"]};
                m_trackData->addDetection({std::string(j["driver_id"]), std::string(j["time"])});
            }
            else if (j["method"] == "ranking") {
                if (j.contains("driver_id")) {
                    std::cout << "Get lap stats for driver " << j["driver_id"] << std::endl;
                    auto lapsData = m_trackData->getDriverStats(j["driver_id"]);
                    json reply = j;
                    reply["result"] = 
                        json{{"total_time", lapsData.totalTime}, {"average_lap", lapsData.averageLap}, {"best_lap", lapsData.bestLap}, {"total_laps", lapsData.lapsTotal},{"best_lap_diff", lapsData.winLapDiff}};
                    std::cout << reply.dump() << std::endl;
                }
                else {
                    std::cout << "Get total lap stats" << std::endl;
                    //m_trackData->getTotalLaps();
                }
            }
        }
        else {
            std::cout << "Incorrect message format" << std::endl;
        }
    }
}