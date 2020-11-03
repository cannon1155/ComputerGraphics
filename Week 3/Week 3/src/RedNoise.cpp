#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <Utils.h>
#include <fstream>
#include <vector>
#include <glm/glm.hpp>
#include <CanvasPoint.h>
#include <Colour.h>
#include <CanvasTriangle.h>

#define WIDTH 320
#define HEIGHT 240


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

uint32_t convertColorToNumber(Colour colour){
	return (255 << 24) + (colour.red << 16) + (colour.blue << 8) + colour.green;
}

void drawLine(CanvasPoint from,CanvasPoint to,Colour colour,DrawingWindow &window){
	 float xDiff = to.x - from.x;
	 float yDiff = to.y - from.y;
	 float numberOfSteps = fmax(abs(xDiff), abs(yDiff));
	 float xStepSize = xDiff/numberOfSteps;
	 float yStepSize = yDiff/numberOfSteps;
	 for (float i=0.0; i<numberOfSteps; i++) {
		 float x = from.x + (xStepSize*i);
		 float y = from.y + (yStepSize*i);
		 window.setPixelColour(round(x), round(y), convertColorToNumber(colour));
	}
}
void drawTriangle(CanvasTriangle triangle,Colour colour,DrawingWindow &window){
	drawLine(triangle.v0(),triangle.v1(),colour,window);
	drawLine(triangle.v1(),triangle.v2(),colour,window);
	drawLine(triangle.v0(),triangle.v2(),colour,window);
}

void drawRandomTriangle(DrawingWindow &window){
	CanvasTriangle triangle = CanvasTriangle(CanvasPoint(rand()%WIDTH,rand()%HEIGHT),CanvasPoint(rand()%WIDTH,rand()%HEIGHT),CanvasPoint(rand()%WIDTH,rand()%HEIGHT));
	Colour colour = Colour(rand()%255,rand()%255,rand()%255);
	drawTriangle(triangle,colour,window);
}





void draw(DrawingWindow &window) {
	window.clearPixels();
	for (size_t y = 0; y < window.height; y++) {
		for (size_t x = 0; x < window.width-10; x++) {
			float red = x % 256;
			float green = y % 256;
			float blue = rand() % 256;
			uint32_t colour = (255 << 24) + (int(red) << 16) + (int(green) << 8) + int(blue);
			window.setPixelColour(x, y, colour);
		}
	}
}

void update(DrawingWindow &window) {
	// Function for performing animation (shifting artifacts or moving the camera)
}


void drawFilledTriangle(CanvasTriangle triangle,Colour colour,DrawingWindow &window){
	CanvasPoint top;
	CanvasPoint right;
	CanvasPoint bottom;
	CanvasPoint left;
	if (triangle.v0().y <= triangle.v1().y && triangle.v0().y <= triangle.v2().y){
		top = triangle.v0();
		if (triangle.v1().y <= triangle.v2().y){
			right =  triangle.v1();
			bottom = triangle.v2();
		}
		else{
			right =  triangle.v2();
			bottom = triangle.v1();
		}

	}else{
		if (triangle.v1().y <= triangle.v2().y){
			top = triangle.v1();
			if (triangle.v0().y <= triangle.v2().y){
				right = triangle.v0();
				bottom = triangle.v2();
			}else{
				right = triangle.v2();
				bottom = triangle.v0();
			}
		}
		else{
			top = triangle.v2();
			if (triangle.v0().y <= triangle.v1().y){
				right = triangle.v0();
				bottom = triangle.v1();
			}else{
				right = triangle.v1();
				bottom = triangle.v0();
			}
		}
	}






	/*std::cout << "top.x" << std::endl;
	std::cout << top.x << std::endl;
	std::cout << top.y << std::endl;
	std::cout << right.x << std::endl;
	std::cout << right.y << std::endl;
	std::cout << bottom.x << std::endl;
	std::cout << bottom.y << std::endl;*/

		std::vector<float> points = interpolateSingleFloats(top.x,bottom.x,abs(top.y-bottom.y));

		left = CanvasPoint(roundf(points.at(right.y - top.y)) ,right.y);

		std::vector<float> leftPoints = interpolateSingleFloats(top.x,left.x,abs(top.y-left.y));
		std::vector<float> rightPoints = interpolateSingleFloats(top.x,right.x,abs(top.y-right.y));

		for (int y =0; y<rightPoints.size();y++){
			for (int x=rightPoints.at(y) ;x<leftPoints.at(y);x++){
				window.setPixelColour(round(x), round(top.y+y), convertColorToNumber(colour));
			}
		}
		for (int y =0; y<rightPoints.size();y++){
			for (int x=leftPoints.at(y) ;x<rightPoints.at(y);x++){
				window.setPixelColour(round(x), round(top.y+y), convertColorToNumber(colour));
			}
		}
		leftPoints = interpolateSingleFloats(left.x,bottom.x,abs(left.y-bottom.y));
		rightPoints = interpolateSingleFloats(right.x,bottom.x,abs(right.y-bottom.y));
		for (int y =0; y<rightPoints.size();y++){
			for (int x=rightPoints.at(y) ;x<leftPoints.at(y);x++){
				window.setPixelColour(round(x), round(left.y+y), convertColorToNumber(colour));
			}
		}
		for (int y =0; y<rightPoints.size();y++){
			for (int x=leftPoints.at(y) ;x<rightPoints.at(y);x++){
				window.setPixelColour(round(x), round(left.y+y), convertColorToNumber(colour));
			}
		}





	drawLine(triangle.v0(),triangle.v1(),colour,window);
	drawLine(triangle.v1(),triangle.v2(),colour,window);
	drawLine(triangle.v0(),triangle.v2(),colour,window);

}


