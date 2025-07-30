/* 
 * Project myProject
 * Author: Your Name
 * Date: 
 * For comprehensive documentation and examples, please visit:
 * https://docs.particle.io/firmware/best-practices/firmware-template/
 */

// Include Particle Device OS APIs
#include "Particle.h"

// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(AUTOMATIC);

// Run the application and system concurrently in separate threads
SYSTEM_THREAD(ENABLED);

// Show system, cloud connectivity, and application logs over USB
// View logs with CLI using 'particle serial monitor --follow'
SerialLogHandler logHandler(LOG_LEVEL_INFO);


Servo pitchServo;
Servo yawServo;
Servo rollServo;

// Yaw: 90:middle 50:down 130:up
// Roll: 0 left 90:stop 180:right

int yawServoVal; //initialize variables to store the current value of each servo
int pitchServoVal = 100;
int rollServoVal;

int pitchMoveSpeed = 8; //this variable is the angle added to the pitch servo to control how quickly the PITCH servo moves - try values between 3 and 10
int yawMoveSpeed = 90; //this variable is the speed controller for the continuous movement of the YAW servo motor. It is added or subtracted from the yawStopSpeed, so 0 would mean full speed rotation in one direction, and 180 means full rotation in the other. Try values between 10 and 90;
int yawStopSpeed = 90; //value to stop the yaw motor - keep this at 90
int rollMoveSpeed = 90; //this variable is the speed controller for the continuous movement of the ROLL servo motor. It is added or subtracted from the rollStopSpeed, so 0 would mean full speed rotation in one direction, and 180 means full rotation in the other. Keep this at 90 for best performance / highest torque from the roll motor when firing.
int rollStopSpeed = 90; //value to stop the roll motor - keep this at 90

int yawPrecision = 150; // this variable represents the time in milliseconds that the YAW motor will remain at it's set movement speed. Try values between 50 and 500 to start (500 milliseconds = 1/2 second)
int rollPrecision = 158; // this variable represents the time in milliseconds that the ROLL motor with remain at it's set movement speed. If this ROLL motor is spinning more or less than 1/6th of a rotation when firing a single dart (one call of the fire(); command) you can try adjusting this value down or up slightly, but it should remain around the stock value (160ish) for best results.

int pitchMax = 140; // this sets the maximum angle of the pitch servo to prevent it from crashing, it should remain below 180, and be greater than the pitchMin
int pitchMin = 50; // this sets the minimum angle of the pitch servo to prevent it from crashing, it should remain above 0, and be less than the pitchMax

void setup();
int controlServos(String command);
void loop();

void leftMove(int moves);
void rightMove(int moves);
void upMove(int moves);
void downMove (int moves);

void fire();
void fireAll();
void homeServos();

// setup() runs once, when the device is first turned on
void setup() {
  // Put initialization like pinMode and begin functions here
  
  // Attach servos to pins D0, D1, and D2
  pitchServo.attach(D1); 
  yawServo.attach(D2);
  rollServo.attach(D0);

  Particle.function("motorControl", controlServos);

  homeServos(); //set servo motors to home position
}

int controlServos(String command) {
  // Format: axis:value (e.g., pitch:90)
  int colonIndex = command.indexOf(':');
  if (colonIndex == -1) return -1;

  String axis = command.substring(0, colonIndex);
  int moves = command.substring(colonIndex + 1).toInt();

  // Clamp angle

  Particle.publish("servo-debug", "Axis: " + axis + ", Angle: " + String(moves), PRIVATE);

  if (axis == "up") {
    upMove(moves);
    
  } else if (axis == "down") {
    downMove(moves);

  } else if (axis == "left") {
    leftMove(moves);

  } else if (axis == "right") {
    rightMove(moves);

  } else if (axis == "fire") {
    fire(); //fire a single dart

  } else if (axis == "fireAll") {
    fireAll(); //fire all darts

  } else if (axis == "home") {
    homeServos(); //set all servos to home position
    
  } else {
    return -1;
  }

  return 1;
}

void loop()
{


}

void leftMove(int moves){
    for (int i = 0; i < moves; i++){
        yawServo.write(yawStopSpeed + yawMoveSpeed); // adding the servo speed = 180 (full counterclockwise rotation speed)
        delay(yawPrecision); // stay rotating for a certain number of milliseconds
        yawServo.write(yawStopSpeed); // stop rotating
        delay(5); //delay for smoothness
  }

}

void rightMove(int moves){ // function to move right
  for (int i = 0; i < moves; i++){
      yawServo.write(yawStopSpeed - yawMoveSpeed); //subtracting the servo speed = 0 (full clockwise rotation speed)
      delay(yawPrecision);
      yawServo.write(yawStopSpeed);
      delay(5);
  }
}

void upMove(int moves){
  for (int i = 0; i < moves; i++){
      if(pitchServoVal > pitchMin){//make sure the servo is within rotation limits (greater than 10 degrees by default)
        pitchServoVal = pitchServoVal + pitchMoveSpeed; //decrement the current angle and update
        pitchServo.write(pitchServoVal);
        delay(50);
      }
  }
}

void downMove (int moves){
  for (int i = 0; i < moves; i++){
        if(pitchServoVal < pitchMax){ //make sure the servo is within rotation limits (less than 175 degrees by default)
        pitchServoVal = pitchServoVal - pitchMoveSpeed;//increment the current angle and update
        pitchServo.write(pitchServoVal);
        delay(50);
      }
  }
}

/**
 * fire does xyz
 */
void fire() { //function for firing a single dart
    rollServo.write(rollStopSpeed + rollMoveSpeed);//start rotating the servo
    delay(rollPrecision);//time for approximately 60 degrees of rotation
    rollServo.write(rollStopSpeed);//stop rotating the servo
    delay(5); //delay for smoothness
}

void fireAll() { //function to fire all 6 darts at once
    rollServo.write(rollStopSpeed + rollMoveSpeed);//start rotating the servo
    delay(rollPrecision * 6); //time for 360 degrees of rotation
    rollServo.write(rollStopSpeed);//stop rotating the servo
    delay(5); // delay for smoothness
}

void homeServos(){
    yawServo.write(yawStopSpeed); //setup YAW servo to be STOPPED (90)
    delay(20);
    rollServo.write(rollStopSpeed); //setup ROLL servo to be STOPPED (90)
    delay(100);
    pitchServo.write(100); //set PITCH servo to 100 degree position
    delay(100);
    pitchServoVal = 100; // store the pitch servo value
}
