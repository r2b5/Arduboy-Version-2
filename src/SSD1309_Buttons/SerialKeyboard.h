#ifndef SERIALKEYBOARD_H
#define SERIALKEYBOARD_H

#include <Arduino.h>

#define SK_UP_BUTTON        7
#define SK_DOWN_BUTTON      6
#define SK_LEFT_BUTTON      9
#define SK_RIGHT_BUTTON     5
#define SK_A_BUTTON         8
#define SK_B_BUTTON         4



class SerialKeyboard {
private:
    char lastKey = 0;

public:

    void begin(unsigned long baud = 115200) {

        Serial.begin(baud);
        pinMode(SK_UP_BUTTON, OUTPUT);  //UP
        pinMode(SK_DOWN_BUTTON, OUTPUT);  //DOWN
        pinMode(SK_LEFT_BUTTON, OUTPUT);  //LEFT
        pinMode(SK_RIGHT_BUTTON, OUTPUT);  //RIGT

        pinMode(SK_A_BUTTON, OUTPUT);  //A
        pinMode(SK_B_BUTTON, OUTPUT);  //B

    }

    void update() {


    digitalWrite(SK_UP_BUTTON,HIGH);
    digitalWrite(SK_DOWN_BUTTON,HIGH);
    digitalWrite(SK_LEFT_BUTTON,HIGH);
    digitalWrite(SK_RIGHT_BUTTON,HIGH);
    digitalWrite(SK_A_BUTTON,HIGH);
    digitalWrite(SK_B_BUTTON,HIGH);


    if (Serial.available() > 0) {
        lastKey = Serial.read();
        switch(lastKey){
           case 'w': digitalWrite(SK_UP_BUTTON,LOW);Serial.print(lastKey);break;
           case 's': digitalWrite(SK_DOWN_BUTTON,LOW);Serial.print(lastKey);break;
           case 'a': digitalWrite(SK_LEFT_BUTTON,LOW);Serial.print(lastKey);break;
           case 'd': digitalWrite(SK_RIGHT_BUTTON,LOW);Serial.print(lastKey);break;
           case 'n': digitalWrite(SK_A_BUTTON,LOW);Serial.print(lastKey);break;
           case 'm': digitalWrite(SK_B_BUTTON,LOW);Serial.print(lastKey);break;
        }
        

        } else {
            lastKey = 0;
        }
    }


};

#endif

