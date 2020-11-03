#include <CanvasTriangle.h>

#include <DrawingWindow.h>

#include <Utils.h>

#include <fstream>

#include <vector>

#include <glm/glm.hpp>

#include <CanvasPoint.h>

#include <Colour.h>

#include <CanvasTriangle.h>

#include <TextureMap.h>

#include <ModelTriangle.h>

#include <unordered_map>

#define WIDTH 600

#define HEIGHT 600







std::unordered_map<std::string, Colour> getcolourMap(){
	std::cout << "0"<< std::endl;
	std::ifstream file ("cornell-box.mtl");

	std::unordered_map<std::string, Colour> colourMap;



	if(file.is_open()){

		std::string line;

		std::string name;

		while(getline(file, line)){



			Colour colour;

			if(line.length() !=0){

				if(line.at(0) == 'n'){

					std::vector<std::string> splitLine = split(line, ' ');

					name = splitLine[1];



				}

				if(line.at(0) == 'K'){

					std::vector<std::string> splitLine = split(line, ' ');

					float f1 = std::stof(splitLine[1]) * 255;

					float f2 = std::stof(splitLine[2]) * 255;

					float f3 = std::stof(splitLine[3]) * 255;

					colour = Colour((int)f1, (int)f2, (int)f3);



					colourMap.insert(std::pair<std::string, Colour>(name, colour));

					//std::cout<<name<<std::endl;

				}

			}

		}

		file.close();

	}



	return colourMap;



}



////////////////////////////////////////////////////////////////////////////////



std::vector<ModelTriangle> readOBJFile(){
	std::cout << "1"<< std::endl;
	std::ifstream file ("cornell-box.obj");

	std::vector<glm::vec3> vs;

	std::vector<ModelTriangle> MTris;

	std::unordered_map<std::string, Colour> colourMap = getcolourMap();

	float vectorScaler = 0.17;

	if(file.is_open()){

		std::string line;

		std::string colourUsed;



		while(getline(file, line)){

			if(line.length() !=0){

				if(line.at(0) == 'o'){



				}

				if(line.at(0) == 'u'){

					std::vector<std::string> splitLine = split(line, ' ');

					colourUsed = splitLine[1];

				}

				if(line.at(0) == 'v'){

					std::vector<std::string> splitLine = split(line, ' ');

					glm::vec3 v = glm::vec3(std::stof(splitLine[1])* vectorScaler, std::stof(splitLine[2])* vectorScaler, std::stof(splitLine[3])* vectorScaler);

					vs.push_back(v);



				}

				if(line.at(0) == 'f'){

					std::vector<std::string> splitLine = split(line, ' ');

					splitLine[1] = splitLine[1].substr(0, splitLine[1].length()-1);

					splitLine[2] = splitLine[2].substr(0, splitLine[2].length()-1);

					splitLine[3] = splitLine[3].substr(0, splitLine[3].length()-1);

					ModelTriangle mTri = ModelTriangle(vs.at(std::stoi(splitLine[1])-1), vs.at(std::stof(splitLine[2])-1), vs.at(std::stof(splitLine[3])-1), colourMap.find(colourUsed)->second);

					MTris.push_back(mTri);

				}

			}

			//std::cout<<MTris.size()<<std::endl;

		}

		file.close();

	}



	return MTris;

}



////////////////////////////////////////////////////////////////////////////////



std::vector<float> interpolateSingleFloat(float start, float end, int steps){
	std::cout << "2"<< std::endl;
	std::vector<float> list;

	for(int i = 0; i<steps; i++){

		list.push_back(start+i*((end-start)/(steps-1)));

	}

	return list;

}



////////////////////////////////////////////////////////////////////////////////



void drawLine(DrawingWindow &window, CanvasPoint From, CanvasPoint To, Colour c){
	std::cout << "3"<< std::endl;
	float xDif = To.x-From.x;

	float yDif = To.y-From.y;

	float stepNumb = fmax(abs(xDif), abs(yDif));

	float xStep = xDif/ stepNumb;

	float yStep = yDif/ stepNumb;

	for(float i =0.0; i<stepNumb;i++){

		float x = From.x +(xStep*i);

		float y = From.y +(yStep*i);

		uint32_t colour = (255 << 24) + (int(c.red) << 16) + (int(c.green) << 8) + int(c.blue);

		window.setPixelColour(round(x), round(y), colour);

	}



}



