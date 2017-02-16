/***********************************
 * World.h
 * By: Tyler Worman
 * Email: tsworman@novaslp.net
 * Summary: An encapsulating world class
 */

#ifndef _WORLD_H
#define _WORLD_H

#include <nds.h>

#include "timer.h"
#include "object.h"
#include <stdlib.h>
#include <time.h>

#include "rngs.h"
#include "rvgs.h"

//needed to load pcx files
#include <nds/arm9/image.h>


#include <list>
#include <vector>
#include <string>

//Graphics
#include "front_pcx.h"
#include "back_pcx.h"
#include "playfield_pcx.h"
//Graphics for boxen
#include "cupcake_pcx.h"
#include "eggbottom_pcx.h"
#include "eggtop_pcx.h"
#include "eggSmall_pcx.h"
#include "heart_pcx.h"
#include "pinkDinoSmall_pcx.h"
#include "blueDinoSmall_pcx.h"
#include "star_pcx.h"
#include "telescope_pcx.h"
#include "tileBackground_pcx.h"

using namespace std;

class object;

class world {
 public:
  world();
  
  // ***********************************
  // public member functions
  // ***********************************
  
  inline void tick() {clock.tick();};
  inline int approxfps() {return clock.approxfps(); };
  inline float instantfps() {return (float)clock.instantfps();};
  void setScreenSize(int, int);
  
  int LoadGameGLTextures();
  int getTexture(int i);
  
  inline int getScreenWidth() {return screenWidth;};
  inline int getScreenHeight() {return screenHeight;};
  
  inline int getNumObjects() {return objectList.size();};
  inline int getDebug() {return debug;};

  //Outside datamember functions
  void addPoints(int Addpoints) { points += Addpoints;};
  int getPoints() { return points;};
  void addLevel() { level++;};
  int getLevel() {return level;};
  void addEgg() { eggs++;};
  int getEggs() {return eggs;};
  void setSpeed(int setSpeed) { currentSpeed = setSpeed;};
  int getSpeed() {return currentSpeed;};
  void setModSpeed() {modSpeed = true;}; //This resets after every update.
  bool getModSpeed() {return modSpeed;};
  void requestNewSet() {newSetNeeded = true;} //Set when ever any block passes the gate.

  bool isGameOver() {return gameOver;};
  //Get Column height
  int getColumnHeight(int i) { return boardHeight[i];};

  //Generate Blocks
  void createFalling(int);

  //Two Player
  int get2PlayerBlocks() {return blocksToSend;}
  void add2PlayerBlocks(int i) {blocksToDrop = blocksToDrop + i;}

  //Controls
  void movePaddleLeft() {if (paddlePosition !=0) { paddlePosition -= 1;}};
  void movePaddleRight() {if (paddlePosition !=2) { paddlePosition += 1;}};
  void flipColumns();
  void add(object *);
  void addToColumn(object *, int column);
  void update();
  void draw();

 private:
  Timer clock;
  list<object *> objectList;
  int boardHeight[4];

  //world info
  int screenWidth;
  int screenHeight;

  //points/powerups
  int level;
  int points;
  int eggs;
  
  //For Two Player mode
  int blocksToDrop;
  int blocksToSend;

  //Paddle either 0 1 2 for position
  int paddlePosition;
  bool isPaddleFront;

  //How fast is the game moving
  bool modSpeed; //User is modifying speed
  int currentSpeed; //Actual game speed
  bool newSetNeeded; //Is anything at the top

  //Is game over?
  bool gameOver;

  //Texture Storage
  int texture[12];


  int debug;
};

#endif
