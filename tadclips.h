SDL_Rect suwaveclips[24];

void set_suwaveclips()
{
    suwaveclips[0].x = 0;
    suwaveclips[0].y = 0;
    suwaveclips[0].w = 300;
    suwaveclips[0].h = 300;

    for(int i = 1; i < 24; i++)
    {
	suwaveclips[i].x = 0;
	suwaveclips[i].y = suwaveclips[i-1].y + 300;
	suwaveclips[i].w = 300;
	suwaveclips[i].h = 300;	
    }
}

SDL_Rect bwaveclips[24];

void set_bwaveclips()
{
    bwaveclips[0].x = 0;
    bwaveclips[0].y = 0;
    bwaveclips[0].w = 150;
    bwaveclips[0].h = 150;

    for(int i = 1; i < 24; i++)
    {
	bwaveclips[i].x = 0;
	bwaveclips[i].y = bwaveclips[i-1].y + 150;
	bwaveclips[i].w = 150;
	bwaveclips[i].h = 150;	
    }
}

SDL_Rect swaveclips[6];

void set_swaveclips()
{
    swaveclips[0].x = 0;
    swaveclips[0].y = 0;
    swaveclips[0].w = 40;
    swaveclips[0].h = 40;

    swaveclips[1].x = 0;
    swaveclips[1].y = 40;
    swaveclips[1].w = 40;
    swaveclips[1].h = 40;

    swaveclips[2].x = 0;
    swaveclips[2].y = 80;
    swaveclips[2].w = 40;
    swaveclips[2].h = 40;

    swaveclips[3].x = 0;
    swaveclips[3].y = 120;
    swaveclips[3].w = 40;
    swaveclips[3].h = 40;

    swaveclips[4].x = 0;
    swaveclips[4].y = 160;
    swaveclips[4].w = 40;
    swaveclips[4].h = 40;

    swaveclips[5].x = 0;
    swaveclips[5].y = 200;
    swaveclips[5].w = 40;
    swaveclips[5].h = 40;
}

SDL_Rect flyclips[4];

void set_flyclips()
{
    flyclips[0].x = 0;
    flyclips[0].y = 0;
    flyclips[0].w = 20;
    flyclips[0].h = 20;

    flyclips[1].x = 0;
    flyclips[1].y = 20;
    flyclips[1].w = 20;
    flyclips[1].h = 20;

    flyclips[2].x = 0;
    flyclips[2].y = 40;
    flyclips[2].w = 20;
    flyclips[2].h = 20;

    flyclips[3].x = 0;
    flyclips[3].y = 60;
    flyclips[3].w = 20;
    flyclips[3].h = 20;
}

SDL_Rect frogclips[20][4];

