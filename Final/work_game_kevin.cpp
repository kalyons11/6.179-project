#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>
#include <random>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

SDL_Window* window;
SDL_Renderer* renderer;

bool init_everything();
bool init_sdl();
bool create_window();
bool create_renderer();
void set_up_renderer();

void render();
void run_game();

int posX = 100;
int posY = 200;
int sizeX = 600;
int sizeY = 400;

SDL_Rect pos_player1;
SDL_Rect pos_player2;
SDL_Rect ballPos;

mt19937 rng;
uniform_real_distribution<double> x_dist(10, sizeX - 10);
uniform_real_distribution<double> y_dist(10, sizeY - 10);

int x_speed = 10;
int y_speed;

bool started = false;

int max_score = 7;
int left_score = 0;
int right_score = 0;

int game_mode = -1;

int paddle_speed = 25;

void print_scores() {
	//cout << "CURRENT SCORING" << endl;
	cout << left_score << " - " << right_score << endl;
	//cout << "----------" << endl << endl;
}

bool go = true;

bool keep_playing() {
	// Determine if we should keep going or end the game...
	if (left_score < max_score && right_score < max_score) {
		return true;
	} else if ((left_score >= max_score && right_score == left_score - 1) || (right_score >= max_score && left_score == right_score - 1)) {
		return true;
		// Win by 2.
	} else if (left_score == right_score) {
		return true;
		// 7-7 issue.
	} else return false;
}


void render() {
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
	SDL_RenderFillRect(renderer, &pos_player1);
	SDL_RenderFillRect(renderer, &pos_player2);
	SDL_RenderFillRect(renderer, & ballPos );
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255 );
	SDL_RenderPresent(renderer);
}

void menu_render(){

}

bool init_everything(){
	TTF_Init();

	if ( !init_sdl() )
		return false;

	if ( !create_window() )
		return false;

	if ( !create_renderer() )
		return false;

	set_up_renderer();

	return true;
}

bool init_sdl(){
	if ( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )
	{
		cout << " Failed to initialize SDL : " << SDL_GetError() << endl;
		return false;
	}

	return true;
}

bool create_window(){
	window = SDL_CreateWindow( "Server", posX, posY, sizeX, sizeY, 0 );

	if ( window == nullptr )
	{
		cout << "Failed to create window : " << SDL_GetError();
		return false;
	}

	return true;
}

bool create_renderer(){
	renderer = SDL_CreateRenderer( window, -1, 0 );

	if ( renderer == nullptr )
	{
		cout << "Failed to create renderer : " << SDL_GetError();
		return false;
	}

	return true;
}

void set_up_renderer(){
	// Set size of renderer to the same as window
	SDL_RenderSetLogicalSize( renderer, sizeX, sizeY );

	// Set color of renderer to green
	SDL_SetRenderDrawColor( renderer, 0, 255, 0, 255 );
}

void run_menu() {

	bool quit = false;
	SDL_Event event;

	SDL_Init(SDL_INIT_VIDEO);
	SDL_Surface* menu = IMG_Load("pong.jpeg");
	SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, menu);

	TTF_Font *font = TTF_OpenFont("terminal.ttf", 18);
	SDL_Color white = {255, 255, 255};
	SDL_Surface *text = TTF_RenderText_Solid(font, "Press ENTER/RETURN to continue!", white);

	SDL_Texture *input_prompt = SDL_CreateTextureFromSurface(renderer, text);
	int texW = 0;
	int texH = 0;
	SDL_QueryTexture(input_prompt, NULL, NULL, &texW, &texH);

	SDL_Rect txtrect = {320 - texW / 2, 300, texW, texH};

	while (! quit && go) // Change this condition.
	{
		SDL_Event event;

		while ( SDL_PollEvent( & event ) )
		{
			if ( event.type == SDL_QUIT )
				go = false;
			else if ( event.type == SDL_KEYDOWN )
			{
				switch ( event.key.keysym.sym ){

					case SDLK_RETURN:
					quit = true;
					break;
				}
			}
		}

		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderCopy(renderer, input_prompt, NULL, & txtrect);
		SDL_RenderPresent(renderer);
	}

	SDL_DestroyTexture(texture);
	SDL_DestroyTexture(input_prompt);

}

void finish_menu() {

	TTF_Font *font = TTF_OpenFont("terminal.ttf", 18);
	SDL_Color white = {255, 255, 255};

	SDL_RenderClear(renderer);

	string result;

	if (left_score > right_score) {
		result = "PLAYER 1 WINS!!!";
	} else {
		result = "PLAYER 2 WINS!!!";
	}

	result = result + "\n'Y' to play again. 'N' to quit.";

	SDL_Surface *text = TTF_RenderText_Solid(font, result.c_str(), white);
	SDL_Texture *input_prompt = SDL_CreateTextureFromSurface(renderer, text);
	int texW = 0;
	int texH = 0;
	SDL_QueryTexture(input_prompt, NULL, NULL, &texW, &texH);
	SDL_Rect txtrect = {320 - texW / 2, 75, texW, texH};

	SDL_RenderCopy(renderer, input_prompt, NULL, & txtrect);
	SDL_RenderPresent(renderer);

}

