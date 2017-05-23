#include "Engine.h"
#include "SDL_image.h"
#include "bmp.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include "computation.h"
#include "Colors.h"
using namespace std;

struct computationThreadInfo {
	Engine *engine;
	int index; //which index this thread belongs to
	int ilo, ihi;
	int jlo, jhi;
};

//if using multple threads, be prepared for some things to break unril fixed up
//the number of threads is #defined inside the header - not using the var in Engine
#define MULTIPLE_THREADS

//how big should the color screen window be?
#define COLOR_SCREEN_HEIGHT 50 

//output an image every frame?
#define IMAGE_OUTPUT

//increase the continuous a region every frame?
//#define INCREASE_CONT .1

//where does the first images a begin
#define CONTINUOUS_REGION_START_VALUE 0

//how many frames to go from starting region to ending region - comment this out to remove interpolation
//#define INTERPOLATE_NUMBER 10

//how fast to zoom into the final area - must be defined if INTERPOLATE_NUMBER is defined
#define INTERPOLATION_EXP 4.0

//blower bound is either 1 or Aend_value - some constant - don't add semi-colons
#define BLOWERBOUND exp(continuousRegionA)  + 1
//Aend_value - 1.1
//exp(continuousRegionA*2) - exp(continuousRegionA)
//Aend_value - 1.1

//the lower bound on the region area - - - BLOWERBOUND can't even be lower than this
#define BMINIMUM -1

//pause at the end of picture gen
#define PAUSE_EVERY_FRAME
bool paused = false;

//give bmp outputs - probably best not to uncomment - png ftw here
//#define BMP

//just a lot more console output text to keep progress of things - generally best not to uncomment
//#define VERBOSE

double interpolate(double from, double to, int i, int max) {
	if (i > max) i = max;
	double pct = pow(1.0 - i / (double)max, INTERPOLATION_EXP);
	return pct * from + (1 - pct)*to;
}

