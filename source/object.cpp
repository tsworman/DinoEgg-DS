/***********************************
 * object.cc
 * By: Tyler Worman
 * Email: tsworman@novaslp.net
 * Summary: A box class
 */

#include "object.h"
#include "world.h"


using namespace std;

/**
 * void box::update();
 * this updates the object
 */
void object::update() {
  //Keep track of updates.
  updateCounter++;
  //update the object every 20 frames unless the modifier for speed is on then every 10.
  if ((updateCounter == 20) || (myWorld->getModSpeed() && (updateCounter == 10))) {
    updateCounter = 0;
    if (isFalling) {
      //Move block down a bit.
      if(myWorld->getSpeed() != 0) {
	distance += 6 * (myWorld->getSpeed());
      } else {
	distance += 6 * (myWorld->getSpeed() + 1);
      }
    }
  }
  //Check if it's position is past the original 16. If so set spawn okay.
  if ((distance > 16) && (!isOutOfGate)) {
    isOutOfGate = true;
    myWorld->requestNewSet();
  }
  
}

/**
 * box::box(world *aWorld, ObjectType, int textureID)
 * Creates a box and sets it's world
 */
object::object(world *aWorld, objectType myType, int textureID, int column) {
  myWorld = aWorld;
  type = myType;
  texture = textureID;
  isDead = false;
  fallColumn = column;
  distance = 0;
  isFalling = false;
  isOutOfGate = false;
  inColumn = false;
  updateCounter = 0;
  height = -1;
}

/**
 * void object::draw
 * causes the object to be drawn
 */
void object::draw() {
  //Control lighting on the surface. Always on
  glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE  | POLY_FORMAT_LIGHT0);
  //Draw blocks.
  //Draw actual image
  glBindTexture(GL_TEXTURE_2D, myWorld->getTexture(texture));
  glBegin(GL_QUADS); //192 is height of screen
  glTexCoord2f(1.0f, 0.0f); glVertex3v16((fallColumn * 25) + 16 + 82, distance,  1);
  glTexCoord2f(1.0f, 1.0f); glVertex3v16((fallColumn * 25) + 16 + 82, distance + 16, 1);
  glTexCoord2f(0.0f, 1.0f); glVertex3v16((fallColumn * 25) + 82, distance + 16, 1);
  glTexCoord2f(0.0f, 0.0f); glVertex3v16((fallColumn * 25) + 82, distance, 1);
  glEnd();
  //Draw box beneath it on when in column.
  if (inColumn) {
    glBindTexture(GL_TEXTURE_2D, myWorld->getTexture(11));
    glBegin(GL_QUADS);
    glVertex3v16((fallColumn * 25) + 16 + 83,distance - 1,1);
    glVertex3v16((fallColumn * 25) + 16 + 83, distance + 17, 1);
    glVertex3v16((fallColumn * 25) + 81, distance + 17, 1);
    glVertex3v16((fallColumn * 25) + 81, distance - 1, 1);
    glEnd();
  }

}
