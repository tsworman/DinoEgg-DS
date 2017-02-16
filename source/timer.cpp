/***********************************
 * timer.cc
 * By: Tyler Worman
 * Email: Worman@cs.moravian.edu
 * Summary: A timer class.
 */

#include "timer.h"

void Timer::tick() {
  numFrames++;
  timeval temp;
  gettimeofday(&temp, '\0');
   long delta;
  if(lastsec != temp.tv_sec) {
    theApproxFPS = numFrames;
    numFrames = 0;
    delta = temp.tv_usec + (1000000 - lastusec);
  } else {
    delta = temp.tv_usec - lastusec;
  }
  theinstantFPS = 1000000.0 /delta;
  //now update previous times
  lastusec = temp.tv_usec;
  lastsec = temp.tv_sec;
}

Timer::Timer() {
  theinstantFPS = 1;
}

float Timer::instantfps() {
  return theinstantFPS;
}

int Timer::approxfps() {
  return theApproxFPS;
}