int computationThread(void *ptr) {
	computationThreadInfo*info = (computationThreadInfo *)ptr;
	Engine *myengine = info->engine;
	int SCREEN_WIDTH, SCREEN_HEIGHT;
	myengine->getScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	printf("SCREEN WIDTH,HEIGHT=%d,%d\n", SCREEN_WIDTH, SCREEN_HEIGHT);
	string ab = myengine->getAB();
	double Aend_value, Astart_value, Bend_value, Bstart_value;
#ifdef INTERPOLATE_NUMBER
	myengine->setContinuousRegionAandAdjustRegion(1.5);
	double Bs1 = 4.5, Be1 = 6.98047, As1 = 4.5, Ae1 = 6.98047;
	double Bs2 = 6.05, Be2 = 6.11, As2 = 6.78, Ae2 = 6.84;
	//double Bs2 = 6.05, Be2 = 6.2, As2 = 6.7, Ae2 = 6.85;
//	double Bs2 = 6.5038, Be2 = 6.5838, As2 = 6.8778, Ae2 = 6.9578;
//	double Bs2 = 6.4488, Be2 = 6.50747, As2 = 6.81913, Ae2 = 6.8778;
	//As2 = 6.0, Ae2 = 6.98047, Bs2 = 4.8, Be2 = 5.78047;
	//interpolating from As1 to As2
#endif


	while (!myengine->getQuit()) {
#ifdef PAUSE_EVERY_FRAME
		if (paused) {
			SDL_Delay(250);
			continue;
		}
#endif
#ifdef MULTIPLE_THREADS
		myengine->waitReady(info->index); //wait til this sem was posted
#endif

#ifdef INTERPOLATE_NUMBER
		Astart_value = interpolate(As1, As2, myengine->getImageCount(), INTERPOLATE_NUMBER);
		Aend_value = interpolate(Ae1, Ae2, myengine->getImageCount(), INTERPOLATE_NUMBER);
		Bstart_value = interpolate(Bs1, Bs2, myengine->getImageCount(), INTERPOLATE_NUMBER);
		Bend_value = interpolate(Be1, Be2, myengine->getImageCount(), INTERPOLATE_NUMBER);
#else
		myengine->getABRange(Astart_value, Aend_value, Bstart_value, Bend_value);
#endif
		double continuousA = myengine->getContinuousRegionA(); // what's the continuous region size
		int lock = myengine->getLock(); // which thing to render
#ifdef MULTIPLE_THREADS
		if (info->index == 0) {
#endif
			cout << "[" << setw(4) << myengine->getImageCount() << "] Range: [" <<
				Bstart_value << ", " << Bend_value << "]-[" <<
				Astart_value << ", " << Aend_value << "]"
				<< " a=[" << continuousA << "] iters=["
				<< myengine->getIterations() << "]*ab=" << ab << endl;
#ifdef IMAGE_OUTPUT
			myengine->LogStatus();
#endif
#ifdef MULTIPLE_THREADS
		}
#endif
		double Arange = Aend_value - Astart_value;
		double Brange = Bend_value - Bstart_value;
		double max = -1;
		double min = 5000;
#ifdef MULTIPLE_THREADS

		int jlo = info->jlo, jhi = info->jhi; // / div, jhi = jlo + 1;
		int ilo = info->ilo, ihi = info->ihi; // index % div, ihi = ilo + 1;
		
#ifdef VERBOSE
		SDL_Delay(info->index * 1000);
		cout << "[" << info->index << "] i: " << ilo << "-" << ihi << " j: " << jlo << " - " << jhi << endl;
#endif
		for (int k = jlo; k < jhi; k++) {
			int j =  info->index % 2 ? k : (int)(jhi - k + jlo - 1);
			for (int i = ilo; i < ihi ; i++) {
#else
		for (int j = 0; j < SCREEN_HEIGHT; j++) {
			for (int i = 0; i < SCREEN_WIDTH; i++) {
#endif
				myengine->setPixel(i, j, info->index-2, -1, -1);
			}
#ifdef MULTIPLE_THREADS
			for (int i = ilo; i < ihi; i++) {
#else
			for (int i = 0; i < SCREEN_WIDTH; i++) {
#endif
				if (myengine->getQuit()) { break; } //get out of the loops if quite as received
				int iters = i + j * SCREEN_WIDTH;



				double a = j * Arange / SCREEN_WIDTH + Astart_value;//start with 2 end at 4
				double b = i * Brange / SCREEN_HEIGHT  + Bstart_value; //start with 2 end at 4
				double l = lambda(a, b, ab, myengine->getIterations(), continuousA, lock); 
				if (l > max) { max = l; } //cout << "[" << a << ", " << b << "] - Max is now: " << l << endl;
				if (l < min && l != -INFINITY) { min = l; }

#ifdef VERBOSE
				if (!(iters % 10000)) cout << "[" << iters << " / " << SCREEN_WIDTH*SCREEN_HEIGHT <<
					"] -- Current N=[" << myengine->getIterations() << "] with ab=" << ab << endl;
#endif
				myengine->setPixel(i, j, l, a, b);
				//	SDL_SetRenderDrawColor(myengine->ren, l, l, 0, 255);
				//	SDL_RenderDrawPoint(myengine->ren, (a - 2) / 2.0*SCREEN_WIDTH, (4 - b) / 2.0*SCREEN_HEIGHT);
				//	cout << "[" << (a - 2)/2.0*SCREEN_WIDTH << ", " << (b - 2)/2.0*SCREEN_WIDTH << "] == " << "[" << a << ", " << b << "] = " << l << " max=" << max << endl;



			}
		}
//		myengine->increaseIterationsByFactor(2);
		//myengine->increaseIterationsByAddition(5);
#ifdef MULTIPLE_THREADS
		myengine->postReady(info->index);
		if (info->index == 0) {
			myengine->waitAllReady();
#endif
			cout << "[" << setw(5) << myengine->getImageCount() << "] Values stored in pixels: Max was: " << max << " and min was " << min << endl;
			fprintf(myengine->getLogger(), "lambda range: %f, %f\n",max, min);
			//myengine->setMinMax(min, max);
#ifdef IMAGE_OUTPUT
//		myengine->LogStatus(); //already done above
			
				myengine->outputWindow(); //output to PNG
#endif
#ifdef INTERPOLATE_NUMBER
			if (myengine->getImageCount() == INTERPOLATE_NUMBER) {
				myengine->setQuit(1);
			}
#endif
#ifdef INCREASE_CONT
			myengine->setContinuousRegionAandAdjustRegion(myengine->getContinuousRegionA() + INCREASE_CONT);
#endif
			myengine->increaseImageCount();
#ifdef MULTIPLE_THREADS
		//	cout << "Signalling next round of computation now." << endl;
			myengine->signalReady();
		}
#endif
		//SDL_Delay(5000);
	}


	return 1;

}

Engine::Engine(int w, int h)
{
	logger = NULL;
#ifdef IMAGE_OUTPUT
	fopen_s(&logger, "../../Images/log.txt", "a+");
	if (!logger) cerr << "Problem opening logger . . . images/log not retained!" << endl;
	else 	     fprintf(logger, "---New Program Execution---\n");
	//fclose(logger);
#endif

	Astart_value = 2, Aend_value = 4; //4.0
	Bstart_value = 2, Bend_value = 4;
	imageCount = 0;
	continuousRegionA = CONTINUOUS_REGION_START_VALUE;
	Aend_value = Bend_value = 0.5 * sqrt(exp(2 * continuousRegionA) + 8 * exp(continuousRegionA)) + 0.5 * exp(continuousRegionA) + 1;
	Astart_value = Bstart_value = 1;
#ifdef BLOWERBOUND
	Astart_value = Bstart_value = BLOWERBOUND;
#endif
#ifdef BMINIMUM
	if (Astart_value < BMINIMUM) {
		Astart_value = Bstart_value = BMINIMUM;
	}
#endif


	colorMode = 0;
	if (w > 0 && h > 0) {
		SCREEN_WIDTH = w;
		SCREEN_HEIGHT = h;
		printf("Engine constructed with %d,%d\n", w, h);
	}
	else {
		SCREEN_WIDTH = SCREEN_HEIGHT = 200;
	}
	pixels = new double[SCREEN_WIDTH*SCREEN_HEIGHT];
	pixelInfo = new info[SCREEN_WIDTH*SCREEN_HEIGHT];
	quit = 0; // don't quit yet!
	iterations = 10; //this becomes the "N" - the number of iterations over all ABABAB

	//ab = "AB";
	ab = "BBBBBBAAAAAA"; //zircon
	//ab = "BBBAAA";
	//ab = "AB";
	//ab = "AABAB"; //jellyfish


	lambda_max = .6;
	lambda_min = -2.5;
	selectionRectangleSize = 100;
	showSelectionRectangle = 0;
	lock = 3;
	//numberOfComputationThreads = 4; covered by a #define now
}

Engine::~Engine()
{
	SDL_DestroyTexture(radio);
	SDL_DestroyTexture(chaos);
	SDL_DestroyRenderer(mainRenderer);
	SDL_DestroyRenderer(colors);
	SDL_DestroyWindow(win);
	SDL_DestroyWindow(colorWin);
	IMG_Quit();
	SDL_Quit();
	if(logger) 	fclose(logger);

}


void Engine::LogStatus() {
    //	logger = NULL;
	//fopen_s(&logger, "../../Images/log.txt", "a"); //arleady opened?
	if (!logger) {
		perror("LogStatus Error:");
		//cerr << "LogStatus Error: Problem opening logger . . . images/log not retained!" << endl;
	}
	else {
		fprintf(logger, "[%4d] Range [%f, %f]-[%f, %f] a=[%f] iters=[%d]*ab=%s\n", imageCount,
			Bstart_value, Bend_value, Astart_value, Aend_value, continuousRegionA, iterations, ab.c_str());
//		fclose(logger); - outputwindow closes this
	}
}


void Engine::outputWindow() {
	SDL_Surface*screen = SDL_GetWindowSurface(win);
	//SDL_LockSurface(screen);
	//	IMG_SavePNG(screen, "justsaveme.png");
	//SDL_SaveBMP(screen, "justsaveme.bmp");
	//SDL_Surface*shot = SDL_PNGFormatAlpha(screen);
	//IMG_SavePNG(shot, "justsaveme3.png");
	//SDL_SavePNG(screen, "justsaveme2.png");
	//SDL_UnlockSurface(screen);

#ifdef BMP
	unsigned char * pixels = new unsigned char[SCREEN_WIDTH*SCREEN_HEIGHT * 3];
#endif
	unsigned char * pixels2 = new unsigned char[SCREEN_WIDTH*SCREEN_HEIGHT * 3];
	
	for (int j = 0; j < SCREEN_HEIGHT; j++) {
		for (int i = 0; i < SCREEN_WIDTH; i++) {
			int r, g, b;
			int h = SCREEN_HEIGHT - 1 - j;
			getColor(i, h, r, g, b);
#ifdef BMP
			pixels[i * 3 + j*SCREEN_WIDTH * 3 + 0] = b;
			pixels[i * 3 + j*SCREEN_WIDTH * 3 + 1] = g;
			pixels[i * 3 + j*SCREEN_WIDTH * 3 + 2] = r;
#endif
			pixels2[i * 3 + j*SCREEN_WIDTH * 3 + 0] = r;
			pixels2[i * 3 + j*SCREEN_WIDTH * 3 + 1] = g;
			pixels2[i * 3 + j*SCREEN_WIDTH * 3 + 2] = b;

			//	pixels[i*4 + h*SCREEN_WIDTH*4  + 0] = 255;
		}
		
	}

	
	stringstream oss; oss << imageCount;
	
	string PNGfilename = "../../Images/" + ab + oss.str() + ".png";
	/*
	string filenameascii = ab + oss.str() + "ascii.ppm";
	outToP3(filenameascii.c_str(), pixels2, SCREEN_WIDTH, SCREEN_HEIGHT);
	string filenamebinar = ab + oss.str() + "binar.ppm";
	outToP6(filenamebinar.c_str(), pixels2, SCREEN_WIDTH, SCREEN_HEIGHT);
	*/
	
#ifdef BMP
	string BMPfilename = ab + oss.str() + ".bmp";
	outToBMP(BMPfilename.c_str(), pixels, SCREEN_WIDTH, SCREEN_HEIGHT);
#endif
	printf("Starting a write to PNGfilename=%s\n", PNGfilename.c_str());
	outToPNG(PNGfilename.c_str(), pixels2, SCREEN_WIDTH, SCREEN_HEIGHT);
	cout << "Outputting pixels to image(s): " << ab << imageCount << ".png" << endl;

//	logger = NULL;
//	fopen_s(&logger, "../../Images/log.txt", "a");
	if (!logger) {
//		cerr << "outputWindow Error: Problem opening logger . . . Writing output status" << endl;
		perror("outputFile:");
	}
	else {
		fprintf(logger, "Wrote File \"%s\"\n", PNGfilename.c_str());
		fflush(logger);
//		fclose(logger); // this was opened by logstatus
	}
#ifdef BMP
	if(pixels)
	  delete[] pixels;
#endif
	if(pixels2) 
	  delete[] pixels2; //TODO - use only one of these later

#ifdef PAUSE_EVERY_FRAME
	//system("pause");
	paused = true; //press c to unpause
#endif
}

void Engine::setMinMax(double min, double max) {
	lambda_min = min;
	lambda_max = max;
}

int Engine::getLock() {
	return lock;
}

void Engine::setLock(int l) {
	lock = l;
}

void logSDLError(ostream &out, string msg) {
	out << msg.c_str() << " Error: " << SDL_GetError() << endl;
}

void logSuccess(ostream &out, string msg) {
	cout << msg.c_str() << " - Successful" << endl;
}

int Engine::init() {

	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		cout << "SDL_INIT Error: " << SDL_GetError() << endl;
		return 1;
	}

	win = SDL_CreateWindow("SuPeR Fractal Demo", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (win == nullptr) {
		logSDLError(cout,"SDL_CreateWindow");
		SDL_Quit();
		return 1;
	}

	colorWin = SDL_CreateWindow("Colors", 100, SCREEN_HEIGHT+150, SCREEN_WIDTH, COLOR_SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (colorWin == nullptr) {
		logSDLError(cout, "SDL_CreateWindow");
		SDL_Quit();
		return 1;
	}

	mainRenderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (mainRenderer == nullptr) {
		SDL_DestroyWindow(win);
		cout << "SDL_CreateRenderer Error: " << SDL_GetError() << endl;
		SDL_Quit();
		return 1;
	}

	colors = SDL_CreateRenderer(colorWin, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (colors == nullptr) {
		SDL_DestroyWindow(colorWin);
		cout << "SDL_CreateRenderer Error: " << SDL_GetError() << endl;
		SDL_Quit();
		return 1;
	}

	/*
	char path[] = "simple.bmp";
	radio = loadTexture("simple.bmp", mainRenderer);
	chaos = loadTexture("chaos.bmp", mainRenderer);
	if (radio == nullptr || chaos == nullptr) {
		SDL_DestroyRenderer(mainRenderer);
		SDL_DestroyWindow(win);
		return 1;
	}


	if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG) {
		logSDLError(std::cout, "IMG_Init");
		SDL_Quit();
		return 1;
	}
	*/




	return 0;
}

void Engine::keypress(SDL_Event &e) {
	double a1, a2, b1, b2;
	double adj_amt = 0.5;
	switch (e.key.keysym.sym) {
	case SDLK_1:
		break;
	case SDLK_c:
		paused = false;
		break;
	case SDLK_s:
		showSelectionRectangle = (showSelectionRectangle + 1 ) % 2;
		break;
	case SDLK_g:
		cout << "Enter an a for continuous Region: ";
		cin >> continuousRegionA;
		setContinuousRegionAandAdjustRegion(continuousRegionA);
		break;
	case SDLK_d:
		cout << "Enter a bunch of parameters:";
		cout << "continuous a value: ";
		cin >> continuousRegionA;
		cout << "A-B range. start then end: ";
		cin >> Astart_value >> Aend_value;
		cin >> Bstart_value >> Bend_value;
		break;
	case SDLK_r:
		if (showSelectionRectangle) {
			cout << "Adjusting region to the given selection Rectangle. New values are:" << endl;
			a2 = ((SCREEN_HEIGHT - selectionRectangleY) / (double)SCREEN_HEIGHT) * (Aend_value - Astart_value) + Astart_value;
			a1 = ((SCREEN_HEIGHT - selectionRectangleY - selectionRectangleSize) / (double)SCREEN_HEIGHT) * (Aend_value - Astart_value) + Astart_value;
			b1 = (selectionRectangleX / (double)SCREEN_WIDTH) * (Bend_value - Bstart_value) + Bstart_value;
			b2 = ((selectionRectangleX + selectionRectangleSize) / (double)SCREEN_WIDTH) * (Bend_value - Bstart_value) + Bstart_value;
			Astart_value = a1;
			Aend_value = a2;
			Bstart_value = b1;
			Bend_value = b2;
			cout << "b= " << Bstart_value << ", " << Bend_value
				 << " -- a= " << Astart_value << ", " << Aend_value << endl;
			showSelectionRectangle = 0;
		}
		break;
	case SDLK_MINUS: case SDLK_KP_MINUS:
		a1 = Aend_value - Astart_value; b1 = Bend_value - Bstart_value;
		Astart_value = Astart_value - a1 / 4; Aend_value = Aend_value + a1 / 4;
		if (Astart_value < 0) { Aend_value = Aend_value - Astart_value; Astart_value = 0; }
		Bstart_value = Bstart_value - b1 / 4; Bend_value = Bend_value + b1 / 4;
		if (Bstart_value < 0) { Bend_value = Bend_value - Bstart_value; Bstart_value = 0; }

		cout << Astart_value << ", " << Aend_value << " -- " << Bstart_value << ", " << Bend_value << endl;
		break;
	case SDLK_PLUS: case SDLK_KP_PLUS: case SDLK_EQUALS:
		a1 = Aend_value - Astart_value; b1 = Bend_value - Bstart_value;
		Astart_value = Astart_value + a1/4; Aend_value = Aend_value - a1/4;
		Bstart_value = Bstart_value + b1/4; Bend_value = Bend_value - b1/4;
		cout << Astart_value << ", " << Aend_value << " -- " << Bstart_value << ", " << Bend_value << endl;
		break;
	case SDLK_UP:
		Astart_value = Astart_value + adj_amt; Aend_value = Aend_value + adj_amt;
		cout << Astart_value << ", " << Aend_value << " -- " << Bstart_value << ", " << Bend_value << endl;
		break;
	case SDLK_DOWN:
		Astart_value = Astart_value - adj_amt; Aend_value = Aend_value - adj_amt;
		if (Astart_value < 0) { Aend_value = Aend_value - Astart_value; Astart_value = 0; }
		cout << Astart_value << ", " << Aend_value << " -- " << Bstart_value << ", " << Bend_value << endl;
		break;
	case SDLK_LEFT:
		Bstart_value = Bstart_value - adj_amt; Bend_value = Bend_value - adj_amt;
		if (Bstart_value < 0) { Bend_value = Bend_value - Bstart_value; Bstart_value = 0; }
		cout << Astart_value << ", " << Aend_value << " -- " << Bstart_value << ", " << Bend_value << endl;
		break;
	case SDLK_RIGHT:
		Bstart_value = Bstart_value + adj_amt; Bend_value = Bend_value + adj_amt;
		cout << Astart_value << ", " << Aend_value << " -- " << Bstart_value << ", " << Bend_value << endl;
		break;
	case SDLK_a:
		Astart_value = Bstart_value = 1.0;
		break;
	case SDLK_p: 
		setContinuousRegionAandAdjustRegion(getContinuousRegionA() + .1);
		cout << "ContinuousRegionA is now: " << continuousRegionA << endl;
		cout << Astart_value << ", " << Aend_value << " -- " << Bstart_value << ", " << Bend_value << endl;
		break;
	case SDLK_l: //change which function will be displayed - newX,newFrac,oldFrac
		lock = (lock + 1) % 4;
		cout << "New function rendered is = " << lock << endl;
		break;
	case SDLK_o:
		outputWindow();
		break;
	case SDLK_ESCAPE:
		quit = true;
		break;
	default:
		break;

	}

}

void Engine::colormousewheel(SDL_Event &e) {
	int x, y;
	if (colorMode >= 2) return; //it means we're in rainbow territory
	SDL_GetMouseState(&x, &y);
//	cout << "movement in y is: " << e.wheel.y << endl;
	int id = (int)(NUM_COLORS * x / (double)SCREEN_WIDTH);
	for (int i = 0; i < 3; i++) {
		colorsList[id][i] = colorsList[id][i] + e.wheel.y*15;
		if (colorsList[id][i] < 0) colorsList[id][i] = 0;
		if (colorsList[id][i] > 255) colorsList[id][i] = 255;
	}
	drawColorBar();
}

void Engine::colormousepress(SDL_Event &e) {
	int id;
	switch (e.button.button) {
	case SDL_BUTTON_RIGHT:
		colorMode = (colorMode + 1) % 4;
		cout << "colorMode=" << colorMode << endl;
		drawColorBar(); //redraw the lower window
		break;
	case SDL_BUTTON_LEFT:
		if (colorMode >= 2) return; //should actually select a color here
		id = (int)(NUM_COLORS * e.button.x / (double) SCREEN_WIDTH);
		if (id < 0 || id > NUM_COLORS) break;
		cout << "New color id=[" << id << "] current=[" 
			 << colorsList[id][0] <<", " << colorsList[id][1] << ", " << colorsList[id][2]
			 <<" as ub 0-255 r,g,b: ";
		cin >> colorsList[id][0] >> colorsList[id][1] >> colorsList[id][2];
		drawColorBar();
		break;
	default:
		break;
	}

}

void Engine::mousewheel(SDL_Event &e) {
	if (showSelectionRectangle) {
		selectionRectangleX -= e.wheel.y;
		selectionRectangleY -= e.wheel.y;
		selectionRectangleSize = selectionRectangleSize + e.wheel.y*2;
	}
}

void Engine::mousepress(SDL_Event &e) {

	switch (e.button.button) {

	case SDL_BUTTON_LEFT:
		int r, g, b;
		getColor(e.button.x, SCREEN_HEIGHT -1 - e.button.y, r, g, b);
		outPixelInfo(cout, e.button.x, SCREEN_HEIGHT - 1 - e.button.y);
//		cout << "Chosen click location [" << e.button.x << ", " << e.button.y << "] " << endl;
//		cout << "Lambda at this position is: " <<  getPixel(e.button.x, SCREEN_HEIGHT - 1 - e.button.y) << endl;
		cout << "Color is [" << r << ", " << g << ", " << b << "]" << endl;
		break;
	case SDL_BUTTON_RIGHT:
		showSelectionRectangle = ( showSelectionRectangle + 1 ) % 2;
		selectionRectangleX = e.button.x - selectionRectangleSize/2;  
		selectionRectangleY = e.button.y - selectionRectangleSize/2;
		cout << "rectangle at: " << selectionRectangleX << " " << selectionRectangleY << " of size " << selectionRectangleSize << endl;
		double a2 = ((SCREEN_HEIGHT - selectionRectangleY) / (double)SCREEN_HEIGHT) * (Bend_value - Bstart_value) + Bstart_value;
		double a1 = (((SCREEN_HEIGHT - selectionRectangleY) - selectionRectangleSize ) / (double)SCREEN_HEIGHT) * (Bend_value - Bstart_value) + Bstart_value;
		double b1 = (selectionRectangleX / (double)SCREEN_WIDTH) * (Aend_value - Astart_value) + Astart_value;
		double b2 = ((selectionRectangleX + selectionRectangleSize ) / (double)SCREEN_WIDTH) * (Aend_value - Astart_value) + Astart_value;
		cout << "New range would be  b=[" << b1 << ", " << b2 << "] " << "a=[" << a1 << ", " << a2 << "] " << endl;
		if (showSelectionRectangle == 1) {
			reta1 = Astart_value;
			reta2 = Aend_value;
			retb1 = Bstart_value;
			retb2 = Bend_value;
			Astart_value = a1;
			Aend_value = a2;
			Bstart_value = b1;
			Bend_value = b2;
		}
		else {
			Astart_value = reta1;
			Aend_value = reta2;
			Bstart_value = retb1;
			Bend_value = retb2;
		}
		break;
	}

}

/*decide what the event is and which function is responsible for handling it! */
void Engine::poll() {
	SDL_Event e;


	while (SDL_PollEvent(&e)) {
		if (e.window.windowID == SDL_GetWindowID(win)) {
			if (e.type == SDL_QUIT) {
				quit = true;
			}
			if (e.type == SDL_KEYDOWN) {
				keypress(e);

			}
			if (e.type == SDL_MOUSEBUTTONDOWN) {
				mousepress(e);
			}
			if (e.type == SDL_MOUSEWHEEL) {
				mousewheel(e);
			}
		}
		else if(e.window.windowID == SDL_GetWindowID(colorWin)) {
			if (e.type == SDL_MOUSEBUTTONDOWN) {
				colormousepress(e);
			}
			if (e.type == SDL_MOUSEWHEEL) {
				colormousewheel(e);
			}
		}
	}

}

void Engine::getABRange(double&AS, double&AE, double&BS, double&BE) {
	AS = Astart_value;
	AE = Aend_value;
	BS = Bstart_value;
	BE = Bend_value;
}

int Engine::getQuit() {
	return quit;
}
void Engine::setQuit(int i) { quit = i;  }

void Engine::getScreenSize(int &W, int&H) {
	W = SCREEN_WIDTH;
	H = SCREEN_HEIGHT;
}
string Engine::getAB() {
	return ab;
}
FILE* Engine::getLogger() { return logger; }
double Engine::getContinuousRegionA() {
	return continuousRegionA;
}

double Engine::getPixel(int x, int y) {
	return pixels[x + y*SCREEN_WIDTH];
}


void Engine::outPixelInfo(std::ostream &out, int x, int y) {
	out << "[" << pixelInfo[x + y*SCREEN_WIDTH].x << ", " << pixelInfo[x + y*SCREEN_WIDTH].y << "] -- "
		<< "[" << pixelInfo[x + y*SCREEN_WIDTH].a << ", " << pixelInfo[x + y*SCREEN_WIDTH].b << "] ==> "
		<< pixelInfo[x + y*SCREEN_WIDTH].value << endl;
}

void Engine::setPixel(int x, int y, double val, double a, double b) {
	pixels[x + y*SCREEN_WIDTH] = val;
	pixelInfo[x + y*SCREEN_WIDTH].x = x;
	pixelInfo[x + y*SCREEN_WIDTH].y = y;
	pixelInfo[x + y*SCREEN_WIDTH].a = a;
	pixelInfo[x + y*SCREEN_WIDTH].b = b;
	pixelInfo[x + y*SCREEN_WIDTH].value = val;

}

//does a sem wait on this index sem
void Engine::waitReady(int index) {
	if(index < NUM_THREADS) 
		SDL_SemWait(threadReady[index]);
}

void Engine::postReady(int index) {
	if (index < NUM_THREADS)
		SDL_SemPost(outputReady[index]);
}
//sends sem post to all threads sem
void Engine::signalReady() {
	for (int i = 0; i < NUM_THREADS; i++) {
		SDL_SemPost(threadReady[i]);
	}
}

//checks the sem values waiting until they've all posted
void Engine::waitAllReady() {
	for (int i = 0;i < NUM_THREADS; i++)
		SDL_SemWait(outputReady[i]);
}

int Engine::getIterations() {
	return iterations;
}

void Engine::increaseIterationsByFactor(int factor) {
	iterations = iterations * factor;
}

void Engine::increaseIterationsByAddition(int factor) {
	iterations = iterations + factor;
}

void Engine::increaseImageCount() {
	imageCount++;
}

void Engine::setContinuousRegionAandAdjustRegion(double amt) {
	continuousRegionA = amt;
	Aend_value = Bend_value = 0.5 * sqrt(exp(2 * continuousRegionA) + 8 * exp(continuousRegionA)) + 0.5 * exp(continuousRegionA) + 1;
	Astart_value = Bstart_value = 1;//Aend_value - 1.5;
#ifdef BLOWERBOUND
	Astart_value = Bstart_value = BLOWERBOUND;
#endif

#ifdef BMINIMUM
	if (Astart_value < BMINIMUM) {
		Astart_value = Bstart_value = BMINIMUM;
	}
#endif
}

int Engine::getImageCount() {
	return imageCount;
}


SDL_Renderer* Engine::getRen() {
	return mainRenderer;
}

/*compute the color for this lyaponuv exponent and store it in the rc/gc/bc variables*/
void Engine::getColor(double l, int &rc, int &gc, int &bc) {

	int c = 0; //desired final color
	int basec = 200;
	//modify l according to desires!
	const int NUM_COLS = NUM_COLORS - 1; // the last one is the positive vlaues

										 //this is for negative inf
	double poscol[] = { 0,0,200 };

	if (l < lambda_min) {
		double e = pow(2.0,l) / pow(2.0, lambda_min);
		rc = (int)( colorsList[0][0]*e);
		gc = (int)( colorsList[0][1]*e);
		bc = (int)( colorsList[0][2]*e);
		return;
	}
	else if (l == 0) {
		rc = 255, gc = 255, bc = 255;
		return;
	}
	else if (l < 0) {
		for (int i = 1; i < NUM_COLS; i++) {
			if (l < lambda_min*offsets[i]) {
				double delta = (offsets[i] - offsets[i - 1])*lambda_min;
				double distFromI = (l - lambda_min * offsets[i - 1]) / delta; //weight to give i color
				rc = (int)(colorsList[i][0] * distFromI + colorsList[i - 1][0] * (1 - distFromI));
				gc = (int)(colorsList[i][1] * distFromI + colorsList[i - 1][1] * (1 - distFromI));
				bc = (int)(colorsList[i][2] * distFromI + colorsList[i - 1][2] * (1 - distFromI));
				return;
			}
		}
	}
	else { //it is positive
		double c = l / lambda_max;
		rc = (int)(colorsList[NUM_COLS][0]);
		gc = (int)(colorsList[NUM_COLS][1]);
		bc = (int)(colorsList[NUM_COLS][2]);
	}
}

void Engine::drawColorBar() {
	drawColors(colors);
}
void Engine::getColor(int x, int y, int&rc, int&gc, int&bc) {
	double l = getPixel(x, y);
	return getColor(l, rc, gc, bc);
}

void Engine::drawColors(SDL_Renderer *colors) {
	SDL_RenderClear(colors);
	int (*cl)[3] = colorsList;
	for (int i = 0; i < SCREEN_WIDTH; i++) {
		int r = 1, g = 1, b = 1;
		int nc = NUM_COLORS;
		if (colorMode == 2 || colorMode == 3) {
			cl = rainbowColorsList;
			nc = NUM_RAINBOW_COLORS;
		}
		double delta = SCREEN_WIDTH / (double)nc;
		double dist = i / (double)SCREEN_WIDTH * nc;
		int bi = (int)dist;
		
		if (colorMode%2 == 0) {
			r = cl[bi][0]; g = cl[bi][1]; b = cl[bi][2];
		}
		else if (colorMode%2 == 1) {
			int b2 = bi + 1; if (b2 >= NUM_COLORS) b2 = NUM_COLORS - 1;
			double pct = dist - bi;
			r = (int)(cl[bi][0] * (1 - pct) + cl[b2][0] * pct);
			g = (int)(cl[bi][1] * (1 - pct) + cl[b2][1] * pct);
			b = (int)(cl[bi][2] * (1 - pct) + cl[b2][2] * pct);
		}
		SDL_SetRenderDrawColor(colors, r, g, b, 255);
		SDL_RenderDrawLine(colors, i, 0, i, 150);
	}
	SDL_RenderPresent(colors);
}

void Engine::drawSelectionRectangle(SDL_Renderer *ren) {
	if (showSelectionRectangle) {
		SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
		SDL_Rect rect = { selectionRectangleX, selectionRectangleY, selectionRectangleSize, selectionRectangleSize };
		SDL_RenderDrawRect(ren, &rect);
	}
}

void Engine::renderAndPoll(SDL_Renderer *ren) {

	poll();
	SDL_RenderClear(ren);
	for (int j = 0; j < SCREEN_HEIGHT; j++) {
		for (int i = 0; i < SCREEN_WIDTH; i++) {

			int r, g, b;
			getColor(i, j, r, g, b);

			SDL_SetRenderDrawColor(ren, r, g, b, 255);

			SDL_RenderDrawPoint(ren, i, SCREEN_HEIGHT - 1 - j);
		}
	}


	drawSelectionRectangle(ren);
	SDL_RenderPresent(ren);


}

void Engine::start(int s) {
	//if s == 1 then split the computation into a couple threads

	quit = 0;

//	SDL_Thread *thread = SDL_CreateThread(renderThread, "RenderThread", (void*)this);
//	if (thread == nullptr) { 		logSDLError(cout, "SDL_CreateThread"); 	}
//	else { 		int threadReturnValue; 		SDL_WaitThread(thread, &threadReturnValue);	}

	int div = 1;
	double deltaW = SCREEN_WIDTH / (double)div; //j i s height
	int ijlo = 0, ijhi = SCREEN_HEIGHT / 2;
	int iilo = 0, iihi = (int)deltaW;
	int ilo = iilo, ihi = iihi;
	int jlo = ijlo, jhi = ijhi;
#ifdef MULTIPLE_THREADS

	div = NUM_THREADS / 2;
	deltaW = SCREEN_WIDTH / (double)div;
	iihi = deltaW;
	ihi = deltaW;
	

	for (int i = 0; i < NUM_THREADS; i++) {
		//threadReady.push_back(0); //set all threads to not ready
		threadReady[i] = SDL_CreateSemaphore(1); // start with a value of 1
		outputReady[i] = SDL_CreateSemaphore(0);
#else
	int i = 0;
#endif
		computationThreadInfo *mythreadinfo = new computationThreadInfo;
		mythreadinfo->engine = this;
		mythreadinfo->index = i;
		mythreadinfo->ilo = ilo;
		mythreadinfo->ihi = ihi;
		mythreadinfo->jlo = jlo;
		mythreadinfo->jhi = jhi;
		if (i == div - 2) { // include entire width
			ilo = ihi;
			ihi = SCREEN_WIDTH;
		}
		else if (i == div - 1) { //reset i
			ilo = iilo;
			ihi = iihi;
		}
		else {
			ilo = ihi;
			ihi = (int)(ilo + deltaW);
		}
		if (i  ==  div -1) {
			jlo = jhi;
			jhi = SCREEN_HEIGHT;
		}

		SDL_Thread *thread = SDL_CreateThread(computationThread, "ComputationThread", (void*)mythreadinfo);
		if (thread == nullptr) {
			logSDLError(cout, "SDL_CreateThread - computationThread");
		}
		else {
			//		int threadReturn;
				//	SDL_WaitThread(thread, &threadReturn);
		}
#ifdef MULTIPLE_THREADS
	}
#endif
	
}

SDL_Texture* Engine::loadTexture(char *filename, SDL_Renderer *ren) {
	SDL_Texture *texture = IMG_LoadTexture(ren, filename);
	if (texture == nullptr) {
		logSDLError(cout, "CreateTextureFromSurface");
	}
	else {
		logSuccess(cout, "LoadTexture: \"" + (string)filename + "\"");
	}
	return texture;
}

void Engine::renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y) {
	int w, h;
	SDL_QueryTexture(tex, NULL, NULL, &w, &h);
	renderTexture(tex, ren, x, y, w, h);
}

void Engine::renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y, int w, int h, SDL_Rect *clip) {
	SDL_Rect dst;
	dst.x = x;
	dst.y = y;
	dst.w = w;
	dst.h = h;
	SDL_RenderCopy(ren, tex, clip, &dst);
}
