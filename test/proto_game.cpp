#include <SDL.h>
#include <iostream>
#include <stdlib.h>
#include <random>
#include <ctime>
#include <stdbool.h>

using namespace std;

bool init_everything();
bool init_sdl();
bool create_window();
bool create_renderer();
void set_up_renderer();

void render();
void run_game();

void print_scores();
bool keep_playing();

int posX = 100;
int posY = 200;
int sizeX = 600;
int sizeY = 400;

SDL_Rect pos_player1;
SDL_Rect pos_player2;

// Kevin.

SDL_Rect ballPos;

mt19937 rng;

uniform_real_distribution<double> x_dist(10, sizeX - 10);
uniform_real_distribution<double> y_dist(10, sizeY - 10);

int x_speed = 10;
int y_speed = 5;

bool started = false;

int max_score = 7;

int left_score = 0;
int right_score = 0;

int game_mode = -1;

void print_scores() {
	cout << "CURRENT SCORING" << endl;
	cout << "Player 1 (Left): " << left_score << " | Player 2 (Right): " << right_score << endl;
	cout << "----------" << endl << endl;
}

bool go = true;

bool keep_playing() {
	// Determine if we should keep going or end the game...
	if (left_score < max_score && right_score < max_score) {
		return true;
	} else if ((left_score == max_score && right_score == max_score - 1) || (right_score == max_score && left_score == max_score - 1)) {
		return true;
		// Win by 2.
	} else return false;
}

// Kevin ends here.

SDL_Window* window;
SDL_Renderer* renderer;

bool init_everything(){
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

bool init_sdl() {
	if(SDL_Init(SDL_INIT_EVERYTHING) == -1){
		cout << "SDL failed to initialize: " << SDL_GetError() << endl;
		return false;
	}
	return true;
}

bool create_window() {
	window = SDL_CreateWindow("KW Pong", posX, posY, sizeX, sizeY, 0);
	if(window == nullptr){
		cout << "Could not create window:" << SDL_GetError()<< endl;
		return false;
	}
	return true;
}

bool create_renderer() {
	renderer = SDL_CreateRenderer(window, -1, 0);
	if(renderer == nullptr){
		cout<< "Could not create renderer: " << SDL_GetError() << endl;
		return false;
	}
	return true;
}

void set_up_renderer() {
	SDL_RenderSetLogicalSize(renderer, sizeX, sizeY);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
}

void render() {
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer, 0, 0, 255, 0);
	SDL_RenderFillRect(renderer, &pos_player1);
	SDL_RenderFillRect(renderer, &pos_player2);
	SDL_RenderFillRect(renderer, & ballPos );
	SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255 );
	SDL_RenderPresent(renderer);
}

void run_game() {

	while ( keep_playing() && go ) // Change this condition.
	{
		SDL_Event event;

		while ( SDL_PollEvent( & event ) )
		{
			if ( event.type == SDL_QUIT )
				go = false;
			else if ( event.type == SDL_KEYDOWN )
			{
				switch ( event.key.keysym.sym )
				{
					case SDLK_s:
						started = true;
						break;
					case SDLK_RIGHT:
						//++playerPos.x;
						break;
					case SDLK_LEFT:
						//--playerPos.x;
						break;
						// Remeber 0,0 in SDL is left-top. So when the user pressus down, the y need to increase
					case SDLK_DOWN:
						//++playerPos.y;
						break;
					case SDLK_UP:
						//--playerPos.y;
						break;
					default:
						break;
				}
			}
		}

		if (started) {
			// Let's move the ball...
			ballPos.x += x_speed;
			ballPos.y += y_speed;
			if (ballPos.x <= 10 || ballPos.x >= sizeX - 10) {
				// Hit left wall.
				x_speed = -1 * x_speed;
			} else if (ballPos.y <= 10 || ballPos.y >= sizeY - 10) {
				// Hit left wall.
				y_speed = -1 * y_speed;
			}
		}

		render();

		SDL_Delay(16);
	}
}

int main(int argc, char* args[]){
	pos_player1.x = 10;
	pos_player1.y = 200;
	pos_player1.w = 20;
	pos_player1.h = 50;

	pos_player2.x = 590;
	pos_player2.y = 200;
	pos_player2.w = 20;
	pos_player2.h = 50;

	ballPos.x = int(x_dist(rng));
	ballPos.y = int(y_dist(rng));
	ballPos.w = 20;
	ballPos.h = 20;

	cout << "Welcome to KW Pong! Select your mode. Type 2 for multiplayer, or 1 for computer." << endl;

	cin >> game_mode;

	while (game_mode != 2 && game_mode != 1) {
		cout << "You must enter a valid game mode! Please try again...";
		cin >> game_mode;
	}

	switch (game_mode) {
		case 1:
			cout << "Now playing against the computer! Press S to begin. Good luck!" << endl;
			break;
		case 2:
			cout << "Player 1 (left) - use A for UP and Z for down. Player 2 (right) - use UP and DOWN keys." << endl;
			cout << "Press S to begin. Good luck!" << endl << endl;
			print_scores();
	}

	if (! init_everything())
		return -1;

	run_game();
}