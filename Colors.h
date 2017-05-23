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

#define NUM_COLORS 12 //there's technically one more for positive values
int colorsList[NUM_COLORS][3] = {
	{ 128,10,10 }, //darker red
	{ 50, 50, 50 }, //dark grey
	{ 128, 128, 128 }, //middle grey
	{ 0,255,0 }, //4 - index 3
	{ 128,64, 0 }, //orange - darker
	{ 255, 128,128 },
	{ 255, 255,255 }, //white
	{ 255,128,0 }, //orange
	{ 128, 128, 0 }, //darker yellow
	{ 190, 190,0 },
	{ 255, 255, 0 }, //bright yellow
	{ 0,0,255 } //LAsT at index 11 - blue
};



//num offsets should equal num cols
double offsets[] = { 1, .9, 0.8, 0.7, .3, .25, .2, .15, .1, .05, 0, -1 }; //color ranges

#define NUM_RAINBOW_COLORS 13
int rainbowColorsList[NUM_RAINBOW_COLORS][3] = {
	{255,0,0},    //red
	{255,128,0}, //orange
	{255,255,0}, //yellow
	{128,255,0},
	{0,255,0},   //green
	{0,255,128},
	{0,255,255}, 
	{0,128,255},
	{0,0,255},  //blue
	{128,0,255},
	{255,0,255}, //purple
	{255,0,128},
	{255,0, 0}  
};
