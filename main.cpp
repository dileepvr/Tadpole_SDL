#define WITH_SOUND
#define PRINT_MESSAGES
##define SERVER_DEBUG
#define TADPOLE_COLLISIONS
//#define LEECH_LIFE_ABOVE_10

#ifdef WITH_SOUND
#include <SDL/SDL_mixer.h>
#endif

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_net.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <sstream>
#include <math.h>
#include <vector>

// My own headers
#include "mysdl_images.h"
#include "mysdl_timer.h"
#include "tadclips.h"		// Storing sprite frames
#include "ipaddr.h"

//const int SCREEN_WIDTH = 1680; const int SCREEN_HEIGHT = 1020;
const int SCREEN_WIDTH = 1024; const int SCREEN_HEIGHT = 740;
//const int SCREEN_WIDTH = 1920; const int SCREEN_HEIGHT = 1030;

const int MAX_NAME_LENGTH = 256;
const int DOT_HEIGHT = 20;  		// Size of player
const int DOT_WIDTH = 20;
const int FRAMES_PER_SECOND = 30;
const int MAX_PLAYERS = 32;        // Maximum number of concurrrent players

// Server network stuff
//const char ETHERNET_INTERFACE[] = "eth0"; // Use "en0" for OSX
const char ETHERNET_INTERFACE[] = "wlp12s0"; // "wlan0"
//const char ETHERNET_INTERFACE[] = "enp9s0";
//const char ETHERNET_INTERFACE[] = "enp0s10"; 
const Uint16 PORT = 13370;         // Port to listen on for tcp
const Uint16 HELPPORT = 13371;         // Help port
const Uint16 JAVAPORT = 3000;   // Wes's Javascript node.js controller
const Uint16 BUFFER_SIZE = 255;
IPaddress serverIP, servIP, *remoteip;
Uint32 ipaddr;
TCPsocket serverSocket, helpSocket, clientSocket;
SDLNet_SocketSet socketSet, clientSet;
int receivedByteCount = 0;
char buf[BUFFER_SIZE+1];
bool localplayer = false, localconn = false, hiscorechange = true;
char serverName[] = "localhost";

const int LEADER_WIDTH = (int)(0.2265625*SCREEN_WIDTH);//435;
const int HISCORE_WIDTH = LEADER_WIDTH;
const int HI_X_OFFSET = (int)(0.773475*SCREEN_WIDTH);//1245;
const int BANNER_HEIGHT = (int)(0.03906*SCREEN_WIDTH);//75;
const int SCREEN_BPP = 32;		// Bits per pixel


int leaderboard_rank[MAX_PLAYERS];

int ntads = 0;         // Number of tads reserved (including alive)
int nalive = 0;        // Number of live tads in play

int txtoffset = (int)(HISCORE_WIDTH/20);


std::stringstream tempchar2, tempchar3, tempchar4;

char* reply;


SDL_Surface *screen;
SDL_Surface *hiscore_layer;
SDL_Surface *message;
SDL_Surface *message1;
SDL_Surface *message2;
SDL_Surface *message3;
SDL_Surface *message4;
SDL_Surface *message5;
SDL_Surface *player1;
SDL_Surface *frog;
SDL_Surface *fly;
SDL_Surface *waves_small;
SDL_Surface *waves_big;
SDL_Surface *waves_super;


SDL_Event event;

#ifdef WITH_SOUND
Mix_Music *ambience = NULL;

Mix_Chunk *fcroak = NULL;
Mix_Chunk *splat1 = NULL;
Mix_Chunk *splat2 = NULL;
Mix_Chunk *splat3 = NULL;
Mix_Chunk *launch1 = NULL;
Mix_Chunk *launch2 = NULL;
Mix_Chunk *damage = NULL;
Mix_Chunk *warp = NULL;
Mix_Chunk *slurp = NULL;
#endif

TTF_Font *font = NULL;
TTF_Font *regfont = NULL;
TTF_Font *larfont = NULL;
TTF_Font *bigfont = NULL;

// Wave tracker [wave #] [frame/flag & coordinates]
const int NUMWAVES = 10;
int smallwaves[MAX_PLAYERS][NUMWAVES][3];
int bigwaves[NUMWAVES][3];
int suwaves[NUMWAVES][3];

const SDL_Color redcolor = {0xFF, 0x44, 0x44};
Uint32 tadcolor[32];
Uint32 transcolor;

SDL_Rect leader_rect = {0, BANNER_HEIGHT, LEADER_WIDTH, SCREEN_HEIGHT-BANNER_HEIGHT};
SDL_Rect hiscore_rect = {0, 0, HISCORE_WIDTH, SCREEN_HEIGHT-BANNER_HEIGHT};
SDL_Rect banner_rect = {LEADER_WIDTH, 0, SCREEN_WIDTH-LEADER_WIDTH-HISCORE_WIDTH, BANNER_HEIGHT};

class hiscore_entry {
  private:
  public:
    char* name;
    int time;
};

std::vector<hiscore_entry> hiscores(32);