////////////////////////////////////////////////////////////////////////////////



void drawStrokedTriangle(DrawingWindow &window, CanvasTriangle tri, Colour c){
	std::cout << "4"<< std::endl;
	drawLine(window, tri.v0(), tri.v1(), c);

	drawLine(window, tri.v0(), tri.v2(), c);

	drawLine(window, tri.v1(), tri.v2(), c);



}



////////////////////////////////////////////////////////////////////////////////



int xyToTexture1D(int x, int y, TextureMap t){
	std::cout << "5"<< std::endl;
	return y*t.width +x;

}



////////////////////////////////////////////////////////////////////////////////



void drawFilledTriangle(DrawingWindow &window, CanvasTriangle tri, Colour c,float depthArray[WIDTH][HEIGHT]){
	std::cout << "6"<< std::endl;


	uint32_t colour = (255 << 24) + (int(c.red) << 16) + (int(c.green) << 8) + int(c.blue);









	if(tri.v2().y<tri.v1().y){

		std::swap(tri.v2(), tri.v1());

	}

	if(tri.v1().y<tri.v0().y){

		std::swap(tri.v1(), tri.v0());

	}

	if(tri.v2().y<tri.v1().y){

		std::swap(tri.v2(), tri.v1());

	}



	//Getting new point

	std::vector<float> xs = interpolateSingleFloat(tri.v0().x, tri.v2().x, int(tri.v2().y-tri.v0().y)+1);

	std::vector<float> ds = interpolateSingleFloat(tri.v0().depth, tri.v2().depth, int(tri.v2().y-tri.v0().y)+1);

	//std::cout<<"here2"<<std::endl;

	CanvasPoint CV = CanvasPoint(xs.at(floor(tri.v1().y-tri.v0().y)), tri.v1().y, ds.at(floor(tri.v1().y-tri.v0().y)));

	//xs.at(tri.v1().y-tri.v0().y)

	//std::cout<<"here3"<<std::endl;



	//Fill top triangle

	int yDif = int(CV.y-tri.v0().y);

	std::vector<float> xs0CV = interpolateSingleFloat(tri.v0().x, CV.x, yDif);

	std::vector<float> xs0CVDs = interpolateSingleFloat(tri.v0().depth, CV.depth, yDif);

	std::vector<float> xs01 = interpolateSingleFloat(tri.v0().x, tri.v1().x, yDif);

	std::vector<float> xs01Ds = interpolateSingleFloat(tri.v0().depth, tri.v1().depth, yDif);
	std::cout<<"2-2"<<std::endl;
	for(int y = 0; y<yDif; y++){

		if(xs0CV.at(y)>xs01.at(y)){

			std::swap(xs0CV, xs01);

		}
		std::cout<<"2-1"<<std::endl;
		std::vector<float> across = interpolateSingleFloat(xs0CV.at(y), xs01.at(y), abs(xs01.at(y)-xs0CV.at(y))+2);
		std::cout<<"2-0"<<std::endl;
		std::vector<float> acrossDs = interpolateSingleFloat(xs0CVDs.at(y), xs01Ds.at(y), abs(xs01.at(y)-xs0CV.at(y))+2);

		std::cout<<"19"<<std::endl;
		for(int i = 0; i<across.size();i++){

			//std::cout<<1/acrossDs.at(i)<<std::endl;
			std::cout<<"16"<<std::endl;
			if ((int)across.at(i) >= HEIGHT || (int)round(tri.v0().y+y)  >= WIDTH){}
			else if(1/acrossDs.at(i) > depthArray[(int)across.at(i)][(int)round(tri.v0().y+y)]){
				std::cout<<"17"<<std::endl;
				depthArray[(int)across.at(i)][(int)round(tri.v0().y+y)] = 1/acrossDs.at(i);
				std::cout<<"18"<<std::endl;
				window.setPixelColour(floor(across.at(i)), round(tri.v0().y+y), colour);

			}

		}

	}
	std::cout<<"2-3"<<std::endl;
	//fill line between points cv and v1

	std::vector<float> line = interpolateSingleFloat(CV.x, tri.v1().x, abs(tri.v1().x-CV.x)+2);

	std::vector<float> lineDs = interpolateSingleFloat(CV.depth, tri.v1().depth, abs(tri.v1().x-CV.x)+2);
std::cout<<"2-4"<<std::endl;
	for(int i = 0; i<line.size();i++){
		std::cout<<"2-6"<<std::endl;
		std::cout<<1/lineDs.at(i)<<std::endl;
		std::cout<<(int)line.at(i)<<std::endl;
		std::cout<<(int)round(tri.v1().y)<<std::endl;
		if ((int)line.at(i) >= WIDTH || ((int)round(tri.v1().y) >=  HEIGHT)){

		}
		else if(1/lineDs.at(i) > depthArray[(int)line.at(i)][(int)round(tri.v1().y)]){
			std::cout<<"2-7"<<std::endl;
			window.setPixelColour(floor(line.at(i)), CV.y, colour);
			std::cout<<"2-8"<<std::endl;
			window.setPixelColour(floor(line.at(i)), CV.y-1, colour);

		}

	}
	std::cout<<"2-5"<<std::endl;




	//fill bottom triangle

	int yDif2 = int(tri.v2().y-CV.y)+1;

	std::vector<float> xsCV2 = interpolateSingleFloat(CV.x,tri.v2().x, yDif2);

	std::vector<float> xsCV2Ds = interpolateSingleFloat(CV.depth,tri.v2().depth, yDif2);

	std::vector<float> xs12 = interpolateSingleFloat(tri.v1().x, tri.v2().x, yDif2);

	std::vector<float> xs12Ds = interpolateSingleFloat(tri.v1().depth, tri.v2().depth, yDif2);

	for(int y = 0; y<yDif2; y++){
		std::cout<<"3-3"<<std::endl;

		if(xsCV2.at(y)>xs12.at(y)){

			std::swap(xsCV2, xs12);

		}

		std::vector<float> across = interpolateSingleFloat(xsCV2.at(y), xs12.at(y), abs(xs12.at(y)-xsCV2.at(y))+2);

		std::vector<float> acrossDs = interpolateSingleFloat(xsCV2Ds.at(y), xs12Ds.at(y), abs(xs12.at(y)-xsCV2.at(y))+2);

		for(int i = 0; i<across.size();i++){

			std::cout<<"2-9"<<std::endl;
			//std::cout<<acrossDs.size()<<std::endl;
			//std::cout<<across.size()<<std::endl;
			//std::cout<<i<<std::endl;
			//std::cout<<acrossDs.at(i)<<std::endl;
			//std::cout<<(int)across.at(i)<<std::endl;
			//std::cout<<(int)round(tri.v1().y+y)<<std::endl;
			if (     (int)across.at(i) >= HEIGHT || ( (int) round(tri.v1().y+y) >=  WIDTH) ){
				std::cout<<"3-2"<<std::endl;
			}
			else if(1/acrossDs.at(i) > depthArray[(int)across.at(i)][(int)round(tri.v1().y+y)]){
				std::cout<<"3-0"<<std::endl;
				depthArray[(int)across.at(i)][(int)round(tri.v1().y+y)] = 1/acrossDs.at(i);
				std::cout<<"3-1"<<std::endl;
				std::cout<<CV.y+y<<std::endl;
				std::cout<<across.at(i)<<std::endl;
				window.setPixelColour((int) floor(across.at(i)), round(CV.y+y), colour);
				std::cout<<"4-6"<<std::endl;
			}

		}
		std::cout<<"3-4"<<std::endl;

	}
	std::cout<<"3-5"<<std::endl;




	//drawStrokedTriangle(window, tri, Colour(255,255,255));



}



