#include <adk.h>
// MD49 (Motor Controaler ) command
#define CMD        (byte)0x00            // MD49 command address of 0                                 
#define GET_SPEED1       0x21
#define GET_SPEED2       0x22
#define GET_ENCODER1     0x23
#define GET_ENCODER2     0x24
#define GET_ENCODERS     0x25
#define GET_VOLTS        0x26
#define GET_CURRENT1     0x27
#define GET_CURRENT2     0x28
#define GET_VERSION      0x29
#define GET_ACCELE       0x2A
#define GET_MODE         0x2B
#define GET_VI           0x2C
#define GET_ERROR        0x2D
#define SET_SPEED1       0x31
#define SET_SPEED2       0x32
#define SET_ACCEL        0x33
#define SET_MODE         0x34
#define REST_ENCODERS    0x35
#define DIS_REGUL        0x36
#define ENABLE_REGUL     0x37
#define DIS_TIMEOUT      0x38
#define ENABLE_TIMEOUT   0x39

int mSpeed = 168;
int mTurn = 3100;
int mDownS = 128 - (mSpeed - 128);


#define BUFFSIZE 4
#define LED 13

// Accessory descriptor. It's how Arduino identifies itself in Android.
char accessoryName[] = "LED actuator";
char manufacturer[] = "Example, Inc.";
char model[] = "ADK-Led";

char versionNumber[] = "0.1.0";
char serialNumber[] = "1";
char url[] = "http://www.example.com";

// ADK configuration
USBHost Usb;
ADK adk(&Usb, manufacturer, model, accessoryName, versionNumber, url, serialNumber);
uint8_t buffer[BUFFSIZE];
uint32_t bytesRead = 0;


//Ultrasonic sensor PINS
const int pingUp = 7; // up
const int pingLeft = 8; // left
const int pingRgiht = 9; // right

int dis = 2;
void setup()
{
  Serial1.begin(9600);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  delay(1000);
  Serial.begin(115200);
  delay(1000);
  Serial.println("Ready for commands...");
  int x = 0;
  Serial1.write(CMD);
  Serial1.write(DIS_TIMEOUT);
  Serial1.write(CMD);
  Serial1.write(SET_MODE);
  Serial1.write(x);
  Serial1.write(CMD);
  Serial1.write(SET_ACCEL);
  Serial1.write(5);
}

void loop()
{
  readingFromADK();
}

long microsecondsToInches(long microseconds) {
  // According to Parallax's datasheet for the PING))), there are
  // 73.746 microseconds per inch (i.e. sound travels at 1130 feet per
  // second).  This gives the distance travelled by the ping, outbound
  // and return, so we divide by 2 to get the distance of the obstacle.
  // See: http://www.parallax.com/dl/docs/prod/acc/28015-PING-v1.3.pdf
  return microseconds / 74 / 2;
}

long microsecondsToCentimeters(long microseconds) {
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  return microseconds / 29 / 2;
}

void readingFromADK()
{
  Usb.Task();

  // Starting listening when ADK is available
  if (adk.isReady()) {
    

    adk.read(&bytesRead, BUFFSIZE, buffer);
    if (bytesRead > 0) {
      commandInterpreter(parseResponse());    
    }
  }
}

uint8_t parseResponse()
{
  return buffer[0] - 48;
}

void commandInterpreter(uint8_t command)
{
  switch (command) {
    case 0:
      /// move forwed
      Serial.println("Received command: 0 -> move forwed");
      digitalWrite(LED, HIGH);
      mUp();
      break;
    case 1:
      // Stop
      Serial.println("Received command: 1 -> Stop");
      digitalWrite(LED, LOW);
      mStop();
      break;
    case 2:
      // turn left
      Serial.println("Received command: 2 -> turn left");
      digitalWrite(LED, HIGH);
      mLeft();
      break;
    case 3:
      // turn right
      Serial.println("Received command: 3 -> turn right");
      digitalWrite(LED, LOW);
      mRight();
      break;
    case 4:
      // client near
      Serial.println("Received command: 4 -> client near");
      digitalWrite(LED, HIGH);
      dis = 0;
      break;
    case 5:
      //client far
      Serial.println("Received command: 5 -> client far");
      digitalWrite(LED, LOW);
      dis = 1;
      break;
    case 6:
      mDown();
      digitalWrite(LED, HIGH);
      break;
    default:
      Serial.println("Command not available");
      break;
  }
}


void mUp() {
  if (dis == 1) {
    if (cUp() > 35) {
      Serial1.write(CMD);
      Serial1.write(SET_SPEED1);
      Serial1.write(mSpeed);
      Serial1.write(CMD);
      Serial1.write(SET_SPEED2);
      Serial1.write(mSpeed);
      delay(1000);
      mStop();
    } else {
      vUp();
    }
  }
}