bool init() {

  char* tempchar;
  tempchar = (char*)malloc(16*sizeof(char));
  GetIP(ETHERNET_INTERFACE,tempchar);
  tempchar2 << "CONTROLLER HTTP:// " << tempchar << " : 3000";
  tempchar3 << "HELP TEXT HTTP:// " << tempchar << " : " << HELPPORT;  
  free(tempchar);

    if(SDL_Init(SDL_INIT_EVERYTHING) == -1) {
	printf("error: SDL failed to intitialize.\n");
    	return false;
    }

    // Initialize SDL_net 
    if(SDLNet_Init()==-1) {
      printf("SDLNet_Init: %s\n",SDLNet_GetError());
      return false;
    }

    // Create socketset with enough space allocated for connections
    socketSet = SDLNet_AllocSocketSet(MAX_PLAYERS+2);
    if(socketSet == NULL) {
      printf("Failed to allocate the socket set: %s\n",
	     SDLNet_GetError());
      return false;
    } else {
#ifdef PRINT_MESSAGES
      printf("Allocated socket set size: %d\n", MAX_PLAYERS+1);
#endif
    }
    
    // Setup server
    if(SDLNet_ResolveHost(&serverIP,NULL,PORT)==-1) {
      printf("SDLNet_ResolveHost: %s\n",SDLNet_GetError());
      return false;
    }

    clientSet = SDLNet_AllocSocketSet(1);
    SDLNet_ResolveHost(&servIP, serverName, PORT);
    
    // open the server socket 
    serverSocket=SDLNet_TCP_Open(&serverIP);
    if(!serverSocket) {
      printf("SDLNet_TCP_Open: %s\n",SDLNet_GetError());
      return false;
    }

#ifdef PRINT_MESSAGES
    printf("Server socket listening on %d.\n",
	   SDLNet_Read16(&serverIP.port));
#endif
    
    // Add server socket to socket set
    SDLNet_TCP_AddSocket(socketSet, serverSocket);

    // Setup help server
    if(SDLNet_ResolveHost(&serverIP,NULL,HELPPORT)==-1) {
      printf("SDLNet_ResolveHost: %s\n",SDLNet_GetError());
      return false;
    }

    // open the help socket 
    helpSocket=SDLNet_TCP_Open(&serverIP);
    if(!helpSocket) {
      printf("SDLNet_TCP_Open: %s\n",SDLNet_GetError());
      return false;
    }
    
    // Add help socket to socket set
    SDLNet_TCP_AddSocket(socketSet, helpSocket);

    reply = (char*)malloc(sizeof(char)*1600);
    sprintf(reply,"HTTP/1.1 200 OK\n"
"\n"
"Welcome to Tadpole_SDL.\n"
"A game by Dileep V. Reddy.\n"
"Network controller written in collaboration with Wes Erickson.\n"
"The project and license details can be found at:\n"
"https://github.com/dileepvr/Tadpole_SDL\n"
"https://github.com/inflamedspirit/Tadpole_Controller\n\n"
"To join the game using our Javascript controller, simply visit %s:%d.\n\n"
"To write your own custom controller, follow the protocol below:\n\n"
"Tadpole server is listening for connections on port %d.\n"
"To spawn a controllable tadpole,\n\n"
"1. Open a TCP connection to %s:%d\n\n"
"2. If a spot is not available, server sends 'X' and disconnects.\n\n"
"3. If spot is available, server sends 'N'.\n\n"
"4. Respond with desired player name.\n\n"
"5. Server sends \"RGBXXYYZZ\", where \"XXYYZZ\" are the RGB color values in hex (as a string). The Tadpole should be visible on screen.\n\n"
"6. To control Tadpole, send the following characters:\n\n"
"'1' = Up-key-pressed\n"
"'2' = Up-key-released\n"
"'3' = Right-key-pressed\n"
"'4' = Right-key-released\n"
"'5' = Down-key-pressed\n"
"'6' = Down-key-released\n"
"'7' = Left-key-pressed\n"
"'8' = Left-key-released\n\n"
"7. To kill Tadpole, send 'K'. Server replies with 'D'.\n\n"
"8. Everytime Tadpole hitpoints change, server sends \"HXX\", where \"XX\" encodes current hitpoint count.\n\n"
"9. Server sends 'D' and closes connection when Tadpole dies.\n\n"
"10. Server sends 'C' and closes connection when shutting down.\n",tempchar,JAVAPORT,PORT,tempchar,PORT);

    
    screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE);
    hiscore_layer = SDL_CreateRGBSurface(SDL_HWSURFACE, HISCORE_WIDTH, SCREEN_HEIGHT - BANNER_HEIGHT, SCREEN_BPP, 0, 0, 0, 0);
    if(screen == NULL) {
	printf("error: SDL_SetVideoMode failed.\n");
    	return false;
    }

    if( TTF_Init() == -1) {
	printf("error: TTF failed to initialize.\n");
    	return false;
    }
    
#ifdef WITH_SOUND
    if ( Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 1024 ) == -1){
	printf("error: OpenAudio failed to initialize.\n");
	return false;
    }
#endif
    SDL_WM_SetCaption("Tadpole", NULL);


    return true;

}


bool load_files() {

  char fname[100];  
  SDL_Surface* tmp;
  strcpy(fname,"images/tad_sprites.png");
  //  player1 = IMG_Load( fname );
  tmp = IMG_Load( fname );
  memset(fname,0,sizeof(fname));

  player1 = SDL_DisplayFormat(tmp);

  //  tmp = SDL_DisplayFormat(player1);
  Uint32 blackkey = SDL_MapRGB(player1->format,0,0,0);  
  transcolor = SDL_MapRGB(player1->format,0x12,0x34,0x56);
  tadcolor[0] = SDL_MapRGB(player1->format,0,0,0);
  tadcolor[1] = SDL_MapRGB(player1->format,0xFF,0,0);
  tadcolor[2] = SDL_MapRGB(player1->format,0,0xFF,0);
  tadcolor[3] = SDL_MapRGB(player1->format,0,0,0xFF);
  tadcolor[4] = SDL_MapRGB(player1->format,0xFF,0,0xDB);
  tadcolor[5] = SDL_MapRGB(player1->format,0xFF,0x9B,0);
  tadcolor[6] = SDL_MapRGB(player1->format,0xC4,0xB1,0x13);  
  tadcolor[7] = SDL_MapRGB(player1->format,0,0x7E,0xFF);  
  tadcolor[8] = SDL_MapRGB(player1->format,0x94,0x8C,0x8C);
  tadcolor[9] = SDL_MapRGB(player1->format,0xDB,0x58,0x58);
  tadcolor[10] = SDL_MapRGB(player1->format,0x17,0xA7,0x09);
  tadcolor[11] = SDL_MapRGB(player1->format,0x29,0x27,0x94);
  tadcolor[12] = SDL_MapRGB(player1->format,0x91,0x24,0xBF);
  tadcolor[13] = SDL_MapRGB(player1->format,0x87,0x5A,0x0D);
  tadcolor[14] = SDL_MapRGB(player1->format,0xFF,0xE5,0x09);
  tadcolor[15] = SDL_MapRGB(player1->format,0x1C,0x9F,0xA4);
  tadcolor[16] = SDL_MapRGB(player1->format,0xD2,0xC7,0xC7);
  tadcolor[17] = SDL_MapRGB(player1->format,0xFF,0x83,0x83);
  tadcolor[18] = SDL_MapRGB(player1->format,0x74,0xA4,0x6F);
  tadcolor[19] = SDL_MapRGB(player1->format,0x55,0x54,0x8A);
  tadcolor[20] = SDL_MapRGB(player1->format,0x5B,0x37,0x6A);
  tadcolor[21] = SDL_MapRGB(player1->format,0xB3,0x91,0x57);
  tadcolor[22] = SDL_MapRGB(player1->format,0xDE,0xFF,0x03);
  tadcolor[23] = SDL_MapRGB(player1->format,0x3C,0x6E,0x70);
  tadcolor[24] = SDL_MapRGB(player1->format,0x4E,0x51,0x53);
  tadcolor[25] = SDL_MapRGB(player1->format,0x7E,0x4F,0x4F);
  tadcolor[26] = SDL_MapRGB(player1->format,0x3E,0x5E,0x3B);
  tadcolor[27] = SDL_MapRGB(player1->format,0x72,0x6F,0xEF);
  tadcolor[28] = SDL_MapRGB(player1->format,0x87,0x0D,0x7A);
  tadcolor[29] = SDL_MapRGB(player1->format,0x7E,0x6F,0x56);
  tadcolor[30] = SDL_MapRGB(player1->format,0xFF,0xF7,0);
  tadcolor[31] = SDL_MapRGB(player1->format,0,0xFF,0xF2);  


  SDL_SetColorKey(player1,SDL_SRCCOLORKEY,blackkey);  
  //  player1 = change_color(player1,transcolor,tadcolor[0]);
  

  SDL_FreeSurface(tmp);
  
  strcpy(fname,"images/frog_sprites2.png");
  frog = load_image( fname );
  memset(fname,0,sizeof(fname));

  strcpy(fname,"images/fly_sprites.png");
  fly = load_image( fname );
  memset(fname,0,sizeof(fname));

  strcpy(fname,"images/waves_small.png");
  waves_small = load_image( fname );
  memset(fname,0,sizeof(fname));

  strcpy(fname,"images/waves_big.png");
  waves_big = load_image( fname );
  memset(fname,0,sizeof(fname));

  strcpy(fname,"images/superwave.png");
  waves_super = load_image( fname );
  memset(fname,0,sizeof(fname));
  
  if( player1 == NULL || frog == NULL || fly == NULL || waves_small == NULL || waves_big == NULL || waves_super == NULL) {
    printf("error: sprite not found.\n");
    return false;
  }

  //  font = TTF_OpenFont( "fonts/CARLETON.TTF", (int)(0.00885*SCREEN_WIDTH));//17
  font = TTF_OpenFont( "fonts/CARLETON.TTF", (int)(0.02*SCREEN_WIDTH));//17  
  regfont = TTF_OpenFont( "fonts/CARLETON.TTF", (int)(0.013*SCREEN_WIDTH));//25
  larfont = TTF_OpenFont( "fonts/STEVE.TTF", (int)(0.8*0.026*SCREEN_WIDTH));//40  
  bigfont = TTF_OpenFont( "fonts/STEVE.TTF", (int)(0.026*SCREEN_WIDTH));//50

  if ((font == NULL) || (bigfont == NULL)) {
    printf("error: ttf file not found\n");
    return false;
  }

  message1 = TTF_RenderText_Solid(bigfont, "Leader Board", redcolor);  
  message2 = TTF_RenderText_Solid(bigfont, "Hi-Scores", redcolor);
  message3 = TTF_RenderText_Solid(bigfont, "TO JOIN GAME:", redcolor);
  message4 = TTF_RenderText_Solid(regfont, tempchar2.str().c_str(), redcolor);
  message5 = TTF_RenderText_Solid(regfont, tempchar3.str().c_str(), redcolor);  

#ifdef WITH_SOUND

  ambience = Mix_LoadMUS("audio/Rainfall_short.ogg");
  splat1 = Mix_LoadWAV("audio/splat1.wav");
  splat2 = Mix_LoadWAV("audio/splat2.wav");
  splat3 = Mix_LoadWAV("audio/splat3.wav");
  launch1 = Mix_LoadWAV("audio/launch1.wav");
  launch2 = Mix_LoadWAV("audio/launch3.wav");
  damage = Mix_LoadWAV("audio/launch2.wav");
  warp = Mix_LoadWAV("audio/fcroak2.wav");
  fcroak = Mix_LoadWAV("audio/fcroak.wav");
  slurp = Mix_LoadWAV("audio/slurp.wav");

  if(ambience == NULL) {
    printf("error: audio file not found.\n");
    return false;
  }

#endif
  
  return true;
  
}

