//
// Created by fogoz on 29/04/2025.
//

#ifndef LIFTHANDLER_H
#define LIFTHANDLER_H
#include <AccelStepper.h>
#include <TMCStepper.h>
#include <Wire.h>
struct LiftHandler;
#define MAKE_PAYLOAD_FUNCTION(name, body) void name(uint8_t *data, size_t size, LiftHandler& lift){\
body\
}
#define MAKE_PAYLOAD_RESPONSE_FUNCTION(name, body) void name(LiftHandler& lift){\
body\
}

#define WRITE_BYTE_BUFFER_WIRE(pointer, size) \
    for(uint8_t i = 0; i < size; i++) \
        Wire.write(pointer[i]);
#define WRITE_TYPE_BUFFER_WIRE(uint, type)\
    type buffer = (type)uint; \
    uint8_t* data = (uint8_t*)(&buffer); \
    WRITE_BYTE_BUFFER_WIRE(data, sizeof(type))


typedef void (*PayloadHandler)(uint8_t *data, size_t size, LiftHandler&);

inline void emptyI2C() {
    while (Wire.available()) {
        Wire.read();
    }
}

extern PayloadHandler payload_handler[];
struct LiftHandler {
    bool changed = false;
    int target = 0;
    float max_speed = 1000;
    float acc = 1500;
    AccelStepper& stepper;
    void (*responseHandler)(LiftHandler&) = nullptr;
    bool homing_mode = false;

    LiftHandler(AccelStepper &stepper);
    LiftHandler() = delete;

    void onDataReceived(int data);

    void onDataRequest();

    void syncAction();

    void doHoming();

    float getMaxSpeed() const;

    float getAcc() const;

    int getTarget() const;

    void setMaxSpeed(float max_speed);

    void setAcc(float acc);

    void setTarget(int target);

    void noticeButtonInterrupt();

};



#endif //LIFTHANDLER_H
