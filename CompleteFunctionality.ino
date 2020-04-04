//Christian Leonard
//March 9, 2020
//HN400

//NOTES:
//----
//Walter is capable of driving Start to Finish, and will successfully draw the maze when prompted in Processing IDE
//Walter utilizes EEPROM to remember the obstacles encountered and movements taken to solve the maze, then transcribes them through Serial

//REMEMBER TO REMOVE BLUETOOTH CHIP BEFORE UPLOADING OR YOU WILL RECEIVE AN ERROR
//----

//serial reading legend:
/*--vehicle movement
   1 = facing backward       3 = turn left
   2 = facing forward        4 = turn right

   99 = move forward in the direction now facing

   10 = object detected front
   11 = object detected left
   12 = object detected right
*/

#include <UCMotor.h>
#include <Servo.h>
#include "UCNEC.h"
#include <SoftwareSerial.h>
#include <EEPROM.h>

#define ULTRASONIC_SENSOR 55
#define SERVO 56

#define TRIG_PIN A2
#define ECHO_PIN A3
#define SERVO_PIN 10

UC_DCMotor leftMotor1(3, MOTOR34_64KHZ);
UC_DCMotor rightMotor1(4, MOTOR34_64KHZ);
UC_DCMotor leftMotor2(1, MOTOR34_64KHZ);
UC_DCMotor rightMotor2(2, MOTOR34_64KHZ);

Servo myservo;

int pos; //position of the servo
int ServoStartPos = 95;

bool RightPathClear;
bool LeftPathClear;
bool ForwardPathClear;

bool NewSolve = true;

int eeAddress;
int cycleThrough;
uint8_t my_int_var;

void setup()
{
  Serial.begin(9600);

  for (int j; j < EEPROM.length(); j++)
  {
    if (EEPROM.read(j) != 0)
    {
      eeAddress = j + 1;
    }

  }
  if (eeAddress == EEPROM.length())
  {
    eeAddress = 0;
  }

  pinMode(ECHO_PIN, INPUT); //Set the connection pin output mode Echo pin
  pinMode(TRIG_PIN, OUTPUT);//Set the connection pin output mode trog pin

  myservo.attach(SERVO_PIN);
  delay(2000);
  ResetServo(); //lines the servo up in the center of the car
  delay(35);
  pos = myservo.read();

  //this is to get the vehicle into the maze initially regardless of a left wall or not - BUT it will only run the first time the car is activated to preserve EERPOM accuracy
  if (EEPROM.read(0) == 0)
  {
    NewSolve = true;
    leftMotor1.run(2); rightMotor1.run(2);
    leftMotor2.run(2); rightMotor2.run(2);
    leftMotor1.setSpeed(224); rightMotor1.setSpeed(250);
    leftMotor2.setSpeed(212); rightMotor2.setSpeed(230);
    delay(250);//run for 1 second (1000 microseconds)
    EEPROM.put(eeAddress, 2);
    eeAddress++;
    EEPROM.put(eeAddress, 99); //------- NO LONGER GOING TO MAP THE INITIAL DRIVE FORWARD
    eeAddress++;
    FullStop();
  }
  else
  {
    NewSolve = false;
    EEPROM.put(eeAddress, 0);
    eeAddress++;
  }
}

void loop()
{
  delay(200);
  Transcribe();
  delay(120);
  WhichPathsAreClear();
  delay(20);
  if (!LeftPathClear && ForwardPathClear) //Yes Left Wall, Nothing in Front
  {
    DriveForward();
    FullStop();
  }
  else if (LeftPathClear && !ForwardPathClear) //No Left Wall, Something in Front
  {
    TurnVehicleLeft();
    FullStop();
  }
  else if (!LeftPathClear && !ForwardPathClear) //Yes Left Wall, Yes something in front
  {
    if (RightPathClear)
    {
    }
    TurnVehicleRight();
    FullStop();
  }
  else if (LeftPathClear && ForwardPathClear) //no Left Wall, no something in front -- probably wont occur in a maze
  {
    delay(200); // -- double check that they are clear
    WhichPathsAreClear();
    if (LeftPathClear && ForwardPathClear)
    {
      CreepForward();
      delay(132);
      TurnVehicleLeft();
    }
    else
    {
    }
  }
  //Transcribe();
}

// --------------PATH CLEARANCE CODE --------------

void isForwardClear() //Checks for obstacles in front of of the vehicle within range
{
  ResetServo();
  delay(20);
  if (readPing() < 25) //shortened the range -- was 25
  {
    ForwardPathClear = false;
    EEPROM.put(eeAddress, 10);
    eeAddress++;
  }
  else
  {
    ForwardPathClear = true;
  }
  delay(20);
}

