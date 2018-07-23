#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <time.h>
#include "joystickController.h"

const char* position[] = {"up","right","down","left","push","none"};
const char* directionDir[] = {"/sys/class/gpio/gpio26/value","/sys/class/gpio/gpio47/value","/sys/class/gpio/gpio46/value","/sys/class/gpio/gpio65/value","/sys/class/gpio/gpio27/value"};
static pthread_t joystick_thread;
static int beatMode = 0;
static _Bool joystickFlag = true;

void writeToFile(char *fileName, int gpio) {
  // Use fopen() to open the file for write access.
FILE *pfile = fopen(fileName, "w");
if (pfile == NULL) {
printf("ERROR: Unable to open export file.\n");
exit(1);
}
// Write to data to the file using fprintf():
fprintf(pfile, "%d", gpio);

// Close the file using fclose():
fclose(pfile);
}

const char * readFromFile(const char *fileName){
FILE *file = fopen(fileName, "r");

if (file == NULL) {
  printf("ERROR: Unable to open file (%s) for read\n", fileName);
  exit(-1);
}

// Read string (line)
//const int max_length = 1024;
char* buff;
buff = malloc(sizeof(char)*100);
fgets(buff, 2, file);
// Close
fclose(file);

return buff;
}


//returns current joystick position
const char * Joystick_checkDir() {
  int i = 0;
  for (i = 0; i < 5; i++) {
    const char * line = readFromFile(directionDir[i]);
    if (strcmp(line, "0")) {
    }
    else {
      //should return correct direction joystick is in
      return position[i];
    }
  }
  //Returns none if no direction is pressed on joystick
  return position[5];

}

static _Bool inputDetect = false;

void Joystick_pollInput() {
  while(joystickFlag) {
    const char *jIn = Joystick_checkDir();
    if(strcmp(jIn, "up") == 0 && inputDetect == false) {
      inputDetect = true;
      //printf("I went up");
    }

    if(strcmp(jIn, "down") == 0 && inputDetect == false) {
      inputDetect = true;
      //printf("I went down");
    }

    if(strcmp(jIn, "left") == 0 && inputDetect == false) {
      inputDetect = true;
      //printf("I went left");
    }

    if(strcmp(jIn, "right") == 0 && inputDetect == false) {
      inputDetect = true;
      //printf("I went right");
    }


    if(strcmp(jIn, "push") == 0 && inputDetect == false) {
     
    }
    if(strcmp(jIn, "none") == 0) {
      inputDetect = false;
    }



    if(inputDetect) {
      nanosleep((const struct timespec[]){{0,100000000}},NULL);
      inputDetect = true;
    }


  }
}

void Joystick_init() {
  writeToFile("/sys/class/gpio/export",26);
  writeToFile("/sys/class/gpio/export",47);
  writeToFile("/sys/class/gpio/export",46);
  writeToFile("/sys/class/gpio/export",65);
  writeToFile("/sys/class/gpio/export",27);
  pthread_create(&joystick_thread, NULL, (void *) Joystick_pollInput, NULL);

}

void Joystick_flag() {
  joystickFlag = false;
}

void Joystick_stop() {
  pthread_join(joystick_thread, NULL);
  joystickFlag = false;
}
