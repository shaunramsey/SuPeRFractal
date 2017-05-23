#pragma once

/*
int computationThreadDelta(void *ptr) {
Engine *myengine = (Engine *)ptr;
int SCREEN_WIDTH, SCREEN_HEIGHT;
myengine->getScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
string ab = myengine->getAB();

while (!myengine->getQuit()) {
cout << "Computing range [a,b] in [2,4] with iterations=["
<< myengine->getIterations() << "]" << endl;
double deltaA = 2.0 / SCREEN_WIDTH;
double deltaB = 2.0 / SCREEN_HEIGHT;
double max = -1;
for (double a = 2; a < 4; a += deltaA) {
for (double b = 2; b < 4; b += deltaB) {
if (myengine->getQuit()) { break; } //get out of the loops if quite as received
int iters = (int)((a - 2) / deltaA*SCREEN_HEIGHT + (b - 2) / deltaB);
double l = lambda(a, b, ab, myengine->getIterations()); l = l * 255;
if (!(iters % 1000)) cout << iters << " / " << SCREEN_WIDTH*SCREEN_HEIGHT << endl;
if (l > max) { max = l; cout << "[" << a << ", " << b << "] - Max is now: " << l << endl; }
myengine->setPixel((int)((a - 2) / 2.0*SCREEN_WIDTH), (int)((b - 2) / 2.0*SCREEN_HEIGHT), (int)l);
//	SDL_SetRenderDrawColor(myengine->ren, l, l, 0, 255);
//	SDL_RenderDrawPoint(myengine->ren, (a - 2) / 2.0*SCREEN_WIDTH, (4 - b) / 2.0*SCREEN_HEIGHT);
//	cout << "[" << (a - 2)/2.0*SCREEN_WIDTH << ", " << (b - 2)/2.0*SCREEN_WIDTH << "] == " << "[" << a << ", " << b << "] = " << l << " max=" << max << endl;



}
}
myengine->increaseIterationsByFactor(2);
}
return 1;

}
int renderThread(void* ptr) {
Engine *myengine = (Engine *)ptr;
while (!myengine->getQuit()) {

myengine->poll();
int SCREEN_WIDTH, SCREEN_HEIGHT;
myengine->getScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);

int TILE_SIZE = 60;
//	for (int i = 0; i < 3; i++) {
SDL_RenderClear(myengine->ren);
//if (i % 2 == 0)			SDL_RenderCopy(ren, radio, NULL, NULL);
//else			SDL_RenderCopy(ren, chaos, NULL, NULL);
******
int bW, bH;
SDL_QueryTexture(chaos, NULL, NULL, &bW, &bH);
for (int i = 0; i < SCREEN_WIDTH; i = i + TILE_SIZE) {
for (int j = 0; j < SCREEN_HEIGHT; j = j + TILE_SIZE) {
renderTexture(chaos, ren, i, j, TILE_SIZE, TILE_SIZE);
}
}
*******

//string ab = "AB";
SDL_SetRenderDrawColor(myengine->ren, 255, 128, 64, 0); //orange
SDL_SetRenderDrawColor(myengine->ren, 205, 64, 208, 0); // purple
//SDL_RenderDrawPoint(ren, 0, 0);

//SCREEN_WIDTH and SCREEN_HEIGHT are equal
double delta = 2.0 / SCREEN_WIDTH;
double max = -1;
for (double a = 2; a < 4; a += delta) {
for (double b = 2; b < 4; b += delta) {

double l = lambda(a, b, myengine->getAB(), 50); l = l * 255;
if (l > max) { max = l; cout << "Max is now: " << l << endl; }
SDL_SetRenderDrawColor(myengine->ren, (int)l, (int)l, 0, 255);
SDL_RenderDrawPoint(myengine->ren, (int)((a - 2) / 2.0*SCREEN_WIDTH), (int)((4 - b) / 2.0*SCREEN_HEIGHT));
//	cout << "[" << (a - 2)/2.0*SCREEN_WIDTH << ", " << (b - 2)/2.0*SCREEN_WIDTH << "] == " << "[" << a << ", " << b << "] = " << l << " max=" << max << endl;

}
}



int CENTER_SIZE = 5;
int x = SCREEN_WIDTH / 2 - CENTER_SIZE / 2;
int y = SCREEN_HEIGHT / 2 - CENTER_SIZE / 2;
//		renderTexture(radio, ren, x, y, CENTER_SIZE, CENTER_SIZE);

SDL_RenderPresent(myengine->ren);
SDL_Delay(2000);
//		}
}
return 1;
}
*/