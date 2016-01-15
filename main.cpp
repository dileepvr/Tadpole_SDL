#define WITH_SOUND

#ifdef WITH_SOUND
#include "/usr/include/SDL/SDL_mixer.h"
#endif

#include "/usr/include/SDL/SDL.h"
#include "/usr/include/SDL/SDL_image.h"
#include "/usr/include/SDL/SDL_ttf.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <vector>

// My own headers
#include "mysdl_images.h"
#include "mysdl_timer.h"
#include "tadclips.h"		// Storing sprite frames


int SCREEN_WIDTH = 1680; int SCREEN_HEIGHT = 1020;
//int SCREEN_WIDTH = 1920; int SCREEN_HEIGHT = 1050;
int LEADER_WIDTH = (int)(0.2265625*SCREEN_WIDTH);//435;
int HISCORE_WIDTH = LEADER_WIDTH;
int HI_X_OFFSET = (int)(0.773475*SCREEN_WIDTH);//1245;
int BANNER_HEIGHT = (int)(0.03906*SCREEN_WIDTH);//75;
int SCREEN_BPP = 32;		// Bits per pixel

int txtoffset = (int)(HISCORE_WIDTH/20);

int DOT_HEIGHT = 20;  		// Size of player
int DOT_WIDTH = 20;


const int FRAMES_PER_SECOND = 30;

SDL_Surface *screen;
SDL_Surface *message;

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
int smallwaves[20][3];
int bigwaves[20][3];
int suwaves[20][3];

SDL_Color redcolor = {0xFF, 0, 0};

char fname[100];

SDL_Rect leader_rect = {0, BANNER_HEIGHT, LEADER_WIDTH, SCREEN_HEIGHT-BANNER_HEIGHT};
SDL_Rect hiscore_rect = {HI_X_OFFSET, BANNER_HEIGHT, HISCORE_WIDTH, SCREEN_HEIGHT-BANNER_HEIGHT};
SDL_Rect banner_rect = {LEADER_WIDTH, 0, SCREEN_WIDTH-LEADER_WIDTH-HISCORE_WIDTH, BANNER_HEIGHT};

bool init() {
    if(SDL_Init(SDL_INIT_EVERYTHING) == -1) {
	printf("error: SDL failed to intitialize.\n");
    	return false;
    }

    screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE);
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

  strcpy(fname,"images/tad_sprites.png");
  player1 = load_image( fname );
  memset(fname,0,sizeof(fname));

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

  font = TTF_OpenFont( "fonts/CARLETON.TTF", (int)(0.00885*SCREEN_WIDTH));//17
  regfont = TTF_OpenFont( "fonts/CARLETON.TTF", (int)(0.013*SCREEN_WIDTH));//25
  larfont = TTF_OpenFont( "fonts/STEVE.TTF", (int)(0.8*0.026*SCREEN_WIDTH));//40  
  bigfont = TTF_OpenFont( "fonts/STEVE.TTF", (int)(0.026*SCREEN_WIDTH));//50

  if ((font == NULL) || (bigfont == NULL)) {
    printf("error: ttf file not found\n");
    return false;
  }

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

  SDL_FreeSurface(screen);
  SDL_FreeSurface(message);
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
  
  SDL_Quit();
  
}


bool draw_sidepanels() {

  Uint32 black_clr = SDL_MapRGB(screen->format, 0,0,0);


  SDL_FillRect(screen, &leader_rect, black_clr);
  SDL_FillRect(screen, &hiscore_rect, black_clr);
  SDL_FillRect(screen, &banner_rect, black_clr);

  message = TTF_RenderText_Solid(bigfont, "Leader Board", redcolor);
  apply_surface(txtoffset, txtoffset, message, screen, NULL);

  message = TTF_RenderText_Solid(bigfont, "Hi-Scores", redcolor);
  apply_surface(HI_X_OFFSET+4*txtoffset, txtoffset, message, screen, NULL);


  message = TTF_RenderText_Solid(bigfont, "TO JOIN GAME:", redcolor);
  apply_surface(LEADER_WIDTH+(int)(txtoffset/2), txtoffset, message, screen, NULL);

  message = TTF_RenderText_Solid(regfont, "TADPOLE_WIFI , THEN HTTP://TADPOLE", redcolor);
  apply_surface(LEADER_WIDTH+23*txtoffset, (int)(0.75*txtoffset), message, screen, NULL);


  message = TTF_RenderText_Solid(regfont, "APIs AVAILABLE: HTTP://TADPOLE/API", redcolor);
  apply_surface(LEADER_WIDTH+23*txtoffset, (int)(3*0.75*txtoffset), message, screen, NULL);
  
  return true;
  
}

// include this late. Requires definitions (I didn't plan this out well)
#include "entities.h"	        // Stores entity class definitions (Tadpole, frog, fly)

