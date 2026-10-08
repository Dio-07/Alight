#include "alight/PIDCompute.hpp"

using namespace alight;

    PID::PID(float kp, float ki, float kd)
    : m_kp(kp), m_ki(ki), m_kd(kd) {};

    float PID::calculate(float error){
        // integral calculations
        integral += error; // integral just adds up all of the errors

        // derivative calculations
        float derivative = error - prevError;
        prevError = error;
        
        int power = (m_kp * error) + (m_ki * integral) + (m_kd * derivative);

        return power;
    }