////////////////////////////////////////////////////////////////////////////////





std::vector<glm::vec3> interpolateThreeElementValues(glm::vec3 start, glm::vec3 end, int steps){
	std::cout << "7"<< std::endl;
	std::vector<glm::vec3> list;

	for(int i = 0; i<steps; i++){

		glm::vec3 temp;

		temp.x = start.x+i*((end.x-start.x)/(steps-1));

		temp.y = start.y+i*((end.y-start.y)/(steps-1));

		temp.z = start.z+i*((end.z-start.z)/(steps-1));



		list.push_back(temp);



	}

	return list;



}

void modifyCameraRotation (glm::vec3 &initalCamera,glm::mat3 toRotate){
	std::cout << "8"<< std::endl;
	initalCamera = initalCamera * toRotate;
}

////////////////////////////////////////////////////////////////////////////////


void modifyCameraPosition(glm::vec3 &initalCamera,glm::vec3 toAdd){
	std::cout << "9"<< std::endl;
	initalCamera.x = initalCamera.x + toAdd.x;
	initalCamera.y = initalCamera.y + toAdd.y;
	initalCamera.z = initalCamera.z + toAdd.z;
}


////////////////////////////////////////////////////////////////////////////////

void vec3ToImagePlane(std::vector<ModelTriangle> vs, DrawingWindow &window,glm::vec3 &initalCamera){
	std::cout << "10"<< std::endl;

	float focal = 2.5;

	glm::vec3 cameraPos = initalCamera;

	Colour c = Colour(255,255,255);

	float planeMulti = 500;

	std::vector<CanvasTriangle> out;







	float x;

	float y;

	CanvasTriangle temp;

	CanvasPoint cp;



	for(ModelTriangle mt : vs){
		std::cout<<"3-7"<<std::endl;

		for(int i = 0; i<3;i++){


			std::cout<<"3-8"<<std::endl;
			glm::vec3 v = mt.vertices[i];

					//calculate u

		v = v - cameraPos;

		x = planeMulti * focal * ((v[0]/v[2])*-1) + (WIDTH/2);

		y = planeMulti * focal * (v[1]/v[2]) + (HEIGHT/2);

		cp = CanvasPoint(x,y, v[2]*-1);

		temp[i] = cp;



		uint32_t colour = (255 << 24) + (int(c.red) << 16) + (int(c.green) << 8) + int(c.blue);
		std::cout<<"5-1"<<std::endl;

		window.setPixelColour(x, y, colour);


		}
		std::cout<<"4-0"<<std::endl;

		//out.push_back(temp);

		std::cout<<"4-1"<<std::endl;
		float depthArray[WIDTH][HEIGHT];
		std::cout<<"4-2"<<std::endl;
		for (int y =0;y<WIDTH;y++){
			for(int x =0;x<HEIGHT;x++){
				//std::cout<<"y" <<std::endl;
				//std::cout<<y <<std::endl;
				//std::cout<<x <<std::endl;
				depthArray[y][x] = 0;
			}
		}



		std::cout<<"4-3"<<std::endl;
		drawFilledTriangle(window, temp, mt.colour,depthArray);

		//std::cout<<mt.colour<<std::endl;



	}

	std::cout<<"3-6"<<std::endl;

	//return out;



}



