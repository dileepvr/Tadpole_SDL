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


int SCREEN_WIDTH = 1920;//800;
int SCREEN_HEIGHT = 1050;//600;
int LEADER_WIDTH = 435;
int HISCORE_WIDTH = 435;
int HI_X_OFFSET = 1485;
int BANNER_HEIGHT = 75;
int SCREEN_BPP = 32;		// Bits per pixel

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

  font = TTF_OpenFont( "fonts/CARLETON.TTF", 17);
  regfont = TTF_OpenFont( "fonts/CARLETON.TTF", 25);
  bigfont = TTF_OpenFont( "fonts/STEVE.TTF", 50);

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
  TTF_CloseFont(bigfont);
  TTF_Quit();
  
  SDL_Quit();
  
}


bool draw_screen() {

  Uint32 black_clr = SDL_MapRGB(screen->format, 0,0,0);
  Uint32 white_clr = SDL_MapRGB(screen->format, 0xFF,0xFF,0xFF);

    // Make screen white
  SDL_FillRect( screen, &screen->clip_rect, white_clr );


  SDL_FillRect(screen, &leader_rect, black_clr);
  SDL_FillRect(screen, &hiscore_rect, black_clr);
  SDL_FillRect(screen, &banner_rect, black_clr);

  message = TTF_RenderText_Solid(bigfont, "Leader Board", redcolor);
  apply_surface(20, 20, message, screen, NULL);

  message = TTF_RenderText_Solid(bigfont, "Hi-Scores", redcolor);
  apply_surface(HI_X_OFFSET+80, 20, message, screen, NULL);


  message = TTF_RenderText_Solid(bigfont, "TO JOIN GAME:", redcolor);
  apply_surface(LEADER_WIDTH+10, 20, message, screen, NULL);

  message = TTF_RenderText_Solid(regfont, "TADPOLE_WIFI , THEN HTTP://TADPOLE", redcolor);
  apply_surface(LEADER_WIDTH+460, 15, message, screen, NULL);


  message = TTF_RenderText_Solid(regfont, "APIs AVAILABLE: HTTP://TADPOLE/API", redcolor);
  apply_surface(LEADER_WIDTH+460, 45, message, screen, NULL);
  
  if(SDL_Flip(screen) == -1) return false;
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

  float ran; // random number storing

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
      if(event.type == SDL_QUIT) running = false;
      
    }



    if(!draw_screen()) running = false;
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
