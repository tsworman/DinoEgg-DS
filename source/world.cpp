/***********************************
 * world.cc
 * By: Tyler Worman
 * Email: tsworman@novaslp.net
 * Summary: A world class.
 */

#include "world.h"
using namespace std;

/**
 * void world::draw
 * causes the world to be drawn
 */
void world::draw() {
  //Draw Playfield
  glLoadIdentity(); // Reset The View
  //Control lighting on the surface. Always on
  glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE  | POLY_FORMAT_LIGHT0);
  glBindTexture(GL_TEXTURE_2D, getTexture(0));
  glBegin(GL_QUADS); //192 is height of screen
  glTexCoord2f(1.0f, 0.0f); glVertex3v16(screenWidth, 0,  0);
  glTexCoord2f(1.0f, 1.0f); glVertex3v16(screenWidth, 256, 0);
  glTexCoord2f(0.0f, 1.0f); glVertex3v16(0,  256, 0);
  glTexCoord2f(0.0f, 0.0f); glVertex3v16(0, 0, 0);
  glEnd();
  //Draw TyTy
  if(isPaddleFront) {
    glBindTexture(GL_TEXTURE_2D, getTexture(1));
  } else {
    glBindTexture(GL_TEXTURE_2D, getTexture(2));
  }  
  glBegin(GL_QUADS); //192 is height of screen
  glTexCoord2f(1.0f, 0.0f); glVertex3v16((paddlePosition * 20) + 140, 174,  1);
  glTexCoord2f(1.0f, 1.0f); glVertex3v16((paddlePosition * 20) +140, 238, 1);
  glTexCoord2f(0.0f, 1.0f); glVertex3v16((paddlePosition * 20) +76,  238, 1);
  glTexCoord2f(0.0f, 0.0f); glVertex3v16((paddlePosition * 20) +76, 174, 1);
  glEnd();
  //Draw all objects on top of world.

  glEnd();


  list<object*>::iterator it = objectList.begin();
  for(; it != objectList.end(); it++) {
    (*it)->draw();
  }
}

/**
 * void world::update
 * causes the world to be updated.
 */