void isLeftClear() //Checks for obstacles to the left of the vehicle within range
{
  ServoFullLeft();
  delay(20);
  if (readPing() < 35) //shortened the range -- was 35
  {
    LeftPathClear = false;
    EEPROM.put(eeAddress, 11);
    eeAddress++;
  }
  else
  {
    LeftPathClear = true;
  }
  delay(20);
}

void isRightClear() //Checks for obstacles to the right of the vehicle within range
{
  ServoFullRight();
  delay(20);
  if (readPing() < 35) //shortened the range -- was 35
  {
    RightPathClear = false;
    EEPROM.put(eeAddress, 12);
    eeAddress++;
  }
  else
  {
    RightPathClear = true;
  }
  delay(20);
}

void WhichPathsAreClear() //checks L, R, and F in a cycle for obstacles
{
  delay(10);
  isLeftClear();

  delay(10);
  isRightClear();

  delay(10);
  isForwardClear();
}

// -- VEHICLE MOVEMENT CODE --
//Motor Movement Numbers
//------
//2 = forward
//1 = backward
//3 = turns left
//4 = turns right
//5 = stop

//Note: left front motor is slightly slower, by 3
//      left back motor is just slightly faster, by 4

void DriveForward() //just drives forward
{
  Center();
  EEPROM.put(eeAddress, 2); //orientates the vehicle forward
  eeAddress++;

  isForwardClear();
  delay(20);

  if (ForwardPathClear)
  {
    leftMotor1.run(2); rightMotor1.run(2);
    leftMotor2.run(2); rightMotor2.run(2);
    leftMotor1.setSpeed(224); rightMotor1.setSpeed(250);
    leftMotor2.setSpeed(212); rightMotor2.setSpeed(230);
    delay(484);//run for 1 second (1000 microseconds)
    EEPROM.put(eeAddress, 99);
    eeAddress++;
    FullStop();
  }
}

void DriveBackward() //drives backward -- isn't used in the maze solving -- instead turns twice to face outward
{
  EEPROM.put(eeAddress, 1); //orientates the vehicle backward
  eeAddress++;

  leftMotor1.run(1); rightMotor1.run(1);
  leftMotor2.run(1); rightMotor2.run(1);
  leftMotor1.setSpeed(253); rightMotor1.setSpeed(250);
  leftMotor2.setSpeed(214); rightMotor2.setSpeed(216);
  delay(520); //run for 1 second (1000 microseconds)
  EEPROM.put(eeAddress, 99);
  eeAddress++;
  FullStop();
}

void TurnVehicleRight()
{
  EEPROM.put(eeAddress, 4); // orientates vehicle right
  eeAddress++;

  //distance to wall correction -----------
  CorrectSideTurnRight();
  AlignLeft();
  CorrectDistanceFront();
  //distance to wall correction -----------

  leftMotor1.run(4); rightMotor1.run(4);
  leftMotor2.run(4); rightMotor2.run(4);
  leftMotor1.setSpeed(253); rightMotor1.setSpeed(250);
  leftMotor2.setSpeed(214); rightMotor2.setSpeed(216);
  delay(420); //run for 1 second (1000 microseconds)

  FullStop();
  delay(35);

  isForwardClear();
  delay(20);

  if (ForwardPathClear) //so it wont drive into a wall after turning, hopefully
  {
    leftMotor1.run(2); rightMotor1.run(2);
    leftMotor2.run(2); rightMotor2.run(2);
    leftMotor1.setSpeed(224); rightMotor1.setSpeed(250);
    leftMotor2.setSpeed(212); rightMotor2.setSpeed(230);
    delay(275);//run for 1 second (1000 microseconds)
    EEPROM.put(eeAddress, 99);
    eeAddress++;
    FullStop();
  }
  delay(40);
}

void TurnVehicleLeft()
{
  EEPROM.put(eeAddress, 3); // orientates the vehicle left
  eeAddress++;

  //distance to wall correction -----------
  CorrectSideTurnLeft();
  AlignRight();
  CorrectDistanceFront();
  //distance to wall correction -----------

  leftMotor1.run(3); rightMotor1.run(3);
  leftMotor2.run(3); rightMotor2.run(3);
  leftMotor1.setSpeed(253); rightMotor1.setSpeed(250);
  leftMotor2.setSpeed(214); rightMotor2.setSpeed(216);
  delay(435); //run for 1 second (1000 microseconds)

  FullStop();
  delay(35);

  isForwardClear();
  delay(20);

  if (ForwardPathClear)//so it wont drive into a wall after turning, hopefully
  {
    leftMotor1.run(2); rightMotor1.run(2);
    leftMotor2.run(2); rightMotor2.run(2);
    leftMotor1.setSpeed(224); rightMotor1.setSpeed(250);
    leftMotor2.setSpeed(212); rightMotor2.setSpeed(230);
    delay(275);//run for 1 second (1000 microseconds)
    EEPROM.put(eeAddress, 99);
    eeAddress++;
    FullStop();
  }
  delay(40);
}

