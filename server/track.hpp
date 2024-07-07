#pragma once
#include <vector>
#include <memory>

struct Detection {

};

class TrackDataListener {
public:
    virtual void onTrackDataUpdate() = 0;
};

class Track {
private:
    std::vector<Detection> m_trackData;

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

    size_t size() const {
        return m_trackData.size();
    }

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
