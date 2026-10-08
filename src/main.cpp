// v2.5
#include "main.h"
#include "lemlib/api.hpp" // IWYU pragma: keep
#include "liblvgl/misc/lv_timer.h"
#include "pros/abstract_motor.hpp"
#include "pros/adi.hpp"
#include "pros/colors.h"
#include "pros/imu.hpp"
#include "pros/motors.hpp"
#include "pros/rtos.hpp"
#include "alight/api.hpp"

// Auton Selector
int CurrentAuton = 0;
const int NumberOfAutons = 5; // The number of Autons in this program, If you need more change this number add a name and add a function
std::array<std::string, NumberOfAutons + 1> Autons = {
    "Auton One", // Name that will appear screen
    "Auton Two",
    "Auton Three",
    "Auton Four",
    "Auton Five"
};

bool fieldConnection = false; // to know wether controller is connected to field false by default

int brainColor = c::COLOR_RED; // brainssss

void AutonIncrease(){
    CurrentAuton ++;
    if(CurrentAuton > NumberOfAutons){
        CurrentAuton = 0;
    }
}

void AutonDecrease(){
    CurrentAuton --;
    if(CurrentAuton < 0){
        CurrentAuton = NumberOfAutons;
    }
}

void runAuton(){
    autonomous();
}

// controller
pros::Controller controller(pros::E_CONTROLLER_MASTER);
 
// Pneumaticss
pros::adi::Pneumatics piston('A', false);
int pisActive = 0; // changes to millis when a piston is used.
// Has the downside of only being able to use 1 piston at a time
// Upside of not interrupting driving with delays

// Motors
pros::MotorGroup groupExample({7, -8});
pros::Motor motorExample(9);

// Drive Train
pros::MotorGroup rightMotors({1, -2, 3},
                            pros::MotorGearset::blue);
pros::MotorGroup leftMotors({-4, 5, -6}, 
                            pros::MotorGearset::blue);
// A negative port means reverse

// Inertial Sensor
pros::Imu imu(10);

// tracking wheels
// horizontal tracking wheel encoder. Rotation sensor
pros::Rotation horizontalEnc(0);
// vertical tracking wheel encoder. Rotation sensor
pros::Rotation verticalEnc(11);
// horizontal tracking wheel. offset, back of the robot (negative)
lemlib::TrackingWheel horizontal(&horizontalEnc, lemlib::Omniwheel::NEW_2, -1);
// vertical tracking wheel. offset, right of the robot (positive)
lemlib::TrackingWheel vertical(&verticalEnc, lemlib::Omniwheel::NEW_2, 0);

// drivetrain settings
lemlib::Drivetrain drivetrain(&leftMotors, // left motor group
                              &rightMotors, // right motor group
                              9, // distance from center of wheels to other side
                              lemlib::Omniwheel::NEW_275, // Wheel Size
                              450, // drivetrain RPM
                              8 // Horizontal drift with Traction = 8, with omnis = 2
                              //Decreasing it will cause the chassis to make a wider turn, while decreasing it will cause the turn to be tighter.
);

// linear constants
float linearkP = 11;
float linearkI = 0;
float linearkD = 70;

// angular constants
float angularkP = 5;
float angularkI = 0;
float angularkD = 54;

// PID controller
// lateral motion controller
lemlib::ControllerSettings linearController(linearkP, // proportional gain (kP)
                                              linearkI, // integral gain (kI)   
                                              linearkD, // derivative gain (kD)
                                              3, // anti windup
                                              0.6, // small error range, in inches
                                              150, // small error range timeout, in milliseconds
                                              1.5, // large error range, in inches
                                              300, // large error range timeout, in milliseconds
                                              0 // maximum acceleration (slew)
);
 
