#ifndef COMPONENT_SENSOR_LINETRACKER_H
#define COMPONENT_SENSOR_LINETRACKER_H

#include "component/sensor/BasicSensor.h"
#include "mcu/Pin.h"
#include "mcu/PinBit.h"
#include "mcu/Port.h"

namespace component::sensor {

template <mcu::is_pinbit PinBit, mcu::is_port Port, mcu::is_ddr DDR, mcu::is_pin Pin>
using LineTrackerInfo = BasicSensorInfo<mcu::FullPinInfo<PinBit, Port, DDR, Pin>, true>;

template <basic_sensor_info_pullup<true> Info> using LineTracker = BasicSensor<Info>;

}

#endif
