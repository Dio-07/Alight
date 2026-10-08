#pragma once
#include "liblvgl/misc/lv_types.h"
#include "pros/motor_group.hpp"
#include "pros/motors.hpp"
#include <vector>
#include <cstdarg>

using text_id_t = uint16_t;

namespace alight {
    class screener {
    public:
        screener();

        /**
        * @brief intializes the screen, makes it run
        *
        * @param ThemeHEX color in hexadecimals of the theme of the brain starts with 0x instead of #
        */
        void initAlightScreen(int ThemeHEX = 0xffffff);

        /**
        * @brief creates the label that gets printed first
        *
        * @param fmt formated string for the text
        * @param index what line the text is printed on
        */
        void text(const char* fmt, int index, ...);

        /**
        * @brief updates the already printed label
        *
        * @param index the line of the label that gets updates
        * @param fmt the text the label gets updated with
        */
        void update(int index, const char* fmt, ...);

        /**
        * @brief what motors get displayed on brain screen
        *
        * @param motorOrder a list of motors that gets printed in the order given
        * @param motorGroupOrder a list of motor groups that get printed, drivetrain motors go first
        */
        void motorInfo(std::vector<pros::Motor*> motorOrder, std::vector<pros::MotorGroup*> motorGroupOrder);

        std::vector<lv_obj_t*> labels;
        std::vector<lv_obj_t*> motorLabels;
        std::vector<lv_obj_t*> motorGroupLabels;
        std::vector<pros::Motor*> motorList; // the same as motorOrder just different variable since one is scope only
        std::vector<pros::MotorGroup*> motorGroupList;

    private:
    };
}