// angular motion (Rotation)
lemlib::ControllerSettings angularController(angularkP, // proportional gain (kP)
                                              angularkI, // integral gain (kI)
                                              angularkD, // derivative gain (kD)
                                              3, // anti windup
                                              1, // small error range, in inches        
                                              160, // small error range timeout, in milliseconds
                                              1.5, // large error range, in inches
                                              300, // large error range timeout, in milliseconds
                                              0 // maximum acceleration (slew)
);

// Dont touch Ki
alight::PID drivePID(linearkP, linearkI, linearkD);

// sensors for odometry
// set to nullptr to use motor IMEs
lemlib::OdomSensors sensors(&vertical, // vertical tracking wheelm , 
                            nullptr, // vertical tracking wheel 2, set to nullptr as we don't have a second one
                            &horizontal, // horizontal tracking wheel
                            nullptr, // horizontal tracking wheel 2, set to nullptr as we don't have a second one
                            &imu // inertial sensor
);
 
// input curve for throttle input during driver control
lemlib::ExpoDriveCurve throttleCurve(10, // joystick deadband out of 127
                                     10, // minimum output where drivetrain will move out of 127
                                     1.019 // expo curve gain
);
 
// input curve for steer input during driver control
lemlib::ExpoDriveCurve steerCurve(10, // joystick deadband out of 127
                                  10, // minimum output where drivetrain will move out of 127
                                  1.019 // expo curve gain
);

// create the chassis
lemlib::Chassis chassis(drivetrain, linearController, angularController, sensors, &throttleCurve, &steerCurve);

//  ---------------------ALIGHT FUNCTIONS-------------------------------------                                              
// distance sensors
pros::Distance frontDistSens(12);
pros::Distance rightDistSens(13);
pros::Distance backDistSens(14);
pros::Distance leftDistSens(15);
alight::distanceFinder frontSensor(&frontDistSens, 
                                    &chassis,
                                    -3.25); 
alight::distanceFinder rightSensor(&rightDistSens, 
                                    &chassis,
                                    5);
alight::distanceFinder backSensor(&backDistSens, 
                                    &chassis,
                                    3);
alight::distanceFinder leftSensor(&leftDistSens, 
                                    &chassis,
                                    4.5);

// chassis constructor for extra drive functions
alight::Drive drive(&leftMotors, &rightMotors, &vertical, &drivePID);

// Lever Controller
alight::extendedPID preciseMotor(&motorExample, 0, 0, 5, pros::E_MOTOR_GEAR_BLUE);

static alight::screener brainScreen;
static void screen_timer_cb(lv_timer_t* timer) {
    auto* screenPtr = static_cast<alight::screener*>(timer->user_data);
    // update, updates the created label
    screenPtr->update(0, "X: %.3f", chassis.getPose().x); // label needs to be created only once so these updates the text instead of making a new one
    screenPtr->update(1, "Y: %.3f", chassis.getPose().y); // we need to do this so we are not creating the same label multiple times
    screenPtr->update(2, "Heading: %.3f°", chassis.getPose().theta);
    screenPtr->update(3, "Auton: %s", Autons[CurrentAuton].c_str());
    screenPtr->update(4, "Battery: %.0f%%", pros::battery::get_capacity());
}

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */

