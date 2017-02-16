/****************************************
 *     	Dino Egg - A Yoshi Clone
 *      By: Tyler Worman
 *      Email: tsworman@novaslp.net
 *
 ****************************************/

#ifndef _MAIN_H
#define _MAIN_H

// include your ndslib
#include <nds.h>
#include <malloc.h>
#include <nds/system.h>
//Enable text to the screen.
#include <nds/arm9/console.h>
//needed to load pcx files
#include <nds/arm9/image.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dswifi9.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>

//Graphics
#include "title_pcx.h"
#include "eggSmall_pcx.h"


//Game World
#include "world.h"

using namespace std;

world* myWorld;                         // The actual game board
bool paused;                            // is the game paused?
bool onTitle;                           // On Title Screen or in game?
bool twoPlayerConfig;                   // Is the game in config mode for net play?
bool onePlayer;                         // is One player or two selected?

int highScore;                   // Current game score

int texture[11]; // Storage For 13 Texture

int DrawGLScene();
void keyHandler ();

// Load PCX files And Convert To Textures
int LoadTitleGLTextures() { 
  sImage pcx; //Title 256x256
  sImage pcx2; //Small Egg 16x16
  //load the textures
  loadPCX((u8*)title_pcx, &pcx); image8to16(&pcx);
  loadPCX((u8*)eggSmall_pcx, &pcx2); image8to16(&pcx2);
  //DS supports no filtering of anykind so no need for more than one texture
  //Load All textures into memory so we can use them later.
  //Title Screen
  glGenTextures(1, &texture[0]);
  glBindTexture(0, texture[0]);
  glTexImage2D(0, 0, GL_RGB, TEXTURE_SIZE_256, TEXTURE_SIZE_256, 0, TEXGEN_TEXCOORD, pcx.image.data8);
  //Egg Small
  glGenTextures(1, &texture[1]);
  glBindTexture(1, texture[1]);
  glTexImage2D(1, 0, GL_RGB, TEXTURE_SIZE_16, TEXTURE_SIZE_16, 0, TEXGEN_TEXCOORD, pcx2.image.data8);

  //Destroy all pointers now that they are in memory..
  imageDestroy(&pcx);
  imageDestroy(&pcx2);
  return TRUE; 
}

//Local Player Paused the game. Notify remote players client
void netSendPaused() {

}

//Local Player Lost the Game. Notify the remote players client.
void netSendLoss() {

}