void clean_up() {

  free(reply);

  SDL_FreeSurface(screen);
  SDL_FreeSurface(hiscore_layer);
  SDL_FreeSurface(message);  
  SDL_FreeSurface(message1);
  SDL_FreeSurface(message2);
  SDL_FreeSurface(message3);
  SDL_FreeSurface(message4);
  SDL_FreeSurface(message5);  
  SDL_FreeSurface(player1);
  SDL_FreeSurface(frog);
  SDL_FreeSurface(fly);
  SDL_FreeSurface(waves_small);
  SDL_FreeSurface(waves_big);
  SDL_FreeSurface(waves_super);
  

#ifdef WITH_SOUND
  
  Mix_FreeChunk(fcroak);
  Mix_FreeChunk(splat1);
  Mix_FreeChunk(splat2);
  Mix_FreeChunk(splat3);
  Mix_FreeChunk(launch1);
  Mix_FreeChunk(launch2);
  Mix_FreeChunk(damage);
  Mix_FreeChunk(warp);
  Mix_FreeChunk(slurp);


  Mix_FreeMusic(ambience);
  
  Mix_CloseAudio();

#endif

  TTF_CloseFont(font);
  TTF_CloseFont(regfont);
  TTF_CloseFont(larfont);  
  TTF_CloseFont(bigfont);
  TTF_Quit();

  SDLNet_Quit();
  SDL_Quit();
  
}

bool draw_sidepanels() {

  Uint32 black_clr = SDL_MapRGB(screen->format, 0,0,0);


  SDL_FillRect(screen, &leader_rect, black_clr);
  SDL_FillRect(screen, &banner_rect, black_clr);

  apply_surface(txtoffset, txtoffset, message1, screen, NULL);

  apply_surface(HI_X_OFFSET+4*txtoffset, txtoffset, message2, screen, NULL);

  apply_surface(LEADER_WIDTH+(int)(txtoffset/2), txtoffset, message3, screen, NULL);

  apply_surface(LEADER_WIDTH+23*txtoffset, (int)(0.75*txtoffset), message4, screen, NULL);

  apply_surface(LEADER_WIDTH+23*txtoffset, (int)(3*0.75*txtoffset), message5, screen, NULL);


  if(hiscorechange) {
    SDL_FillRect(hiscore_layer, &hiscore_rect, black_clr);
    for(int loop2 = 0; loop2 < 32; loop2++) {
      tempchar4.str(" ");
      tempchar4 << hiscores[loop2].name;
      message = TTF_RenderText_Solid(font, tempchar4.str().c_str(), redcolor);
      apply_surface(20, 10+loop2*30, message, hiscore_layer, NULL);    
      tempchar4.str(" ");
      char temp[4];
      sprintf(temp,"%4d",hiscores[loop2].time);
      tempchar4 << temp;
      message = TTF_RenderText_Solid(font, tempchar4.str().c_str(), redcolor);
      apply_surface(HISCORE_WIDTH-70, 10+loop2*30, message, hiscore_layer, NULL);    
    }
    hiscorechange = false;
  }

  apply_surface(HI_X_OFFSET,BANNER_HEIGHT,hiscore_layer, screen, NULL);

  

  return true;
  
}


// include this late. Requires definitions (I didn't plan this out well)
#include "entities.h"	        // Stores entity class definitions (Tadpole, frog, fly)