void set_frogclips()
{
    int i, j;

    frogclips[0][0].x = 11;
    frogclips[0][0].y = 25;
    frogclips[0][0].w = 38;
    frogclips[0][0].h = 20;

    frogclips[0][1].x = 11;
    frogclips[0][1].y = 25;
    frogclips[0][1].w = 38;
    frogclips[0][1].h = 20;

    frogclips[1][0].x = 53;
    frogclips[1][0].y = 0;
    frogclips[1][0].w = 30;
    frogclips[1][0].h = 25;

    frogclips[1][1].x = 53;
    frogclips[1][1].y = 55;
    frogclips[1][1].w = 30;
    frogclips[1][1].h = 25;

    frogclips[2][0].x = 90;
    frogclips[2][0].y = 0;
    frogclips[2][0].w = 30;
    frogclips[2][0].h = 28;

    frogclips[2][1].x = 90;
    frogclips[2][1].y = 0;
    frogclips[2][1].w = 30;
    frogclips[2][1].h = 28;

    frogclips[3][0].x = 125;
    frogclips[3][0].y = 0;
    frogclips[3][0].w = 40;
    frogclips[3][0].h = 25;

    frogclips[3][1].x = 125;
    frogclips[3][1].y = 55;
    frogclips[3][1].w = 40;
    frogclips[3][1].h = 25;

    frogclips[4][0].x = 177;
    frogclips[4][0].y = 0;
    frogclips[4][0].w = 40;
    frogclips[4][0].h = 28;

    frogclips[4][1].x = 177;
    frogclips[4][1].y = 53;
    frogclips[4][1].w = 40;
    frogclips[4][1].h = 28;

    frogclips[5][0].x = 226;
    frogclips[5][0].y = 0;
    frogclips[5][0].w = 30;
    frogclips[5][0].h = 28;

    frogclips[5][1].x = 226;
    frogclips[5][1].y = 52;
    frogclips[5][1].w = 30;
    frogclips[5][1].h = 28;

    frogclips[6][0].x = 273;
    frogclips[6][0].y = 0;
    frogclips[6][0].w = 25;
    frogclips[6][0].h = 28;

    frogclips[6][1].x = 273;
    frogclips[6][1].y = 51;
    frogclips[6][1].w = 30;
    frogclips[6][1].h = 28;

    frogclips[7][0].x = 314;
    frogclips[7][0].y = 0;
    frogclips[7][0].w = 20;
    frogclips[7][0].h = 28;

    frogclips[7][1].x = 314;
    frogclips[7][1].y = 50;
    frogclips[7][1].w = 20;
    frogclips[7][1].h = 28;

    frogclips[8][0].x = 352;
    frogclips[8][0].y = 0;
    frogclips[8][0].w = 25;
    frogclips[8][0].h = 28;

    frogclips[8][1].x = 352;
    frogclips[8][1].y = 52;
    frogclips[8][1].w = 25;
    frogclips[8][1].h = 28;

    frogclips[9][0].x = 392;
    frogclips[9][0].y = 0;
    frogclips[9][0].w = 30;
    frogclips[9][0].h = 28;

    frogclips[9][1].x = 392;
    frogclips[9][1].y = 52;
    frogclips[9][1].w = 30;
    frogclips[9][1].h = 28;

    frogclips[10][0].x = 438;
    frogclips[10][0].y = 0;
    frogclips[10][0].w = 40;
    frogclips[10][0].h = 28;

    frogclips[10][1].x = 438;
    frogclips[10][1].y = 53;
    frogclips[10][1].w = 40;
    frogclips[10][1].h = 28;

    frogclips[11][0].x = 495;
    frogclips[11][0].y = 0;
    frogclips[11][0].w = 40;
    frogclips[11][0].h = 25;

    frogclips[11][1].x = 495;
    frogclips[11][1].y = 55;
    frogclips[11][1].w = 40;
    frogclips[11][1].h = 25;

    frogclips[12][0].x = 537;
    frogclips[12][0].y = 0;
    frogclips[12][0].w = 30;
    frogclips[12][0].h = 28;

    frogclips[12][1].x = 537;
    frogclips[12][1].y = 0;
    frogclips[12][1].w = 30;
    frogclips[12][1].h = 28;

    frogclips[13][0].x = 570;
    frogclips[13][0].y = 1;
    frogclips[13][0].w = 30;
    frogclips[13][0].h = 25;

    frogclips[13][1].x = 570;
    frogclips[13][1].y = 55;
    frogclips[13][1].w = 30;
    frogclips[13][1].h = 25;

    frogclips[14][0].x = 612;
    frogclips[14][0].y = 25;
    frogclips[14][0].w = 38;
    frogclips[14][0].h = 20;

    frogclips[14][1].x = 612;
    frogclips[14][1].y = 25;
    frogclips[14][1].w = 38;
    frogclips[14][1].h = 20;

    for(i = 0; i < 15; i ++)
	for(j = 2; j < 4; j++)
	{
	    frogclips[i][j].x = frogclips[i][j-2].x;
	    frogclips[i][j].y = frogclips[i][j-2].y + 80;
	    frogclips[i][j].w = frogclips[i][j-2].w;
	    frogclips[i][j].h = frogclips[i][j-2].h;
	}

}



SDL_Rect tadclip[9][4];

