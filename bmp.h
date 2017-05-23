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
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>


void outToBMP(const char*filename, unsigned char*pixels, int w, int h) {

	FILE *f;
	unsigned char *img = pixels;
	int filesize = 54 + 3 * w*h;  //w is your image width, h is image height, both int
	//img = (unsigned char *)malloc(3 * w*h);
	//memset(img, 0, sizeof(img));
	/*
	for (int i = 0; i<w; i++)
	{
		for (int j = 0; j<h; j++)
		{
			x = i; y = (h - 1) - j;
			r = red[i][j] * 255;
			g = green[i][j] * 255;
			b = blue[i][j] * 255;
			if (r > 255) r = 255;
			if (g > 255) g = 255;
			if (b > 255) b = 255;
			img[(x + y*w) * 3 + 2] = (unsigned char)(r);
			img[(x + y*w) * 3 + 1] = (unsigned char)(g);
			img[(x + y*w) * 3 + 0] = (unsigned char)(b);
		}
	}
	*/

	unsigned char bmpfileheader[14] = { 'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0 };
	unsigned char bmpinfoheader[40] = { 40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0 };
	unsigned char bmppad[3] = { 0,0,0 };

	bmpfileheader[2] = (unsigned char)(filesize);
	bmpfileheader[3] = (unsigned char)(filesize >> 8);
	bmpfileheader[4] = (unsigned char)(filesize >> 16);
	bmpfileheader[5] = (unsigned char)(filesize >> 24);

	bmpinfoheader[4] = (unsigned char)(w);
	bmpinfoheader[5] = (unsigned char)(w >> 8);
	bmpinfoheader[6] = (unsigned char)(w >> 16);
	bmpinfoheader[7] = (unsigned char)(w >> 24);
	bmpinfoheader[8] = (unsigned char)(h);
	bmpinfoheader[9] = (unsigned char)(h >> 8);
	bmpinfoheader[10] = (unsigned char)(h >> 16);
	bmpinfoheader[11] = (unsigned char)(h >> 24);

	fopen_s(&f, filename, "wb");
	fwrite(bmpfileheader, 1, 14, f);
	fwrite(bmpinfoheader, 1, 40, f);
	for (int i = 0; i < h; i++)
	{
		fwrite(img + (w*(h - i - 1) * 3), 3, w, f);
		fwrite(bmppad, 1, (4 - (w * 3) % 4) % 4, f);
	}
	fclose(f);


}

//ascii ppm
void outToP3(const char*filename, unsigned char*pixels, int w, int h) {
	std::ofstream fout(filename);
	fout << "P3\n " << w << " " << h << "\n " << 255 << "\n";
	for (int j = 0; j < h; j++) {
		for (int i = 0; i < w; i++) {
			fout << (int) pixels[i * 3 + j*w * 3+ 0 ] << " " 
				<< 	(int)pixels[i * 3 + j*w * 3 + 1] << " "
				<<	(int)pixels[i * 3 + j*w * 3 + 2] << " ";
		}
		fout << std::endl;
	}
	fout.close();
}

//binary ppm
#include <iostream>
void outToP6(const char*filename, unsigned char*pixels, int w, int h) {
	FILE* f;
	fopen_s(&f, filename, "wb");
	fprintf(f,"P3\n %d %d\n 256\n", w, h);
	fwrite(pixels, sizeof(unsigned char), w*h*3, f);
	fclose(f);
}


#include <png.h>
int outToPNG(const char* filename, unsigned char*pixels, int w, int h) {
	char* title = "SuPeRFractal";
	int code = 0;
	FILE *fp = NULL;
	png_structp png_ptr = NULL;
	png_infop info_ptr = NULL;
	png_bytep row = NULL;

	// Open file for writing (binary mode)
	//	fp = fopen(filename, "wb");
	
	fopen_s(&fp, filename, "wb");

	if (fp == NULL) {
		fprintf(stderr, "Could not open file %s for writing\n", filename);
		code = 1;
		goto final;
	}

	// Initialize write structure
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL) {
		fprintf(stderr, "Could not allocate write struct\n");
		code = 1;
		goto final;
	}

	// Initialize info structure
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		fprintf(stderr, "Could not allocate info struct\n");
		code = 1;
		goto final;
	}

	// Setup Exception handling
	if (setjmp(png_jmpbuf(png_ptr))) {
		fprintf(stderr, "Error during png creation\n");
		code = 1;
		goto final;
	}


	png_init_io(png_ptr, fp);

	// Write header (8 bit colour depth)
	png_set_IHDR(png_ptr, info_ptr, w, h,
		8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	// Set title
	if (title != NULL) {
		png_text title_text;
		title_text.compression = PNG_TEXT_COMPRESSION_NONE;
		title_text.key = "SuPeRFractals";
		title_text.text = title;
		png_set_text(png_ptr, info_ptr, &title_text, 1);
	}

	png_write_info(png_ptr, info_ptr);
	
	// Allocate memory for one row (3 bytes per pixel - RGB)
	//row = (png_bytep)malloc(3 * w * sizeof(png_byte));

	// Write image data
	int y;//int x, y;
	for (y = 0; y < h; y++) {
		row = pixels + y*w * 3; //get the row from the pointer into the pixel data arleady
		png_write_row(png_ptr, row);
	}

	// End write
	png_write_end(png_ptr, NULL);
	

final:
	if (fp != NULL) fclose(fp);
	if (info_ptr != NULL) png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
	if (png_ptr != NULL) png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
//	if (row != NULL) free(row);

	return code;
}
