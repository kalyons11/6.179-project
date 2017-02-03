#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
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
uniform_real_distribution<double> y_dist(10, sizeY - 10);

int x_speed = 5;
int y_speed;
int max_score = 7;
int left_score = 0;
int right_score = 0;

int game_mode = -1;
int paddle_speed = 25;

bool go = true;
bool started = false;
bool play_again = true;

Mix_Music *techno;
Mix_Chunk *hit;
Mix_Chunk *win_1;
Mix_Chunk *win_2;
Mix_Chunk *score;
Mix_Chunk *bounce;
int win = 0;

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

bool init_everything() {
	TTF_Init();
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048);

	if ( !init_sdl() )
		return false;

	if ( !create_window() )
		return false;

	if ( !create_renderer() )
		return false;

	set_up_renderer();

	return true;
}

bool init_sdl() {
	if ( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )
	{
		cout << " Failed to initialize SDL : " << SDL_GetError() << endl;
		return false;
	}

	return true;
}

bool create_window() {
	window = SDL_CreateWindow( "KW PONG", posX, posY, sizeX, sizeY, 0 );

	if ( window == nullptr )
	{
		cout << "Failed to create window : " << SDL_GetError();
		return false;
	}

	return true;
}

bool create_renderer() {
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

void music(){
	techno = Mix_LoadMUS("techno.mp3");
	hit = Mix_LoadWAV("hit.mp3");
	win_1 = Mix_LoadWAV("win_1.mp3");
	win_2 = Mix_LoadWAV("win_2.mp3");
	score = Mix_LoadWAV("score.mp3");
	bounce = Mix_LoadWAV("bounce.mp3");
	Mix_PlayMusic(techno, -1);
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

void finish_menu(SDL_Texture *left_texture, SDL_Texture *right_texture, SDL_Rect leftRect, SDL_Rect rightRect) {
	Mix_HaltMusic();
	if (win == 0){
		Mix_PlayChannel(-1, win_1, 0);
		win = 1;
	}
	else{
		Mix_PlayChannel(-1, win_2, 0);
		win = 0;
	}
	TTF_Font *font = TTF_OpenFont("terminal.ttf", 24);
	SDL_Color white = {255, 255, 255};

	string result;

	if (left_score > right_score) {
		result = "PLAYER 1 WINS!!!";
	} else {
		result = "PLAYER 2 WINS!!!";
	}

	left_score = 0;
	right_score = 0;

	SDL_Surface *first = TTF_RenderText_Solid(font, result.c_str(), white);
	SDL_Texture *first_prompt = SDL_CreateTextureFromSurface(renderer, first);
	int w = 0;
	int h = 0;
	SDL_QueryTexture(first_prompt, NULL, NULL, &w, &h);
	SDL_Rect firstRect = {300 - w / 2, 100, w, h};

	SDL_Surface *text = TTF_RenderText_Solid(font, "'Y' to play again. 'N' to quit.", white);
	SDL_Texture *input_prompt = SDL_CreateTextureFromSurface(renderer, text);
	int texW = 0;
	int texH = 0;
	SDL_QueryTexture(input_prompt, NULL, NULL, &texW, &texH);
	SDL_Rect txtrect = {300 - texW / 2, 200, texW, texH};

	bool keep = true;

	while (keep) {

		SDL_Event event;

		while ( SDL_PollEvent( & event ) )
		{
			if ( event.type == SDL_QUIT ) {
				keep = false;
				play_again = false;
			}
			else if ( event.type == SDL_KEYDOWN )
			{
				switch ( event.key.keysym.sym )
				{
					case SDLK_y:
						keep = false;
						play_again = true;
						break;
					case SDLK_n:
						keep = false;
						play_again = false;
						break;
				}
			}
		}

		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, left_texture, NULL, & leftRect);
		SDL_RenderCopy(renderer, right_texture, NULL, & rightRect);
		SDL_RenderCopy(renderer, first_prompt, NULL, & firstRect);
		SDL_RenderCopy(renderer, input_prompt, NULL, & txtrect);
		SDL_RenderPresent(renderer);
	}
	Mix_PlayMusic(techno, -1);
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

	bool do_delay = false;

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
				if (ballPos.y + 15 >= pos_player1.y && ballPos.y <= pos_player1.y + pos_player1.h) {
					// All good...
					x_speed = -1 * x_speed;
					Mix_PlayChannel(-1, hit, 0);
				} else {
					right_score++;
					string x = to_string(right_score);
					right_surface = TTF_RenderText_Solid(bigFont, x.c_str(), white);
					right_texture = SDL_CreateTextureFromSurface(renderer, right_surface);
					SDL_QueryTexture(right_texture, NULL, NULL, &rightW, &rightH);
					ballPos.x = 295;
					ballPos.y = int(y_dist(rng));
					do_delay = true;
					Mix_PlayChannel(-1, score, 0);
				}
			} 
			else if (ballPos.x >= sizeX - 15) {
				if (ballPos.y + 15>= pos_player2.y && ballPos.y <= pos_player2.y + pos_player2.h) {
					// All good...
					x_speed = -1 * x_speed;
					Mix_PlayChannel(-1, hit, 0);
				} else {
					left_score++;
					string x = to_string(left_score);
					left_surface = TTF_RenderText_Solid(bigFont, x.c_str(), white);
					left_texture = SDL_CreateTextureFromSurface(renderer, left_surface);
					SDL_QueryTexture(left_texture, NULL, NULL, &leftW, &leftH);
					ballPos.x = 295;
					ballPos.y = int(y_dist(rng));
					do_delay = true;
					Mix_PlayChannel(-1, score, 0);
				}
				

			}
			else if (ballPos.y <= 10 || ballPos.y >= sizeY - 10) {
				// Hit left wall.
				y_speed = -1 * y_speed;
				Mix_PlayChannel(-1, bounce, 0);
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

		if (do_delay) {
			SDL_Delay(1000);
			do_delay = false;
		}
		
		SDL_Delay(16);
	}

	if (go) {
		
		finish_menu(left_texture, right_texture, leftRect, rightRect);

	} else {
		play_again = false;
	}
}

int main(int argc, char* args[]) {

	if ( ! init_everything() ) 
		return -1;

	music();

	pos_player1.x = 0;
	pos_player1.y = 200;
	pos_player1.w = 10;
	pos_player1.h = 100;

	pos_player2.x = 590;
	pos_player2.y = 200;
	pos_player2.w = 10;
	pos_player2.h = 100;

	ballPos.x = 295;
	ballPos.y = int(y_dist(rng));
	ballPos.w = 15;
	ballPos.h = 15;

	random_device rd;
	mt19937 rng(rd());
	uniform_int_distribution<int> uni(2, 5);
	y_speed = uni(rng);

	while (play_again) {

		run_menu();

		run_game();

		started = false;
	}

	SDL_Quit();
	
	TTF_Quit();
}