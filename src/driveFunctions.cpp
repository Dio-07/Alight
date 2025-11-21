#include "alight/driveFunctions.hpp"
#include "pros/motor_group.hpp"

namespace alight{
    Drive::Drive(pros::MotorGroup* leftMotors, pros::MotorGroup* rightMotors)
    : m_leftMotors(leftMotors),
      m_rightMotors(rightMotors) {}
      
    void Drive::wiggle(int voltageIntensity, int repetitions){
        for (int oscillations = 0; oscillations != repetitions; oscillations ++){
            m_leftMotors->move(-(voltageIntensity)); // Turns Left
            m_rightMotors->move(voltageIntensity);
            pros::delay(250);
            m_leftMotors->move(voltageIntensity); // Turns Right
            m_rightMotors->move(-(voltageIntensity));
            pros::delay(500);
            m_leftMotors->move(-(voltageIntensity)); // Returns to middle
            m_rightMotors->move(voltageIntensity);
            pros::delay(250);
        }
        m_leftMotors->brake(); // sets the motors to brake after finishing
        m_rightMotors->brake();
    }

    void Drive::jiggle(int voltageIntensity, int repetitions){
        for (int oscillations = 0; oscillations != repetitions; oscillations ++){

            m_leftMotors->move(voltageIntensity); // Moves the robot forwards
            m_rightMotors->move(voltageIntensity);
            pros::delay(250);
            m_leftMotors->move(-(voltageIntensity)); // Moves the robot backwards after a delay
            m_rightMotors->move(-(voltageIntensity));
            pros::delay(250);
        }
        m_leftMotors->brake(); // sets the motors to brake after finishing
        m_rightMotors->brake();
    }

    void Drive::driveFor(int time, int volts){
        m_leftMotors->move(volts); // Moves the robot forwards
        m_rightMotors->move(volts);
        pros::delay(time);
        m_leftMotors->brake(); // sets the motors to brake after finishing
        m_rightMotors->brake();
    }
}
