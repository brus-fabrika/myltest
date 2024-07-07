#include "track.hpp"

void Track::addDetection(const Detection& oneDetection) {
    notifyListeners();
}

void Track::addDetection(Detection&& oneDetection) {
    addDetection(oneDetection);
}