////////////////////////////////////////////////////////////////////////////////







void draw(DrawingWindow &window) {
	std::cout << "12"<< std::endl;
	window.clearPixels();

	//std::vector<float> greyscale;

	//greyscale = interpolateSingleFloat(0, 255, WIDTH);



	std::vector<glm::vec3> red2yellow;

	red2yellow  = interpolateThreeElementValues(glm::vec3(255, 0, 0), glm::vec3(255, 255, 0), HEIGHT);



	std::vector<glm::vec3> blue2green;

	blue2green  = interpolateThreeElementValues(glm::vec3(0, 0, 255), glm::vec3(0, 255, 0), HEIGHT);



	for (size_t y = 0; y < window.height; y++) {



		std::vector<glm::vec3> RY2BG;

		RY2BG  = interpolateThreeElementValues(red2yellow.at(y), blue2green.at(y), WIDTH);



		for (size_t x = 0; x < window.width; x++) {

			float red = RY2BG.at(x).x;

			float green = RY2BG.at(x).y;

			float blue = RY2BG.at(x).z;

			uint32_t colour = (255 << 24) + (int(red) << 16) + (int(green) << 8) + int(blue);

			window.setPixelColour(x, y, colour);

		}

	}

}



////////////////////////////////////////////////////////////////////////////////



void update(DrawingWindow &window) {
	std::cout << "13"<< std::endl;
	// Function for performing animation (shifting artifacts or moving the camera)

}



////////////////////////////////////////////////////////////////////////////////



