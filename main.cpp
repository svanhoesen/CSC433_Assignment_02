///
/// \file main.cpp
/// \brief SDL Demo Code
/// \author Joshua A. Levine <josh@email.arizona.edu>
/// \co-author Steffan Van Hoesen <svanhoesen@email.arizona.edu>
/// \date 01/15/18
///
/// This code provides an introductory demonstration of SDL.  When run, a
/// small window is displayed that draws an image using an SDL_Texture
///


/*
 ***********************************************************************

 Copyright (C) 2018, Joshua A. Levine
 University of Arizona

 Permission is hereby granted, free of charge, to any person obtaining
 a copy of this software and associated documentation files (the
 "Software"), to deal in the Software without restriction, including
 without limitation the rights to use, copy, modify, merge, publish,
 distribute, sublicense, and/or sell copies of the Software, and to
 permit persons to whom the Software is furnished to do so, subject to
 the following conditions:

 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.

 ***********************************************************************
 */


 //include SDL2 libraries
#include <SDL.h>

//C++ includes
#include <iostream>
#include <cfloat>
#include <cmath>
#include <vector>
#include <algorithm>
#include <string>
#include <fstream>
#include <sstream>
#include <exception>

using namespace std;

class ppm {
	void init();
	//info about the PPM file (height and width)
	unsigned int n_r;
	unsigned int n_c;

  public:
	//arrays for storing the Red (r), Green (g), and Blue (b) values
	std::vector<unsigned char> r;
	std::vector<unsigned char> g;
	std::vector<unsigned char> b;

	//A raw data array of characters.  Each column is drawn using the r, g, and b
    //arrays to produce an image from the file that was originally input.
	unsigned char *data;
	unsigned int height;
	unsigned int width;
	unsigned int max_color_val;

	//total number of elements (in this case pixels)
	unsigned int size;

	ppm();
	//create a PPM object and fill it with data stored in the PPM file referenced as fileName 
	ppm(const std::string &fileName);
	//create an "epmty" PPM image with a given width and height; the Red, Green, and Blue arrays are filled with zeros
	ppm(const unsigned int _width, const unsigned int _height);
	//read the PPM image from the PPM file referenced as fileName
	void load(const std::string &fileName);
	//write the PPM image in the PPM file referenced as fileName
	void save(const std::string &fileName);

	unsigned char* getData();

	void rescaler(float gain, float bias, float gamma);

	void resizer(int w, int h, int NC, int NR);
};

///This will initialize a PPM object to default values
void ppm::init() {
	width = 0;
	height = 0;
	max_color_val = 255;
}

///This will create a PPM object
ppm::ppm() {
	init();
}

///This will create a PPM object then fill it with data stored in the PPM file referenced as fileName 
///
/// \param fileName the referenced PPM file
///
ppm::ppm(const std::string &fileName) {
	init();
	load(fileName);
}

///This will create an "epmty" PPM image with a given width and height; 
///the  Red, Green, and Blue arrays are filled with zeros
///
/// \param _width the number of rows
/// \param _height the number of columns
///
ppm::ppm(const unsigned int _width, const unsigned int _height) {
	init();
	width = _width;
	height = _height;
	n_r = height;
	n_c = width;
	size = width * height;

	// resize and fill r, g and b arrays with 0
	r.resize(size);
	g.resize(size);
	b.resize(size);
	data = new unsigned char[(size * 3)];
}

///This will load the PPM image from the PPM file referenced as fileName
///If there are any errors in the format of the file errors are reported or
///exceptions are thrown.
///
/// \param fileName the referenced PPM file
/// 
void ppm::load(const std::string &fileName) {
	std::ifstream input(fileName.c_str(), std::ios::in | std::ios::binary);
	//Check to see if the file was opened, if it wasn't report an error.
	if (input.is_open()) {
		std::string line;
		std::getline(input, line);
		//If the first line doesn't contain "P6" report an error
		if (line != "P6") {
			std::cout << "Error. Unrecognized file format." << std::endl;
			return;
		}
		std::getline(input, line);
		while (line[0] == '#') {
			std::getline(input, line);
		}
		std::stringstream dimensions(line);
		//If the dimensions can't be obtained from the line catch the exception and report the error
		try {
			dimensions >> width;
			dimensions >> height;
			n_r = height;
			n_c = width;
		}
		catch (std::exception &ex) {
			std::cout << "Header file format error. " << ex.what() << std::endl;
			return;
		}
		std::getline(input, line);
		std::stringstream max_val(line);
		//If the maximum color value can't be obtained from the line catch the exception and report the error
		try {
			max_val >> max_color_val;
		}
		catch (std::exception &ex) {
			std::cout << "Header file format error. " << ex.what() << std::endl;
			return;
		}
		size = width * height;
		r.reserve(size); //resize the r vector
		g.reserve(size); //resize the g vector
		b.reserve(size); //resize the b vector
		char channel;
		//read and store color values from the input to the r, g, and b vectors arrays
		///TODO: load directly into unsigned char data
		int loc = 0;
		data = new unsigned char[(size * 3)];
		for (unsigned int i = 0; i < size; ++i) {
			input.read(&channel, 1);
			r[i] = (unsigned char)channel;
			data[loc] = (unsigned char)channel;
			loc++;
			input.read(&channel, 1);
			g[i] = (unsigned char)channel;
			data[loc] = (unsigned char)channel;
			loc++;
			input.read(&channel, 1);
			b[i] = (unsigned char)channel;
			data[loc] = (unsigned char)channel;
			loc++;
		}
		
	}
	else {
		std::cout << "Error. Unable to open " << fileName << std::endl;
	}
	input.close();
}

