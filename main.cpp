//#define WITH_SOUND

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

#include "mysdl_images.h"
#include "mysdl_timer.h"
#include "tadclips.h"				// Storing sprite frames

int SCREEN_WIDTH = 1920;//800;
int SCREEN_HEIGHT = 1050;//600;
int LEADER_WIDTH = 435;
int HISCORE_WIDTH = 435;
int HI_X_OFFSET = 1485;
int BANNER_HEIGHT = 75;
int SCREEN_BPP = 32;				// Bits per pixel

const int FRAMES_PER_SECOND = 30;

SDL_Surface *screen;
SDL_Surface *message;

SDL_Event event;

TTF_Font *font = NULL;
TTF_Font *bigfont = NULL;

SDL_Color redcolor = {0xFF, 0, 0};

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

  font = TTF_OpenFont( "fonts/CARLETON.TTF", 15);
  bigfont = TTF_OpenFont( "fonts/STEVE.TTF", 50);

  return true;
  
}

void clean_up() {

  SDL_FreeSurface(screen);
  SDL_FreeSurface(message);  

#ifdef WITH_SOUND
  Mix_CloseAudio();
#endif


  TTF_CloseFont(font);
  TTF_CloseFont(bigfont);
  TTF_Quit();
  
  SDL_Quit();
  
}


bool draw_screen() {

  Uint32 black_clr = SDL_MapRGB(screen->format, 0x00,0x00,0x00);
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
  apply_surface(LEADER_WIDTH+20, 20, message, screen, NULL);

  message = TTF_RenderText_Solid(font, "CONNECT TO TADPOLE_WIFI , THEN VISIT:    HTTP://TADPOLE", redcolor);
  apply_surface(LEADER_WIDTH+500, 20, message, screen, NULL);


  message = TTF_RenderText_Solid(font, "C++ AND PYTHON APIs AVAILABLE:    HTTP://TADPOLE/API", redcolor);
  apply_surface(LEADER_WIDTH+500, 50, message, screen, NULL);
  
  if(SDL_Flip(screen) == -1) return false;
  return true;
  
}


int main(int argc, char* argv[]) {

  Timer fps;
  bool running = true;
 
  
  if (init() == false) return 1;

  if (load_files() == false) return 1;
  
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

  fps.stop();
  
  clean_up();
  return 0;

}
