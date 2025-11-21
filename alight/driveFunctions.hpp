#pragma once

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
        Drive(pros::MotorGroup* leftMotors, pros::MotorGroup* rightMotors);

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

    private:
        pros::MotorGroup* m_leftMotors;
        pros::MotorGroup* m_rightMotors;
    };

} // namespace alight