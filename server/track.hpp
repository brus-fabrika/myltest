#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>

struct Detection {
    std::string driverId;
    std::string timeMark;
};

struct DriverStats {
    size_t lapsTotal = 0;
    size_t totalTime = 0;
    size_t bestLap = 0;
    size_t averageLap = 0;
    size_t winLapDiff = 0;
};

class TrackDataListener {
public:
    virtual void onTrackDataUpdate() = 0;
};

class Track {
private:
    std::unordered_map<std::string, std::vector<std::string>> m_trackData;

    //std::vector<Detection> m_trackData;

    double m_sumVelocity = 0;
    double m_sumSlope = 0;

    std::vector<TrackDataListener*> m_trackListeners;

public:
    Track() = default;
    ~Track() = default;
    
    Track(Track& other) = default;
    Track(Track&& other) = default;
    Track& operator=(const Track& other) = default;
    Track& operator=(Track&& other) = default;

    void addDetection(const Detection& oneDetection);
    void addDetection(Detection&& oneDetection);

    std::vector<std::string> getDriverLaps(const std::string& driverId) const;
    DriverStats getDriverStats(const std::string& driverId) const;

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
};
