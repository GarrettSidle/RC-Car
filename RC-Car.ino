#include <IRremote.h>

//input for remote
int IRInput = 7;

//pins for displaying killMode
int ModeRed = 3;
int ModeGreen = 2;

//the current state of killMode
bool IsKillMode = false;

//pins for H bridge
int LeftEnable = 13;
int LeftMotors1 = 12;
int LeftMotors2 = 11;
int RightMotors1 = 9;
int RightMotors2 = 10;
int RightEnable = 8;

//pins for ping
int PingTrig = 5;
int PingEcho = 6;
//current state of ping on/off
bool PingToggle = false;

//used for storing previous IR code
//this is needed so you can hold the button down to move
int PrevIRCode;

IRrecv irrecv(IRInput);
decode_results results;
void setup() {
  irrecv.enableIRIn();
  pinMode(ModeRed, OUTPUT);
  pinMode(ModeGreen, OUTPUT);
  pinMode(PingEcho, INPUT);
  pinMode(PingTrig, OUTPUT);
  pinMode(LeftMotors1, OUTPUT);
  pinMode(LeftMotors2, OUTPUT);
  pinMode(LeftEnable, OUTPUT);
  pinMode(RightMotors1, OUTPUT);
  pinMode(RightMotors2, OUTPUT);
  pinMode(RightEnable, OUTPUT);
  Serial.begin(9600);
}
void loop() {
  //Check the IR Sensors
  checkIRSens();
  //checkPing
  runPing();
}


void runPing() {
  //If ping toggle is active
  if (PingToggle) {
    // get distance value
    int Distance = ping();
    //If it is within range
    if (Distance < 5) {
      //If kill mode is active
      if (IsKillMode) {
        //then ATTACK!!!!
        forward();
      }  //If boring mode is active
      else {
        //then avoid
        turnRight();
      }
    }
  }
}

int ping() {
  long distance;

  //pulse the sensor and read the distance
  digitalWrite(PingTrig, LOW);
  delayMicroseconds(2);
  digitalWrite(PingTrig, HIGH);
  delayMicroseconds(5);
  digitalWrite(PingTrig, LOW);
  distance = pulseIn(PingEcho, HIGH);

  //convert to CM
  distance = distance / 29 / 2;
  return distance;
}

void checkIRSens() {
  if (irrecv.decode(&results)) {  //if we recieve an ir signal
    int IRCode = results.value;
    switch (IRCode) {
      case (25245):  // "CH" Button / forward
        forward();
        PrevIRCode = 25245;
        break;
      case 8925:  //"Rewind" Button / Left
        turnLeft();
        PrevIRCode = 8925;
        break;
      case 765:  //"Fast Forward" Button/ Backwards
        backwards();
        PrevIRCode = 765;
        break;
      case -15811:  //"Skip" Button / Right
        turnRight();
        PrevIRCode = -15811;
        break;
      case 21165:  //"9" Button / Toggle Ping
        PingToggle = !PingToggle;
        PrevIRCode = 0;
        doLights();
        break;
      case -7651:                  //"CH+"  Button / Toggle Kill State
        IsKillMode = !IsKillMode;  //swap kill mode
        PrevIRCode = 0;
        doLights();

        break;
      case -1:
        /*
      The remote sends -1 when a button is held down
      The program saves the prevous value and tries to continously 
      rerun it until it recieves no value from the remote
      */
        switch (PrevIRCode) {
          case (25245):  // Keypad button "CH" / forward;
            forward();
            break;
          case 8925:  //Keypad button "Rewind" / Left
            turnLeft();
            break;
          case 765:  //Keypad button "Fast Forward"/ Backwards
            backwards();
            break;
          case -15811:  //Keypad button "Skip" / Right
            turnRight();
            break;
        }
      default:
        break;
    }
    //return to reading the sensor
    irrecv.resume();
  }
}

void doLights() {
  if (PingToggle) {
    //if kill mode is on
    if (IsKillMode) {
      //turn on red light
      digitalWrite(ModeRed, HIGH);
      digitalWrite(ModeGreen, LOW);
    } else {
      //turn on green light
      digitalWrite(ModeRed, LOW);
      digitalWrite(ModeGreen, HIGH);
    }
  }
  else{
      digitalWrite(ModeRed, LOW);
      digitalWrite(ModeGreen, LOW);    
  }
}

void backwards() {
  //both tires go backwards
  leftTire(true);
  rightTire(true);
  delay(500);  //stop
  stopTires();
}
void turnLeft() {
  //left tire goes back, right goes forward
  leftTire(true);
  rightTire(false);
  delay(500);  //stop
  stopTires();
}
void turnRight() {
  //left tire goes back, right goes forward
  leftTire(false);
  rightTire(true);
  delay(500);  //stop
  stopTires();
}
void forward() {
  //both tires go forward
  leftTire(false);
  rightTire(false);
  delay(500);  //stop
  stopTires();
}

void leftTire(bool isForward) {
  //write pins based on the passed parameter
  if (isForward) {
    digitalWrite(LeftEnable, LOW);
    digitalWrite(LeftMotors2, HIGH);
    digitalWrite(LeftMotors1, LOW);
    analogWrite(LeftEnable, 255);
  } else {
    digitalWrite(LeftEnable, LOW);
    digitalWrite(LeftMotors2, LOW);
    digitalWrite(LeftMotors1, HIGH);
    analogWrite(LeftEnable, 255);
  }
}

void rightTire(bool isForward) {
  //write pins based on the passed parameter
  if (isForward) {
    digitalWrite(RightEnable, LOW);
    digitalWrite(RightMotors2, LOW);
    digitalWrite(RightMotors1, HIGH);
    analogWrite(RightEnable, 255);
  } else {
    digitalWrite(RightEnable, LOW);
    digitalWrite(RightMotors2, HIGH);
    digitalWrite(RightMotors1, LOW);
    analogWrite(RightEnable, 255);
  }
}

void stopTires() {
  //Write everthing low
  digitalWrite(RightEnable, LOW);
  digitalWrite(RightMotors1, LOW);
  digitalWrite(RightMotors2, LOW);
  analogWrite(RightEnable, LOW);

  digitalWrite(LeftEnable, LOW);
  digitalWrite(LeftMotors2, LOW);
  digitalWrite(LeftMotors1, LOW);
  analogWrite(LeftEnable, LOW);
}