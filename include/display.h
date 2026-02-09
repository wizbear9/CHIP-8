#include <SDL3/SDL.h>

class Display
{
public:
	const int W = 640;
	const int H = 480;

	Display();
	~Display();

	void render_frame(uint8_t*);

private:
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture* texture;
};