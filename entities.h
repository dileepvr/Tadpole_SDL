// ****************************** Physics ************************************** //


struct Circle
{
    int x, y;
    int r;
};

double distance( int x1, int y1, int x2, int y2 )
{
    // Return the distance between the two points
    return sqrt( pow( x2 - x1, 2 ) + pow( y2 - y1, 2 ) );
}

bool check_collision1( Circle &A, Circle &B )
{
    // If the distance between the centers of the circles is less than the sum of their radii
    if( distance(A.x, A.y, B.x, B.y) < (A.r + B.r) )
    	return true;

    // If the circles haven't collided
    return false;
}

bool check_collision2( Circle &A, std::vector<SDL_Rect> &B )
{
    // Closest point on collision box
    int cX, cY;

    //Go through the B boxes
    for( int Bbox = 0; Bbox<B.size(); Bbox++ )
    {
	// Find closest x offset
	if ( A.x < B[ Bbox ].x )
	{
	    cX = B[ Bbox ].x;
	}
	else if( A.x > B[ Bbox ].x + B[ Bbox ].w )
	{ cX = B[ Bbox ].x + B[ Bbox ].w; }
	else
	{ cX = A.x; }
	// Find the closest y offset
	if( A.y < B[ Bbox ].y )
	{ cY = B[ Bbox ].y; }
	else if( A.y > B[ Bbox ].y + B[ Bbox ].h )
	{ cY = B[ Bbox ].y + B[ Bbox ].h; }
	else
	{ cY = A.y; }
	// If the closest point is inside the circle
	if ( distance ( A.x, A.y, cX, cY ) < A.r )
	    return true;
    }
    
    // If the shapes have not collided
    return false;
}



// *************************** Tadpole definition ********************************** //

class Tadpole
{
    private:

    // Sprite Markers
    int angl, ud, tadswim;

    public:

    // Health of the Tadpole
    int hpoints;

    // The are of the Tadpole
    Circle c;

    // The velocity of the Tadpole
    int xVel, yVel;
 
    // The acceleration flags
    int xflag, yflag;

    // Initializes the variables
    Tadpole(int X, int Y);

    // Tkes key presses and adjusts the Tadpole's velocity
    void handle_input();

    // Moves the Tadpole
    void move(int swim);

    // Shows the Tadpole on the screen
    void show();
};


Tadpole::Tadpole(int X, int Y)
{
    // Initialize sprite markers
    angl = 4;
    ud = 0;
    tadswim = 0;

    // Make alive
    hpoints = 12;

    // Initialize the offsets
    c.x = X;
    c.y = Y;
    c.r = 10;

    // Initialize the velocity
    xVel = 0;
    yVel = 0;

    // Initialize acceleration flags
    xflag = 0;
    yflag = 0;
}

void Tadpole::handle_input()
{
    // If a key was pressed
    if(event.type == SDL_KEYDOWN)
    {

      // Adjust the velocity
	switch(event.key.keysym.sym)
	{
	    case SDLK_UP: 
	    case SDLK_w: yflag -= 1; break;
	    case SDLK_DOWN: 
	    case SDLK_s: yflag += 1; break;
	    case SDLK_LEFT: 
	    case SDLK_a: xflag -= 1; break;
	    case SDLK_RIGHT:
	    case SDLK_d: xflag += 1; break;
	}
    }
    
    // If a key was released
    else if(event.type == SDL_KEYUP)
    {
	// Adjust the velocity
 	switch(event.key.keysym.sym)
	{
	    case SDLK_UP:
	    case SDLK_w: yflag += 1; break;
	    case SDLK_DOWN:
	    case SDLK_s: yflag -= 1; break;
	    case SDLK_LEFT:
	    case SDLK_a: xflag += 1; break;
	    case SDLK_RIGHT:
	    case SDLK_d: xflag -= 1; break;
	}
    }

//    if(keystate[SDLK_LEFT]) printf("left pressed.\n");
/*    if(keystate[SDLK_LEFT]) xflag = -1;
    if(keystate[SDLK_RIGHT]) xflag = 1;
    if(keystate[SDLK_LEFT] == keystate[SDLK_RIGHT]) xflag = 0; 
    if(keystate[SDLK_UP]) yflag = -1;
    if(keystate[SDLK_DOWN]) yflag = 1;
    if(keystate[SDLK_UP] == keystate[SDLK_DOWN]) yflag = 0; 
*/
}

