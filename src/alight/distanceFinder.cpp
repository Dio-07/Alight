#include "alight/distanceFinder.hpp"
#include "lemlib/chassis/chassis.hpp"
#include "pros/distance.hpp"
#include <math.h>

namespace alight {
    float headingRad;
    float distanceReading;
    float currentY;
    float currentX;
    int sensorConfidence;

    DistanceSensor::DistanceSensor(pros::Distance* sensor , lemlib::Chassis* chassis, pros::Imu* InertialSensor, float sensorOffset, int angleOffset)
    : m_sensor(sensor), 
      m_sensorOffset(sensorOffset),
      m_imu(InertialSensor),
      m_angleOffset(angleOffset),
      chassis(chassis) {}

    void DistanceSensor::setFieldOffsets(float xOffset, float yOffset) {
        m_robotXoffset = xOffset;
        m_robotYoffset = yOffset;
    }

        // --- Facing RIGHT wall ---
    void DistanceSensor::resetRight() {
        while(true){ // While loop so it keeps trying until robot stops
            if(!chassis->isInMotion()) { // If the robot is not moving then:
                headingRad = (m_imu->get_heading() + m_angleOffset) * M_PI / 180; // returns the current heading in radians to be used in trig function This calculation makes the front 0 instead of 90
                currentY = chassis->getPose().y;
                distanceReading = m_sensor->get() / 25.4; // mm to inches
                // The distance reading from sensor to wall + the offset gives the distance from wall to tracking point
                // 70.5 - robotXoffset gives the distance from starting point to wall
                // Subtract those 2 and it should give the coordinate
                robotX = (70.5 - m_robotXoffset) - ((abs(sin(headingRad)) * distanceReading) + m_sensorOffset); // Should give coordinate according to right wall
                chassis->setPose(robotX, currentY, m_imu->get_heading());
                break;
            }
            pros::delay(50); // delay to save resources
        }
    }

        // --- Facing LEFT wall ---
    void DistanceSensor::resetLeft() {
        while(true) {
            if(!chassis->isInMotion()) {
                headingRad = (m_imu->get_heading() + m_angleOffset) * M_PI / 180; // deg to rad
                currentY = chassis->getPose().y;
                distanceReading = m_sensor->get() / 25.4; // mm to inches
                robotX = (abs((sin(headingRad)) * distanceReading) + m_sensorOffset) - (70.5 + m_robotXoffset); // Should coordinate according to wall
                chassis->setPose(robotX, currentY, m_imu->get_heading());           
                break;                 
            }
            pros::delay(50);
        }
    }

        // --- Facing FRONT wall ---
    void DistanceSensor::resetFront() {
        while(true) {
            if(!chassis->isInMotion()) {
                headingRad = (m_imu->get_heading() + m_angleOffset) * M_PI / 180; // deg to rad
                currentX = chassis->getPose().x;
                distanceReading = m_sensor->get() / 25.4; // mm to inches
                robotY = (141 - m_robotYoffset) - ((abs(sin(headingRad)) * distanceReading) + m_sensorOffset);
                chassis->setPose(currentX, robotY, m_imu->get_heading());
                break;                    
            }
            pros::delay(50);
        }

    }

        // --- Facing BACK wall ---
    void DistanceSensor::resetBack() {
        while(!chassis->isInMotion()) {
            if(!chassis->isInMotion()) {
                headingRad = (m_imu->get_heading() + m_angleOffset) * M_PI / 180; // deg to rad
                currentX = chassis->getPose().x;
                distanceReading = m_sensor->get() / 25.4; // mm to inches
                robotY = ((abs(sin(headingRad)) * distanceReading) + m_sensorOffset) - (m_robotYoffset);
                chassis->setPose(currentX, robotY, m_imu->get_heading());   
                break;                     
            }
            pros::delay(50);
        }

    }

} // namespace alight