void run_game() {

	TTF_Font *font = TTF_OpenFont("terminal.ttf", 18);
	TTF_Font *bigFont = TTF_OpenFont("terminal.ttf", 40);
	SDL_Color white = {255, 255, 255};

	SDL_Surface *text = TTF_RenderText_Solid(font, "'S' to start. 'P' to pause.", white);
	SDL_Texture *input_prompt = SDL_CreateTextureFromSurface(renderer, text);
	int texW = 0;
	int texH = 0;
	SDL_QueryTexture(input_prompt, NULL, NULL, &texW, &texH);
	SDL_Rect txtrect = {320 - texW / 2, 75, texW, texH};

	SDL_Surface *left_surface = TTF_RenderText_Solid(bigFont, "0", white);
	SDL_Texture *left_texture = SDL_CreateTextureFromSurface(renderer, left_surface);
	int leftW = 0;
	int leftH = 0;
	SDL_QueryTexture(left_texture, NULL, NULL, &leftW, &leftH);
	SDL_Rect leftRect = { 270 - leftW , 10 , leftW, leftH };

	SDL_Surface *right_surface = TTF_RenderText_Solid(bigFont, "0", white);
	SDL_Texture *right_texture = SDL_CreateTextureFromSurface(renderer, right_surface);
	int rightW = 0;
	int rightH = 0;
	SDL_QueryTexture(right_texture, NULL, NULL, &rightW, &rightH);
	SDL_Rect rightRect = { 330 , 10 , rightW, rightH };

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
					case SDLK_p:
						if (started) {
							started = false;
						}
						break;
					case SDLK_DOWN:
						if (pos_player2.y + paddle_speed + pos_player2.h <= sizeY && started)
							pos_player2.y += paddle_speed;
						break;
					case SDLK_UP:
						if (pos_player2.y - paddle_speed >= 0 && started)
							pos_player2.y -= paddle_speed;
						break;
					case SDLK_z:
						if (pos_player1.y + paddle_speed + pos_player1.h <= sizeY && started)
							pos_player1.y += paddle_speed;
						break;
					case SDLK_a:
						if (pos_player1.y - paddle_speed >= 0 && started)
							pos_player1.y -= paddle_speed;
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
			if (ballPos.x <= 0) {
				// Hit left wall.
				// Check if we hit the paddle...
				if (ballPos.y >= pos_player1.y && ballPos.y <= pos_player1.y + pos_player1.h) {
					// All good...
				} else {
					right_score++;
					string x = to_string(right_score);
					right_surface = TTF_RenderText_Solid(bigFont, x.c_str(), white);
					right_texture = SDL_CreateTextureFromSurface(renderer, right_surface);
					SDL_QueryTexture(right_texture, NULL, NULL, &rightW, &rightH);
					//print_scores();
				}
				x_speed = -1 * x_speed;
			} else if (ballPos.x >= sizeX - 10) {
				if (ballPos.y >= pos_player2.y && ballPos.y <= pos_player2.y + pos_player2.h) {
					// All good...
				} else {
					left_score++;
					string x = to_string(left_score);
					left_surface = TTF_RenderText_Solid(bigFont, x.c_str(), white);
					left_texture = SDL_CreateTextureFromSurface(renderer, left_surface);
					SDL_QueryTexture(left_texture, NULL, NULL, &leftW, &leftH);
					//print_scores();
				}
				x_speed = -1 * x_speed;
			}
			else if (ballPos.y <= 10 || ballPos.y >= sizeY - 10) {
				// Hit left wall.
				y_speed = -1 * y_speed;
			}
		}

		SDL_RenderClear(renderer);
		if (! started) {
			SDL_RenderCopy(renderer, input_prompt, NULL, & txtrect);
		}
		SDL_RenderCopy(renderer, left_texture, NULL, & leftRect);
		SDL_RenderCopy(renderer, right_texture, NULL, & rightRect);
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
		SDL_RenderFillRect(renderer, &pos_player1);
		SDL_RenderFillRect(renderer, &pos_player2);
		SDL_RenderFillRect(renderer, & ballPos );
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255 );
		SDL_RenderPresent(renderer);
		
		SDL_Delay(16);
	}

	if (go) {
		
		finish_menu();

	}
}

int main( int argc, char* args[] )
{

	if ( ! init_everything() ) 
		return -1;

	pos_player1.x = 0;
	pos_player1.y = 200;
	pos_player1.w = 10;
	pos_player1.h = 100;

	pos_player2.x = 590;
	pos_player2.y = 200;
	pos_player2.w = 10;
	pos_player2.h = 100;

	ballPos.x = int(x_dist(rng));
	ballPos.y = int(y_dist(rng));
	ballPos.w = 20;
	ballPos.h = 20;

	random_device rd;
	mt19937 rng(rd());
	uniform_int_distribution<int> uni(2, 5);
	y_speed = uni(rng);

	run_menu();

	run_game();

	SDL_Quit();
	
	TTF_Quit();
}
