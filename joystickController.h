#ifndef _JOYSTICK_CONTROL_H
#define _JOYSTICK_CONTROL_H

void Joystick_init();
const char* Joystick_checkDir();
void Joystick_stop();
void Joystick_pollInput();
void Joystick_flag();

#endif
