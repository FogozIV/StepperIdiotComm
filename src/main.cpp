#include <Arduino.h>
#include <AccelStepper.h>
#include <TMCStepper.h>
#include <Wire.h>
#include "LiftHandler.h"

#define ENABLE_PIN 4
#define STEP_PIN 5
#define DIR_PIN 6
#define DIAG_PIN 7
#define INDEX_PIN 8
#define BUTTON_PIN 11

#define DRIVER_ADDRESS 0b00
#define SERIAL_PORT Serial

AccelStepper stepper(AccelStepper::DRIVER, STEP_PIN, DIR_PIN);
LiftHandler lift_handler(stepper);
bool button_state;

void stallInterrupt() {

}
/*
ISR(PCINT0_vect) {
    if (button_state && !digitalRead(BUTTON_PIN)) {
        lift_handler.noticeButtonInterrupt();
        button_state = false;
    }else if (!button_state && digitalRead(BUTTON_PIN)) {
        button_state = true;
    }

}*/

void receiveEvent(int i) {
    lift_handler.onDataReceived(i);
}

void requestEvent() {
    lift_handler.onDataRequest();
}

void setup() {
    pinMode(0, OUTPUT);
    pinMode(1, OUTPUT);
    digitalWrite(0, LOW);
    digitalWrite(1, HIGH);
    pinMode(INDEX_PIN, INPUT);
    pinMode(DIAG_PIN, INPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    button_state = digitalRead(BUTTON_PIN);


    pinMode(DIR_PIN, OUTPUT);
    pinMode(STEP_PIN, OUTPUT);
    pinMode(ENABLE_PIN, OUTPUT);
    attachInterrupt(digitalPinToInterrupt(DIAG_PIN), stallInterrupt, RISING);
    PCMSK0 |= 1 << PCINT7;


    Wire.begin(0x22);
    Wire.onReceive(receiveEvent);
    Wire.onRequest(requestEvent);

    stepper.setMaxSpeed(1000);
    stepper.setAcceleration(1000);
    stepper.setEnablePin(ENABLE_PIN);
    stepper.setPinsInverted(false, false, true);
    stepper.enableOutputs();

    stepper.moveTo(1500);
    while (stepper.run());

}

void loop() {
    lift_handler.syncAction();
}
