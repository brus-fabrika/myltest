#pragma once
#include "track.hpp"

class MYLEventHandler {
public:
    explicit MYLEventHandler(std::shared_ptr<Track> trackData);
    ~MYLEventHandler() = default;

    void HandleEvent(std::string_view messages);

private:
    std::shared_ptr<Track> m_trackData;
};