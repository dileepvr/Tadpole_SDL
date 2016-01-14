
SDL_Surface *load_image(char* filename)
{
    
    // Temporary storage for the image that's loaded
    SDL_Surface* loadedImage = NULL;
    
    // The optimized image that will be used
    SDL_Surface *optimizedImage = NULL;

    // Load the image
    loadedImage = IMG_Load(filename);

    // If loading was successful
    if (loadedImage != NULL)
    {

  	// Create an optimized image
 	optimizedImage = SDL_DisplayFormatAlpha(loadedImage);

   	SDL_FreeSurface(loadedImage);	

/*	if(optimizedImage != NULL)
	{
	    
	}*/
    }
    return optimizedImage;

}


void apply_surface(int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip)
{
    SDL_Rect offset;
    offset.x = x;
    offset.y = y;

    SDL_BlitSurface( source, clip, destination, &offset);
}