void xyToTexture(){

}

/*void drawTextureTriangle(CanvasTriangle canvasTriangle,CanvasTriangle textureTriangle ,Colour colour,DrawingWindow &window){
	TextureMap map = TextureMap("texture.pmm");
	CanvasPoint top;
	CanvasPoint right;
	CanvasPoint bottom;
	CanvasPoint left;
	if (triangle.v0().y <= triangle.v1().y && triangle.v0().y <= triangle.v2().y){
		top = triangle.v0();
		if (triangle.v1().y <= triangle.v2().y){
			right =  triangle.v1();
			bottom = triangle.v2();
		}
		else{
			right =  triangle.v2();
			bottom = triangle.v1();
		}

	}else{
		if (triangle.v1().y <= triangle.v2().y){
			top = triangle.v1();
			if (triangle.v0().y <= triangle.v2().y){
				right = triangle.v0();
				bottom = triangle.v2();
			}else{
				right = triangle.v2();
				bottom = triangle.v0();
			}
		}
		else{
			top = triangle.v2();
			if (triangle.v0().y <= triangle.v1().y){
				right = triangle.v0();
				bottom = triangle.v1();
			}else{
				right = triangle.v1();
				bottom = triangle.v0();
			}
		}
	}

	std::vector<float> points = interpolateSingleFloats(top.x,bottom.x,abs(top.y-bottom.y));

	left = CanvasPoint(roundf(points.at(right.y - top.y)) ,right.y);

	std::vector<float> leftPoints = interpolateSingleFloats(top.x,left.x,abs(top.y-left.y));
	std::vector<float> rightPoints = interpolateSingleFloats(top.x,right.x,abs(top.y-right.y));

	std::vector<float> texturePointsXa = interpolateSingleFloats(TextureMap.v0().x,TextureMap.v1().x,abs(top.y-left.y));
	std::vector<float> texturePointsYa = interpolateSingleFloats(TextureMap.v0().y,TextureMap.v1().y,abs(top.y-left.y));
	std::vector<float> texturePointsXb = interpolateSingleFloats(TextureMap.v0().x,TextureMap.v2().x,abs(top.y-left.y));
	std::vector<float> texturePointsYb = interpolateSingleFloats(TextureMap.v0().y,TextureMap.v2().y,abs(top.y-left.y));

	for (int y =0; y<rightPoints.size();y++){
		std::vector<float> texturePointsX = interpolateSingleFloats(texturePointsXa.at(y),texturePointsYa.at(y),rightPoints.size();
		std::vector<float> texturePointsY = interpolateSingleFloats(texturePointsXb.at(y),texturePointsYb.at(y),rightPoints.size();
		for (int x=rightPoints.at(y) ;x<leftPoints.at(y);x++){
			window.setPixelColour(round(x), round(top.y+y), getPixelFromTexture(texturePointsX.at(x),texturePointsY.at(x)));
		}
	}
	for (int y =0; y<rightPoints.size();y++){
		for (int x=leftPoints.at(y) ;x<rightPoints.at(y);x++){
			window.setPixelColour(round(x), round(top.y+y), convertColorToNumber(colour));
		}
	}
	leftPoints = interpolateSingleFloats(left.x,bottom.x,abs(left.y-bottom.y));
	rightPoints = interpolateSingleFloats(right.x,bottom.x,abs(right.y-bottom.y));
	for (int y =0; y<rightPoints.size();y++){
		for (int x=rightPoints.at(y) ;x<leftPoints.at(y);x++){
			window.setPixelColour(round(x), round(left.y+y), convertColorToNumber(colour));
		}
	}
	for (int y =0; y<rightPoints.size();y++){
		for (int x=leftPoints.at(y) ;x<rightPoints.at(y);x++){
			window.setPixelColour(round(x), round(left.y+y), convertColorToNumber(colour));
		}
	}

}*/

void drawRandomFilledTriangle(DrawingWindow &window){
	CanvasTriangle triangle = CanvasTriangle(CanvasPoint(rand()%WIDTH,rand()%HEIGHT),CanvasPoint(rand()%WIDTH,rand()%HEIGHT),CanvasPoint(rand()%WIDTH,rand()%HEIGHT));
	Colour colour = Colour(rand()%255,rand()%255,rand()%255);
	drawFilledTriangle(triangle,colour,window);
}

void handleEvent(SDL_Event event, DrawingWindow &window) {
	if (event.type == SDL_KEYDOWN) {
		if (event.key.keysym.sym == SDLK_LEFT) std::cout << "LEFT" << std::endl;
		else if (event.key.keysym.sym == SDLK_u) drawRandomTriangle(window);
		else if (event.key.keysym.sym == SDLK_s) drawRandomFilledTriangle(window);
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
		drawFilledTriangle(CanvasTriangle(CanvasPoint(50,100),CanvasPoint(75,125),CanvasPoint(100,55)),Colour(255,255,255),window);
		//draw(window);
		// Need to render the frame at the end, or nothing actually gets shown on the screen !
		window.renderFrame();
	}
}
