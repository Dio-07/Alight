#include "alight/driveFunctions.hpp"
#include "lemlib/chassis/trackingWheel.hpp"
#include "pros/motor_group.hpp"

using namespace alight;

Drive::Drive(pros::MotorGroup* leftMotors, pros::MotorGroup* rightMotors, lemlib::TrackingWheel* verticalEncoder, PID* calculator)
: m_leftMotors(leftMotors),
m_rightMotors(rightMotors),
m_verticalEncoder(verticalEncoder), 
m_calculator(calculator) {};

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

void Drive::driveDistance(float distance){
    float error;
    int output;
    int settleTimer;
    int timer;

    while(true){
        timer ++;
        distanceTraveled = m_verticalEncoder->getDistanceTraveled();
        error = distance - distanceTraveled;

        output = m_calculator->calculate(error);

        m_leftMotors->move(output);
        m_rightMotors->move(output);

        // settling conditions
        if(error <= 3 && error >= -3){
            settleTimer ++;
            if(settleTimer > 25 && error <= 1 && error >= -1){
                break;
            }
        }

        if(timer > (distance * 10)){
            break;
        }

        pros::delay(10); // delay to save resources
    }
    m_leftMotors->move(0);
    m_rightMotors->move(0);
}