void world::update() {
  //Figure out how many pieces to drop at once.
  if (newSetNeeded) {
    //Handle extra blocks dropped in two player matches.
    if (blocksToDrop <= 2) {
      createFalling(2 + blocksToDrop);
      blocksToDrop = 0;
    } else {
      //Decrement the queue.
      createFalling(4);
      blocksToDrop = blocksToDrop - 2;
    }
    newSetNeeded = false;
  }
  //Advance level
  level = (getPoints() / 150);
  //Change Speed based on level.
  currentSpeed = (getLevel() + 1) % 5;
  //Check the tops of each column to see if it matches.
  list<object*>::iterator it;
  for (int i = 0; i < 4; i++) {
    //For each column
    if (boardHeight[i] >= 1) {
      //Column has some stuff in it.
      it = objectList.begin();
      while(it != objectList.end()) {
	if(((*it)->getColumn() == i) && ((*it)->getHeight() == boardHeight[i])) {
	  //Found the top block in a column
	  if ((*it)->getType() == EGGTOP) {
	    //If it's a top/scan for a bottom.
	    //If no bottom, delete it.
	    list<object*>::iterator it2 = objectList.begin();
	    int tempHeight = -1;
	    //Scan for tallest eggbottom
	    while(it2 != objectList.end()) {
	      if (((*it2)->getColumn() == i) && 
		  ((*it2)->getType() == EGGBOTTOM) &&
		  ((*it2)->getHeight() > tempHeight) &&
		  (it2 != it)) {
		tempHeight = (*it2)->getHeight();
		debug=tempHeight;
	      }
	      it2++;
	    }
	    if (tempHeight != -1) {
	      int tempHeightCount = 0;
	      //We found an egg bottom
	      it2 = objectList.begin();
	      //Scan list again marking each item between these heights as dead.
	      while(it2 != objectList.end()) {
		if (((*it2)->getColumn() == i) && 
		    ((*it2)->getHeight() >= tempHeight)) {
		  //It's in the same column and the same height as the bottom or better.
		  (*it2)->setDead();
		  tempHeightCount++;
		}	      
		it2++;
	      }
	      //Calculate score. 50 points for anything under 3 tiles. If over.. 25 points a tile.
	      if (tempHeightCount < 3) {
		addPoints(50);
	      } else {
		addPoints(tempHeightCount * 25);
		//Add a block to send for everything above 3 tiles.
		//In single player this number is never decremented.
		blocksToSend += tempHeightCount - 2;
	      }
	      addEgg();
	    } else {
	      //Found none so just delete the top.
	      (*it)->setDead();
	    }
	  } else {
	    list<object*>::iterator it2 = objectList.begin();
	    while(it2 != objectList.end()) {
	      if(((*it2)->getColumn() == i) && (*it2)->getHeight() == (boardHeight[i] - 1)) {
		//Found the second heighest in a column..
		if ((*it)->getType() == (*it2)->getType()) {
		  //They match exactly
		  (*it)->setDead();
		  (*it2)->setDead();
		  addPoints(5);
		}
	      }
	      it2++;
	    }
	  }
	}
	it++;
      }
    }
  }
  
  
  //Delete anything that is "dead"
  it = objectList.begin();
  while(it != objectList.end()) {
    if((*it)->getDead()) {
      boardHeight[(*it)->getColumn()]--;
      delete *it;
      it = objectList.erase(it);
    } else {
      it ++;
    }
  } 
  //If any column is still over 8 high you lose.
  for (int i = 0; i < 4; i++) {
    if (boardHeight[i] > 8) {
      gameOver = true;
    }
  }

  if (!gameOver) {
    //Scan level and if nothing if nothing isFalling
    //then set all that are not isOutOfGate to falling status.
    bool anythingFalling = false;//This is norm false
    it = objectList.begin();
    while(it != objectList.end()) {
      if((*it)->getIsFalling()) {
	anythingFalling = true;
      }
      it ++;
    } 
    if (!anythingFalling) { //Nothing falling.
      it = objectList.begin(); //Reset the iterator
      while(it != objectList.end()) {
	if(!(*it)->getIsOutOfGate()) {
	  (*it)->setIsFalling(true); //Make them fall..
	}
	it ++;
      }  
    }
    //Attach Blocks to columns if they are at the right position.
    it = objectList.begin(); //Reset the iterator
    while(it != objectList.end()) {
      if ((*it)->getIsFalling() && (*it)->getIsOutOfGate() && !(*it)->getIsInColumn()) {
	int colNum = (*it)->getColumn();
	if ((colNum < 0) || (colNum > 3)) {
	  debug = colNum;
	}
	int columnHeight = boardHeight[colNum];
	//Check if column height + TYTY is less than the distance..
	if ((*it)->getDistance() > (screenHeight - ((18 * columnHeight + 1) + 34))) {
	  (*it)->setDistance(screenHeight - ((18 * columnHeight + 1) + 34));
	  (*it)->setIsFalling(false);
	  (*it)->setIsInColumn(true);
	  boardHeight[colNum]++;
	  (*it)->setHeight(boardHeight[colNum]);
	}
      }
      it ++;
    }  
    
    //Update Everything
    it = objectList.begin();
    for(; it != objectList.end();it++) {
      (*it)->update();
    } 
  } //Game over check..
  //All updates done. Reset the Mod speed.
  modSpeed = false; //This is set on every frame.. 
  //So if I user lets off this fixes it fast.
}

/**
 * void world::setScreenSize(int height, int width)
 * sets the screen size
 */
void world::setScreenSize(int height, int width) {
  screenHeight = height;
  screenWidth = width;
}

/**
 * void world::add(object *newOb)
 * Adds an object to the world
 */
void world::add(object *newOb) {
  objectList.push_front(newOb);
}

/**
 * int world::getTexture(int num)
 * returns the unique texture number of a texture
 */
int world::getTexture(int num) {
  return texture[num];
}


/**
 * world::world() 
 * world constructor
 */
world::world() {
  //Set all col heights to 0
  boardHeight[0] = 0;
  boardHeight[1] = 0;
  boardHeight[2] = 0;
  boardHeight[3] = 0;
  level =0;
  points = 0;
  eggs = 0;
  screenWidth = 0;
  screenHeight = 0;
  debug = -1;
  paddlePosition = 1;
  isPaddleFront = true;
  currentSpeed = 0;
  modSpeed = false;
  gameOver = false;
  newSetNeeded = true;
  //For Two Player mode
  blocksToDrop = 0;
  blocksToSend = 0;
  //Seed random and figure out time.
  time_t unixTime = time(NULL);
  struct tm* timeStruct = gmtime((const time_t *)&unixTime);
  int seconds = timeStruct->tm_sec;
  PlantSeeds (seconds); //Seed the random
  LoadGameGLTextures();
}


/**
 * world::flipColumns 
 * Flips two columns of tiles.
 */
