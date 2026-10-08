#include "pros/motors.h"
#include "pros/motors.hpp"
#include "alight/leverControl.hpp"

using namespace alight;

leverFunction::leverFunction(pros::Motor* motor, float kP, float kD, std::int8_t settleError, pros::motor_gearset_e gearSet)
: m_motor(motor), m_kP(kP), m_kD(kD), m_settleError(settleError), m_gearSet(gearSet)
{};

void leverFunction::score(std::int16_t targetDegree, bool async, int maxSpeed){
    m_motor->set_gearing(m_gearSet);

    if (async) {
        pros::Task task([&]() { score(targetDegree, false, maxSpeed); });
        pros::delay(10); // delay giving task time to run Once it returns it exits scope and variables get deleted
        return;
    }

    scoring  = true;

    m_motor->set_encoder_units(pros::E_MOTOR_ENCODER_DEGREES);

    std::int8_t timeoutInteger = 0;
    std::int8_t settleTimeInteger = 0;

    prevError = targetDegree - m_motor->get_position(); // sets starting error as distance to target degree

    while(!async){
        if(scoring){
            timeoutInteger ++;
            error = targetDegree - m_motor->get_position();
            
            // PD CALCULATIONS BEGIN HERE
            derivative = error - prevError; // deriv gain calculation
            prevError = error; // after doing derivative gain calculation set new prev error

            power = (m_kP * error) + (m_kD * derivative);

            if(power > maxSpeed){ // sets motor max speed
                power = maxSpeed;
            }else if(power < -maxSpeed){
                power = -maxSpeed;
            }

            // MOVES THE MOTOR
            m_motor->move(power); // move the motor according to PID

            if(m_motor->get_position() < targetDegree + m_settleError && m_motor->get_position() > targetDegree - m_settleError){
                settleTimeInteger ++;
                if(settleTimeInteger > 20){ // the 20 means it checks after 200ms 1 second is 50 cycles
                    scoring = false;
                }
            }

            if(timeoutInteger > 120){ // timeout of 1200 ms
                scoring = false;
            }

        }else{
            m_motor->move_absolute(0, -127); // moves back to starting position
            if(m_motor->get_position() < 5){
                m_motor->tare_position();
                break; // ONLY BREAKS OUT OF LOOP ONCE ITS BACK
            }
        }

    pros::delay(10); // delay to save resources
    }

    m_motor->move(0); // makes the motor more resistant to slop
}