int main(int argc, char* argv[]) {


  bool running = true;
  int i, j, xfrog, yfrog, xfly, yfly, leech = 0, tadswim = 0, tadchange = 0, swavenum = 0, swave_clk = 0;
  int nfrogs = 10; // Number of frogs
  int sp = 3;    // Speed (1-3)
  int a_i = 3;  // AI (1-3)

  int bwavenum = 0, bwave_clk, mm_clk = 0, suwavenum = 0, suwave_clk = 0;  // various wave clocks and wave counters (storing latest wave to be animated)

  int pause = 0, nherd[10], nflychasers = 0; // nherd[i] counts number of close frogs to ith frog in jumpstate 0.
  int tadflip = 0, mmtadx = SCREEN_WIDTH/2, mmtady = SCREEN_HEIGHT/2+BANNER_HEIGHT/2;

  float ran; // random number storing
  char caption[32];



  set_frogclips();
  set_tadclips();
  set_flyclips();
  set_swaveclips();
  set_bwaveclips();
  set_suwaveclips();
  
  Timer fps;  
  Timer lifespan;
  Timer flyspan;
  std::vector<Timer> warpspan(nfrogs);
  
//    Setting wave flags to 'not animate / done animation'

  for(j=0; j<10; j++) {
    nherd[j] = 0;
    smallwaves[j][0] = 6;
  }

  for(j=0; j<20; j++) {
    bigwaves[j][0] = 24;
    suwaves[j][0] = 24;
  }
  
  if (init() == false) return 1;

  if (load_files() == false) return 1;

  // Spawn Tadpole
  Tadpole myTad(mmtadx, mmtady);

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


  myTad.show();

  for(i=0; i<nfrogs; i++)
    myfrogs[i].show(sp);

  myfly.show();


  if(SDL_Flip(screen) == -1) return 1;

  SDL_Delay(500);

  lifespan.start();
  
  

#ifdef WITH_SOUND
//    if(Mix_PlayMusic(ambience, -1) == -1)
    if(Mix_FadeInMusic(ambience, -1, 2000) == -1)
	return 1;
#endif
  

  // Game Server loop
  while (running) {

    fps.start();
    
    while(SDL_PollEvent ( &event )) {

      if(event.type == SDL_KEYDOWN) {
	if(event.key.keysym.sym == SDLK_ESCAPE) {
	  running = false;
	}
      }

      myTad.handle_input();
      
      if(event.type == SDL_QUIT) running = false;
      
    }

    // ******************** STUFF ********************* //

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
      if(a_i == 3) {
	if(nherd[i] > 1) {
	  if(nflychasers >= 2) {
	    myfrogs[i].handle_events(myTad.c.x+100*(drand48()-1), myTad.c.y+100*(drand48()-1), myTad.xVel, myTad.yVel, myfly.c.x, myfly.c.y, 0, sp);
	  } else{
	    myfrogs[i].handle_events(myTad.c.x+100*(drand48()-1), myTad.c.y+100*(drand48()-1), myTad.xVel, myTad.yVel, myfly.c.x, myfly.c.y, myfly.spawn, sp);
	  }
	} else {
	  if(nflychasers >= 2) {
	    myfrogs[i].handle_events(myTad.c.x, myTad.c.y, myTad.xVel, myTad.yVel, myfly.c.x, myfly.c.y, 0, sp);
	  } else {
	    myfrogs[i].handle_events(myTad.c.x, myTad.c.y, myTad.xVel, myTad.yVel, myfly.c.x, myfly.c.y, myfly.spawn, sp);
	  }
	}
      } else {
	myfrogs[i].handle_events(myTad.c.x, myTad.c.y, myTad.xVel, myTad.yVel, myfly.c.x, myfly.c.y, myfly.spawn, sp);
      }
    }


    if(myfly.spawn == 1) {
      if( myfly.handle_collision(myTad.c.x, myTad.c.y) ) {
	if(myTad.hpoints <= 18)
	  myTad.hpoints += 2;
	else if(myTad.hpoints == 19)
	  myTad.hpoints += 1;
	flyspan.start();
	suwaves[suwavenum][0] = -1;
	suwaves[suwavenum][1] = myfly.c.x - 150;
	suwaves[suwavenum][2] = myfly.c.y - 150;
	if(++suwavenum == 20) suwavenum = 0;
#ifdef WITH_SOUND
	Mix_PlayChannel(-1, slurp, 0);
#endif
      }
      for(i=0; i<nfrogs; i++) {
	if( myfly.handle_collision(myfrogs[i].c.x, myfrogs[i].c.y) ) { 
	  myfrogs[i].speed = sp + 2;
	  warpspan[i].start();
	  flyspan.start();
	  suwaves[suwavenum][0] = -1;
	  suwaves[suwavenum][1] = myfly.c.x - 150;
	  suwaves[suwavenum][2] = myfly.c.y - 150;
	  if(++suwavenum == 20) suwavenum = 0;
#ifdef WITH_SOUND
	  Mix_PlayChannel(-1, slurp, 0);
#endif
	  break;
	}
      }
    }



	// ********************************* LOGIC *************************************** //


    if( (lifespan.get_ticks() - tadswim) >= 50 ) {
      tadswim = lifespan.get_ticks();
      myTad.move(abs(myTad.xflag)+abs(myTad.yflag));
    } else {
      myTad.move(0);
    }


    for(i=0; i<nfrogs; i++) {
      if(warpspan[i].get_ticks() >= 5000 ) {
	warpspan[i].stop();
	myfrogs[i].speed = sp;
      }
      myfrogs[i].move();
    }


    for(i=0; i<nfrogs; i++) {
      if(distance(myTad.c.x, myTad.c.y, myfrogs[i].c.x, myfrogs[i].c.y) <= myTad.c.r + myfrogs[i].c.r) {
	suwaves[suwavenum][0] = -1;
	suwaves[suwavenum][1] = myTad.c.x - 150;
	suwaves[suwavenum][2] = myTad.c.y - 150;
	if(++suwavenum == 20) suwavenum = 0;
#ifdef WITH_SOUND
	Mix_PlayChannel(-1, damage, 0);
#endif
	myTad.hpoints--;
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

    if(myTad.hpoints <= 10) { leech = lifespan.get_ticks();}

    if( (lifespan.get_ticks() - leech) >= 5000 ) {
      leech = lifespan.get_ticks();
      if(myTad.hpoints > 10)
	myTad.hpoints--;
    }

    for(i=0; i<nfrogs; i++) {
      if(myfrogs[i].jumpstate == 1) {
	bigwaves[bwavenum][0] = -1;
	bigwaves[bwavenum][1] = myfrogs[i].c.x - 75;
	bigwaves[bwavenum][2] = myfrogs[i].c.y - 75;
	if(++bwavenum == 20) bwavenum = 0;
      }
    }

    if(tadchange == 1) {
      bigwaves[bwavenum][0] = -1;
      bigwaves[bwavenum][1] = myTad.c.x - 75;
      bigwaves[bwavenum][2] = myTad.c.y - 75;
      if(++bwavenum == 20) bwavenum = 0;
      tadchange = 0;
    }

    if(lifespan.get_ticks() - bwave_clk >= 36) {
      bwave_clk = lifespan.get_ticks();
      for(j=0; j<24; j++) {
	if(bigwaves[j][0] < 24) bigwaves[j][0]++;
      }
    }

    if(lifespan.get_ticks() - suwave_clk >= 36)	{
      suwave_clk = lifespan.get_ticks();
      for(j=0; j<24; j++)
	{
	  if(suwaves[j][0] < 24) suwaves[j][0]++;
	}
    }

    if(abs(myTad.xflag)+abs(myTad.yflag) == 0) {
      smallwaves[swavenum][0] = 6;
//	    swave_clk[19] = lifespan.get_ticks();
    }
//	else
    if(lifespan.get_ticks() - swave_clk >= 75) {
      swave_clk = lifespan.get_ticks();
      if(++swavenum > 9) swavenum = 0;
      smallwaves[swavenum][0] = -1;
      smallwaves[swavenum][1] = myTad.c.x - myTad.c.r - 10;
      smallwaves[swavenum][2] = myTad.c.y - myTad.c.r - 10;
      for(j=0; j<10; j++)
	if(smallwaves[j][0] < 6) smallwaves[j][0]++;
    }

    
    // ************** DRAW STUFF ***********************//
    // Make screen white
  SDL_FillRect( screen, &screen->clip_rect, SDL_MapRGB(screen->format, 0xFF,0xFF,0xFF) );
    

    for(j=0; j<10; j++){
      if(smallwaves[j][0] != 6){
	apply_surface(smallwaves[j][1], smallwaves[j][2], waves_small, screen, &swaveclips[smallwaves[j][0]]);
      }
    }

    for(j=0; j<20; j++){
      if(bigwaves[j][0] != 24){
	apply_surface(bigwaves[j][1], bigwaves[j][2], waves_big, screen, &bwaveclips[bigwaves[j][0]]);
      }
      if(suwaves[j][0] != 24){
	apply_surface(suwaves[j][1], suwaves[j][2], waves_super, screen, &suwaveclips[suwaves[j][0]]);
      }
    }



    myTad.show();
    
    for(i=0; i<nfrogs; i++)
      myfrogs[i].show(sp);

    if(myfly.spawn == 1)
      myfly.show();


    // Draw the leader board stuff
    if(!draw_sidepanels()) running = false;
    
    if(SDL_Flip(screen) == -1) return false;
    sprintf(caption,"Hit Points = %d", myTad.hpoints);
    SDL_WM_SetCaption(caption, NULL);

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

  
  clean_up();
  return 0;

}
