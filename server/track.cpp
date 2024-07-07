#include <chrono>
#include <algorithm>
#include "track.hpp"


void Track::addDetection(const Detection& oneDetection) {
    const std::lock_guard<std::recursive_mutex> lock(m_trackDataLock);
    m_trackData[oneDetection.driverId].emplace_back(oneDetection);
    notifyListeners();
}

void Track::addDetection(Detection&& oneDetection) {
    addDetection(oneDetection);
}

std::vector<Detection> Track::getDriverLaps(const std::string& driverId) {
    const std::lock_guard<std::recursive_mutex> lock(m_trackDataLock);
    if (m_trackData.find(driverId) != m_trackData.end()) {
        return m_trackData.at(driverId);
    }
    return {};
}

DriverStats Track::getDriverStats(const std::string& driverId) {
    const std::lock_guard<std::recursive_mutex> lock(m_trackDataLock);
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
            const auto diff = oneDetection.timePoint - prevTimeMark;
            const size_t currentLapSec = std::chrono::round<std::chrono::seconds>(diff).count();
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

    return stats;
}

TotalDriverStats Track::getTotalStats() {
    const std::lock_guard<std::recursive_mutex> lock(m_trackDataLock);
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
