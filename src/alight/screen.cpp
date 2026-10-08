#include "alight/screen.hpp"
#include "liblvgl/core/lv_obj.h"
#include "liblvgl/core/lv_obj_event.h"
#include "liblvgl/core/lv_obj_pos.h"
#include "liblvgl/core/lv_obj_style_gen.h"
#include "liblvgl/display/lv_display.h"
#include "liblvgl/font/lv_font.h"
#include "liblvgl/font/lv_symbol_def.h"
#include "liblvgl/misc/lv_area.h"
#include "liblvgl/misc/lv_color.h"
#include "liblvgl/misc/lv_event.h"
#include "liblvgl/misc/lv_timer.h"
#include "liblvgl/misc/lv_types.h"
#include "liblvgl/widgets/image/lv_image.h"
#include "liblvgl/widgets/label/lv_label.h"
#include "liblvgl/widgets/button/lv_button.h"
#include "pros/motor_group.hpp"
#include <cstdint>
#include <cstdio>
#include <vector>

/* OBJ ARRAY INDEXES
MAIN_MENU = 0
MENU_PANEL = 1
MOTOR_VITALS = 2
BRAIN_THEME_MENU = 3
PID_MENU = 4
*/

/* BTN ARRAY INDEXES
MENU_BTN = 0
CLOSE_MENU_BTN = 1
INCREASE_AUTON_BTN = 2
DECREASE_AUTON_BTN = 3
RUN_AUTON_BTN = 4
MOTOR_VITALS_BUTTON = 5
PID_BTN = 6
kP + = 7
ki + = 8
kd + = 9
kP - = 10
ki - = 11
kd - = 12
RETURN_HOME_BUTTON = 13
*/

/* LBL ARRAY INDEXES
MENU_LBL = 0
CLOSE_MENU_LBL = 1
AUTON_SELECTOR_TEXT = 2
INCREASE_AUTON_LBL = 3
DECREASE_AUTON_LBL = 4
RUN_AUTON_LBL = 5
MOTOR_VITALS_TEXT = 6
MOTOR_VITALS_BUTTON_LABEL = 7
RETURN_HOME_LABEL = 8
PID_LABEL = 9
PID_TEXT_LABEL = 10
kP + = 10
ki + = 11
kd + = 12
kP - = 13
ki - = 14
kd - = 15
*/

extern void AutonIncrease();
extern void AutonDecrease();
extern void runAuton();

LV_IMAGE_DECLARE(AviatorRoboticsLogo);
int THEME;

using namespace alight;

lv_obj_t* Image;
lv_timer_t* motorTimer;

std::vector<lv_obj_t*> objArray;
std::vector<lv_obj_t*> btnArray;
std::vector<lv_obj_t*> lblArray;

// Object Creation Functions
// Create Obj
void createObj(lv_obj_t* parent, int x, int y, int width, int height, int index){
    lv_obj_t* obj = lv_obj_create(parent);
    lv_obj_set_size(obj, width, height);
    lv_obj_set_style_bg_opa(obj, LV_OPA_0, LV_PART_MAIN);
    lv_obj_set_style_border_color(obj, lv_color_hex(THEME), LV_PART_MAIN);
    lv_obj_align(obj, LV_ALIGN_TOP_LEFT, x, y);
    objArray.insert(objArray.begin() + index, obj);
}

void createBtn(lv_obj_t* parent, int x, int y, int width, int height, lv_event_cb_t eventCb, int index){
    lv_obj_t *btn = lv_button_create(parent);
    lv_obj_set_size(btn, width, height);
    lv_obj_align(btn, LV_ALIGN_TOP_LEFT, x, y); // position
    lv_obj_set_style_bg_opa(btn, LV_OPA_0, LV_PART_MAIN);
    lv_obj_add_event_cb(btn, eventCb, LV_EVENT_CLICKED, nullptr);
    btnArray.insert(btnArray.begin() + index, btn);
}