void FullStop()
{
  leftMotor1.run(5); rightMotor1.run(5);
  leftMotor2.run(5); rightMotor2.run(5);//STOPS the vehicle
}

void ShiftRight() //too close to left wall to turn to the right, must move away - NOTHING TO DO WITH EEPROM
{
  //turn right
  leftMotor1.run(4); rightMotor1.run(4);
  leftMotor2.run(4); rightMotor2.run(4);
  leftMotor1.setSpeed(253); rightMotor1.setSpeed(250);
  leftMotor2.setSpeed(214); rightMotor2.setSpeed(216);
  delay(45); //run for 1 second (1000 microseconds)
  FullStop();
  delay(200);

  //drive forward
  leftMotor1.run(2); rightMotor1.run(2);
  leftMotor2.run(2); rightMotor2.run(2);
  leftMotor1.setSpeed(224); rightMotor1.setSpeed(250);
  leftMotor2.setSpeed(212); rightMotor2.setSpeed(230);
  delay(185 / 2); //run for 1 second (1000 microseconds)
  FullStop();
  delay(200);

  //turn left
  leftMotor1.run(3); rightMotor1.run(3);
  leftMotor2.run(3); rightMotor2.run(3);
  leftMotor1.setSpeed(253); rightMotor1.setSpeed(250);
  leftMotor2.setSpeed(214); rightMotor2.setSpeed(216);
  delay(45); //run for 1 second (1000 microseconds)
  FullStop();
  delay(200);

  //back up to starting point of this loop
  leftMotor1.run(1); rightMotor1.run(1);
  leftMotor2.run(1); rightMotor2.run(1);
  leftMotor1.setSpeed(130); rightMotor1.setSpeed(128);
  leftMotor2.setSpeed(102); rightMotor2.setSpeed(106);
  delay(175.5); //run for 1 second (1000 microseconds) --- previously was set to 200, specifically
  FullStop();
  delay(350);

}

void ShiftLeft() //too close to right wall, shimmy left  - NOTHING TO DO WITH EEPROM
{
  //turn left
  leftMotor1.run(3); rightMotor1.run(3);
  leftMotor2.run(3); rightMotor2.run(3);
  leftMotor1.setSpeed(253); rightMotor1.setSpeed(250);
  leftMotor2.setSpeed(214); rightMotor2.setSpeed(216);
  delay(45); //run for 1 second (1000 microseconds)
  FullStop();
  delay(200);

  //drive forward
  leftMotor1.run(2); rightMotor1.run(2);
  leftMotor2.run(2); rightMotor2.run(2);
  leftMotor1.setSpeed(224); rightMotor1.setSpeed(250);
  leftMotor2.setSpeed(212); rightMotor2.setSpeed(230);
  delay(185 / 2); //run for 1 second (1000 microseconds)
  FullStop();
  delay(200);

  //turn right(straighten out)
  leftMotor1.run(4); rightMotor1.run(4);
  leftMotor2.run(4); rightMotor2.run(4);
  leftMotor1.setSpeed(253); rightMotor1.setSpeed(250);
  leftMotor2.setSpeed(214); rightMotor2.setSpeed(216);
  delay(40.5); //run for 1 second (1000 microseconds)
  FullStop();
  delay(200);

  //back up to starting point of this loop
  leftMotor1.run(1); rightMotor1.run(1);
  leftMotor2.run(1); rightMotor2.run(1);
  leftMotor1.setSpeed(130); rightMotor1.setSpeed(128);
  leftMotor2.setSpeed(102); rightMotor2.setSpeed(106);
  delay(180.5); //run for 1 second (1000 microseconds)  --- previously it was set to 210, specifically
  FullStop();
  delay(350);
}

void CreepForward()
{
  //drive forward slowly until the desired distance is reached
  leftMotor1.run(2); rightMotor1.run(2);
  leftMotor2.run(2); rightMotor2.run(2);
  leftMotor1.setSpeed(224 / 2.5); rightMotor1.setSpeed(250 / 2.5);
  leftMotor2.setSpeed(212 / 2.5); rightMotor2.setSpeed(230 / 2.5);
}

void CreepBackward()
{
  //drive backward until the desired distance is achieved
  leftMotor1.run(1); rightMotor1.run(1);
  leftMotor2.run(1); rightMotor2.run(1);
  leftMotor1.setSpeed(164); rightMotor1.setSpeed(190);
  leftMotor2.setSpeed(154); rightMotor2.setSpeed(170);
}

