#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <iostream>
#include <random>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

//Declare variables used in several functions

SDL_Window* window;
SDL_Renderer* renderer;

bool init_everything();
bool init_sdl();
bool create_window();
bool create_renderer();
void set_up_renderer();

void run_game();

int posX = 100;
int posY = 200;
int sizeX = 600;
int sizeY = 400;

SDL_Color white = {255, 255, 255};
SDL_Color black = {0, 0, 0};
SDL_Color red = {255, 0, 0};
SDL_Color purple = {102, 0, 204};
SDL_Color gold = {102, 102, 0};
SDL_Color grey = {125, 125, 125};
SDL_Color color;

SDL_Rect pos_player1;
SDL_Rect pos_player2;
SDL_Rect ballPos;
SDL_Rect line;
SDL_Rect screen;

int color_code;

mt19937 rng;
uniform_real_distribution<double> y_dist(10, sizeY - 10);

int x_speed = 7;
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


//create function to keep playig after game
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

//initialize everything in game
bool init_everything() {
	TTF_Init();
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048);

	if (!init_sdl()){
		return false;
	}
	if (!create_window()){
		return false;
	}
	if (!create_renderer()){
		return false;
	}

	set_up_renderer();
	return true;
}

//initialize sdl
bool init_sdl() {
	if ( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )
	{
		cout << " Failed to initialize SDL : " << SDL_GetError() << endl;
		return false;
	}

	return true;
}
//create the window
bool create_window() {
	window = SDL_CreateWindow( "KW PONG", posX, posY, sizeX, sizeY, 0 );

	if ( window == nullptr )
	{
		cout << "Failed to create window : " << SDL_GetError();
		return false;
	}

	return true;
}

//create renderer to post to window
bool create_renderer() {
	renderer = SDL_CreateRenderer( window, -1, 0 );

	if ( renderer == nullptr )
	{
		cout << "Failed to create renderer : " << SDL_GetError();
		return false;
	}

	return true;
}

//set up the renderer with window size
void set_up_renderer(){
	// Set size of renderer to the same as window
	SDL_RenderSetLogicalSize( renderer, sizeX, sizeY );
}

//create function to load music from same directory as file
void music(){
	techno = Mix_LoadMUS("techno.mp3");
	hit = Mix_LoadWAV("hit.mp3");
	win_1 = Mix_LoadWAV("win_1.mp3");
	win_2 = Mix_LoadWAV("win_2.mp3");
	score = Mix_LoadWAV("score.mp3");
	bounce = Mix_LoadWAV("bounce.mp3");
	Mix_PlayMusic(techno, -1);
}

//setup the color menu
void color_menu(){
	bool quit2 = false;

	screen.x = 0;
	screen.y = 0;
	screen.w = 600;
	screen.h = 400;

	TTF_Font *font = TTF_OpenFont("terminal.ttf", 24);

	SDL_Surface *text = TTF_RenderText_Solid(font, "Select a color for pong:", white);
	SDL_Texture *color_prompt = SDL_CreateTextureFromSurface(renderer, text);
	int texW = 0;
	int texH = 0;
	SDL_QueryTexture(color_prompt, NULL, NULL, &texW, &texH);
	SDL_Rect colrect = {5 , 10, texW, texH};

	SDL_Surface *cltext = TTF_RenderText_Solid(font, "Press b for classic colors", white);
	SDL_Texture *classic_prompt = SDL_CreateTextureFromSurface(renderer, cltext);
	texW = 0;
	texH = 0;
	SDL_QueryTexture(classic_prompt, NULL, NULL, &texW, &texH);
	SDL_Rect clrect = {50, 10 + 2 * texH, texW, texH};

	SDL_Surface *intext = TTF_RenderText_Solid(font, "Press i for inverse classic colors", grey);
	SDL_Texture *inverse_prompt = SDL_CreateTextureFromSurface(renderer, intext);
	texW = 0;
	texH = 0;
	SDL_QueryTexture(inverse_prompt, NULL, NULL, &texW, &texH);
	SDL_Rect inrect = {50, 10 + 4 * texH, texW, texH};

	SDL_Surface *retext = TTF_RenderText_Solid(font, "Press r for red and black", red);
	SDL_Texture *red_prompt = SDL_CreateTextureFromSurface(renderer, retext);
	texW = 0;
	texH = 0;
	SDL_QueryTexture(red_prompt, NULL, NULL, &texW, &texH);
	SDL_Rect rerect = {50, 10 + 6 * texH, texW, texH};

	SDL_Surface *putext = TTF_RenderText_Solid(font, "Press p for purple and green", purple);
	SDL_Texture *purple_prompt = SDL_CreateTextureFromSurface(renderer, putext);
	texW = 0;
	texH = 0;
	SDL_QueryTexture(purple_prompt, NULL, NULL, &texW, &texH);
	SDL_Rect purect = {50, 10 + 8 * texH, texW, texH};

	SDL_Surface *gotext = TTF_RenderText_Solid(font, "Press g for gold and navy", gold);
	SDL_Texture *gold_prompt = SDL_CreateTextureFromSurface(renderer, gotext);
	texW = 0;
	texH = 0;
	SDL_QueryTexture(gold_prompt, NULL, NULL, &texW, &texH);
	SDL_Rect gorect = {50, 10 + 10 * texH, texW, texH};

	//create while loop to handle keyboard events
	while (! quit2 && go) // Change this condition.
	{
		SDL_Event event;

		while ( SDL_PollEvent( & event ) )
		{
			if ( event.type == SDL_QUIT )
				go = false;
			else if ( event.type == SDL_KEYDOWN )
			{
				switch ( event.key.keysym.sym ){

					case SDLK_c:
					color_code = 0;
					quit2 = true;
					break;

					case SDLK_i:
					color_code = 1;
					quit2 = true;
					break;

					case SDLK_r:
					color_code = 2;
					quit2 = true;
					break;

					case SDLK_p:
					color_code = 3;
					quit2 = true;
					break;

					case SDLK_g:
					color_code = 4;
					quit2 = true;
					break;

					
				}
			}
		}
		//render all text inputed earlier
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255 );
		SDL_RenderFillRect(renderer, &screen);
		SDL_RenderCopy(renderer, color_prompt, NULL, & colrect);
		SDL_RenderCopy(renderer, classic_prompt, NULL, & clrect);
		SDL_RenderCopy(renderer, inverse_prompt, NULL, & inrect);
		SDL_RenderCopy(renderer, red_prompt, NULL, & rerect);
		SDL_RenderCopy(renderer, purple_prompt, NULL, & purect);
		SDL_RenderCopy(renderer, gold_prompt, NULL, & gorect);
		SDL_RenderPresent(renderer);
	}
}

