//
// Created by fogoz on 29/04/2025.
//

#include "LiftHandler.h"


LiftHandler::LiftHandler(AccelStepper &stepper, TMC2209 &driver): stepper(stepper),
                                                                  driver(driver) {
    stepper.setMaxSpeed(max_speed);
    stepper.setAcceleration(acc);
}

void LiftHandler::onDataReceived(int why) {
    uint8_t packet_id;
    if (Wire.available()) {
        packet_id = Wire.read();
    }else {
        return;
    }
    if (!Wire.available()) {
        return;
    }
    Serial.println(Wire.available());
    auto* data = new uint8_t[Wire.available()];
    uint8_t size = Wire.readBytes(data, Wire.available());
    payload_handler[packet_id](data, size, *this);
    delete[] data;
}

void LiftHandler::onDataRequest() {
    if (responseHandler!=nullptr) {
        responseHandler(*this);
    }
    responseHandler = nullptr;
}

void LiftHandler::syncAction() {
    if (!homing_mode) {
        stepper.run();
    }else {
        stepper.runSpeed();
    }
    if(!changed)
        return;
    stepper.setMaxSpeed(max_speed);
    stepper.setAcceleration(acc);
    stepper.moveTo(target);
    changed = false;
}

float LiftHandler::getMaxSpeed() const {
    return max_speed;
}

float LiftHandler::getAcc() const {
    return acc;
}

int LiftHandler::getTarget() const {
    return target;
}

void LiftHandler::setMaxSpeed(float max_speed) {
    this->max_speed = max_speed;
    this->changed = true;
}

void LiftHandler::setAcc(float acc) {
    this->acc = acc;
    this->changed = true;
}

void LiftHandler::setTarget(int target) {
    this->target = target;
    this->changed = true;
}

void LiftHandler::noticeButtonInterrupt() {
    if (homing_mode) {
        homing_mode = false;
        stepper.setSpeed(0);
        stepper.setCurrentPosition(0);
        setTarget(0);
    }
}

void LiftHandler::doHoming() {
    homing_mode = true;
    stepper.setSpeed(-200);
}


MAKE_PAYLOAD_FUNCTION(setTargetPos,
    auto pos = (int16_t*)data;
    lift.target = *pos;
    lift.changed = true;
)

MAKE_PAYLOAD_FUNCTION(setCurrentPos,
    auto pos = (int16_t*)data;
    lift.stepper.setCurrentPosition(*pos);
    )
MAKE_PAYLOAD_FUNCTION(setMaxSpeed,
    auto max_speed = (int16_t*)data;
    lift.max_speed = static_cast<float>(*max_speed);
    lift.changed = true;
    )
MAKE_PAYLOAD_FUNCTION(setAcceleration,
    auto acc = (int16_t*)data;
    lift.acc = static_cast<float>(*acc);
    lift.changed = true;
    )


MAKE_PAYLOAD_FUNCTION(setRMSCurrent,
    auto current = (int16_t*)data;
    current = 0;
)

MAKE_PAYLOAD_FUNCTION(doHoming,
    lift.doHoming();
)

MAKE_PAYLOAD_FUNCTION(shutdown,
    lift.stepper.disableOutputs();
)

MAKE_PAYLOAD_FUNCTION(turnOn,
    lift.stepper.enableOutputs();
)

MAKE_PAYLOAD_RESPONSE_FUNCTION(currentPosResponse,
    WRITE_TYPE_BUFFER_WIRE(lift.stepper.currentPosition(), int16_t)
)

MAKE_PAYLOAD_FUNCTION(getCurrentPos,
    lift.responseHandler = currentPosResponse;
)

MAKE_PAYLOAD_RESPONSE_FUNCTION(currentAccResponse,
    WRITE_TYPE_BUFFER_WIRE(lift.acc, int16_t)
)


MAKE_PAYLOAD_FUNCTION(getCurrentAcc,
    lift.responseHandler = currentAccResponse;
)

MAKE_PAYLOAD_RESPONSE_FUNCTION(currentMaxSpeedResponse,
    WRITE_TYPE_BUFFER_WIRE(lift.max_speed, int16_t)
)

MAKE_PAYLOAD_FUNCTION(getMaxSpeed,
    lift.responseHandler = currentMaxSpeedResponse;
)

MAKE_PAYLOAD_RESPONSE_FUNCTION(isMovingResponse,
    WRITE_TYPE_BUFFER_WIRE(lift.stepper.isRunning(), int8_t)
)

MAKE_PAYLOAD_FUNCTION(isMoving,
    lift.responseHandler = isMovingResponse;
)



PayloadHandler payload_handler[] = {
        [0x00] = setTargetPos,
        [0x01] = setCurrentPos,
        [0x02] = setMaxSpeed,
        [0x03] = setAcceleration,
        [0x04] = setRMSCurrent,
        [0x05] = doHoming,
        [0x06] = getMaxSpeed,
        [0x07] = getCurrentPos,
        [0x08] = getCurrentAcc,
        [0x09] = isMoving,
        [0x0A] = shutdown,
        [0x0B] = turnOn
};