void Tadpole::move(int swim)
{

//    float tangent;
    if(xflag != 0 && abs(xVel) < DOT_WIDTH / 2)
    {
	xVel = xVel + xflag;
    }

    if(xflag == 0 && xVel != 0)
	if(abs(xVel) == 1) xVel = 0;
    //    	else xVel -= ceil(xVel/2);
    	else xVel -= (int)(xVel/2);


    // Move the dot left or right
    c.x += xVel;

    // If the dor went too far to the left or right
    if( (c.x - c.r < LEADER_WIDTH) || (c.x + c.r > SCREEN_WIDTH - HISCORE_WIDTH ) )
    {
	c.x -= xVel;
	xVel = 0;
    }


    if(yflag != 0 && abs(yVel) < DOT_HEIGHT / 2)
    {
	yVel = yVel + yflag;
    }

    if(yflag == 0 && yVel != 0)
	if(abs(yVel) == 1) yVel = 0; 
	else yVel -= (int)(yVel/2);

    // Move the dot up or down
    c.y += yVel;

    // If the dot went too far up or down
    if( (c.y - c.r < BANNER_HEIGHT) || (c.y + c.r > SCREEN_HEIGHT) )
    {
	c.y -= yVel;
	yVel = 0;
    }

/*    if(xVel != 0 || yVel != 0 )
    {
	if(yVel > 0)
	    ud = 2;
	else
	    ud = 0;
	tangent = abs(1.0*xVel/(1.0*yVel + 0.01));
	if(tangent <= 0.1989)
	{
	    if(xVel > 0)
		angl = 8;
	    else
		angl = 0;
	}
	else if(tangent <= 0.6682)
	{
	    if(xVel > 0)
		angl = 7;
	    else
		angl = 1;
	}
	else if(tangent <= 1.4966)
	{
	    if(xVel > 0)
		angl = 6;
	    else
		angl = 2;
	}
	else if(tangent <= 5.0273)
	{
	    if(xVel > 0)
		angl = 5;
	    else
		angl = 3;
	}
	else
	{ 
	    angl = 4;
	}
    }
*/


	if(xflag < 0)
	{
	    if(yflag > 0)
	    {
		angl = 2;
		ud = 2;
	    }
	    else if(yflag < 0)
	    {
		angl = 2;
		ud = 0;
	    }
	    else
	    {
		angl = 0;
	    }
	}
	else if(xflag > 0)
	{
	    if(yflag > 0)
	    {
		angl = 6;
		ud = 2;
	    }
	    else if(yflag < 0)
	    {
		angl = 6;
		ud = 0;
	    }
	    else
	    {
		angl = 8;
	    }
	}
	else
	{
	    if(yflag > 0)
		{ud = 2; angl = 4;}
	    else if(yflag < 0)
		{ud = 0; angl = 4;}
	}




    if( swim != 0 )
    {
	switch(ud)
	{
	    case 0: ud = 1; break;
	    case 1: ud = 0; break;
	    case 2: ud = 3; break;
	    case 3: ud = 2; break;
	}
    }

}

void Tadpole::show()
{
    // Show the Tadpole
    apply_surface( c.x - c.r, c.y - c.r, player1, screen, &tadclip[angl][ud] );
}



// ******************************* Frog definition ********************************** //

class Frog
{
    private:

    // Sprite Markers
    int angl, ud;

    // Boundary wall flag   (clockwise from left -> 0, 1, 2, 3
    int bflag, xsign, xoffset;

    public:

    // Health of the Frog
    int hpoints;

    // Chase flag (0 == chase player, 1 == chase fly)
    int chase_flag;

    // Jump speed of the Frog
    int speed;

    // Intelligence of the Frog
    int AI;

    // The are of the Frog
    Circle c;

    // The velocity of the Frog
    int xVel, yVel;
 
    // The acceleration flags
    int xflag, yflag;

    // Distance to Tadpole
    int dist;

