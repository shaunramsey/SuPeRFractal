
/*Copyright 2017 Shaun Ramsey

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http ://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
#include "SDL.h"
#include "Engine.h"
#include <iostream>
//using namespace std;//TODO: get rid of this nastynasty using namespace




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
			std::cout << "last 12000 frames are: " << 12000000.0 / milli30 << " fps." << std::endl;
		}
		if (millis < 1000) {

		}
		else {
		  std::cout << millis << " milliseconds for just this frame " << std::endl;
		}
	//	SDL_Delay(1000);
	}
	


	std::cout << "Exiting Program - Calling SDL_Quit()" << std::endl;




	SDL_Quit();

	return 0;
}


int main(int argc, char *argv[]) {

//	mainOutput(argc, argv);
	mainRender(argc, argv); //render to screen
	return 0;
}
