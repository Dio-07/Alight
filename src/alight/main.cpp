#include "main.h"
#include "lemlib/api.hpp" // IWYU pragma: keep
#include "lemlib/chassis/chassis.hpp"
#include "lemlib/chassis/trackingWheel.hpp"
#include "liblvgl/llemu.hpp"
#include "pros/abstract_motor.hpp"
#include "pros/adi.hpp"
#include "pros/imu.hpp"
#include "pros/misc.h"
#include "pros/motors.hpp"
#include "pros/rtos.hpp"
#include <array>
#include <string>
#include "alight/distanceFinder.hpp" // Custom folder for distance sensor
#include "alight/driveFunctions.hpp" // Custom folder for drive functions

// Auton Selector
int CurrentAuton = 0;
const int NumberOfAutons = 5; // The number of Autons in this program, If you need more change this number add a name and add a function
std::array<std::string, NumberOfAutons> Autons = {
    "Auton One", // Name that will appear screen
    "Auton Two",
    "Auton Three",
    "Auton Four",
    "Auton Five"
};

bool fieldConnection = false; // to know wether controller is connected to field false by default

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
pros::adi::Pneumatics Piston('A', false);
int pisActive = 0; // changes to millis when a piston is used.
// Has the downside of only being able to use 1 piston at a time
// Upside of not interrupting driving with delays

// Motors
pros::Motor Motor(0);
pros::MotorGroup MotorGroup({0, 0});
 
// Drive Train
pros::MotorGroup rightMotors({-0, 0, -0},
                            pros::MotorGearset::blue);
pros::MotorGroup leftMotors({0, -0,0}, 
                            pros::MotorGearset::blue);
// A negative port means reverse

// Inertial Sensor
pros::Imu imu(0);

// tracking wheels
// horizontal tracking wheel encoder. Rotation sensor
pros::Rotation horizontalEnc(0);
// vertical tracking wheel encoder. Rotation sensor
pros::Rotation verticalEnc(0);
// horizontal tracking wheel. offset, back of the robot (negative)
lemlib::TrackingWheel horizontal(&horizontalEnc, lemlib::Omniwheel::NEW_2, -1);
// vertical tracking wheel. offset, right of the robot (positive)
lemlib::TrackingWheel vertical(&verticalEnc, lemlib::Omniwheel::NEW_2, 1);

// drivetrain settings
lemlib::Drivetrain drivetrain(&leftMotors, // left motor group
                              &rightMotors, // right motor group
                              9, // distance from center of wheels to other side
                              lemlib::Omniwheel::NEW_275,
                              450, // drivetrain RPM
                              8 // Horizontal drift with Traction = 8, with omnis = 2
                              //Decreasing it will cause the chassis to make a wider turn, while decreasing it will cause the turn to be tighter.
);
 
// PID controller
// lateral motion controller
lemlib::ControllerSettings linearController(1, // proportional gain (kP)
                                              0, // integral gain (kI)
                                              0, // derivative gain (kD)
                                              3, // anti windup
                                              0.5, // small error range, in inches
                                              100, // small error range timeout, in milliseconds
                                              2, // large error range, in inches
                                              500, // large error range timeout, in milliseconds
                                              0 // maximum acceleration (slew)
);
 
// angular motion (Rotation)
lemlib::ControllerSettings angularController(1, // proportional gain (kP)
                                              0, // integral gain (kI)
                                              0, // derivative gain (kD)
                                              3, // anti windup
                                              1, // small error range, in inches
                                              100, // small error range timeout, in milliseconds
                                              3, // large error range, in inches
                                              500, // large error range timeout, in milliseconds
                                              0 // maximum acceleration (slew)
);