    // Jump state flag
    int jumpstate;

    // Jump timer
    Timer jumpdelay;

    // Initializes the variables
    void Frog_set(int X, int Y, int sp, int a_i);

    void handle_events(int X, int Y, int Xv, int Yv, int Xf, int Yf, int fspawn, int sp);

    // Moves the Frog
    void move();

    // Shows the Frog on the screen
    void show(int sp);
};


void Frog::Frog_set(int X, int Y, int sp, int a_i)
{
    // Initialize sprite markers
    angl = 4;
    ud = 0;

    bflag = 4;
    xsign = 1;
    xoffset = 1;

    // Make alive
    hpoints = 1;

    // chase flag initialised
    chase_flag = 0;

    // Set speed
    speed = sp;

    // Set intelligence
    AI = a_i;

    // Initialize the offsets
    c.x = X;
    c.y = Y;
    c.r = 10;

    // Initialize the velocity
    xVel = 0;
    yVel = 0;

    // Initialize acceleration flags
    xflag = 0;
    yflag = 0;

    dist = 0;

    // Set Jump state flag to 1
    jumpstate = 1;
}


void Frog::handle_events(int X, int Y, int Xv, int Yv, int Xf, int Yf, int fspawn, int sp)
{
    if(jumpstate == 1 )
    {
	jumpstate = 2;
	xflag = 0;
	yflag = 0;
	jumpdelay.start();

#ifdef WITH_SOUND

	if(speed > sp)
 	    Mix_PlayChannel(-1, warp, 0);
	else
	{
	switch((int)(drand48()*1))
	{
	    case 0: Mix_PlayChannel(-1, launch1, 0); break; 
	    case 1: Mix_PlayChannel(-1, launch2, 0); break;
	}
	}

#endif

    }
    else if((jumpstate == 2) && (jumpdelay.get_ticks() >= drand48()*(1000/speed)) )
    {
	jumpdelay.stop();
	jumpstate = 0;
	dist = distance(X, Y, c.x, c.y);
	if(AI > 1)
	{
	    X += Xv*dist*2/speed/DOT_WIDTH;
	    Y += Yv*dist*2/speed/DOT_HEIGHT;
	    if(X < LEADER_WIDTH) X = c.r + LEADER_WIDTH;
	    if(X > SCREEN_WIDTH - HISCORE_WIDTH) X = SCREEN_WIDTH - HISCORE_WIDTH - c.r;
	    if(Y < BANNER_HEIGHT) Y = c.r + BANNER_HEIGHT;
	    if(Y > SCREEN_HEIGHT) Y = SCREEN_HEIGHT - c.r;
	    dist = distance(X, Y, c.x, c.y);
	    if(fspawn == 1 && dist > distance(Xf, Yf, c.x, c.y))
	    {
		X = Xf;
		Y = Yf;
		dist = distance(Xf, Yf, c.x, c.y);
		chase_flag = 1;
	    } else { chase_flag = 0;}
	}
	xflag = (X - c.x);
	yflag = (Y - c.y);
    }
}


