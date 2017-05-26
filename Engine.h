#pragma once
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
#include "conf.h"
#include <string>

#define NUM_THREADS 8

struct info {
public:
	int x, y;
	double a, b;
	double value;
};


class Engine
{
private: 
	FILE* logger;
	SDL_Renderer *mainRenderer, *colors;
	SDL_Window *win, *colorWin;
	SDL_Texture *chaos, *radio;
	int SCREEN_WIDTH, SCREEN_HEIGHT;
	int quit;
	std::string ab;
	double *pixels;
	info *pixelInfo;
	int iterations; //infinity gives better results
	double lambda_max, lambda_min;
	int colorMode;
	int selectionRectangleX, selectionRectangleY;
	int selectionRectangleSize;
	int showSelectionRectangle;
	double Astart_value, Aend_value; 
	double Bstart_value, Bend_value;
	double reta1, reta2, retb1, retb2; //for restoring a and b values 
	double continuousRegionA;
	int lock;
	int imageCount;
	//int numberOfComputationThreads;
	SDL_sem *threadReady[NUM_THREADS]; // 4 is the numberofcomputationthreads... change to dynamic later
	SDL_sem *outputReady[NUM_THREADS];
public:

	
	Engine(int w, int h);
	~Engine();
	int init();
	void start(int);
	void poll();
	void keypress(SDL_Event &);
	void mousepress(SDL_Event &);
	void mousewheel(SDL_Event &);
	void colormousepress(SDL_Event &);
	void colormousewheel(SDL_Event &);
	void getColor(double, int&, int&, int&);
	void getColor(int, int, int&, int&, int&);
	FILE* getLogger();
	void LogStatus();
	void setLock(int);
	void postReady(int);
	void waitReady(int); // which index to wait at
	void signalReady(); //signal that this thread is ready to compute again
	void waitAllReady(); //index 0 thread waits till all threads are ready
	int getLock();
	void getABRange(double&, double&, double&, double&);
	std::string getAB();
	int getImageCount();
	int getIterations();
	void setContinuousRegionAandAdjustRegion(double);
	double getContinuousRegionA();
	void increaseIterationsByFactor(int);
	void increaseIterationsByAddition(int);
	void increaseImageCount();
	void setMinMax(double, double);
	int getQuit();
	void setQuit(int);
	void setPixel(int x, int y, double val, double a, double b);
	double getPixel(int x, int y);
	void outputWindow();
	void outPixelInfo(std::ostream &out, int x, int y);
	void getScreenSize(int&, int&);
	void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y, int w, int h, SDL_Rect* clip = nullptr);
	void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y);
	void renderAndPoll(SDL_Renderer *ren);
	void drawColors(SDL_Renderer *ren);
	void drawColorBar();
	void drawSelectionRectangle(SDL_Renderer *ren);
	SDL_Texture* loadTexture(char *filename, SDL_Renderer *ren);
	SDL_Renderer* getRen();
};

