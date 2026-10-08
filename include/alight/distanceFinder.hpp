#pragma once
#include "pros/distance.hpp"
#include "lemlib/chassis/chassis.hpp"

namespace alight {

    class distanceFinder {
    public:
        /**
         *  @brief Distance Sensor constructor
         *
         *  @details To use the distance sensor to reset coordinates you need to add a "sensor.setFieldOffset()" in your auton for EACH sensor, This also assumes that the starting theta will always be straight forward, facing the front wall. If you start offset from your theta being 0 you should do a chassis.setPose(0, 0, offset) to correct for this. Make sure to turn Async to false before resetting
         *
         *  @param sensor Pointer to PROS distance sensor
         *  @param sensorOffset Left/Right, Front/Back offset from tracking point
         *  @param rightOrBack offset from front of robot right values are positive left are negative
         */
        distanceFinder(pros::Distance* distanceSensor, lemlib::Chassis* chassis, float odomOffset);

        /**
         *  @brief Set robot’s field offsets must ALWAYS be set
         *
         *  @param robotXoffset Distance from midline to tracking point (+ = right, − = left)
         *  @param robotYoffset Distance from back wall to tracking point (+ = front, − = back)
         */
        void fieldOffsets(float robotXoffset = 0.0, float robotYoffset = 0.0);

        /**
        *   @brief reset position with the right wall alignment does not matter
        */
        void resetRight();

        /**
        *   @brief reset position with the left wall alignment does not matter
        */
        void resetLeft();

        /**
        *  @brief reset position with the front wall alignment does not matter
        */
        void resetFront();

        /** 
        *  @brief reset position with the back wall alignment does not matter
        */
        void resetBack();

    private:
        pros::Distance* m_distanceSensor;   // Pointer to PROS distance sensor
        lemlib::Chassis* m_chassis;   // Pointer to chassis
        float m_odomOffset;   // Left/right sensor offset from center (in inches)
        float x_coor;
        float y_coor;
        float x_offset = 0.0;         // Computed robot X coordinate
        float y_offset = 0.0;         // Computed robot Y coordinate
    };
} // namespace alight