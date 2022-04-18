#include <iostream>
#include <Smartcar.h>

void setup() {
    Serial.begin(9600);
    pinMode(0, OUTPUT);
    Serial.println("Setup hello");
    std::cout << "setup!\n";
}

int pass = 0;
void loop() {
    ++pass;
    Serial.println(pass);
    analogWrite(0, pass * 10);
    std::cout << "loop!\n";
}
