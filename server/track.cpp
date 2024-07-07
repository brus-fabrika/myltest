#include <chrono>
#include <time.h>
#include <iomanip>
#include <sstream>
#include <iostream>
#include "track.hpp"

std::chrono::system_clock::time_point convertFromString(const std::string& timeString, const std::string& timeFormat) {
    struct tm tm;
    std::istringstream ss(timeString);
    ss >> std::get_time(&tm, timeFormat.c_str());
    return std::chrono::system_clock::from_time_t(mktime(&tm));
}

void Track::addDetection(const Detection& oneDetection) {
    m_trackData[oneDetection.driverId].emplace_back(oneDetection.timeMark);
    notifyListeners();
}

void Track::addDetection(Detection&& oneDetection) {
    addDetection(oneDetection);
}

std::vector<std::string> Track::getDriverLaps(const std::string& driverId) const {
    if (m_trackData.find(driverId) != m_trackData.end()) {
        return m_trackData.at(driverId);
    }
    return {};
}

DriverStats Track::getDriverStats(const std::string& driverId) const {
    if (m_trackData.find(driverId) == m_trackData.end()) {
        return {};
    }

    DriverStats stats;

    std::chrono::system_clock::time_point prevTimeMark;

    for (auto timeMark: getDriverLaps(driverId)) {
        if (stats.lapsTotal == 0) {
            // no laps yet
            prevTimeMark = convertFromString(timeMark, "%T");
        }
        else {
            const auto currentTimeMark = convertFromString(timeMark, "%T");
            const auto diff = currentTimeMark - prevTimeMark;
            const auto currentLapSec = std::chrono::round<std::chrono::seconds>(diff).count();
            stats.totalTime += currentLapSec;
            stats.averageLap = stats.totalTime / stats.lapsTotal;
            if (stats.lapsTotal == 1 || currentLapSec < stats.bestLap) {
                stats.bestLap = currentLapSec;
            }
            stats.winLapDiff = 0; // for now

            prevTimeMark = currentTimeMark;
        }
        stats.lapsTotal += 1;
    }

    stats.lapsTotal -= 1; // correct to the right number of laps

    return stats;
}


