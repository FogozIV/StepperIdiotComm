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
#define SERIAL_PORT Serial1

#define R_SENSE 0.11f
#define STALL_VALUE 25
TMC2209Stepper driver(&SERIAL_PORT, R_SENSE, DRIVER_ADDRESS);
AccelStepper stepper(AccelStepper::DRIVER, STEP_PIN, DIR_PIN);
LiftHandler lift_handler(stepper, driver);
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
    SERIAL_PORT.begin(115200);
    Serial.begin(115200);
    delay(4000);
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

    driver.begin();
    driver.rms_current(2000);
    driver.pwm_autoscale(true);
    driver.microsteps(8);
    driver.TCOOLTHRS(0xFFFFF); // 20bit max
    driver.SGTHRS(STALL_VALUE);
    driver.COOLCONF(0xFFFFF);
    driver.ihold(31);
    driver.hold_multiplier(1);
    uint8_t connTest = driver.test_connection();
    Serial.println(connTest);
    stepper.setMaxSpeed(1000);
    stepper.setAcceleration(1000);
    stepper.setEnablePin(ENABLE_PIN);
    stepper.setPinsInverted(false, false, true);
    stepper.enableOutputs();

}

void loop() {
    lift_handler.syncAction();
}
