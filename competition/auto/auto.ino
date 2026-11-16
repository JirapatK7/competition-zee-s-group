#include <EEPROM.h>

int ena = 5, in1 = 6, in2 = 7;
int in3 = 8, in4 = 9, enb = 10;
int trig = A0, echo = A1;

int speed = 160;
int turnTime = 380;
int cellTime = 650;
int openDist = 15;
int parkDist = 80;

struct Robot {
  int moves;
  unsigned long startTime;
};
Robot robot;

char names[3] = {'R', 'F', 'L'};
int dist[3];

void motors(int a, int b, int c, int d) {
  digitalWrite(in1, a);
  digitalWrite(in2, b);
  digitalWrite(in3, c);
  digitalWrite(in4, d);
  analogWrite(ena, speed);
  analogWrite(enb, speed);
}

void stopRobot() {
  motors(0, 0, 0, 0);
}

void turnRight() {
  motors(1, 0, 0, 1);
  delay(turnTime);
  stopRobot();
  delay(150);
}

void turnLeft() {
  motors(0, 1, 1, 0);
  delay(turnTime);
  stopRobot();
  delay(150);
}

void goForward(Robot *r) {
  motors(1, 0, 1, 0);
  delay(cellTime);
  stopRobot();
  r->moves++;
}

int getDistance() {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  long t = pulseIn(echo, HIGH, 25000);
  if (t == 0) return 300;
  return t * 0.034 / 2;
}

void printDistances(int *d) {
  for (int i = 0; i < 3; i++) {
    Serial.print(names[i]);
    Serial.print("=");
    Serial.print(*(d + i));
    Serial.print("  ");
  }
  Serial.println();
}

void setup() {
  Serial.begin(9600);
  pinMode(ena, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(enb, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  stopRobot();

  byte mode = EEPROM.read(0);
  if (mode == 1) mode = 0;
  else mode = 1;
  EEPROM.update(0, mode);
  if (mode == 0) {
    while (true) {
      stopRobot();
    }
  }

  delay(500);
  robot.moves = 0;
  robot.startTime = millis();
}

void loop() {
  if (millis() - robot.startTime > 88000) {
    stopRobot();
    while (true) {}
  }

  for (int i = 0; i < 3; i++) dist[i] = -1;

  turnRight();
  for (int i = 0; i < 3; i++) {
    if (i > 0) turnLeft();
    dist[i] = getDistance();
    printDistances(dist);

    if (dist[i] > parkDist) {
      goForward(&robot);
      goForward(&robot);
      stopRobot();
      while (true) {}
    }
    if (dist[i] > openDist) {
      goForward(&robot);
      return;
    }
  }

  turnLeft();
  goForward(&robot);
}
