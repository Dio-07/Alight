#pragma once

#include "alight/PIDCompute.hpp"
#include "lemlib/chassis/trackingWheel.hpp"
#include "pros/motor_group.hpp"

namespace alight{

    class Drive{
    public:

        /**
        * @brief extra drive functions that do not work off coordinates
        *
        * @param leftMotors pointer to the left motors
        * @param rightMotors pointer to right motors
        *
        */
        Drive(pros::MotorGroup* leftMotors, pros::MotorGroup* rightMotors, lemlib::TrackingWheel* verticalEncoder, PID* calculator);

        /**
        * @brief wiggles the robot left and right
        *
        * @param voltageIntensity only accepts numbers through 1 - 127
        * @param repetitions how many times the robot oscillates 1 oscillation is left and right
        *
        */
        void wiggle(int voltageIntensity, int repetitions);

        /**
        * @brief shakes the robot front and back
        *
        * @param voltageIntensity only accepts numbers through 1 - 127
        * @param repetitions how many times the robot oscillates 1 oscillation is front and back
        */
        void jiggle(int voltageIntensity, int repetitions);

        /**
        * @brief drives for time instead of coordinates
        *
        * @param time how long the robot drives for
        * @param volts speed, a negative value means reverse
        */
        void driveFor(int time, int volts);

        /**
        * @brief drives a specified distance
        *
        * @param distance what distance the robot has to drive
        */
        void driveDistance(float distance);

    private:
        pros::MotorGroup* m_leftMotors;
        pros::MotorGroup* m_rightMotors;
        lemlib::TrackingWheel* m_verticalEncoder;
        float distanceTraveled = 0;
        PID* m_calculator;
    };

} // namespace alight