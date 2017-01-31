#include <SDL.h>
#include <iostream>
using namespace std;

int posX = 100;
int posY = 200;
int sizeX = 600;
int sizeY = 400;

SDL_Window* window;
SDL_Renderer* renderer;

bool InitEverything(){
	if(!init_sdl()){
		return false;
	}
	if(!create_window()){
		return false;
	}
	if(!create_renderer()){
		return false;
	}
	set_up_renderer();
	return true;
}

bool init_sdl(){
	if(SDL_Init(SDL_INIT_EVERYTHING) == -1){
		cout << "SDL failed to initialize: " << SDL_GetError() << endl;
		return false;
	}
	return true;
}

bool create_window(){
	window = SDL_CreateWindow("KW Pong", posX, posY, sizeY, sizeX);
	if(window == nullptr){
		cout << "Could not create window:" << SDL_GetError()<< endl;
		return false;
	}
	return true;
}

bool create_renderer(){
	renderer = SDL_CreateRenderer(window, -1, 0);
	if(renderer == nullptr){
		cout<< "Could not create renderer: " << SDL_GetError() << endl;
		return false;
	}
	return true;
}
void set_up_renderer(){
	SDL_RenderSetLogicalSize(renderer, sizeX, sizeY);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

}

void render(){
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0);
	SDL_RenderFillRect(renderer, &pos_player1);
	SDL_RenderFillRect(renderer, &pos_player2);
	SDL_SetRenderDrawColor(renderer, 0, 0 , 0, 255);
}

void run_game(){
	bool loop = true;
	while(loop){
		SDL_Event event;
		while(SDL_PollEvent(&event)){
			if(event.type == SDL)
		}
	}
}

SDL_Rect pos_player1;
SDL_Rect pos_player2;

int main( int argc, char* args[]){
	pos_player1.x = 10
	pos_player1.y = 200
	pos_player1.w = 20
	pos_player1.h = 50

	pos_player2.x = 590
	pos_player2.y = 200
	pos_player2.w = 20
	pos_player2.h = 50

	run_game();
}