// sensors for odometry
lemlib::OdomSensors sensors(&vertical, // vertical tracking wheel
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
pros::Distance frontDistSens(0);
pros::Distance rightDistSens(0);
alight::DistanceSensor frontSensor(&frontDistSens, 
                                    &chassis,
                                    &imu, 
                                    0, // left right offset if sensor is on sides, else front back offset
                                    0); // an offset of 0 means its in the front -90 means left and 90 means right
alight::DistanceSensor rightSensor(&rightDistSens, 
                                    &chassis,
                                    &imu, 
                                    0, // left right offset if sensor is on sides, else front back offset
                                    90); // an offset of 0 means its in the front -90 means left and 90 means right
                                
// chassis constructor for extra drive functions
alight::Drive drive(&leftMotors, &rightMotors);

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
 
void initialize() {
    pros::lcd::initialize(); // initialize brain screen    
    chassis.calibrate(); // calibrate sensors
 
    // the default rate is 50. however, if you need to change the rate, you
    // can do the following.
    // lemlib::bufferedStdout().setRate(...);
    // If you use bluetooth or a wired connection, you will want to have a rate of 10ms
 
    // for more information on how the formatting for the loggers
    // works, refer to the fmtlib docs

    // thread to for brain screen and position logging
    pros::Task screenTask([&]() {
        while (true) {
            // print robot location to the brain screen
            pros::lcd::print(0, "X: %f", chassis.getPose().x); // x
            pros::lcd::print(1, "Y: %f", chassis.getPose().y); // y
            pros::lcd::print(2, "Theta: %f", chassis.getPose().theta); // heading
            pros::lcd::print(3, " %s", "Auton: " + Autons[CurrentAuton]); // Set Auton
            
            pros::lcd::print(5, " %f", rightMotors.get_temperature()); // Temperature of right motors
            pros::lcd::print(6, " %f", leftMotors.get_temperature()); // Temperature of left motors

            
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

            // Brain Screen Buttons
            pros::lcd::register_btn0_cb(AutonDecrease); // Left button decreases Auton
            pros::lcd::register_btn1_cb(runAuton); // Middle Button Runs Auton
            pros::lcd::register_btn2_cb(AutonIncrease); // Right Button Increases Auton
 
 
            // log position telemetry
            lemlib::telemetrySink()->info("Chassis pose: {}", chassis.getPose());
            // delay to save resources
            pros::delay(50);
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
 
// get a path used for pure pursuit
// this needs to be put outside a function
ASSET(example_txt); // '.' replace with "_" to make c++ happy

// Autons
void AutonOne() { 
    // When writing an Auton You should delete all of this, This helps you understand what is available to you

    // set position to x:0, y:0, heading: 90
    chassis.setPose(0, 0, 90);
    frontSensor.setFieldOffsets(10, 10); // The field offset are to the tracking point not sensors
    rightSensor.setFieldOffsets(10, 10); // Thus they should be the same

    chassis.moveToPoint(0, 15, 1000);
    chassis.moveToPose(0, 15, 0, 1000);
 
    chassis.turnToHeading(90, 1000);
    chassis.turnToPoint(10, 10, 1000);
 
    chassis.swingToHeading(90, lemlib::DriveSide::LEFT, 1000);
    chassis.swingToPoint(10, 10, lemlib::DriveSide::RIGHT, 1000);

    chassis.moveToPoint( 10,10, 50, {}, false); // Setting async to false means the robot must complete this before moving on with the rest of the code
    chassis.turnToPoint(10, 90, 1000, {.forwards = false}, false);
                                                            // Parameters are extra items to tune your Auton
                                                            // Example are .maxSpeed, .minSpeed, .forwards
    chassis.moveToPose(0, 15, 0, 1000);

    drive.driveFor(1000, -127); // drives reverse for 1 second
    drive.wiggle(127, 5); // wiggles at maximum intensity 5 times
    drive.jiggle(127, 5); // jiggles at maximum intensity 5 times

    frontSensor.resetRight(); // This resets the x coordinate according to the distance from the right wall
    // MoveToPoint finds the direct route while MoveToPose lets you chose what direction the robot faces at the end
    // turnToHeading turns to the degree instructed in place
    // turnToPoint turns to a point on the field will always turn to this point regardless of location
    // chassis.swingToHeading(90, lemlib::DriveSide::LEFT) (Locks left side)
    // swingToPoint swings the robot to the point instructed
 
 
    // Timeout is how long the robot has to complete that action
 
    // To use pure pursuit use website pathjerry, Download the text file and put it under static folder

    
}
 
void AutonTwo(){
// Code for Auton Two
}
 
void AutonThree(){
// Code For Auton Three
}
 
void AutonFour(){
// Code For Auton Four
}
 
void AutonFive(){
// Code For Auton Five
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
    chassis.setPose(0, 0 ,0); // Starting Pose In driver Control
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
            Piston.toggle();                                                        // 350 milliseconds
            pisActive = pros::millis();
        }
 
        // Spins the single motor when R1 or R2 is pressed
        if (controller.get_digital(DIGITAL_R1)){
            Motor.move(127);
        }else if (controller.get_digital(DIGITAL_R2)){
            Motor.move(-127);
        }else{
            Motor.brake();
        }

        // Spins the motor group when L1 or L2 is pressed
        if (controller.get_digital(DIGITAL_L1)){
            MotorGroup.move(-127);
        }else if (controller.get_digital(DIGITAL_L2)){
            MotorGroup.move(127);
        }else{
            MotorGroup.brake();
        }

    }
}