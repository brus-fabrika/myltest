#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <map>
#include <memory>
#include <chrono>

struct Detection {
    std::string driverId;
    std::string timeMark;
    std::chrono::system_clock::time_point timePoint;
};

struct DriverStats {
    std::string driverId;
    size_t lapsTotal = 0;
    size_t totalTime = 0;
    size_t bestLap = 0;
    size_t averageLap = 0;
    size_t winLapDiff = 0;
};

using TotalDriverStats = std::vector<DriverStats>;

class TrackDataListener {
public:
    virtual void onTrackDataUpdate() = 0;
};

class Track {
public:
    Track() = default;
    ~Track() = default;
    
    Track(Track& other) = default;
    Track(Track&& other) = default;
    Track& operator=(const Track& other) = default;
    Track& operator=(Track&& other) = default;

    void addDetection(const Detection& oneDetection);
    void addDetection(Detection&& oneDetection);

    std::vector<Detection> getDriverLaps(const std::string& driverId) const;
    DriverStats getDriverStats(const std::string& driverId) const;
    TotalDriverStats getTotalStats() const;

    void registerTrackListener(TrackDataListener* listener) {
        if (listener) {
            m_trackListeners.emplace_back(listener);
        }
    }

    void notifyListeners() {
        if (m_trackListeners.size() > 0) {
            for (auto listener: m_trackListeners) {
                listener->onTrackDataUpdate();
            }
        }
    }

private:
    std::unordered_map<std::string, std::vector<Detection>> m_trackData;
    std::vector<TrackDataListener*> m_trackListeners;
};