void initialize() {
    chassis.calibrate(); // calibrate sensors
    brainScreen.initAlightScreen(brainColor); // initialize alight screen
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    brainScreen.motorInfo({&motorExample}, {&rightMotors, &leftMotors, &groupExample}); // put them in the order you want them to appear
    // for drivetrain right motors right go first then the left motors
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // text creates a label
    brainScreen.text("X: %.2f", 0, chassis.getPose().x); // creates the label for the updates to stick to
    brainScreen.text("Y: %.2f", 1, chassis.getPose().y);
    brainScreen.text("Heading: %.2f°", 2, chassis.getPose().theta);
    brainScreen.text("Auton: %s", 3, Autons[CurrentAuton].c_str());
    brainScreen.text("Battery: %.0f%%", 4, pros::battery::get_capacity());

        // This timer updates the text on brain screen
        lv_timer_create(
            screen_timer_cb,
            50,
            &brainScreen
        );
        
    // Controller Screen task
    pros::Task screenTask([&]() {
        while (true) {
            // Controller Screens setup
            if(fieldConnection){
                // print current auton on controller screen if connected to field
                controller.print(0, 0," %s", "Auton: " + Autons[CurrentAuton]);
            }else{
                // print robot location to the controller screen
                std::string xPose = "X: " + std::to_string(int(round(chassis.getPose().x)));
                std::string yPose = " Y: " + std::to_string(int(round(chassis.getPose().y)));
                std::string thetaPose = "\u0398 : " + std::to_string(chassis.getPose().theta) + "\u00B0";
                controller.set_text(0, 0, xPose + yPose + thetaPose);
            }

            pros::delay(20); // delay to save resources
            // Color Scroller
        }
    });
}
 
/**
 * Runs while the robot is disabled
 */
void disabled() {}

/**
 * runs after initialize if the robot is connected to field control
 */
void competition_initialize() {
    fieldConnection = true;
}
 
// get a path used for pure pursuitss
// this needs to be put outside a function
ASSET(example_txt); // '.' replace with "_" to make c++ happy

// Write your macros under this comment

// Autons
void AutonOne() {
    chassis.setPose(0, 0, 0);
    frontSensor.fieldOffsets(-15, 24); // sets reference point for auton
    backSensor.fieldOffsets(-15, 24);
    rightSensor.fieldOffsets(-15, 24);
    leftSensor.fieldOffsets(-15, 24);

    leftSensor.resetLeft(); // resets X coordinate
    backSensor.resetBack(); // resets Y coordinate

    preciseMotor.moveTo(90); // example target degree
}
 
void AutonTwo(){
    // Second auton goes here
}
 
void AutonThree(){
    // Third auton goes here
}
 
void AutonFour(){
    // Fourth auton goes here
}

void AutonFive(){
    // Fifth auton goes here
}
 
/**
 * Runs during auto
 *
 * Depending on what auton was chosen it will run that one
 */
void autonomous() {
    // Selects what Auton to run, If you have added more Autons add another case and keep the order
    // You will also need to add its respective function
    switch(CurrentAuton) {
        case 0:
            AutonOne();
            break;
        case 1:
            AutonTwo();
            break;
        case 2:
            AutonThree();
            break;
        case 3:
            AutonFour();
            break;
        case 4:
            AutonFive();
            break;
    }
}

/**
 * Runs in driver control
 */
void opcontrol() {
    // controller
    // loop to continuously update motors
    while (true) {
        // get joystick positions
        int leftY = controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);
        int rightX = controller.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X);
        // move the chassis with curvature drive
        chassis.arcade(leftY, rightX);
        // delay to save resources
        pros::delay(10);

// Start Auton while in Drive
        if (controller.get_digital(DIGITAL_LEFT) && controller.get_digital(DIGITAL_A) && !fieldConnection){ // Left Arrow and A starts auton if youre not connected to field
           autonomous();
        }

        if (controller.get_digital(DIGITAL_LEFT) && controller.get_digital(DIGITAL_X)){ // Left arrow and X cycle Auton
            AutonIncrease();
            pros::delay(300);
        }

        if (controller.get_digital(DIGITAL_B) && (pros::millis() - pisActive) > 350){ // Current millis - millis on activation = time elapsed
            piston.toggle();                                               // ^^^^^^350 milliseconds
            pisActive = pros::millis();
        }

        if(controller.get_digital(DIGITAL_R1)){
            groupExample.move(127);
        }else if (controller.get_digital(DIGITAL_R2)){
            motorExample.move(-127);
        }else{
            groupExample.brake();
            motorExample.brake();
        }

        pros::delay(10); // 10ms delay between each loop to save resources
    }
}