void createLbl(lv_obj_t* parent, const char * txt, int index, int x = 0, int y = 0, const lv_font_t * font = &lv_font_montserrat_24){
    lv_obj_t *lbl = lv_label_create(parent);
    lv_label_set_text(lbl, txt);
    lv_obj_align(lbl, LV_ALIGN_CENTER, x, y);
    lv_obj_set_style_text_color(lbl, lv_color_hex(THEME), LV_PART_MAIN); // label color
    lv_obj_set_style_text_font(lbl, font, LV_PART_MAIN);
    lblArray.insert(lblArray.begin() + index, lbl);
}

static void increaseAuton_cb(lv_event_t* e) {
    AutonIncrease();
}

static void decreaseAuton_cb(lv_event_t* e) {
    AutonDecrease();
}

static void runAuton_cb(lv_event_t* e) {
    runAuton();
}

static void menuButtonCB(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t* btn = (lv_obj_t *)lv_event_get_target(e);

    if(code == LV_EVENT_CLICKED) {
        lv_obj_add_flag(btn, LV_OBJ_FLAG_HIDDEN);
        lv_obj_remove_flag(objArray[1], LV_OBJ_FLAG_HIDDEN); // unhides menu
        lv_obj_remove_flag(btnArray[1], LV_OBJ_FLAG_HIDDEN); // unhides close button
    }
}

static void closeButtonCB(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t* btn = (lv_obj_t *)lv_event_get_target(e);

    if(code == LV_EVENT_CLICKED) {
        lv_obj_add_flag(btn, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(objArray[1], LV_OBJ_FLAG_HIDDEN);
        lv_obj_remove_flag(btnArray[0], LV_OBJ_FLAG_HIDDEN);
    }
}

static void motorVitals_cb(lv_event_t * e){
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t* btn = (lv_obj_t *)lv_event_get_target(e);

        if (code == LV_EVENT_CLICKED){
            lv_obj_add_flag(objArray[0], LV_OBJ_FLAG_HIDDEN);
            lv_obj_remove_flag(btnArray[13], LV_OBJ_FLAG_HIDDEN);
            lv_obj_remove_flag(objArray[2], LV_OBJ_FLAG_HIDDEN);
            lv_timer_resume(motorTimer);

        }
}

static void pidTuner_cb(lv_event_t * e){
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t* btn = (lv_obj_t *)lv_event_get_target(e);

        if (code == LV_EVENT_CLICKED){
            lv_obj_add_flag(objArray[0], LV_OBJ_FLAG_HIDDEN); // hide main menu
            lv_obj_remove_flag(btnArray[13], LV_OBJ_FLAG_HIDDEN); // unhide return button
            lv_obj_remove_flag(objArray[4], LV_OBJ_FLAG_HIDDEN); // unhide pid menu
            lv_timer_resume(motorTimer);

        }
}

static void returnMenu_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t* btn = (lv_obj_t *)lv_event_get_target(e);
    
    if (code == LV_EVENT_CLICKED){
        lv_obj_add_flag(btnArray[13], LV_OBJ_FLAG_HIDDEN); // hides itself
        lv_obj_add_flag(objArray[2], LV_OBJ_FLAG_HIDDEN); // hides motor vitals
        lv_obj_add_flag(objArray[4], LV_OBJ_FLAG_HIDDEN); // hides PIDTuner
        lv_obj_remove_flag(objArray[0], LV_OBJ_FLAG_HIDDEN); // unhide
    }
}

static lv_color_t hsv_to_rgb(uint16_t h, uint8_t s, uint8_t v) {
    float hf = h / 60.0f;
    int i = (int)hf;
    float f = hf - i;

    float sv = s / 255.0f;
    float pv = v * (1 - sv);
    float qv = v * (1 - sv * f);
    float tv = v * (1 - sv * (1 - f));

    uint8_t r, g, b;

    switch (i % 6) {
        case 0: r = v;  g = tv; b = pv; break;
        case 1: r = qv; g = v;  b = pv; break;
        case 2: r = pv; g = v;  b = tv; break;
        case 3: r = pv; g = qv; b = v;  break;
        case 4: r = tv; g = pv; b = v;  break;
        default: r = v; g = pv; b = qv; break;
    }

    return lv_color_make(r, g, b);
}


static uint16_t rainbow_hue = 0;

