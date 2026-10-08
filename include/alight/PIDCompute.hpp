#pragma once

namespace alight{
    class PID{
        public:
            /**
            * @brief PID controller for drivetrain
            *
            * @param kp constant for proportional gain
            * @param ki constant for integral gain
            * @param kd constant for derivative gain
            */
            PID(float kp, float ki, float kd); // gets constants to use in calculation

            /**
            * @brief calculates the output power
            *
            * @param error distance to target
            */
            float calculate(float error); // returns output volatage to motors

        private:
        float m_kp;
        float m_ki;
        float m_kd;

        float prevError = 0;
        float integral = 0;
    };
}