void CorrectSideTurnRight()
{
  ServoFullLeft();
  int gapL = readPing();
  if (!LeftPathClear) //if no wall there, don't adjust to it
  {
    if (gapL < 18)
    {
      while (readPing() < 18)
      {
        ShiftRight();
      }
      FullStop();
      delay(350);
    }
    else if (gapL > 18)
    {
      while (readPing() > 18)
      {
        ShiftLeft();
      }
      FullStop();
      delay(350);
    }
  }
  else
  {
  }
}

void CorrectSideTurnLeft()
{
  ServoFullRight();
  int gapR = readPing();
  if (!RightPathClear) //if no wall there, don't adjust to it
  {
    if (gapR < 17)
    {
      while (readPing() < 17)
      {
        ShiftLeft();
      }
    }
    else if (gapR > 17)
    {
      while (readPing() > 17)
      {
        ShiftRight();
      }
    }
  }
  else
  {

  }
}

void CorrectDistanceFront()
{
  ResetServo();
  int gapF = readPing();
  if (!ForwardPathClear) //if no wall there, don't adjust to it
  {
    if (gapF > 6)
    {
      while (readPing() > 6)
      {
        CreepForward();
      }
      FullStop();
      delay(200);
    }
    else if (gapF < 6)
    {
      while (readPing() < 6)
      {
        CreepBackward();
      }
      FullStop();
      delay(200);
    }
  }
  else
  {
  }
}

void Center()
{
  if (!LeftPathClear)
  {
    ServoFullLeft();
    delay(50);
    while (readPing() > 14)
    {
      ShiftLeft();
    }
    while (readPing() < 14)
    {
      ShiftRight();
    }
  }
  else if (!RightPathClear)
  {
    ServoFullRight();
    delay(50);
    while (readPing() > 13)
    {
      ShiftRight();
    }
    while (readPing() < 13)
    {
      ShiftLeft();
    }
  }
  else
  {
  }
}

void AlignRight()
{

  leftMotor1.run(3); rightMotor1.run(3);
  leftMotor2.run(3); rightMotor2.run(3);
  leftMotor1.setSpeed(154); rightMotor1.setSpeed(170);
  leftMotor2.setSpeed(80); rightMotor2.setSpeed(82);
  delay(165); //run for 1 second (1000 microseconds)

  FullStop();
  delay(400);

}

void AlignLeft()
{

  leftMotor1.run(4); rightMotor1.run(4);
  leftMotor2.run(4); rightMotor2.run(4);
  leftMotor1.setSpeed(154); rightMotor1.setSpeed(170);
  leftMotor2.setSpeed(98); rightMotor2.setSpeed(100);
  delay(165); //run for 1 second (1000 microseconds)

  FullStop();
  delay(400);

}

// -- SERVO CODE --

void ServoFullRight() //from start position, turn entirely left
{
  for (pos; pos >= 0 ; pos -= 5)
  {
    myservo.write(pos);
    delay(15);
  }
}

void ServoFullLeft() //from start position, turn entirely right
{
  for (pos; pos <= 180; pos += 5)
  {
    myservo.write(pos);
    delay(15);
  }
}

int servoPos() //return the current position of the servo
{
  return myservo.read();
  delay(100);
}

int readPing()
{
  long duration, cm; // establish variables for duration of the ping and the distance result in inches and centimeters

  pinMode(TRIG_PIN, OUTPUT);
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(5);
  digitalWrite(TRIG_PIN, LOW);

  pinMode(ECHO_PIN, INPUT);
  duration = pulseIn(ECHO_PIN, HIGH);

  // convert the time into a distance
  cm = microsecondsToCentimeters(duration);
  return cm;
}

long microsecondsToCentimeters(long microseconds)
{
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the object we take half of the distance travelled.
  return microseconds / 29 / 2;
}

void ResetServo() //Centers the servo and sensor back in the middle of the vehicle, facing forward.
{
  int CurrentPos = myservo.read();
  delay(150);
  if (CurrentPos > ServoStartPos)
  {
    for (CurrentPos; CurrentPos >= ServoStartPos; CurrentPos -= 5)
    {
      myservo.write(CurrentPos);
      delay(15);
    }
  }
  if (CurrentPos < ServoStartPos)
  {
    for (CurrentPos; CurrentPos <= ServoStartPos; CurrentPos += 5)
    {
      myservo.write(CurrentPos);
      delay(15);
    }
  }
  pos = ServoStartPos;
}

void Transcribe()
{
  for (cycleThrough; cycleThrough < eeAddress; cycleThrough++)
  {
    delay(200);
    int val = EEPROM.get(cycleThrough, my_int_var);
    if (val != 0)
    {
      Serial.println(val);
    }
  }
}