static void rainbow_timer_cb(lv_timer_t* timer) {
    rainbow_hue = (rainbow_hue + 1) % 360;

    lv_color_t color = hsv_to_rgb(rainbow_hue, 255, 255);

    // Border + panel
    for(int i; i != objArray.size(); i ++){
        lv_obj_set_style_border_color(objArray[i], color, LV_PART_MAIN);
    }

    for(int i; i != lblArray.size(); i ++){
        lv_obj_set_style_text_color(lblArray[i], color, LV_PART_MAIN);
    }

    // Logo logo recolor
    lv_obj_set_style_image_recolor(Image, color, LV_PART_MAIN);
}

screener::screener() {}

static void motorInfoTimer_cb(lv_timer_t* timer); // declares existence of timer before function

void screener::motorInfo(std::vector<pros::Motor*> motorOrder, std::vector<pros::MotorGroup*> motorGroupOrder){
    motorList = motorOrder;
    motorGroupList = motorGroupOrder;

    for (int i = 0; i != motorList.size(); i ++){ // Single Motors For loop to print on screen
        lv_obj_t* label = lv_label_create(objArray[2]);
        int y = 5 + (i * 25); // sets the alignment based on the index

        lv_obj_set_style_text_font(label, &lv_font_montserrat_18, LV_PART_MAIN);
        lv_obj_align(label, LV_ALIGN_TOP_LEFT, 5, y);

        motorLabels.insert(motorLabels.begin() + i, label);
    }

    for (int i = 0; i != motorGroupList.size(); i ++){
        lv_obj_t* label = lv_label_create(objArray[2]);
        int y = 5 + ((i + motorLabels.size()) * 25); // sets the alignment based on the index and size of individual motors

        lv_obj_set_style_text_font(label, &lv_font_montserrat_18, LV_PART_MAIN);
        lv_obj_align(label, LV_ALIGN_TOP_LEFT, 5, y);

        motorGroupLabels.insert(motorGroupLabels.begin() + i, label);
    }

    motorTimer = lv_timer_create(motorInfoTimer_cb, 100, this); // this references the current object its being called in. ie the class screener
    lv_timer_pause(motorTimer);
    
}

static void motorInfoTimer_cb(lv_timer_t* timer) { // defines timer after function
    alight::screener * screenerAccess = static_cast<alight::screener*>(lv_timer_get_user_data(timer)); // converts the timer into an object for screener
    auto motor = screenerAccess->motorList[0];

    for (int i = 0; i != screenerAccess->motorList.size(); i ++){
    auto motorName = screenerAccess->motorList[i];
    int motorTemp = screenerAccess->motorList[i]->get_temperature();
    int motorWatts = screenerAccess->motorList[i]->get_power();

    lv_label_set_text_fmt(screenerAccess->motorLabels[i], "- Port %hhd - Temp: %i \u00B0C - Watts: %i", screenerAccess->motorList[i]->get_port(), motorTemp, motorWatts);

        if (motorTemp > 1000){
            lv_label_set_text_fmt(screenerAccess->motorLabels[i], "- Port %hhd disconnected", screenerAccess->motorList[i]->get_port());
        }
    }

    for (int i = 0; i != screenerAccess->motorGroupList.size(); i ++){


        int motorGroupTemp = screenerAccess->motorGroupList[i]->get_temperature();
        int motorGroupWatts = screenerAccess->motorGroupList[i]->get_power();

        lv_label_set_text_fmt(screenerAccess->motorGroupLabels[i], "- Right Drive Train - Temp %i \u00B0C - Watts %i", motorGroupTemp, motorGroupWatts);
                
        if (motorGroupTemp > 1000){
            lv_label_set_text_fmt(screenerAccess->motorGroupLabels[i], "- Motor Group disconnected");
        } else if(i == 1){
            lv_label_set_text_fmt(screenerAccess->motorGroupLabels[i], "- Left Drive Train - Temp %i \u00B0C - Watts %i", motorGroupTemp, motorGroupWatts);
        } else{
            lv_label_set_text_fmt(screenerAccess->motorGroupLabels[i], "- Group Port %hhd - Temp: %i \u00B0C - Watts: %i", screenerAccess->motorGroupList[i]->get_port_all()[0], motorGroupTemp, motorGroupWatts);
        }
    }
}