//create function to run the main menu
void run_menu() {

	//initialize all positions of objects so that they are reset when another game is started
	line.x = 299;
	line.y = 0;
	line.w = 3;
	line.h = 400;

	pos_player1.x = 0;
	pos_player1.y = 150;
	pos_player1.w = 10;
	pos_player1.h = 100;

	pos_player2.x = 590;
	pos_player2.y = 150;
	pos_player2.w = 10;
	pos_player2.h = 100;

	ballPos.x = 293;
	ballPos.y = int(y_dist(rng));
	ballPos.w = 15;
	ballPos.h = 15;

	random_device rd;
	mt19937 rng(rd());
	uniform_int_distribution<int> uni(2, 5);
	y_speed = uni(rng);

	bool quit = false;
	SDL_Event event;

	SDL_Init(SDL_INIT_VIDEO);
	SDL_Surface* menu = IMG_Load("pong.jpeg");
	SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, menu);

	TTF_Font *font = TTF_OpenFont("terminal.ttf", 24);
	SDL_Color grey = {175, 175, 175};
	SDL_Surface *text = TTF_RenderText_Solid(font, "Press ENTER/RETURN to play!", white);

	SDL_Texture *input_prompt = SDL_CreateTextureFromSurface(renderer, text);
	int texW = 0;
	int texH = 0;
	SDL_QueryTexture(input_prompt, NULL, NULL, &texW, &texH);

	SDL_Rect txtrect = {320 - texW / 2, 300, texW, texH};

	//create a while loop to handle keyboard event to continue
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

//create a menu after the game is over
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

	string result;

	if (left_score > right_score) {
		result = "PLAYER 1 WINS!!!";
	} else {
		result = "PLAYER 2 WINS!!!";
	}

	left_score = 0;
	right_score = 0;

	SDL_Surface *first = TTF_RenderText_Solid(font, result.c_str(), grey);
	SDL_Texture *first_prompt = SDL_CreateTextureFromSurface(renderer, first);
	int w = 0;
	int h = 0;
	SDL_QueryTexture(first_prompt, NULL, NULL, &w, &h);
	SDL_Rect firstRect = {300 - w / 2, 100, w, h};

	SDL_Surface *text = TTF_RenderText_Solid(font, "'Y' to play again. 'N' to quit.", grey);
	SDL_Texture *input_prompt = SDL_CreateTextureFromSurface(renderer, text);
	int texW = 0;
	int texH = 0;
	SDL_QueryTexture(input_prompt, NULL, NULL, &texW, &texH);
	SDL_Rect txtrect = {300 - texW / 2, 200, texW, texH};

	bool keep = true;

	//handle keyboard events
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

		//render 
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, left_texture, NULL, & leftRect);
		SDL_RenderCopy(renderer, right_texture, NULL, & rightRect);
		SDL_RenderCopy(renderer, first_prompt, NULL, & firstRect);
		SDL_RenderCopy(renderer, input_prompt, NULL, & txtrect);
		SDL_RenderPresent(renderer);
	}
	Mix_PlayMusic(techno, -1);
}