void handleEvent(SDL_Event event, DrawingWindow &window,glm::vec3 &initalCamera ) {
	std::cout << "14"<< std::endl;
	if (event.type == SDL_KEYDOWN) {

		if (event.key.keysym.sym == SDLK_LEFT) std::cout << "LEFT" << std::endl;

		else if (event.key.keysym.sym == SDLK_RIGHT) std::cout << "RIGHT" << std::endl;

		else if (event.key.keysym.sym == SDLK_UP) std::cout << "UP" << std::endl;

		else if (event.key.keysym.sym == SDLK_DOWN) std::cout << "DOWN" << std::endl;

		else if (event.key.keysym.sym == SDLK_u) drawStrokedTriangle(window, CanvasTriangle(CanvasPoint(rand()%WIDTH,rand()%HEIGHT), CanvasPoint(rand()%WIDTH,rand()%HEIGHT), CanvasPoint(rand()%WIDTH,rand()%HEIGHT)), Colour(rand()%256,rand()%256,rand()%256));

		//else if (event.key.keysym.sym == SDLK_f) drawFilledTriangle(window, CanvasTriangle(CanvasPoint(rand()%WIDTH,rand()%HEIGHT), CanvasPoint(rand()%WIDTH,rand()%HEIGHT), CanvasPoint(rand()%WIDTH,rand()%HEIGHT)), Colour(rand()%256,rand()%256,rand()%256));

		else if (event.key.keysym.sym == SDLK_a) modifyCameraPosition(initalCamera,glm::vec3(0.1, 0, 0));

		else if (event.key.keysym.sym == SDLK_d) modifyCameraPosition(initalCamera,glm::vec3(-0.2, 0, 0));

		else if (event.key.keysym.sym == SDLK_o) modifyCameraPosition(initalCamera,glm::vec3(0.0, 0.1, 0));

		else if (event.key.keysym.sym == SDLK_p) modifyCameraPosition(initalCamera,glm::vec3(0.0, -0.1, 0));

		else if (event.key.keysym.sym == SDLK_w) modifyCameraPosition(initalCamera,glm::vec3(0.0, 0, 0.1));

		else if (event.key.keysym.sym == SDLK_s) modifyCameraPosition(initalCamera,glm::vec3(0.0, 0, -0.1));

	} else if (event.type == SDL_MOUSEBUTTONDOWN) window.savePPM("output.ppm");

}



////////////////////////////////////////////////////////////////////////////////



int main(int argc, char *argv[]) {
	std::cout << "15"<< std::endl;
	DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);

	SDL_Event event;



	//std::vector<float> result;

	//result = interpolateSingleFloat(2.2, 8.5, 7);

	std::vector<glm::vec3> result = interpolateThreeElementValues(glm::vec3 (1, 4, 9.2), glm::vec3 (4, 1, 9.8), 4);

	for(size_t i=0; i<result.size(); i++) std::cout << result[i].x << " ";

	for(size_t i=0; i<result.size(); i++) std::cout << result[i].y << " ";

	for(size_t i=0; i<result.size(); i++) std::cout << result[i].z << " ";

	std::cout << std::endl;



	std::vector<ModelTriangle> modelTriangles = readOBJFile();

	glm::vec3 initalCamera  = glm::vec3(0.0, 0.0, 3.0);
	glm::mat3 rotation = glm::mat3(
   0.0, 0.0, 0.0, // first column (not row!)
   0.0, 0.1, 0.1, // second column
   0.0, 0.1, 0.1  // third column
	);
	//modifyCameraPosition(initalCamera,glm::vec3(0.1, 0.1, -0.1));
	//modifyCameraRotation(initalCamera,rotation);
	std::cout << initalCamera.x<< std::endl;
	std::cout << initalCamera.y<< std::endl;
	std::cout << initalCamera.z<< std::endl;
	while (true) {

		// We MUST poll for events - otherwise the window will freeze !

		if (window.pollForInputEvents(event)) handleEvent(event, window,initalCamera);

		update(window);



		vec3ToImagePlane(modelTriangles, window ,initalCamera );



		//std::vector<CanvasTriangle> tris = vec3ToImagePlane(modelTriangles, window);

		//for(CanvasTriangle ct : tris){

			//drawStrokedTriangle(window, ct, Colour(255,255,255));

		//}



		//draw(window);

		//drawLine(window, CanvasPoint(0, 0), CanvasPoint(WIDTH/2, HEIGHT/2), Colour(0,0,255));

		// Need to render the frame at the end, or nothing actually gets shown on the screen !

		window.renderFrame();
		window.clearPixels();

	}

}