void Frog::move()
{

    if(xflag != 0 && abs(xVel) < abs(xflag) * speed * DOT_WIDTH / 2 / dist )
    {
	xVel = xVel + xflag/abs(xflag);
    }

    if(xflag == 0 && xVel != 0)
	xVel = 0; 


    // Move the frog left or right
    c.x += xVel;

    // If the frog went too far to the left or right
    if( c.x - c.r < LEADER_WIDTH )
    {
	c.x = c.r + 1 + LEADER_WIDTH;
	xVel = 0;
	jumpstate = 1;
	bflag = 0;
#ifdef WITH_SOUND
	switch((int)(drand48()*2))
	{
	    case 0: Mix_PlayChannel(-1, splat1, 0); break; 
	    case 1: Mix_PlayChannel(-1, splat2, 0); break;
	    case 2: Mix_PlayChannel(-1, splat3, 0); break;
	}
#endif
    }

    if( c.x + c.r > SCREEN_WIDTH - HISCORE_WIDTH )
    {
	c.x = SCREEN_WIDTH - c.r - 1 - HISCORE_WIDTH;
	xVel = 0;
	jumpstate = 1;
	bflag = 2;
#ifdef WITH_SOUND
	switch((int)(drand48()*2))
	{
	    case 0: Mix_PlayChannel(-1, splat1, 0); break; 
	    case 1: Mix_PlayChannel(-1, splat2, 0); break;
	    case 2: Mix_PlayChannel(-1, splat3, 0); break;
	}
#endif
    }
   


    if(yflag != 0 && abs(yVel) < abs(yflag) * speed * DOT_HEIGHT / 2 / dist )
    {
	yVel = yVel + yflag/abs(yflag);
    }

    if(yflag == 0 && yVel != 0)
	yVel = 0; 


    // Move the dot up or down
    c.y += yVel;

    // If the dot went too far up or down
    if(c.y - c.r < BANNER_HEIGHT)
    {
	c.y = c.r + 1 + BANNER_HEIGHT;
	yVel = 0;
	jumpstate = 1;
	bflag = 1;
#ifdef WITH_SOUND
	switch((int)(drand48()*2))
	{
	    case 0: Mix_PlayChannel(-1, splat1, 0); break; 
	    case 1: Mix_PlayChannel(-1, splat2, 0); break;
	    case 2: Mix_PlayChannel(-1, splat3, 0); break;
	}
#endif
    }

    if(c.y + c.r > SCREEN_HEIGHT)
    {
	c.y = SCREEN_HEIGHT - c.r - 1;
	yVel = 0;
	jumpstate = 1;
	bflag = 3;
#ifdef WITH_SOUND
	switch((int)(drand48()*2))
	{
	    case 0: Mix_PlayChannel(-1, splat1, 0); break; 
	    case 1: Mix_PlayChannel(-1, splat2, 0); break;
	    case 2: Mix_PlayChannel(-1, splat3, 0); break;
	}
#endif
    }

}

void Frog::show(int sp)
{

    ud = 0;
    if(speed > sp)
	ud = 2;

    if(yVel > 0)
	ud++;


    if(xVel >= 0) xsign = 1;
    else xsign = -1;     

    if(jumpstate == 0)
    {
    	if(xVel == 0)
    	{
	    xoffset = 0;
    	} else if(abs(1.0*xVel) < abs(0.12*yVel))
    	{
	    xoffset = 1;
    	} else if(abs(1.0*xVel) < abs(0.67*yVel))
    	{
	    xoffset = 2;
    	} else if(abs(1.0*xVel) < abs(1.50*yVel))
    	{
	    xoffset = 3;	
    	} else if(abs(1.0*xVel) < abs(45.83*yVel))
    	{
	    xoffset = 4;
    	} else
   	{
	    xoffset = 7;
    	}
    } else
    {
	switch(bflag)
	{
	    case 0: xoffset = 5; break;
	    case 1: xoffset = 6; ud++; break;
	    case 2: xoffset = -5; break;
	    case 3: xoffset = 6; break;
	}
    }

    angl = 7 + xsign*xoffset;    

    // Show the frog
    
    apply_surface( c.x - c.r, c.y - c.r, frog, screen, &frogclips[angl][ud] );
}


// ******************************* Fly definition ********************************** //

class Fly
{
    private:

    // Sprite tracker
    int frame;

    public:

    // Spawn flag of the Fly
    int spawn;

    // The are of the Fly
    Circle c;

    // Initializes the variables
    Fly(int X, int Y);

    bool handle_collision(int X, int Y);

//    bool handle_spawn();

    // Shows the Fly on the screen
    void show();
};

Fly::Fly(int X, int Y)
{
    // Initiate sprite frame
    frame = 0;

    // Make alive
    spawn = 1;

    // Initialize the offsets
    c.x = X;
    c.y = Y;
    c.r = 10;
}

bool Fly::handle_collision(int X, int Y)
{
    if(distance(X, Y, c.x, c.y) < 1.5*c.r)
    {
	spawn = 0;
	return true;
    }
    return false;
}

void Fly::show()
{
    frame++;
    if(frame == 4) frame = 0;
    apply_surface( c.x - c.r, c.y - c.r, fly, screen, &flyclips[frame] );
}