//create function to run game itself change color corresponding to selection
void run_game() {

	if(color_code == 0){
		color = white;
	}
	else if(color_code == 1 || color_code == 2){
		color = black;
	}
	else if(color_code ==3){
		color = purple;
	}
	else{
		color = gold;
	}

	TTF_Font *font = TTF_OpenFont("terminal.ttf", 24);
	TTF_Font *bigFont = TTF_OpenFont("terminal.ttf", 40);

	SDL_Surface *text = TTF_RenderText_Solid(font, "'S' to start. 'P' to pause.",grey);
	SDL_Texture *input_prompt = SDL_CreateTextureFromSurface(renderer, text);
	int texW = 0;
	int texH = 0;
	SDL_QueryTexture(input_prompt, NULL, NULL, &texW, &texH);
	SDL_Rect txtrect = {300 - texW / 2, 85, texW, texH};

	SDL_Surface *textcon = TTF_RenderText_Solid(font, "CONTROLS", grey);
	SDL_Texture *control = SDL_CreateTextureFromSurface(renderer, textcon);
	texW = 0;
	texH = 0;
	SDL_QueryTexture(control, NULL, NULL, &texW, &texH);
	SDL_Rect rectcon = {300 - texW / 2, 85 + 2 * texH, texW, texH};

	SDL_Surface *text1 = TTF_RenderText_Solid(font, "Player 1: Up = a, Down = z", grey);
	SDL_Texture *control1 = SDL_CreateTextureFromSurface(renderer, text1);
	texW = 0;
	texH = 0;
	SDL_QueryTexture(control1, NULL, NULL, &texW, &texH);
	SDL_Rect rect1 = {300 - texW / 2, 85 + 3.5 * texH, texW, texH};

	SDL_Surface *text2 = TTF_RenderText_Solid(font, "Player 2: Up = Up Key, Down = Down Key", grey);
	SDL_Texture *control2 = SDL_CreateTextureFromSurface(renderer, text2);
	texW = 0;
	texH = 0;
	SDL_QueryTexture(control2, NULL, NULL, &texW, &texH);
	SDL_Rect rect2 = {300 - texW / 2, 85 + 5 * texH, texW, texH};

	SDL_Surface *left_surface = TTF_RenderText_Solid(bigFont, "0", color);
	SDL_Texture *left_texture = SDL_CreateTextureFromSurface(renderer, left_surface);
	int leftW = 0;
	int leftH = 0;
	SDL_QueryTexture(left_texture, NULL, NULL, &leftW, &leftH);
	SDL_Rect leftRect = { 250, 10 , leftW, leftH };

	SDL_Surface *right_surface = TTF_RenderText_Solid(bigFont, "0", color);
	SDL_Texture *right_texture = SDL_CreateTextureFromSurface(renderer, right_surface);
	int rightW = 0;
	int rightH = 0;
	SDL_QueryTexture(right_texture, NULL, NULL, &rightW, &rightH);
	SDL_Rect rightRect = { 330 , 10 , rightW, rightH };

	bool do_delay = false;

	//handle keyboard events and update objects on screen
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
					right_surface = TTF_RenderText_Solid(bigFont, x.c_str(), color);
					right_texture = SDL_CreateTextureFromSurface(renderer, right_surface);
					SDL_QueryTexture(right_texture, NULL, NULL, &rightW, &rightH);
					ballPos.x = 293;
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
					left_surface = TTF_RenderText_Solid(bigFont, x.c_str(), color);
					left_texture = SDL_CreateTextureFromSurface(renderer, left_surface);
					SDL_QueryTexture(left_texture, NULL, NULL, &leftW, &leftH);
					ballPos.x = 293;
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
			SDL_RenderCopy(renderer, control1, NULL, & rect1);
			SDL_RenderCopy(renderer, control2, NULL, & rect2);
			SDL_RenderCopy(renderer, control, NULL, & rectcon);
		}
		SDL_RenderCopy(renderer, left_texture, NULL, & leftRect);
		SDL_RenderCopy(renderer, right_texture, NULL, & rightRect);

		if(color_code == 0){
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
		}
		else if(color_code == 1 || color_code == 2){
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
		}
		else if(color_code ==3){
			SDL_SetRenderDrawColor(renderer, 102, 0, 204, 0);
		}
		else{
			SDL_SetRenderDrawColor(renderer, 102, 102, 0, 0);
		}
		
		SDL_RenderFillRect(renderer, &pos_player1);
		SDL_RenderFillRect(renderer, &pos_player2);
		SDL_RenderFillRect(renderer, & ballPos);
		SDL_RenderFillRect(renderer, &line);

		if(color_code == 0){
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255 );
		}
		else if(color_code == 1){
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255 );
		} 
		else if(color_code == 2){
			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255 );
		}
		else if(color_code ==3){
			SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255 );
		}
		else{
			SDL_SetRenderDrawColor(renderer, 0, 0, 102, 255 );
		}
		
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

//create main function 
int main(int argc, char* args[]) {

	if ( ! init_everything() ) 
		return -1;

	music();


	while (play_again) {

		run_menu();

		color_menu();

		run_game();

		started = false;
	}

	SDL_Quit();
	
	TTF_Quit();
}