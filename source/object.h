/***********************************
 * object.h
 * By: Tyler Worman
 * Email: tsworman@novaslp.net
 * Summary: An object header
 */

#ifndef _OBJECT_H
#define _OBJECT_H

class world;
enum objectType {CUPCAKE, HEART, STAR, TELESCOPE, EGGTOP, EGGBOTTOM, DINO};

class object {
 public:
  object(){}
  ~object() {}
  object(world* aWorld, objectType Type, int textureID, int column);
  inline objectType getType() {return type;};
  void draw();
  void update();
  void setIsFalling(bool fallState) {isFalling = fallState;};
  void setColumn(int column) {fallColumn = column;};
  void setIsInColumn(bool inCol) {inColumn = inCol;};
  void setDistance(int dist) {distance = dist;};
  int getColumn() {return fallColumn;};
  int getDistance() {return distance;};
  bool getIsFalling() {return isFalling;};
  bool getIsOutOfGate() {return isOutOfGate;};
  bool getIsInColumn() {return inColumn;};
  bool getDead() {return isDead;};
  void setDead() {isDead = true;};
  //Height of block in column
  int getHeight() {return height;};
  void setHeight(int newHeight) {height = newHeight;}; 
 protected:
  world* myWorld;
  int texture;

 protected:
  bool isDead;
  int distance; //The distance in pixels from the top of the screen.
  int height; //This is used to speed up the deletion alg.
  objectType type;
  bool inColumn; //Is it assigned to a column?
  bool isFalling; //Is it falling/in column or top of screen
  bool isOutOfGate; //Did it pass the first blocks?
  int fallColumn; //Column it's falling in. If it's inColumn this is ignored.
  int updateCounter; //This keeps tracks of frames.. Every 60 it resets.
};

#endif
