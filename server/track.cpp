#include <chrono>
#include <time.h>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <algorithm>
#include "track.hpp"

void Track::addDetection(const Detection& oneDetection) {
    m_trackData[oneDetection.driverId].emplace_back(oneDetection);
    notifyListeners();
}

void Track::addDetection(Detection&& oneDetection) {
    addDetection(oneDetection);
}

std::vector<Detection> Track::getDriverLaps(const std::string& driverId) const {
    if (m_trackData.find(driverId) != m_trackData.end()) {
        return m_trackData.at(driverId);
    }
    return {};
}

DriverStats Track::getDriverStats(const std::string& driverId) const {
    if (m_trackData.find(driverId) == m_trackData.end()) {
        return {};
    }

    DriverStats stats = {driverId, 0, 0, 0, 0, 0};
    
    std::chrono::system_clock::time_point prevTimeMark;

    for (auto oneDetection: getDriverLaps(stats.driverId)) {
        if (stats.lapsTotal == 0) {
            // no laps yet
            prevTimeMark = oneDetection.timePoint;
        }
        else {
            std::cout << oneDetection.timeMark << std::endl;
            const auto diff = oneDetection.timePoint - prevTimeMark;
            const auto currentLapSec = std::chrono::round<std::chrono::seconds>(diff).count();
            std::cout << currentLapSec << std::endl;
            stats.totalTime += currentLapSec;
            stats.averageLap = stats.totalTime / stats.lapsTotal;
            if (stats.lapsTotal == 1 || currentLapSec < stats.bestLap) {
                stats.bestLap = currentLapSec;
            }
            stats.winLapDiff = 0; // for now

            prevTimeMark = oneDetection.timePoint;
        }
        stats.lapsTotal += 1;
    }

    stats.lapsTotal -= 1; // correct to the right number of laps

    std::cout << stats.averageLap << std::endl;
    std::cout << stats.bestLap << std::endl;

    return stats;
}

TotalDriverStats Track::getTotalStats() const {
    if (m_trackData.empty()) {
        return {};
    }

    TotalDriverStats totalStats;
    for (auto [driverId, driverLaps]: m_trackData) {
        totalStats.emplace_back(getDriverStats(driverId));
    }
    std::sort(totalStats.begin(), totalStats.end(), [](const DriverStats& a, const DriverStats& b) {return a.bestLap < b.bestLap;});
    for (size_t i = 1; i < totalStats.size(); ++i) {
        totalStats[i].winLapDiff = totalStats[i].bestLap - totalStats[0].bestLap;
    }

    return totalStats;
}