//Find a valid Access Point and do connection
void findAP(void){
	int selected = 0;  
	int i;
	int count = 0;
	int status = ASSOCSTATUS_DISCONNECTED;
        unsigned char* wepkey = new unsigned char[10];
	lcdSwap(); //Swap to touch screen.
	//Setup Wifi
	static Wifi_AccessPoint ap;
	Wifi_ScanMode(); //this allows us to search for APs

	//Press A to select one. Press B to cancel.
	while(!(keysDown() & KEY_A) & !(keysDown() & KEY_B))
	{
		scanKeys();
		//find out how many APs there are in the area
		count = Wifi_GetNumAP();

		consoleClear();
		iprintf("Select a WiFi AP and press A.\n Press B to return to title.\n");
		iprintf("Number of APs found: %d\n", count);

		//display the APs to the user
		for(i = 0; i < count; i++)
		{
			Wifi_AccessPoint ap;

			Wifi_GetAPData(i, &ap);

			// display the name of the AP
			iprintf("%s %s Wep:%s Sig:%i\n", 
				i == selected ? "*" : " ", 
				ap.ssid, 
				ap.flags & WFLAG_APDATA_WEP ? "Yes " : "No ",
				ap.rssi * 100 / 0xD0);

		}

		//move the selection asterick
		if(keysDown() & KEY_UP)
		{
			selected--;
			if(selected < 0)
			{
				selected = 0;
			}
		}

		if(keysDown()&KEY_DOWN)
		{
			selected++;
			if(selected >= count) 
			{
				selected = count - 1;
			}
		}
		swiWaitForVBlank();
	}

	if ((keysDown() & KEY_A)) {
	  //user has made a choice so grab the ap and return it
	  Wifi_GetAPData(selected, &ap);
	  consoleClear();
	  iprintf("Connecting to %s\n", (&ap)->ssid);
	  //this tells the wifi lib to use dhcp for everything
	  Wifi_SetIP(0,0,0,0,0);
	  if (ap.flags & WFLAG_APDATA_WEP) {
	    //Uses WEP
	    consoleClear();
	    iprintf("Enter 64bit WEP KEY.\n");
	    //Keyboard* kb = keyboardInit(NULL, 3, BgType_Text4bpp, BgSize_T_256x512, 20, 0, false, true);
	    //keyboardShow();
	    //scanf("%s", wepkey);
	    wepkey = (unsigned char*) "beef012345";
	    Wifi_ConnectAP(&ap, WEPMODE_40BIT, 0, wepkey);
	  } else {
	    //Just Connect
	    Wifi_ConnectAP(&ap, WEPMODE_NONE, 0, 0);
	  }
	  //Actually Connect to the host.
		while(status != ASSOCSTATUS_ASSOCIATED && status != ASSOCSTATUS_CANNOTCONNECT)
		{
			int oldStatus = status;
			status = Wifi_AssocStatus();
			iprintf("%s", oldStatus != status ? ASSOCSTATUS_STRINGS[status] : ".");
			scanKeys();
			if(keysDown() & KEY_B) break;
			swiWaitForVBlank();
		}

		consoleClear();
		//consoleSetWindow(NULL, 0,0,32,10);

		char url[256];

		if(status == ASSOCSTATUS_ASSOCIATED) {
		  while(1)
		    {
		      u32 ip = Wifi_GetIP();
		      
		      iprintf("ip: [%i.%i.%i.%i]", (ip ) & 0xFF, (ip >> 8) & 0xFF, (ip >> 16) & 0xFF, (ip >> 24) & 0xFF);
		      
		      scanf("%s", url);
		      
		      struct hostent *host = gethostbyname(url);
		      
		      if(host)
			iprintf("IP (%s) : %s\n",  url, inet_ntoa(*(struct in_addr *)host->h_addr_list[0]));
		      else
			iprintf("Could not resolve\n");
		      
		      scanKeys();
		      
		      if(keysDown() & KEY_B) break;
		      
		      swiWaitForVBlank();
		    }
		  twoPlayerConfig = true;
		} 
	} else {
	  //user wanted to cancel. Disable Wifi.
	  Wifi_InitDefault(false);
	  twoPlayerConfig = false;
	}
	lcdSwap(); //Swap back to nonTouch Screen.
}



