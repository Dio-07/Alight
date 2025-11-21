#pragma once
#include "pros/distance.hpp"
#include "lemlib/chassis/chassis.hpp"
#include "pros/imu.hpp"

namespace alight {

    class DistanceSensor {
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
        DistanceSensor(pros::Distance* sensor , lemlib::Chassis* chassis, pros::Imu* InertialSensor, float sensorOffset, int angleOffset = 0);

        /**
         *  @brief Set robot’s field offsets must ALWAYS be set
         *
         *  @param robotXoffset Distance from midline to tracking point (+ = right, − = left)
         *  @param robotYoffset Distance from back wall to tracking point (+ = front, − = back)
         */
        void setFieldOffsets(float robotXoffset = 0.0, float robotYoffset = 0.0);

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
        pros::Distance* m_sensor;   // Pointer to PROS distance sensor
        pros::Imu* m_imu;           // Pointer to Inertial Sensor
        lemlib::Chassis* chassis;   // Pointer to chassis
        int m_angleOffset;          // If the distance sensor is on the right or behinf the robot
        float m_sensorOffset;   // Left/right sensor offset from center (in inches)
        float m_robotXoffset;       // Robot’s X offset from field center (in inches)
        float m_robotYoffset;       // Robot’s Y offset from field center (in inches)
        float robotX = 0.0;         // Computed robot X coordinate
        float robotY = 0.0;         // Computed robot Y coordinate
    };
} // namespace alight