void world::flipColumns() {

  //Scan and switch nodes.
  list<object*>::iterator it = objectList.begin();
  while(it != objectList.end()) {
    if((!(*it)->getIsFalling()) && ((*it)->getIsInColumn())) {
      if ((*it)->getColumn() == paddlePosition) {
	//Column should change.
	(*it)->setColumn(paddlePosition + 1);
      } else if ((*it)->getColumn() == (paddlePosition + 1)) {
	//Column Should change.
	(*it)->setColumn(paddlePosition);
      }
    } else if ((*it)->getIsFalling()) {
      if ((*it)->getColumn() == paddlePosition) {
	//Object is falling in column we are switching.
	if ((*it)->getDistance() > (screenHeight - ((18 * boardHeight[paddlePosition + 1] + 1) + 34))) {
	  (*it)->setColumn(paddlePosition+1);
	}
      } else if ((*it)->getColumn() == paddlePosition + 1) {
	//Object is falling in column we are switching.
	if ((*it)->getDistance() > (screenHeight - ((18 * boardHeight[paddlePosition] + 1) + 34))) {
	  (*it)->setColumn(paddlePosition);
	}
      }
    }
    it ++;
  }   
  //Switch column height
  int tempHeight;
  tempHeight = boardHeight[paddlePosition];
  boardHeight[paddlePosition] = boardHeight[paddlePosition + 1];
  boardHeight[paddlePosition + 1] = tempHeight;
  //Flip the Graphic
  isPaddleFront = !isPaddleFront;
  //Flip the columns.list
}


/**
 * world::CreateFalling(int number)
 * Creates a certain number of falling blocks.
 */ 
void world::createFalling(int number) {
  //check num is < 4
  if (number > 4) {
    number = 4;
  }
  int last[4];
  last[0] = -1;
  last[1] = -1;
  last[2] = -1;
  last[3] = -1;  
  bool dupes;
  dupes = false;
  int i = 0;
  while ((i < number) || dupes) {
    dupes = false;
    int randomColumn = Equilikely(0,3);
    last[i] = randomColumn;
    for (int j = 0; j < i; j++) {
      if (last[i] == last[j]) {
	dupes = true;
      }
    }
    if (!dupes) {
      //Didn't gen a dupe.. So inc counter.
      i++;
    }
  }
  for (int k=0; k < number; k++) {
    long randomType = Equilikely(0,22);
    if (randomType < 4) {
      object* newBox = new object(this, HEART, 4, last[k]);
      add(newBox);
    } else if (randomType < 8) {
      object* newBox = new object(this, STAR, 5, last[k]);
      add(newBox);
    } else if (randomType < 12) {
      object* newBox = new object(this, CUPCAKE, 3, last[k]);
      add(newBox);
    } else if (randomType < 16) {
      object* newBox = new object(this, TELESCOPE, 6, last[k]);
      add(newBox);
    } else if (randomType < 19) {
      object* newBox = new object(this, EGGBOTTOM, 7, last[k]);
      add(newBox);
    } else if (randomType <= 22) {
      object* newBox = new object(this, EGGTOP, 8, last[k]);
      add(newBox);
    }
  }       

}



/**
 * world::LoadGameGLTextures()
 * Loads all textures needed for the game into memory.
 */