int main() {	
  //Setup basic stuff for game
  paused = false;                    
  onTitle = true; 
  onePlayer = true;
  twoPlayerConfig = false;
  // Turn on everything
  powerOn(POWER_ALL);
  // Setup wifi chipset.
  // Setup the Main screen for 3D 
  videoSetMode(MODE_0_3D);
  //Allocate Texture memory
  vramSetBankA(VRAM_A_TEXTURE);
  vramSetBankB(VRAM_B_TEXTURE);
  vramSetBankD(VRAM_D_TEXTURE);
  // Setup console screen to display information in text.
  videoSetModeSub(MODE_0_2D | DISPLAY_BG0_ACTIVE); //Print To DSIPLAY_BG0_ACTIVE
  vramSetBankC(VRAM_C_SUB_BG);
  REG_BG0CNT_SUB = BG_MAP_BASE(31);
  BG_PALETTE_SUB[255] = RGB15(31,31,31); //Default font
  //consoleDemoInit((u16*)SCREEN_BASE_BLOCK_SUB(31), (u16*)CHAR_BASE_BLOCK_SUB(0), 16);
  consoleDemoInit();
  irqSet(IRQ_VBLANK, 0);
  
  // initialize the geometry engine
  glInit();
  lcdSwap();
  // enable textures
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  
  // enable antialiasing
  glEnable(GL_ANTIALIAS);
  Wifi_InitDefault(false);
  // setup the rear plane
  glClearColor(0,0,0,31); // BG must be opaque for AA to work
  glClearPolyID(63); // BG must have a unique polygon ID for AA to work
  glClearDepth(0x7FFF);
  
  // Set our viewport to be the same size as the screen
  glViewport(0,0,255,191);
  
  //Clear Texture memory and load Title textures
  glResetTextures();
  LoadTitleGLTextures();
  
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrthof32(0,256,192,0,-1,1);
  //set up a directional ligth arguments are light number (0-3), light color, 
  //and an x,y,z vector that points in the direction of the light
  glLight(0, RGB15(31,31,31) , 0, floattov10(-1.0), 0);
  glColor3f(1,1,1);
  glMatrixMode(GL_MODELVIEW);
  
  //need to set up some material properties since DS does not have them set by default
  glMaterialf(GL_AMBIENT, RGB15(16,16,16));
  glMaterialf(GL_DIFFUSE, RGB15(16,16,16));
  glMaterialf(GL_SPECULAR, BIT(15) | RGB15(8,8,8));
  glMaterialf(GL_EMISSION, RGB15(16,16,16));
  
  //ds uses a table for shinyness..this generates a half-ass one
  glMaterialShinyness();
  
  while (1) {
    //Check Inputs
    keyHandler();

    
    //Draw the scene (includes updates)
    DrawGLScene();
    // flush to screen	
    glFlush(0);
    //Print Text
    printf("\x1b[2J");
    printf("         _Super Dino Egg_\nBy: Tyler Worman\n\n");
    printf("High Score: %d \n\n", highScore);
    if (!onTitle) {
      printf("Current Score: %d \n\n", myWorld->getPoints());
      printf("Mode: 1 player\n");
      printf("Level: %d Speed: %d \n", myWorld->getLevel(),myWorld->getSpeed());
      printf("Completed Eggs: %d \n\n", myWorld->getEggs());
      printf("D-Pad (L|R) moves\nD-Pad (D) doubles speed of block\nA or B to flip columns\n\n-Match blocks to destroy them\n-Complete Eggs to destroy blocks in between.\n\n");
      if (paused) {
	printf ("         _Game Paused_\n");
      }
      //printf("Debug: %d", myWorld->getDebug());
    }
    // wait for the screen to refresh
    swiWaitForVBlank();
  }
  
  return 0;
}

/*************
 * void keyHandler()
 * Handle all keyboard presses
 */
