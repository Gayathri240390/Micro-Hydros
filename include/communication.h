#pragma once

#include "measurement_data.h"

class Communication {
public:
    void begin();
    void loop();
    void send(const MeasurementData& measurement);
};