int world::LoadGameGLTextures() { 
  glResetTextures();
  sImage pcx; //Play Field 256x256
  sImage pcx2; //front 64x64
  sImage pcx3; //back 64x64
  sImage pcx4; //cupcake 16x16
  sImage pcx5; //heart 16x16
  sImage pcx6; //star 16x16
  sImage pcx7; //telescope 16x16
  sImage pcx8; //egg bottom 16x16
  sImage pcx9; //egg top 16x16
  sImage pcx10; //blue Dino 32x32
  sImage pcx11; //pink Dino 32x32
  sImage pcx12; //plane background 16x16
  //load the textures
  loadPCX((u8*)playfield_pcx, &pcx); image8to16(&pcx);
  loadPCX((u8*)front_pcx, &pcx2); image8to16(&pcx2);
  loadPCX((u8*)back_pcx, &pcx3); image8to16(&pcx3);
  loadPCX((u8*)cupcake_pcx, &pcx4); image8to16(&pcx4);
  loadPCX((u8*)heart_pcx, &pcx5); image8to16(&pcx5);
  loadPCX((u8*)star_pcx, &pcx6); image8to16(&pcx6);
  loadPCX((u8*)telescope_pcx, &pcx7); image8to16(&pcx7);
  loadPCX((u8*)eggbottom_pcx, &pcx8); image8to16(&pcx8);
  loadPCX((u8*)eggtop_pcx, &pcx9); image8to16(&pcx9);
  loadPCX((u8*)blueDinoSmall_pcx, &pcx10); image8to16(&pcx10);
  loadPCX((u8*)pinkDinoSmall_pcx, &pcx11); image8to16(&pcx11);
  loadPCX((u8*)tileBackground_pcx, &pcx12); image8to16(&pcx12);
  //DS supports no filtering of anykind so no need for more than one texture
  //Load All textures into memory so we can use them later.
  glGenTextures(1, &texture[0]);
  glBindTexture(0, texture[0]);
  glTexImage2D(0, 0, GL_RGB, TEXTURE_SIZE_256, TEXTURE_SIZE_256, 0, TEXGEN_TEXCOORD, pcx.image.data8);
  //front
  glGenTextures(1, &texture[1]);
  glBindTexture(1, texture[1]);
  glTexImage2D(1, 0, GL_RGB, TEXTURE_SIZE_64, TEXTURE_SIZE_64, 0, TEXGEN_TEXCOORD, pcx2.image.data8);
  //back
  glGenTextures(1, &texture[2]);
  glBindTexture(2, texture[2]);
  glTexImage2D(2, 0, GL_RGB, TEXTURE_SIZE_64, TEXTURE_SIZE_64, 0, TEXGEN_TEXCOORD, pcx3.image.data8);
  //cupcake
  glGenTextures(1, &texture[3]);
  glBindTexture(3, texture[3]);
  glTexImage2D(3, 0, GL_RGB, TEXTURE_SIZE_16, TEXTURE_SIZE_16, 0, TEXGEN_TEXCOORD, pcx4.image.data8);
  //heart
  glGenTextures(1, &texture[4]);
  glBindTexture(4, texture[4]);
  glTexImage2D(4, 0, GL_RGB, TEXTURE_SIZE_16, TEXTURE_SIZE_16, 0, TEXGEN_TEXCOORD, pcx5.image.data8);
  //star
  glGenTextures(1, &texture[5]);
  glBindTexture(5, texture[5]);
  glTexImage2D(5, 0, GL_RGB, TEXTURE_SIZE_16, TEXTURE_SIZE_16, 0, TEXGEN_TEXCOORD, pcx6.image.data8);
  //telescope
  glGenTextures(1, &texture[6]);
  glBindTexture(6, texture[6]);
  glTexImage2D(6, 0, GL_RGB, TEXTURE_SIZE_16, TEXTURE_SIZE_16, 0, TEXGEN_TEXCOORD, pcx7.image.data8);
  //Egg bottom
  glGenTextures(1, &texture[7]);
  glBindTexture(7, texture[7]);
  glTexImage2D(7, 0, GL_RGB, TEXTURE_SIZE_16, TEXTURE_SIZE_16, 0, TEXGEN_TEXCOORD, pcx8.image.data8);
  //Egg top
  glGenTextures(1, &texture[8]);
  glBindTexture(8, texture[8]);
  glTexImage2D(8, 0, GL_RGB, TEXTURE_SIZE_16, TEXTURE_SIZE_16, 0, TEXGEN_TEXCOORD, pcx9.image.data8);
  //blue dino
  glGenTextures(1, &texture[9]);
  glBindTexture(9, texture[9]);
  glTexImage2D(9, 0, GL_RGB, TEXTURE_SIZE_32, TEXTURE_SIZE_32, 0, TEXGEN_TEXCOORD, pcx10.image.data8);
  //pink dino
  glGenTextures(1, &texture[10]);
  glBindTexture(10, texture[10]);
  glTexImage2D(10, 0, GL_RGB, TEXTURE_SIZE_32, TEXTURE_SIZE_32, 0, TEXGEN_TEXCOORD, pcx11.image.data8);
  //pink dino
  glGenTextures(1, &texture[11]);
  glBindTexture(11, texture[11]);
  glTexImage2D(11, 0, GL_RGB, TEXTURE_SIZE_32, TEXTURE_SIZE_32, 0, TEXGEN_TEXCOORD, pcx12.image.data8);
  //Destroy all pointers now that they are in memory..
  imageDestroy(&pcx);
  imageDestroy(&pcx2);
  imageDestroy(&pcx3);
  imageDestroy(&pcx4);
  imageDestroy(&pcx5);
  imageDestroy(&pcx6); 
  imageDestroy(&pcx7);
  imageDestroy(&pcx8);
  imageDestroy(&pcx9);
  imageDestroy(&pcx10);
  imageDestroy(&pcx11);
  imageDestroy(&pcx12);
  return TRUE; 
}