// print function with variadic arguments for formatted strings
void screener::text(const char* fmt, int index, ...) {
    lv_obj_t* label = lv_label_create(objArray[0]);
    int y = 5 + (index * 30); // vertical spacing of 30 pixels per index

    char buffer[256];
    va_list args;
    va_start(args, index);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    lv_label_set_text(label, buffer);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_18, LV_PART_MAIN);
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 5, y);

    labels.insert(labels.begin() + index, label);
}

void screener::update(int index, const char* fmt, ...) {
    char buffer[256];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    lv_label_set_text(labels[index], buffer);
}

void screener::initAlightScreen(int ThemeHEX) {
    THEME = ThemeHEX;
    // UI Screen setup
    const int WIDTH = 480;
    const int HEIGHT = 240;
    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0x000000), LV_PART_MAIN); // bg color

    // Main menu object where everything is place. When hid everything gets hid
    createObj(lv_screen_active(), 0, 0, WIDTH, HEIGHT, 0); // MAIN MENU IS INDEX 0
    lv_obj_remove_flag(objArray[0], LV_OBJ_FLAG_SCROLLABLE); // makes non scrollable
    
    // Logo STUFF
    Image = lv_image_create(objArray[0]);
    lv_image_set_src(Image, &AviatorRoboticsLogo);
    lv_obj_align(Image, LV_ALIGN_CENTER, 0, -80);
    lv_image_set_scale(Image, 60);
    lv_obj_set_style_image_opa(Image, LV_OPA_50, LV_PART_MAIN);
    lv_obj_set_style_image_recolor(Image, lv_color_hex(THEME), LV_PART_MAIN);
    lv_obj_set_style_image_recolor_opa(Image, LV_OPA_100, LV_PART_MAIN);

    /////////////////////////////////
    // MENU BUTTONS//////////////////
    /////////////////////////////////

    // Menu Button
    createBtn(objArray[0], WIDTH - 60, -10, 24, 24, menuButtonCB, 0); // MENU BUTTON INDEX 0
    createLbl(btnArray[0], LV_SYMBOL_LIST, 0); // MENU LABEL INDEX 0

    // Close Button for Menu
    createBtn(objArray[0], 230, -10, 40, 40, closeButtonCB, 1); // CLOSE MENU BUTTON INDEX 1
    lv_obj_add_flag(btnArray[1], LV_OBJ_FLAG_HIDDEN);
    createLbl(btnArray[1], LV_SYMBOL_CLOSE, 1); // CLOSE MENU LABEL INDEX  1
    lv_obj_set_style_text_color(lblArray[1], lv_color_hex(0xffffff), LV_PART_MAIN); // sets color to white

    // Menu Panel
    createObj(objArray[0], 262, -18, 200, HEIGHT, 1); // MENU PANEL IS INDEX 1
    lv_obj_set_style_bg_opa(objArray[1], LV_OPA_10, LV_PART_MAIN);
    lv_obj_set_style_bg_color(objArray[1], lv_color_hex(THEME), LV_PART_MAIN);
    lv_obj_add_flag(objArray[1], LV_OBJ_FLAG_HIDDEN);

    //  Auton Selector Section
    createLbl(objArray[1], "Auton Selector", 2, 0, -90, &lv_font_montserrat_18); // Auton Selector Label index 2
    lv_obj_set_style_text_color(lblArray[2], lv_color_hex(0xffffff), LV_PART_MAIN); // label color

    // Increase button
    createBtn(objArray[1], 0, 0, 30, 30, increaseAuton_cb, 2); // INCREASE AUTON BUTTON INDEX 2
    createLbl(btnArray[2], LV_SYMBOL_RIGHT, 3); // INCREASE AUTON LABEL INDEX  3
    lv_obj_align(btnArray[2], LV_ALIGN_CENTER, 50, -60);

    // Decrease button
    createBtn(objArray[1], 0, 0, 30, 30, decreaseAuton_cb, 3); // DECREASE AUTON BUTTON INDEX 3 
    createLbl(btnArray[3], LV_SYMBOL_LEFT, 4); // INCREASE AUTON LABEL INDEX  4
    lv_obj_align(btnArray[3], LV_ALIGN_CENTER, -50, -60);

    // Run auton button
    createBtn(objArray[1], 0, 0, 30, 30, runAuton_cb, 4); // RUN AUTON BUTTON INDEX 4
    createLbl(btnArray[4], LV_SYMBOL_PLAY, 5); // RUN AUTON LABEL INDEX  5
    lv_obj_align(btnArray[4], LV_ALIGN_CENTER, 0, -60);

    // Motor Vitals section
    createObj(lv_screen_active(), 0, 0, WIDTH, HEIGHT, 2); // MOTORVITALS PANELS 2
    lv_obj_add_flag(objArray[2], LV_OBJ_FLAG_HIDDEN);

    createLbl(objArray[1], "Motor Vitals", 6, 0, -20, &lv_font_montserrat_18); // Motor Vitals Txt index 6
    lv_obj_set_style_text_color(lblArray[6], lv_color_hex(0xffffff), LV_PART_MAIN); // label color

    // Motor Vitals Button
    createBtn(objArray[1], 0, 20, 45, 40, motorVitals_cb, 5); // MOTOR VITALS BUTTON INDEX 5
    createLbl(btnArray[5], LV_SYMBOL_CHARGE, 7); // MOTOR VITALS LABEL INDEX  7
    lv_obj_align(btnArray[5], LV_ALIGN_CENTER, 0, 15);

    // BRAIN THEME TEXT AND STUFF
    createObj(lv_screen_active(), 0, 0, WIDTH, HEIGHT, 3); // Brain Theme index 3
    lv_obj_add_flag(objArray[3], LV_OBJ_FLAG_HIDDEN);

    // PID BUTTON
    createBtn(objArray[1], 0, 80, 45, 40, pidTuner_cb, 6); // PID menu button index 6
    createLbl(btnArray[6], LV_SYMBOL_SETTINGS, 9); // PID menu index 9
    lv_obj_align(btnArray[6], LV_ALIGN_CENTER, 0, 75);

    // PID MENU
    createObj(lv_screen_active(), 0, 0, WIDTH, HEIGHT, 4); // PID menu index 4
    lv_obj_add_flag(objArray[4], LV_OBJ_FLAG_HIDDEN);

    createBtn(objArray[4], 280, 20, 45, 40, motorVitals_cb, 7); // kP + Button
    createLbl(btnArray[7], LV_SYMBOL_PLUS, 10);
    createBtn(objArray[4], 200, 50, 45, 40, motorVitals_cb, 8); // kI + Button
    createLbl(btnArray[8], LV_SYMBOL_PLUS, 11);
    createBtn(objArray[4], 280, 80, 45, 40, motorVitals_cb, 9); // kD + Button
    createLbl(btnArray[9], LV_SYMBOL_PLUS, 12);

    createBtn(objArray[4], 310, 20, 45, 40, motorVitals_cb, 10); // kP - Button
    createLbl(btnArray[10], LV_SYMBOL_MINUS, 13);
    createBtn(objArray[4], 310, 50, 45, 40, motorVitals_cb, 11); // kI - Button
    createLbl(btnArray[11], LV_SYMBOL_MINUS, 14);
    createBtn(objArray[4], 310, 80, 45, 40, motorVitals_cb, 12); // kD - Button
    createLbl(btnArray[12], LV_SYMBOL_MINUS, 15);


    // PID LABEL
    createLbl(objArray[1], "PID Tuner", 10, 0, 45, &lv_font_montserrat_18); // PID tuner Txt index 6
    lv_obj_set_style_text_color(lblArray[10], lv_color_hex(0xffffff), LV_PART_MAIN); // label color
    
    // Return Menu BTN
    createBtn(lv_screen_active(),WIDTH - 40, 10, 25, 25, returnMenu_cb, 13); // RETURN BUTTON INDEX 13
    createLbl(btnArray[13], LV_SYMBOL_HOME, 8); // RETURN LABEL INDEX  8
    lv_obj_set_style_text_color(lblArray[8], lv_color_hex(0xffffff), LV_PART_MAIN);
    lv_obj_add_flag(btnArray[13], LV_OBJ_FLAG_HIDDEN);

    //lv_timer_create(rainbow_timer_cb,30, nullptr); // comment this out to turn off rainbow feature
}
