

#include "SDL.h"
#include "Engine.h"
#include <iostream>
using namespace std;




int mainOutput(int argc, char ** argv) {
	Engine eng(500, 500);
	if (eng.init() != 0) {
		return 1;
	}
	eng.start(1);
	//	if (eng2.init() != 0) {		return 1;	}
	while (!eng.getQuit()) {
	//	eng.drawColorBar();
	//	eng.renderAndPoll(eng.getRen());
	}
	return 0;
}


int mainRender(int argc , char ** argv) {

	//Engine eng(128,128);
	//Engine eng(300, 300);
	Engine eng(302, 302);
	if (eng.init() != 0) {
		return 1;
	}
//	if (eng2.init() != 0) {		return 1;	}
	eng.start(0);

	
	eng.drawColorBar();
	//eng2.start();

	int ticks = SDL_GetTicks();
	int frame30 = SDL_GetTicks();
	int frames = 0;
	while (!eng.getQuit()) { // && ! eng2.getQuit()) {
		frames++;
		ticks = SDL_GetTicks();
		//eng.renderAndPoll(eng.getRen());
		eng.renderAndPoll(eng.getRen());
		
		int millis = SDL_GetTicks() - ticks;
		if (frames % 12000 == 0) {
			int milli30 = SDL_GetTicks() - frame30;
			frame30 = SDL_GetTicks();
			cout << "last 12000 frames are: " << 12000000.0 / milli30 << " fps." << endl;
		}
		if (millis < 1000) {

		}
		else {
			cout << millis << " milliseconds for just this frame " << endl;
		}
	//	SDL_Delay(1000);
	}
	


	cout << "Exiting Program - Calling SDL_Quit()" << endl;




	SDL_Quit();

	return 0;
}


int main(int argc, char **argv) {

//	mainOutput(argc, argv);
	mainRender(argc, argv); //render to screen
	return 0;
}