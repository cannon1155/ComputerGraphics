#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <Utils.h>
#include <fstream>
#include <vector>
#include <glm/glm.hpp>

#define WIDTH 255
#define HEIGHT 255

//gets a vector of floats between from and to with numberOfSteps-2 between them
std::vector<float> interpolateSingleFloats(float from,float to,int numberOfSteps){
	std::vector<float> v;
	float difference = (to-from)/(numberOfSteps-1);
	int i;
	for (i=0;i<numberOfSteps;i++){
		v.push_back(from + (difference * i));
	}
	return v;
}


std::vector<glm::vec3 > interpolateThreeElementValues( glm::vec3 from, glm::vec3 to,int numberOfSteps){
	std::vector<glm::vec3> v;
	int i ;
	v.push_back(from);
	for (i=1;i<numberOfSteps-1;i++){
		glm::vec3 tempV;
		tempV = glm::vec3(from.x + ((to.x-from.x)/(numberOfSteps-1) * i),from.y + ((to.y-from.y)/(numberOfSteps-1) * i),from.z + ((to.z-from.z)/(numberOfSteps-1) * i));
		/*tempV.push_back(from.at(0) + ((to.at(0)-from.at(0))/(numberOfSteps-1) * i));
		tempV.push_back(from.at(1) + ((to.at(1)-from.at(1))/(numberOfSteps-1) * i));
		tempV.push_back(from.at(2) + ((to.at(2)-from.at(2))/(numberOfSteps-1) * i));*/
		v.push_back(tempV);
	}
	v.push_back(to);
	return v;
}

void draw(DrawingWindow &window) {
	window.clearPixels();
	std::vector<float>  floats =  interpolateSingleFloats(0,255,255);
	glm::vec3 topLeft(255,0,0);
	glm::vec3 topRight(0,0,255);
	glm::vec3 bottomRight(0,255,0);
	glm::vec3 bottomLeft(255,255,0);
	std::vector<glm::vec3> left = interpolateThreeElementValues(topLeft,bottomLeft,255);
	std::vector<glm::vec3> right = interpolateThreeElementValues(topRight,bottomRight,255);
	for (size_t y = 0; y < window.height; y++) {
		std::vector<glm::vec3> across = interpolateThreeElementValues(left.at(y),right.at(y),255);
		for (size_t x = 0; x < window.width; x++) {
			float red = across.at(x).x;
			float green = across.at(x).y;
			float blue = across.at(x).z;
			float whiteBlack = 255;
			uint32_t colour = (int(whiteBlack) << 24) + (int(red) << 16) + (int(green) << 8) + int(blue);
			window.setPixelColour(x, y, colour);
		}
	}
}

void update(DrawingWindow &window) {
	// Function for performing animation (shifting artifacts or moving the camera)
}

void handleEvent(SDL_Event event, DrawingWindow &window) {
	if (event.type == SDL_KEYDOWN) {
		if (event.key.keysym.sym == SDLK_LEFT) std::cout << "LEFT" << std::endl;
		else if (event.key.keysym.sym == SDLK_RIGHT) std::cout << "RIGHT" << std::endl;
		else if (event.key.keysym.sym == SDLK_UP) std::cout << "UP" << std::endl;
		else if (event.key.keysym.sym == SDLK_DOWN) std::cout << "DOWN" << std::endl;
	} else if (event.type == SDL_MOUSEBUTTONDOWN) window.savePPM("output.ppm");
}




int main(int argc, char *argv[]) {
	DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);
	SDL_Event event;
	while (true) {
		// We MUST poll for events - otherwise the window will freeze !
		if (window.pollForInputEvents(event)) handleEvent(event, window);
		update(window);
		draw(window);
		// Need to render the frame at the end, or nothing actually gets shown on the screen !
		window.renderFrame();
	}
}
