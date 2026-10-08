#include "pros/motors.h"
#include "pros/motors.hpp"
#include <cstdint>

namespace alight{
    class extendedPID{
        public:
        /**
        * @brief lever controller
        *
        * @param motor A pointer to the motor
        * @param kP proportional constant
        * @param kD derivative consant
        * @param settleError the range in degree the arm can settle
        */
        extendedPID(pros::Motor* motor, float kP, float kD, std::int8_t settleError, pros::motor_gearset_e gearSet);

        /**
        * @brief what moves the arm
        *
        * @param target degree the arm moves to
        * @param async if the arm moves independently from the rest
        * @param maxSpeed maximum voltage
        */
        void moveTo(std::int16_t targetDegree, bool async = true, int maxSpeed = 127);

        private:
        pros::Motor* m_motor;
        float m_kP;
        float m_kD;
        std::int8_t m_settleError;
        std::int16_t targetDegree;
        int maxSpeed = 127;
        bool async = true;
        pros::motor_gearset_e m_gearSet;

        bool scoring;
        int prevError;
        int error;
        int derivative;
        int power;

    };

}