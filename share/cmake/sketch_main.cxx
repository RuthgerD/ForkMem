#include <iostream>

void sketch_main(void (*)(), void (*)());

extern void setup();
extern void loop();

extern "C" void sketch_entry() { sketch_main(setup, loop); }
