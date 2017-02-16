/***********************************
 * timer.h
 * By: Tyler Worman
 * Email: Worman@cs.moravian.edu
 * Summary: A timer header
 */

#include <sys/time.h>

#ifndef _TIMER_H
#define _TIMER_H
class Timer{
public:
  Timer();
  void tick();
  int approxfps();
  float instantfps();
  
private:
  long lastsec;
  long lastusec;
  int theApproxFPS;
  float theinstantFPS;
  int numFrames;
};

#endif