void mDown() {
  Serial1.write(CMD);
  Serial1.write(SET_SPEED1);
  Serial1.write(88);
  Serial1.write(CMD);
  Serial1.write(SET_SPEED2);
  Serial1.write(88);
  delay(1000);
  mStop();
}
void mStop() {
  Serial1.write(CMD);
  Serial1.write(SET_SPEED1);
  Serial1.write(128);
  Serial1.write(CMD);
  Serial1.write(SET_SPEED2);
  Serial1.write(128);
}
void mLeft() {
//  mUp();
  if (dis == 1) {
    if (cLeft() > 35) {
      Serial1.write(CMD);
      Serial1.write(SET_SPEED1);
      Serial1.write(128);
      Serial1.write(CMD);
      Serial1.write(SET_SPEED2);
      Serial1.write(mSpeed);

      delay(mTurn);

      mStop();
    } else {
      vLeft();
    }
  }
}
void mRight() {
//  mUp();
  if (dis == 1) {
    if (cRight() > 35) {
      Serial1.write(CMD);
      Serial1.write(SET_SPEED1);
      Serial1.write(mSpeed);
      Serial1.write(CMD);
      Serial1.write(SET_SPEED2);
      Serial1.write(128);

      delay(mTurn);

      mStop();
    } else {
      vRight();
    }
  }
}

long cUp() {
  long duration1, inches1, cm1;

  // The PING))) is triggered by a HIGH pulse of 2 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  pinMode(pingUp, OUTPUT);
  digitalWrite(pingUp, LOW);
  delayMicroseconds(2);
  digitalWrite(pingUp, HIGH);
  delayMicroseconds(5);
  digitalWrite(pingUp, LOW);

  // The same pin is used to read the signal from the PING))): a HIGH
  // pulse whose duration1 is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  pinMode(pingUp, INPUT);
  duration1 = pulseIn(pingUp, HIGH);

  // convert the time into a distance
  inches1 = microsecondsToInches(duration1);
  cm1 = microsecondsToCentimeters(duration1);

  Serial.print(inches1);
  Serial.print("in1, ");
  Serial.print(cm1);
  Serial.print("cm1");
  Serial.println();

  return cm1;
}

long cLeft() {
  long duration2, inches2, cm2;

  pinMode(pingLeft, OUTPUT);
  digitalWrite(pingLeft, LOW);
  delayMicroseconds(2);
  digitalWrite(pingLeft, HIGH);
  delayMicroseconds(5);
  digitalWrite(pingLeft, LOW);

  pinMode(pingLeft, INPUT);
  duration2 = pulseIn(pingLeft, HIGH);

  // convert the time into a distance
  inches2 = microsecondsToInches(duration2);
  cm2 = microsecondsToCentimeters(duration2);

  Serial.print(inches2);
  Serial.print("in, ");
  Serial.print(cm2);
  Serial.print("cm2");
  Serial.println();
  return cm2;
}

long cRight() {
  long duration3, inches3, cm3;

  pinMode(pingRgiht, OUTPUT);
  digitalWrite(pingRgiht, LOW);
  delayMicroseconds(3);
  digitalWrite(pingRgiht, HIGH);
  delayMicroseconds(5);
  digitalWrite(pingRgiht, LOW);

  pinMode(pingRgiht, INPUT);
  duration3 = pulseIn(pingRgiht, HIGH);

  // convert the time into a distance
  inches3 = microsecondsToInches(duration3);
  cm3 = microsecondsToCentimeters(duration3);

  Serial.print(inches3);
  Serial.print("in, ");
  Serial.print(cm3);
  Serial.print("cm3");
  Serial.println();

  return cm3;
}

void vUp() {
  mStop();
  if (cLeft() > cRight() ) {
    mSLeft();
    mSUp();
    mSRight();
    while ( cRight() < 35 ) {
      // go up
      mSUp();
    }
    mSRight();
    // go up
    mSUp();
    mSLeft();

  } else if (cLeft() < cRight() ) {
    mSRight();
    // go up
    mSUp();
    mSLeft();
    while (cLeft() < 35) {
      // go up
      mSUp();
    }
    mSLeft();
    mSUp();
    mSRight();
  }
}

void vLeft() {
  while (cLeft() < 35) {
    // go up
    mSUp();
  }
  //turn left
  mSLeft();
  mSUp();
  mSRight();
}
void vRight() {
  while ( cRight() < 35 ) {
    mSUp();
  }
  mSRight();
  mSUp();
  mSLeft();
}

void mSUp() {
  Serial1.write(CMD);
  Serial1.write(SET_SPEED1);
  Serial1.write(mSpeed);
  Serial1.write(CMD);
  Serial1.write(SET_SPEED2);
  Serial1.write(mSpeed);
  delay(1000);
  mStop();
}

void mSRight() {
  Serial1.write(CMD);
  Serial1.write(SET_SPEED1);
  Serial1.write(mSpeed);
  Serial1.write(CMD);
  Serial1.write(SET_SPEED2);
  Serial1.write(128);
  delay(mTurn);
  mStop();
}

void mSLeft() {
  Serial1.write(CMD);
  Serial1.write(SET_SPEED1);
  Serial1.write(128);
  Serial1.write(CMD);
  Serial1.write(SET_SPEED2);
  Serial1.write(mSpeed);
  delay(mTurn);
  mStop();
}