///save the new PPM image in specified file
///
/// \param fileName the referenced PPM file
///

void ppm::save(const std::string &fileName) {
	std::ofstream input(fileName.c_str(), std::ios::out | std::ios::binary);
	if (input.is_open()) {

		input << "P6\n";
		input << "# " << fileName << "\n";
		input << width;
		input << " ";
		input << height << "\n";
		input << max_color_val << "\n";
		char channel;
		for (unsigned int i = 0; i < size*3; ++i) {
			channel = (char)data[i];
			input.write(&channel, 1);

		}
	}
	else {
		std::cout << "Error. Unable to open " << fileName << std::endl;
	}
	input.close();
}

unsigned char* ppm::getData() {
	return data;
}

void ppm::rescaler(float gain, float bias, float gamma) {
	int num_cols = width;
	int num_rows = height;
	int size = num_cols * num_rows;
	float newLum;
	float luma;
	
	//potentially use a color class with an array 
	for (int r = 0; r < num_rows; r++) {
		for (int c = 0; c < num_cols; c++) {
			float red = (float)data[3 * (r*num_cols + c) + 0]/255.0;
			float green = (float)data[3 * (r*num_cols + c) + 1]/255.0;
			float blue = (float)data[3 * (r*num_cols + c) + 2]/255.0;
			
			newLum = ((1.0 / 61.0) * (20.0*red + 40.0*green + blue));
			luma = pow(gain * newLum + bias, gamma);
			float scale = luma / newLum;
		///TODO:pair min and max finctions instead of clamp fmax = 0, fmin = 1	
			float wRed = (red * scale);
			wRed = std::fmax(wRed, 0.0);
			wRed = std::fmin(wRed, 1.0);
			float wGreen = (green * scale);
			wGreen = std::fmax(wGreen, 0.0);
			wGreen = std::fmin(wGreen, 1.0);
			float wBlue = (blue * scale);
			wBlue = std::fmax(wBlue, 0.0);
			wBlue = std::fmin(wBlue, 1.0);

			data[3 * (r*num_cols + c) + 0] = (unsigned char)wRed * 255;
			data[3 * (r*num_cols + c) + 1] = (unsigned char)wGreen * 255;
			data[3 * (r*num_cols + c) + 2] = (unsigned char)wBlue * 255;
		}
	}

}


void ppm::resizer(int w, int h, int NC, int NR) {	

	float ratio_C = float(NC) / w;
	float ratio_R = float(NR) / h;
	float x, y;
	int size = NC * NR;
	unsigned char* temp[(3*size)];
	
	//r is row, c is column
	for (int r = 0; r < NR; r++) {
		for (int c = 0; c < NC; c++) {
			x = floor(c*ratio_C);
			y = floor(r*ratio_R);
			data[(int)(r*NC) + c] = data[(int)((y*w) + x)];
		}
	}
}

/// 
/// Log an SDL error with some error message to the output stream of our
/// choice
///
/// \param os The output stream to write the message to
/// \param msg The error message to write, SDL_GetError() appended to it
///
void logSDLError(std::ostream &os, const std::string &msg) {
	os << msg << " error: " << SDL_GetError() << std::endl;
}


/// 
/// Draw an SDL_Texture to an SDL_Renderer at position x, y, preserving
/// the texture's width and height
///
/// \param tex The source texture we want to draw
/// \param ren The renderer we want to draw to
/// \param x The x coordinate to draw to
/// \param y The y coordinate to draw to
///
void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y) {
	//Setup the destination rectangle to be at the position we want
	SDL_Rect dst;
	dst.x = x;
	dst.y = y;

	//Query the texture to get its width and height to use
	SDL_QueryTexture(tex, NULL, NULL, &dst.w, &dst.h);
	SDL_RenderCopy(ren, tex, NULL, &dst);
}





/// 
/// Main function.  Initializes an SDL window, renderer, and texture,
/// and then goes into a loop to listen to events and draw the texture.
///
/// \param argc Number of command line arguments
/// \param argv Array of command line arguments
/// \return integer indicating success (0) or failure (nonzero)
///

