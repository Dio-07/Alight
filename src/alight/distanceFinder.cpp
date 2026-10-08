#include "pros/distance.hpp"
#include "alight/distanceFinder.hpp"

using namespace alight;
    
    distanceFinder::distanceFinder(pros::Distance* distanceSensor, lemlib::Chassis* chassis, float odomOffset)
    : m_odomOffset(odomOffset),
    m_distanceSensor(distanceSensor),
    m_chassis(chassis)
    {};
    
    void distanceFinder::fieldOffsets(float x_offs, float y_offs){ 
        x_offset = x_offs;
        y_offset = y_offs;
    }


    void distanceFinder::resetRight(){
            x_coor = (70.5 - x_offset) - (m_distanceSensor->get()/25.4 + m_odomOffset);
            m_chassis->setPose(x_coor, m_chassis->getPose().y, m_chassis->getPose().theta );
        };

    void distanceFinder::resetLeft(){
        x_coor = (m_distanceSensor->get()/25.4 + m_odomOffset) - (70.5 + x_offset);
        m_chassis->setPose(x_coor, m_chassis->getPose().y, m_chassis->getPose().theta );
    };

     void distanceFinder::resetFront(){
        y_coor = (141 - y_offset) - (m_distanceSensor->get()/25.4 + m_odomOffset);
        m_chassis->setPose(m_chassis->getPose().x, y_coor, m_chassis->getPose().theta);

     };

      void distanceFinder::resetBack(){
        y_coor = (m_distanceSensor->get()/25.4 + m_odomOffset) - (y_offset);
        m_chassis->setPose(m_chassis->getPose().x, y_coor, m_chassis->getPose().theta);
      };