#include <SDL.h>

//This code will not run it is psuedocode

//detailed explanation at http://www.sdltutorials.com/sdl-tutorial-basics 

//create a class for the app
class CApp{

	private:
		bool Running;

	public:
		CApp();
		int OnExecute();

	public:
		//initializes the display
		bool OnInit(){
			if(SDL_Init(SDL_INIT_EVERYTHING) < 0){
				return(false);
			}
			if((Surf_Display = SDL_SetVideoMode(640, 480, 32, SDL_HWSURFACE | SDL_DOUBLEBUF)) == NULL){
				return(false);
			}
			return(true);
		}

		//these are the 4 main events in any game
		//event is when an input is given to the machine
		void OnEvent(SDL_Event* event){
			if(Event -> type == SDL_QUIT){
				Running = false;
			}
		}
		//Onloop updates the display
		void OnLoop();
		//OnRender creates the display (i believe)
		void OnRender();
		//cleanup frees memory and closes the display. Should free any other memory in this method
		void OnCleanup(){
			SDL_Quit();
		}
}


CApp::CApp(){
	Surf_Display  = NULL;

	Running = true;
}

int CApp::OnExecute(){
	if(OnInit() == false){
		return(-1);
	}

	SDL_Event Event;

	while(Running){
		while(SDL_PollEvent(&Event)){
			OnEvent(&Event);
		}
		OnLoop();
		OnRender();
	}
	OnCleanup();
	return(0);
}

int main(int argc, char* argv[]){
	CApp theApp;

	return theApp.OnExecute()
}