int main(int argc, char** argv) {
	//Integers specifying the width (number of columns) and height (number
	//of rows) of the image
	const int NC = *argv[1];
	const int NR = *argv[2];
	const char* fileName = argv[3];
	const char* outFileName = argv[4];
	ppm pixmap(fileName);
	
	int num_cols = pixmap.width;
	int num_rows = pixmap.height;
	unsigned char* data = pixmap.getData();

	float gain = 1;
	float bias = 0;
	float gamma = 0;
	//Start up SDL and make sure it went ok
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		logSDLError(std::cout, "SDL_Init");
		return 1;
	}

	//Setup our window and renderer
	SDL_Window *window = SDL_CreateWindow("Basic SDL Test", 100, 100, num_cols, num_rows, SDL_WINDOW_SHOWN);
	if (window == NULL) {
		logSDLError(std::cout, "CreateWindow");
		SDL_Quit();
		return 1;
	}
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == NULL) {
		logSDLError(std::cout, "CreateRenderer");
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}

	//The textures we'll be using
	SDL_Texture *background;
	//Initialize the texture.  SDL_PIXELFORMAT_RGB24 specifies 3 bytes per
	//pixel, one per color channel
	background = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STATIC, num_cols, num_rows);
	//Copy the raw data array into the texture.
	SDL_UpdateTexture(background, NULL, data, 3 * num_cols);
	if (background == NULL) {
		logSDLError(std::cout, "CreateTextureFromSurface");
	}


	//Make sure they both loaded ok
	if (background == NULL) {
		SDL_DestroyTexture(background);
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}



	//Variables used in the rendering loop
	SDL_Event event;
	bool flag = false;
	bool quit = false;
	bool leftMouseButtonDown = false;
	int start_mouseX;
	int start_mouseY;
	float orig_x_angle;
	float orig_y_angle;

	while (!quit) {
		//Grab the time for frame rate computation
		const Uint64 start = SDL_GetPerformanceCounter();

		//Clear the screen
		SDL_RenderClear(renderer);

		//Event Polling
	//This while loop responds to mouse and keyboard commands.
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				quit = true;
			}
			//Use number input to select which clip should be drawn
			if (event.type == SDL_KEYDOWN) {
				switch (event.key.keysym.sym) {
				case SDLK_ESCAPE:
					quit = true;
					break;
				case SDLK_s:
					pixmap.save(outFileName);
					break;
				case SDLK_1:
					// rescale up gain
					gain += .05;
					cout << gain << "\n";
					pixmap.rescaler(gain, bias, gamma);
					//SDL_RenderSetScale(renderer, 2);
					break;
				case SDLK_2:
					// rescale down gain
					gain -= .05;
					cout << gain << "\n";
					pixmap.rescaler(gain, bias, gamma);
					//SDL_RenderSetScale(renderer, .5);
					break;
				case SDLK_3:
					// rescale up bias
					bias += .05;
					cout << bias << "\n";
					pixmap.rescaler(gain, bias, gamma);
					//SDL_RenderSetScale(renderer, 2);
					break;
				case SDLK_4:
					//rescale down bias
					bias -= .05;
					cout << bias << "\n";
					pixmap.rescaler(gain, bias, gamma);
					break ;
				case SDLK_5:
					// rescale up gamma
					gamma += .05;
					cout << gamma << "\n";
					pixmap.rescaler(gain, bias, gamma);
					break;
				case SDLK_6:
					// rescale down gamma
					gamma -= .05;
					cout << gamma << "\n";
					pixmap.rescaler(gain, bias, gamma);
					break;
				case SDLK_7:
					pixmap.resizer(num_cols, num_rows, NC, NR);
					SDL_SetWindowSize(window, NC, NR);
					SDL_Texture *newBackground;
					newBackground = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STATIC, NC, NR);
					background = newBackground;
					break;
//				case SDLK_r:
//					ppixmap.load(fileName);
//					break;
				default:
					break;
				}
			}
			else if (event.type == SDL_MOUSEBUTTONUP) {
				if (event.button.button == SDL_BUTTON_LEFT)
					leftMouseButtonDown = false;

			}
			else if (event.type == SDL_MOUSEBUTTONDOWN) {
				if (event.button.button == SDL_BUTTON_LEFT) {
					leftMouseButtonDown = true;
				}
			}
			else if (event.type == SDL_MOUSEMOTION) {
				if (leftMouseButtonDown)
				{
					int mouseX = event.motion.x;
					int mouseY = event.motion.y;

					pixmap.data[3 * (mouseY*num_cols + mouseX) + 0] = 255;
					pixmap.data[3 * (mouseY*num_cols + mouseX) + 1] = 0;
					pixmap.data[3 * (mouseY*num_cols + mouseX) + 2] = 0;
				}
			}
		}
		
		//Update the texture, assuming data has changed.
		///TODO: change data field ie. pixmap.getdata
		data = pixmap.getData();
		SDL_UpdateTexture(background, NULL, data, 3 * num_cols);
		//display the texture on the screen
		renderTexture(background, renderer, 0, 0);
		//Update the screen
		SDL_RenderPresent(renderer);

		//Display the frame rate to stdout
		const Uint64 end = SDL_GetPerformanceCounter();
		const static Uint64 freq = SDL_GetPerformanceFrequency();
		const double seconds = (end - start) / static_cast<double>(freq);
		//You may want to comment this line out for debugging purposes
		std::cout << "Frame time: " << seconds * 1000.0 << "ms" << std::endl;
	}


	//After the loop finishes (when the window is closed, or escape is
	//pressed, clean up the data that we allocated.
	delete[] data;
	SDL_DestroyTexture(background);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();


	return 0;
}