int main(int argc, char* argv[]) {

  bool running = true;
  int i, j, xfrog, yfrog, xfly, yfly, leech[MAX_PLAYERS], tadswim = 0, swavenum[MAX_PLAYERS], swave_clk[MAX_PLAYERS], cls, kk;
  float dist;
  int nfrogs = 7; // Number of frogs
  int sp = 1;    // Speed (1-3)
  int a_i = 3;  // AI (1-3)
  FILE *fp;

  // Open hiscores.txt

  if((fp = fopen("hiscores.txt","r")) == NULL) {
    printf("Can't find file: hiscores.txt\n");
    return 1;
  }

  for(kk = 0; kk < 32; kk++) {
    hiscores[kk].name = (char*)malloc(MAX_NAME_LENGTH*sizeof(char));
    hiscores[kk].time = 0;
  }

  
  int ret = 1;
  kk = 0;
  while(ret != EOF) {
    ret = fscanf(fp,"%s %d", hiscores[kk].name, &hiscores[kk].time);
    kk++;
  }

  // Close hiscores.txt
  fclose(fp);

  // Bubble sort
  for(i=0; i<31; i++) {
    for(j=31; j > i; j--) {
      if(hiscores[j].time > hiscores[j-1].time) {
	strcpy(buf,hiscores[j-1].name);
	kk = hiscores[j-1].time;
	strcpy(hiscores[j-1].name,hiscores[j].name);
	hiscores[j-1].time = hiscores[j].time;
	strcpy(hiscores[j].name,buf);
	hiscores[j].time = kk;
      }
    }
  }

  
#ifdef TADPOLE_COLLISIONS  

  int tempvel;
  int collisions[MAX_PLAYERS][MAX_PLAYERS];

  for(i=0;i<MAX_PLAYERS;i++) {
    for(j=0;j<MAX_PLAYERS;j++) {
      collisions[i][j] = 0;
    }
  }

#endif


  for(i=0;i<MAX_PLAYERS;i++) {
    leech[i] = 0;
    swavenum[i] = 0;
    swave_clk[i] = 0;
    leaderboard_rank[i] = -1;
  }
  
  int bwavenum = 0, bwave_clk, mm_clk = 0, suwavenum = 0, suwave_clk = 0;  // various wave clocks and wave counters (storing latest wave to be animated)

  int nherd[10], nflychasers = 0; // nherd[i] counts number of close frogs to ith frog in jumpstate 0.
  int tadflip = 0, mmtadx = SCREEN_WIDTH/2, mmtady = SCREEN_HEIGHT/2+BANNER_HEIGHT/2;

  float ran; // random number storing
  char Tadname[MAX_NAME_LENGTH];


  strcpy(Tadname,"Player_0"); // Name of local player, no spaces

  set_frogclips();
  set_tadclips();
  set_flyclips();
  set_swaveclips();
  set_bwaveclips();
  set_suwaveclips();
  
  Timer fps;  
  Timer gameclock;
  Timer flyspan;
  std::vector<Timer> warpspan(nfrogs);
  
//    Setting wave flags to 'not animate / done animation'

  for(j=0; j<NUMWAVES; j++) {
    nherd[j] = 0;
    for(kk=0; kk<MAX_PLAYERS; kk++) {
      smallwaves[kk][j][0] = 6;
    }
  }

  for(j=0; j<NUMWAVES; j++) {
    bigwaves[j][0] = 24;
    suwaves[j][0] = 24;
  }

  
  if (init() == false) return 1;

  if (load_files() == false) return 1;

  // Spawn Tadpole
  //  Tadpole myTad;
  std::vector<Tadpole> myTad(MAX_PLAYERS);


  // Spawn fly
  xfly = 10+drand48()*(SCREEN_WIDTH-LEADER_WIDTH-HISCORE_WIDTH-20)+LEADER_WIDTH;
  yfly = 10+drand48()*(SCREEN_HEIGHT-BANNER_HEIGHT-20) + BANNER_HEIGHT;
  Fly myfly( xfly, yfly );

  // Spawn Frogs
  std::vector<Frog> myfrogs(nfrogs);


  for(i = 0; i < nfrogs; i++ ) {
    xfrog = drand48()*(SCREEN_WIDTH-LEADER_WIDTH-HISCORE_WIDTH-20)+LEADER_WIDTH;
    yfrog = drand48()*(SCREEN_HEIGHT-BANNER_HEIGHT-20) + BANNER_HEIGHT;
    if(distance(xfrog, yfrog, mmtadx, mmtady) < (SCREEN_HEIGHT-BANNER_HEIGHT)/4) {
      i--;
    }
    myfrogs[i].Frog_set(xfrog, yfrog, sp, a_i);
  }


  for(i=0; i<nfrogs; i++)
    myfrogs[i].show(sp);

  myfly.show();


  if(SDL_Flip(screen) == -1) return 1;

  SDL_Delay(500);

  gameclock.start();
  
#ifdef PRINT_MESSAGES  
  printf("Tadpole server is up!\n");
#endif
  

#ifdef WITH_SOUND
//    if(Mix_PlayMusic(ambience, -1) == -1)
    if(Mix_FadeInMusic(ambience, -1, 2000) == -1)
	return 1;
#endif
  

  // Game Server loop
  while (running) {

    fps.start();

    // Listen for connections, argument 0 means no wait-time
    int numActiveSockets = SDLNet_CheckSockets(socketSet, 0);

    if(numActiveSockets > 0) {

      // Check help socket. If activity detected, then open temporary socket and dump help text. Then close socket.
      int helpSocketActivity = SDLNet_SocketReady(helpSocket);
      if(helpSocketActivity != 0) {
	TCPsocket tempSock = SDLNet_TCP_Accept(helpSocket);
	SDLNet_TCP_Send(tempSock,(void*)reply,strlen(reply)+1);
	// Close temporary connection
	SDLNet_TCP_Close(tempSock);
      }

      
      // Check if our server socket has received any data
      // Note: SocketReady can only be called on a socket which is part of a set and that has CheckSockets called on it (the set, that is)
      // SDLNet_SocketRead returns non-zero for activity, and zero is returned for no activity.
      int serverSocketActivity = SDLNet_SocketReady(serverSocket);
      // All server activity interpreted as request for TCP connection
      if(serverSocketActivity != 0) {
	// If we have room for more clients...
	if(ntads < MAX_PLAYERS) {
	  int freeSpot = -99;
	  for(int i = 0; i < MAX_PLAYERS; i++) {
	    if(!myTad[i].alive && !myTad[i].TCP_limbo) {
	      freeSpot = i;
	      break;
	    }
	  }
	  // Accept the client connection
	  myTad[freeSpot].socket = SDLNet_TCP_Accept(serverSocket);

	  // get clients IP
	  remoteip = SDLNet_TCP_GetPeerAddress(myTad[freeSpot].socket);
	  if(!remoteip) {
#ifdef SERVER_DEBUG
	    printf("SDLNet_TCP_GetPeerAddress: %s\n",SDLNet_GetError());
#endif
	    // No remote ip?! NO CONNECTION!
	    SDLNet_TCP_Close(myTad[freeSpot].socket);	  
	  } else {
#ifdef SERVER_DEBUG	  
	    ipaddr=SDL_SwapBE32(remoteip->host);
	    printf("Accepted a connection from %d.%d.%d.%d port %hu.",
		   ipaddr>>24,
		   (ipaddr>>16)&0xff,
		   (ipaddr>>8)&0xff,
		   ipaddr&0xff,
		   remoteip->port);
	    printf(" There are now %d client(s) online.\n",
		   ntads+1);
#endif
	    ntads++;
	    myTad[freeSpot].TCP_limbo = true;
	    // Add new client socket to socket set, to check activity
	    SDLNet_TCP_AddSocket(socketSet, myTad[freeSpot].socket);
	    // Send 'N' asking for name
	    strcpy(buf,"N");
	    SDLNet_TCP_Send(myTad[freeSpot].socket,(void*)buf,2);
	  }
	} else { // No new room for clients
#ifdef SERVER_DEBUG
	  printf("No room. Rejecting client.\n");
#endif
	  // Accept client connection to clear it from incoming list
	  TCPsocket tempSock = SDLNet_TCP_Accept(serverSocket);
	  // Send 'X' telling that there is no room
	  strcpy(buf,"X");
	  SDLNet_TCP_Send(tempSock,(void*)buf,2);
	  // Close temporary connection
	  SDLNet_TCP_Close(tempSock);
	}
      }

      // Check all possible client sockets for activity
      for(i = 0; i < MAX_PLAYERS; i++) {
	int clientSocketActivity = SDLNet_SocketReady(myTad[i].socket);
	if(clientSocketActivity != 0) {
	  receivedByteCount = SDLNet_TCP_Recv(myTad[i].socket,buf,BUFFER_SIZE);
	  // If activity, but no data, then assume disconnection
	  if(receivedByteCount <= 0) {
#ifdef SERVER_DEBUG
	    printf("Client %d disconnected. Now there are %d clients.\n", i, ntads-1);
#endif
	    // Close connection
	    SDLNet_TCP_DelSocket(socketSet, myTad[i].socket);
	    if(strcmp(myTad[i].name,Tadname)==0) {
	      localplayer = false;
	    }
	    if(hiscores[31].time <= (int)(myTad[i].age_clock.get_ticks()/1000)) {
	      hiscorechange = true;
	      hiscores[31].time = (int)(myTad[i].age_clock.get_ticks()/1000);
	      strcpy(hiscores[31].name,myTad[i].name);
	      // Bubble sort
	      for(j=31; j > 0; j--) {
		if(hiscores[j].time > hiscores[j-1].time) {
		  strcpy(buf,hiscores[j-1].name);
		  kk = hiscores[j-1].time;
		  strcpy(hiscores[j-1].name,hiscores[j].name);
		  hiscores[j-1].time = hiscores[j].time;
		  strcpy(hiscores[j].name,buf);
		  hiscores[j].time = kk;
		}
	      }
	      }
	    myTad[i].kill();
	    for(kk=0; kk<MAX_PLAYERS; kk++) {
	      if(myTad[kk].alive) {
		if(leaderboard_rank[kk] > leaderboard_rank[i]) {
		  leaderboard_rank[kk]--;
		}
	      }
	    }
	    leaderboard_rank[i] = -1;
	    ntads--;
	    nalive--;
	  } else { // Got message from client
	    buf[receivedByteCount] = '\0';
	    if((buf[receivedByteCount-1] == '\r') || (buf[receivedByteCount-1] == '\n')) {
	      buf[receivedByteCount-1] = '\0';	      
	    }
#ifdef SERVER_DEBUG
	    printf("Client: %d sent: %s.\n", i, buf);
#endif
	    if(buf[0]!='\n') {
	    
	      if(myTad[i].TCP_limbo) {
		//		buf[MAX_NAME_LENGTH] = '\0';
		buf[10] = '\0';
		//		for(kk=0;kk<MAX_NAME_LENGTH; kk++) {
		for(kk=0;kk<10; kk++) {
		  if((buf[kk] == ' ')||(buf[kk] == '\t')) {
		    buf[kk] = '_';
		  }
		}
		myTad[i].spawn(buf, i);
		leaderboard_rank[i] = nalive++;
		sprintf(buf,"RGB%2x%2x%2x",(tadcolor[i]>>16)&0xff,(tadcolor[i]>>8)&0xff,tadcolor[i]&0xff);
		if(((tadcolor[i]>>16)&0xff) < 0x0f) {
		  buf[3] = '0';
		}
		if(((tadcolor[i]>>8)&0xff) < 0x0f) {
		  buf[5] = '0';
		}
		if((tadcolor[i]&0xff) < 0x0f) {
		  buf[7] = '0';
		}
		SDLNet_TCP_Send(myTad[i].socket,(void*)buf,10);
	      } else {
		for(j = 0; j < receivedByteCount; j++) {
		  // If client asked to quit by sending 'K' as first char
		  if(buf[j] == 'K') {
#ifdef SERVER_DEBUG
		    printf("Client %d asked to be disconnected. Now there are %d clients.\n", i, ntads-1);
#endif
		    // Send 'D' telling that server is killing client
		    strcpy(buf,"D");
		    SDLNet_TCP_Send(myTad[i].socket,(void*)buf,2);
		    // Close connection
		    SDLNet_TCP_DelSocket(socketSet, myTad[i].socket);
		    if(strcmp(myTad[i].name,Tadname)==0) {
		      localplayer = false;
		    }
		    if(hiscores[31].time <= (int)(myTad[i].age_clock.get_ticks()/1000)) {
		      hiscorechange = true;
		      hiscores[31].time = (int)(myTad[i].age_clock.get_ticks()/1000);
		      strcpy(hiscores[31].name,myTad[i].name);
		      // Bubble sort
		      for(j=31; j > 0; j--) {
			if(hiscores[j].time > hiscores[j-1].time) {
			  strcpy(buf,hiscores[j-1].name);
			  kk = hiscores[j-1].time;
			  strcpy(hiscores[j-1].name,hiscores[j].name);
			  hiscores[j-1].time = hiscores[j].time;
			  strcpy(hiscores[j].name,buf);
			  hiscores[j].time = kk;
			}
		      }
		      }
		    myTad[i].kill();
		    for(kk=0; kk<MAX_PLAYERS; kk++) {
		      if(myTad[kk].alive) {
			if(leaderboard_rank[kk] > leaderboard_rank[i]){
			  leaderboard_rank[kk]--;
			}
		      }
		    }
		    leaderboard_rank[i] = -1;		    
		    nalive--;
		    ntads--;
		    break;
		  } else {
		    myTad[i].handle_input(buf[j]);
		  }
		}
	      }
	    }
	  }
	}
      }
      
    }




    // ********* Local client socket handling *********** //

    if (localconn) {
      if(SDLNet_CheckSockets(clientSet, 0) > 0) {
	int serverResponseByteCount = SDLNet_TCP_Recv(clientSocket, buf, BUFFER_SIZE);
	for(j=0; j<strlen(buf); j++) {
	  if((serverResponseByteCount <= 0) || (buf[j] == 'X')) {
#ifdef SERVER_DEBUG
	    printf("Server rejected local player!");
#endif
	    localconn = false;
	    SDLNet_TCP_DelSocket(clientSet, clientSocket);
	    break;
	  } else if(buf[j] == 'N') {
	    int namelen = strlen(Tadname) + 1;
	    SDLNet_TCP_Send(clientSocket,(void*)Tadname,namelen);
	    localplayer = true;
	  } else if((buf[j] == 'D')||(buf[j]=='X')||(buf[j]=='C')||(buf[j]=='X')) {
	    localconn = false;
	    SDLNet_TCP_DelSocket(clientSet, clientSocket);	    
	    localplayer = false;
	    break;
	  }
	}
      }
    }
      
    
    // *********** local keyboard events *********** //
    
    while(SDL_PollEvent ( &event )) {

      if(event.type == SDL_KEYDOWN) {
	if(event.key.keysym.sym == SDLK_ESCAPE) {
	  running = false;
	}
	if(event.key.keysym.sym == SDLK_RETURN) { // SPAWN NEW TAD
	  if((!localplayer) && (!localconn)) {
	    // Open local socket as controller
#ifdef SERVER_DEBUG
	    printf("Opening local player socket.\n");
#endif
	    clientSocket = SDLNet_TCP_Open(&servIP);
	    if(clientSocket != NULL) {
	      localconn = true;
	      SDLNet_TCP_AddSocket(clientSet, clientSocket);	      
	    } else {
#ifdef SERVER_DEBUG
	      printf("Local player connection rejected.\n");
#endif	      
	    }
	  }
	} else if (localplayer){
	  switch(event.key.keysym.sym) {
	  case SDLK_UP:
	  case SDLK_w:
	    SDLNet_TCP_Send(clientSocket,"1",2);
	    break;
	  case SDLK_DOWN:
	  case SDLK_s:
	    SDLNet_TCP_Send(clientSocket,"5",2);
	    break;
	  case SDLK_LEFT:
	  case SDLK_a:
	    SDLNet_TCP_Send(clientSocket,"7",2);
	    break;
	  case SDLK_RIGHT:
	  case SDLK_d:
	    SDLNet_TCP_Send(clientSocket,"3",2);
	    break;
	  case SDLK_k:
	    SDLNet_TCP_Send(clientSocket,"K",2);
	    break;
	  }
	}
      }


      if(event.type == SDL_KEYUP) {
	if (localplayer){
	  switch(event.key.keysym.sym) {
	  case SDLK_UP:
	  case SDLK_w:
	    SDLNet_TCP_Send(clientSocket,"2",2);
	    break;
	  case SDLK_DOWN:
	  case SDLK_s:
	    SDLNet_TCP_Send(clientSocket,"6",2);
	    break;
	  case SDLK_LEFT:
	  case SDLK_a:
	    SDLNet_TCP_Send(clientSocket,"8",2);
	    break;
	  case SDLK_RIGHT:
	  case SDLK_d:
	    SDLNet_TCP_Send(clientSocket,"4",2);
	    break;
	  }
	}	
      }      
      
      if(event.type == SDL_QUIT) running = false;
      
    }


    

//***************** Tad-Tad collisions ****************//    
#ifdef TADPOLE_COLLISIONS


    if(ntads > 1) {
      for(i = 0; i < MAX_PLAYERS; i++) {
	if (myTad[i].alive) {
	  for(j = i+1; j < MAX_PLAYERS; j++) {
	    if(myTad[j].alive) {
	      if(distance(myTad[i].c.x,myTad[i].c.y,myTad[j].c.x,myTad[j].c.y) <= myTad[i].c.r+myTad[j].c.r) {
		if(collisions[i][j] == 0) {
		  collisions[i][j] = 1;
		  collisions[j][i] = 1;
		  tempvel = myTad[i].xVel;
		  myTad[i].xVel = myTad[j].xVel;
		  myTad[j].xVel = tempvel;
		  tempvel = myTad[i].yVel;
		  myTad[i].yVel = myTad[j].yVel;
		  myTad[j].yVel = tempvel;
		}
	      } else {
		collisions[i][j] = 0;
		collisions[j][i] = 0;
	      }
	    }
	  }
	}
      }
    }
    
    
#endif    
    
//*****************************************************//
	
// ******************** FROGS CHASING FLIES ********************* //

    if(a_i == 3) {
      nflychasers = 0;
      for(i=0; i<nfrogs; i++) {
	if(myfrogs[i].jumpstate == 2){
	  for(j=0; j<nfrogs; j++) {
	    if(j!=i && myfrogs[j].jumpstate == 2 && distance(myfrogs[i].c.x, myfrogs[i].c.y, myfrogs[j].c.x, myfrogs[j].c.y) < 200)
	      nherd[i]++;
	  }
	} else {
	  nherd[i] = 0;
	  if(myfrogs[i].chase_flag == 1) //&& myfrogs[i].speed == sp)
	    nflychasers ++;
	}
      }
    }

    for(i=0; i<nfrogs; i++) {

      // Find the live Tadpole closest to myfrogs[i]
      cls = 0;
      dist = 4*SCREEN_WIDTH;

      for(kk=0;kk<MAX_PLAYERS;kk++){
	if(myTad[kk].alive) {
	  if((dist>distance(myTad[cls].c.x,myTad[cls].c.y,myfrogs[i].c.x,myfrogs[i].c.y)) && (myTad[kk].hpoints > 0)) {
	    cls = kk;
	    dist=distance(myTad[cls].c.x,myTad[cls].c.y,myfrogs[i].c.x,myfrogs[i].c.y);
	  }
	}
      }
      
      
      if(a_i == 3) {
	if(nherd[i] > 1) {
	  if(nflychasers >= 2) {
	    myfrogs[i].handle_events(myTad[cls].c.x+100*(drand48()-1), myTad[cls].c.y+100*(drand48()-1), myTad[cls].xVel, myTad[cls].yVel, myfly.c.x, myfly.c.y, 0, sp,myTad[cls].alive);
	  } else{
	    myfrogs[i].handle_events(myTad[cls].c.x+100*(drand48()-1), myTad[cls].c.y+100*(drand48()-1), myTad[cls].xVel, myTad[cls].yVel, myfly.c.x, myfly.c.y, myfly.spawn, sp,myTad[cls].alive);
	  }
	} else {
	  if(nflychasers >= 2) {
	    myfrogs[i].handle_events(myTad[cls].c.x, myTad[cls].c.y, myTad[cls].xVel, myTad[cls].yVel, myfly.c.x, myfly.c.y, 0, sp,myTad[cls].alive);
	  } else {
	    myfrogs[i].handle_events(myTad[cls].c.x, myTad[cls].c.y, myTad[cls].xVel, myTad[cls].yVel, myfly.c.x, myfly.c.y, myfly.spawn, sp,myTad[cls].alive);
	  }
	}
      } else {
	myfrogs[i].handle_events(myTad[cls].c.x, myTad[cls].c.y, myTad[cls].xVel, myTad[cls].yVel, myfly.c.x, myfly.c.y, myfly.spawn, sp,myTad[cls].alive);
      }
    }

    // ************ IS THE FLY GETTING EATEN? ************ //
    if(myfly.spawn == 1) {
      for(kk=0; kk<MAX_PLAYERS; kk++) {
	if(myTad[kk].alive) {
	  if( myfly.handle_collision(myTad[kk].c.x, myTad[kk].c.y) ) {
	    if(myTad[kk].hpoints <= 18)
	      myTad[kk].hpoints += 2;
	    else if(myTad[kk].hpoints == 19)
	      myTad[kk].hpoints += 1;
	    sprintf(buf,"H%2d",myTad[kk].hpoints);
	    if(myTad[kk].hpoints < 10) buf[1] = '0';
	    SDLNet_TCP_Send(myTad[kk].socket,(void*)buf,4);
	    flyspan.start();
	    suwaves[suwavenum][0] = -1;
	    suwaves[suwavenum][1] = myfly.c.x - 150;
	    suwaves[suwavenum][2] = myfly.c.y - 150;
	    if(++suwavenum == NUMWAVES) suwavenum = 0;
#ifdef WITH_SOUND
	    Mix_PlayChannel(-1, slurp, 0);
#endif
	  }
	}
      }
      
      for(i=0; i<nfrogs; i++) {
	if( myfly.handle_collision(myfrogs[i].c.x, myfrogs[i].c.y) ) { 
	  myfrogs[i].speed = sp + 2;
	  warpspan[i].start();
	  flyspan.start();
	  suwaves[suwavenum][0] = -1;
	  suwaves[suwavenum][1] = myfly.c.x - 150;
	  suwaves[suwavenum][2] = myfly.c.y - 150;
	  if(++suwavenum == NUMWAVES) suwavenum = 0;
#ifdef WITH_SOUND
	  Mix_PlayChannel(-1, slurp, 0);
#endif
	  break;
	}
      }
    }



	// ********************************* LOGIC *************************************** //



    if( (gameclock.get_ticks() - tadswim) >= 50 ) {
      tadswim = gameclock.get_ticks();
      for(kk=0; kk<MAX_PLAYERS; kk++) {
	if(myTad[kk].alive) {
	  myTad[kk].move(abs(myTad[kk].xflag)+abs(myTad[kk].yflag));
	}
      }
    } else {
      for(kk=0; kk<MAX_PLAYERS; kk++) {
	if(myTad[kk].alive) {	
	  myTad[kk].move(0);
	}
      }
    }


    for(i=0; i<nfrogs; i++) {
      if(warpspan[i].get_ticks() >= 5000 ) {
	warpspan[i].stop();
	myfrogs[i].speed = sp;
      }
      myfrogs[i].move();
    }


    for(i=0; i<nfrogs; i++) {
      for(kk=0; kk<MAX_PLAYERS; kk++) {
	if((distance(myTad[kk].c.x, myTad[kk].c.y, myfrogs[i].c.x, myfrogs[i].c.y) <= myTad[kk].c.r + myfrogs[i].c.r)&& myTad[kk].alive) {
	  suwaves[suwavenum][0] = -1;
	  suwaves[suwavenum][1] = myTad[kk].c.x - 150;
	  suwaves[suwavenum][2] = myTad[kk].c.y - 150;
	  if(++suwavenum == NUMWAVES) suwavenum = 0;
#ifdef WITH_SOUND
	  Mix_PlayChannel(-1, damage, 0);
#endif
	  myTad[kk].hpoints--;
	  sprintf(buf,"H%2d",myTad[kk].hpoints);
	  if(myTad[kk].hpoints < 10) buf[1] = '0';
	  SDLNet_TCP_Send(myTad[kk].socket,(void*)buf,4);
	}
      }
    }

    if(myfly.spawn == 0) {
      if(flyspan.get_ticks() >= 1000) {
	flyspan.stop();
	myfly.spawn = 1;
	myfly.c.x = 10+drand48()*(SCREEN_WIDTH-LEADER_WIDTH-HISCORE_WIDTH-20)+LEADER_WIDTH;
	myfly.c.y = 10+drand48()*(SCREEN_HEIGHT-BANNER_HEIGHT-20) + BANNER_HEIGHT;
      }
    }

#ifdef LEECH_LIFE_ABOVE_10

    for(kk=0; kk<MAX_PLAYERS; kk++) {
      if(myTad[kk].alive) {    
	if(myTad[kk].hpoints <= 10) {
	  leech[kk] = gameclock.get_ticks();
	}
	if( (gameclock.get_ticks() - leech[kk]) >= 5000 ) {
	  leech[kk] = gameclock.get_ticks();
	  if(myTad[kk].hpoints > 10) {
	    myTad[kk].hpoints--;
	    sprintf(buf,"H%2d",myTad[kk].hpoints);
	    if(myTad[kk].hpoints < 10) buf[1] = '0';
	    SDLNet_TCP_Send(myTad[kk].socket,(void*)buf,4);
	  }
	}
      }
    }

#endif
    
    for(i=0; i<nfrogs; i++) {
      if(myfrogs[i].jumpstate == 1) {
	bigwaves[bwavenum][0] = -1;
	bigwaves[bwavenum][1] = myfrogs[i].c.x - 75;
	bigwaves[bwavenum][2] = myfrogs[i].c.y - 75;
	if(++bwavenum == NUMWAVES) bwavenum = 0;
      }
    }

    // For spawning
    for(kk=0; kk<MAX_PLAYERS; kk++) {
      if(myTad[kk].justborn) {
	bigwaves[bwavenum][0] = -1;
	bigwaves[bwavenum][1] = myTad[kk].c.x - 75;
	bigwaves[bwavenum][2] = myTad[kk].c.y - 75;
	if(++bwavenum == NUMWAVES) bwavenum = 0;
	myTad[kk].justborn = false;
      }
    }

    if(gameclock.get_ticks() - bwave_clk >= 36) {
      bwave_clk = gameclock.get_ticks();
      for(j=0; j<NUMWAVES; j++) {
	if(bigwaves[j][0] < 24) bigwaves[j][0]++;
      }
    }

    if(gameclock.get_ticks() - suwave_clk >= 36)	{
      suwave_clk = gameclock.get_ticks();
      for(j=0; j<NUMWAVES; j++)
	{
	  if(suwaves[j][0] < 24) suwaves[j][0]++;
	}
    }

    for(kk=0; kk<MAX_PLAYERS; kk++) {

      if(abs(myTad[kk].xflag)+abs(myTad[kk].yflag) == 0) {
	smallwaves[kk][swavenum[kk]][0] = 6;
      }

      if(gameclock.get_ticks() - swave_clk[kk] >= 75) {
	swave_clk[kk] = gameclock.get_ticks();
	if(++swavenum[kk] >= NUMWAVES) swavenum[kk] = 0;
	smallwaves[kk][swavenum[kk]][0] = -1;
	smallwaves[kk][swavenum[kk]][1] = myTad[kk].c.x - myTad[kk].c.r - 10;
	smallwaves[kk][swavenum[kk]][2] = myTad[kk].c.y - myTad[kk].c.r - 10;
	for(j=0; j<NUMWAVES; j++)
	  if(smallwaves[kk][j][0] < 6) smallwaves[kk][j][0]++;
      }
    }

    
    // Kill dead tadpoles
    for(i=0;i<MAX_PLAYERS;i++) {    
      if((myTad[i].hpoints <= 0) && myTad[i].alive) {
	// Send 'D' telling that Tadpole has died
	strcpy(buf,"D");
	SDLNet_TCP_Send(myTad[i].socket,(void*)buf,2);
	// Close connection
	SDLNet_TCP_DelSocket(socketSet, myTad[i].socket);
	if(strcmp(myTad[i].name,Tadname)==0) {
	  localplayer = false;
	}
	if(hiscores[31].time <= (int)(myTad[i].age_clock.get_ticks()/1000)) {
	  hiscorechange = true;
	  hiscores[31].time = (int)(myTad[i].age_clock.get_ticks()/1000);
	  strcpy(hiscores[31].name,myTad[i].name);
	  // Bubble sort
	  for(j=31; j > 0; j--) {
	    if(hiscores[j].time > hiscores[j-1].time) {
	      strcpy(buf,hiscores[j-1].name);
	      kk = hiscores[j-1].time;
	      strcpy(hiscores[j-1].name,hiscores[j].name);
	      hiscores[j-1].time = hiscores[j].time;
	      strcpy(hiscores[j].name,buf);
	      hiscores[j].time = kk;
	    }
	  }
	}
	myTad[i].kill();
	for(kk=0; kk<MAX_PLAYERS; kk++) {
	  if(myTad[kk].alive) {
	    if(leaderboard_rank[kk] > leaderboard_rank[i]) {
	      leaderboard_rank[kk]--;
	    }
	  }
	}
	leaderboard_rank[i] = -1;	
	nalive--;
	ntads--;		
      }
    }

    
    // ************** DRAW STUFF ***********************//
    // Make screen white
    SDL_FillRect( screen, &screen->clip_rect, SDL_MapRGB(screen->format, 0xFF,0xFF,0xFF) );
    
    for(i=0;i<MAX_PLAYERS;i++) {
      if(myTad[i].alive) {
	for(j=0; j<NUMWAVES; j++){
	  if(smallwaves[i][j][0] != 6){
	    apply_surface(smallwaves[i][j][1], smallwaves[i][j][2], waves_small, screen, &swaveclips[smallwaves[i][j][0]]);
	  }
	}
      }
    }

    for(j=0; j<NUMWAVES; j++){
      if(bigwaves[j][0] != 24){
	apply_surface(bigwaves[j][1], bigwaves[j][2], waves_big, screen, &bwaveclips[bigwaves[j][0]]);
      }
      if(suwaves[j][0] != 24){
	apply_surface(suwaves[j][1], suwaves[j][2], waves_super, screen, &suwaveclips[suwaves[j][0]]);
      }
    }

    // Draw the leader board stuff
    if(!draw_sidepanels()) running = false;


    for(i=0;i<MAX_PLAYERS;i++) {
      myTad[i].show(leaderboard_rank[i]);
    }
    
    for(i=0; i<nfrogs; i++)
      myfrogs[i].show(sp);

    if(myfly.spawn == 1)
      myfly.show();


    if(SDL_Flip(screen) == -1) return false;


    if(hiscorechange) {
      if((fp = fopen("hiscores.txt","w")) == NULL) {
	printf("Can't open file: hiscores.txt\n");
	return 1;
      }

      for(kk = 0; kk < 32; kk++) {
	fprintf(fp,"%s\t%d\n",hiscores[kk].name,hiscores[kk].time);
	free(hiscores[kk].name);
      }

      fclose(fp);
    }
    

    // Wait for next frame render time
    if(fps.get_ticks() < (1000 / FRAMES_PER_SECOND)){
      SDL_Delay((1000/FRAMES_PER_SECOND) - fps.get_ticks());
    }
    
  }

#ifdef WITH_SOUND
    if(Mix_HaltMusic() == -1)
	return 1;
#endif

  
  fps.stop();

#ifdef PRINT_MESSAGES  
  printf("Closing remaining client connections.\n");
#endif
  
  // Close all surviving Tadpole sockets
  for(i=0; i < MAX_PLAYERS; i++) {
    if(myTad[i].alive) {
      // Send 'C' telling that server is closing
      strcpy(buf,"C");
      SDLNet_TCP_Send(myTad[i].socket,(void*)buf,2);
      // Close connection
      SDLNet_TCP_DelSocket(socketSet, myTad[i].socket);
      if(hiscores[31].time <= (int)(myTad[i].age_clock.get_ticks()/1000)) {
	hiscorechange = true;	
	hiscores[31].time = (int)(myTad[i].age_clock.get_ticks()/1000);
	strcpy(hiscores[31].name,myTad[i].name);
	// Bubble sort
	for(j=31; j > 0; j--) {
	  if(hiscores[j].time > hiscores[j-1].time) {
	    strcpy(buf,hiscores[j-1].name);
	    kk = hiscores[j-1].time;
	    strcpy(hiscores[j-1].name,hiscores[j].name);
	    hiscores[j-1].time = hiscores[j].time;
	    strcpy(hiscores[j].name,buf);
	    hiscores[j].time = kk;
	  }
	}
      }
      myTad[i].kill();
      nalive--;
      ntads--;      
#ifdef SERVER_DEBUG
      printf("Client %d has been disconnected.\n", i);
#endif
    }
  }

#ifdef PRINT_MESSAGES  
  printf("Server shutting down!\n");
#endif

  if(localplayer || localconn) { SDLNet_TCP_Close(clientSocket); }
  // Free socket set
  SDLNet_FreeSocketSet(socketSet);
  // Close server socket
  SDLNet_TCP_Close(serverSocket);
  // Close help socket
  SDLNet_TCP_Close(helpSocket);


  if((fp = fopen("hiscores.txt","w")) == NULL) {
    printf("Can't open file: hiscores.txt\n");
    return 1;
  }

  for(kk = 0; kk < 32; kk++) {
    fprintf(fp,"%s\t%d\n",hiscores[kk].name,hiscores[kk].time);
    free(hiscores[kk].name);
  }

  fclose(fp);

  clean_up();
  return 0;

}