void keyHandler () {
  //Read the buttons
  scanKeys();
  //A button
  if ((keysDown() & KEY_A)) {
    if (!onTitle & !paused) {
      //In Game
      myWorld->flipColumns();
    }
  }
  //B Button
  if ((keysDown() & KEY_B)) {
    if (!onTitle & !paused) {
      //In Game
      myWorld->flipColumns();
    }

  }
  if (keysHeld() & KEY_R) {}
  if (keysHeld() & KEY_L) {}
  //Left D Pad
  if (keysDown() & KEY_LEFT) {
    if (!onTitle & !paused) {
      //In Game
      myWorld->movePaddleLeft();
    }
  }
  //Right D Pad
  if (keysDown() & KEY_RIGHT) {
    if (!onTitle & !paused) {
      //In Game
      myWorld->movePaddleRight();
    }
  }
  //Up D Pad
  if (keysHeld() & KEY_UP) {
    if (onTitle) {
      //Select other player option
      if (!onePlayer) {
	onePlayer = !onePlayer;
      } else {
	//Do nothing one player is already selected
      }
    } else {
      //Does nothing
    }
  }
  if (keysHeld() & KEY_DOWN) {
    if (onTitle) {
      //Select other player option
      if (onePlayer) {
	onePlayer = !onePlayer;
      } else {
	//Do Nothing two player is selected.
      }
    } else {
      //Speed up the game
      myWorld->setModSpeed();
    }
  }
  if (keysHeld() & KEY_X) {}
  if (keysHeld() & KEY_Y) {}
  if (keysDown() & KEY_START) {
    if (onTitle & !onePlayer & !twoPlayerConfig) {
      //Show the config screen for network play. Select an AP and then Connect
      glResetTextures();
      findAP();
    } else if ((onTitle & onePlayer) | (onTitle & !onePlayer & twoPlayerConfig)) {
	//Start the Game
	glResetTextures(); //Clear Texture memory
	myWorld = new world(); //Create the world
	myWorld->setScreenSize(192,256); //DS Screen size
	onTitle = !onTitle; //Tell it it's not the title anymore
	twoPlayerConfig = false; //Force player to reconfig after the game.
    } else {
      //Pause the game
      paused = !paused;
      if (!onePlayer) {
	//Force the other player to pause/unpause.
	netSendPaused();
      }
    }
  }    
}


//Draw the actual scene
int DrawGLScene() {
  glLoadIdentity(); // Reset The View
  //Control lighting on the surface. Always on
  glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE  | POLY_FORMAT_LIGHT0);
  //Render Title...
  if (onTitle) {
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glBegin(GL_QUADS); //192 is height of screen
    glTexCoord2f(1.0f, 0.0f); glVertex3v16(256 , 0,  0);
    glTexCoord2f(1.0f, 1.0f); glVertex3v16(256, 256, 0);
    glTexCoord2f(0.0f, 1.0f); glVertex3v16(0,  256, 0);
    glTexCoord2f(0.0f, 0.0f); glVertex3v16(0, 0, 0);
    glEnd();
    if (onePlayer) {
      //Draw Egg on top!
      glBindTexture(GL_TEXTURE_2D, texture[1]);
      glBegin(GL_QUADS); //192 is height of screen
      glTexCoord2f(1.0f, 0.0f); glVertex3v16(97, 136,  1);
      glTexCoord2f(1.0f, 1.0f); glVertex3v16(97, 152, 1);
      glTexCoord2f(0.0f, 1.0f); glVertex3v16(81, 152, 1);
      glTexCoord2f(0.0f, 0.0f); glVertex3v16(81, 136, 1);
      glEnd();
    } else {
      //Draw Egg Below
      glBindTexture(GL_TEXTURE_2D, texture[1]);
      glBegin(GL_QUADS); //192 is height of screen
      glTexCoord2f(1.0f, 0.0f); glVertex3v16(97, 157,  1);
      glTexCoord2f(1.0f, 1.0f); glVertex3v16(97, 173, 1);
      glTexCoord2f(0.0f, 1.0f); glVertex3v16(81, 173, 1);
      glTexCoord2f(0.0f, 0.0f); glVertex3v16(81, 157, 1);
      glEnd();
    }
  } else {
    if(!paused) {
      //If it's not paused update the game. Otherwise just draw the screen.
      myWorld->update();
    }
    //Check if game is over (if it's in progress
    //Update highscore.
    //Game in progress, check if it is over
    if (myWorld->isGameOver()) {		 
      //Game is over
      //Send to other player that this one lost.
      if (!onePlayer) {
	netSendLoss();
      }
      //set the high score
      if (highScore < myWorld->getPoints()) {
	//Score is > high score.
	highScore = myWorld->getPoints();
      }
      //Delete the world and reset everything to the title screen
      delete myWorld;
      glResetTextures();
      LoadTitleGLTextures();
      onTitle = !onTitle;
    } else {
      //Game still going
      myWorld->draw();
    }
  }
  return 0; // Keep Going 
}
#endif