void set_tadclips()
{
tadclip[0][0].x = -2;
tadclip[0][0].y = 3;
tadclip[0][0].w = 26;
tadclip[0][0].h = 20;

tadclip[1][0].x = 24;
tadclip[1][0].y = 3;
tadclip[1][0].w = 25;
tadclip[1][0].h = 20;

tadclip[2][0].x = 49;
tadclip[2][0].y = 3;
tadclip[2][0].w = 21;
tadclip[2][0].h = 20;

tadclip[3][0].x = 70;
tadclip[3][0].y = 1;
tadclip[3][0].w = 18;
tadclip[3][0].h = 24;

tadclip[4][0].x = 90;
tadclip[4][0].y = 0;
tadclip[4][0].w = 20;
tadclip[4][0].h = 25;

tadclip[5][0].x = 110;
tadclip[5][0].y = 1;
tadclip[5][0].w = 18;
tadclip[5][0].h = 24;

tadclip[6][0].x = 131;
tadclip[6][0].y = 3;
tadclip[6][0].w = 21;
tadclip[6][0].h = 20;

tadclip[7][0].x = 142;
tadclip[7][0].y = 3;
tadclip[7][0].w = 25;
tadclip[7][0].h = 20;

tadclip[8][0].x = 175;
tadclip[8][0].y = 3;
tadclip[8][0].w = 26;
tadclip[8][0].h = 20;


tadclip[0][1].x = -2;
tadclip[0][1].y = 28;
tadclip[0][1].w = 26;
tadclip[0][1].h = 20;

tadclip[1][1].x = 24;
tadclip[1][1].y = 28;
tadclip[1][1].w = 25;
tadclip[1][1].h = 20;

tadclip[2][1].x = 49;
tadclip[2][1].y = 28;
tadclip[2][1].w = 21;
tadclip[2][1].h = 20;

tadclip[3][1].x = 70;
tadclip[3][1].y = 26;
tadclip[3][1].w = 18;
tadclip[3][1].h = 24;

tadclip[4][1].x = 90;
tadclip[4][1].y = 25;
tadclip[4][1].w = 20;
tadclip[4][1].h = 25;

tadclip[5][1].x = 110;
tadclip[5][1].y = 26;
tadclip[5][1].w = 18;
tadclip[5][1].h = 24;

tadclip[6][1].x = 131;
tadclip[6][1].y = 28;
tadclip[6][1].w = 21;
tadclip[6][1].h = 20;

tadclip[7][1].x = 142;
tadclip[7][1].y = 28;
tadclip[7][1].w = 25;
tadclip[7][1].h = 20;

tadclip[8][1].x = 175;
tadclip[8][1].y = 28;
tadclip[8][1].w = 26;
tadclip[8][1].h = 20;


tadclip[0][2].x = -2;
tadclip[0][2].y = 53;
tadclip[0][2].w = 26;
tadclip[0][2].h = 20;

tadclip[1][2].x = 24;
tadclip[1][2].y = 53;
tadclip[1][2].w = 25;
tadclip[1][2].h = 20;

tadclip[2][2].x = 49;
tadclip[2][2].y = 53;
tadclip[2][2].w = 21;
tadclip[2][2].h = 20;

tadclip[3][2].x = 70;
tadclip[3][2].y = 51;
tadclip[3][2].w = 18;
tadclip[3][2].h = 24;

tadclip[4][2].x = 90;
tadclip[4][2].y = 50;
tadclip[4][2].w = 20;
tadclip[4][2].h = 25;

tadclip[5][2].x = 110;
tadclip[5][2].y = 51;
tadclip[5][2].w = 18;
tadclip[5][2].h = 24;

tadclip[6][2].x = 131;
tadclip[6][2].y = 53;
tadclip[6][2].w = 21;
tadclip[6][2].h = 20;

tadclip[7][2].x = 142;
tadclip[7][2].y = 53;
tadclip[7][2].w = 25;
tadclip[7][2].h = 20;

tadclip[8][2].x = 175;
tadclip[8][2].y = 53;
tadclip[8][2].w = 26;
tadclip[8][2].h = 20;


tadclip[0][3].x = -2;
tadclip[0][3].y = 78;
tadclip[0][3].w = 26;
tadclip[0][3].h = 20;

tadclip[1][3].x = 24;
tadclip[1][3].y = 78;
tadclip[1][3].w = 25;
tadclip[1][3].h = 20;

tadclip[2][3].x = 49;
tadclip[2][3].y = 78;
tadclip[2][3].w = 21;
tadclip[2][3].h = 20;

tadclip[3][3].x = 70;
tadclip[3][3].y = 76;
tadclip[3][3].w = 18;
tadclip[3][3].h = 24;

tadclip[4][3].x = 90;
tadclip[4][3].y = 75;
tadclip[4][3].w = 20;
tadclip[4][3].h = 25;

tadclip[5][3].x = 110;
tadclip[5][3].y = 76;
tadclip[5][3].w = 18;
tadclip[5][3].h = 24;

tadclip[6][3].x = 131;
tadclip[6][3].y = 78;
tadclip[6][3].w = 21;
tadclip[6][3].h = 20;

tadclip[7][3].x = 142;
tadclip[7][3].y = 78;
tadclip[7][3].w = 25;
tadclip[7][3].h = 20;

tadclip[8][3].x = 175;
tadclip[8][3].y = 78;
tadclip[8][3].w = 26;
tadclip[8][3].h = 20;
}
