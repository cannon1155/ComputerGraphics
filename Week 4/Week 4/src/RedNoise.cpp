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
#include <cmath>
//#include <smath>

#define WIDTH 600

#define HEIGHT 600







std::unordered_map<std::string, Colour> getcolourMap(){
	//std::cout << "0"<< std::endl;
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
	//std::cout << "1"<< std::endl;
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
	//std::cout<<steps<<std::endl;
	std::vector<float> list;
	if(steps == 1){
		list.push_back(start);
	}else{
		for(int i = 0; i<steps; i++){
			list.push_back(start+i*((end-start)/(steps-1)));
		}
	}
	//std::cout<<list.at(0)<<std::endl;
	return list;
}



////////////////////////////////////////////////////////////////////////////////



void drawLine(DrawingWindow &window, CanvasPoint From, CanvasPoint To, Colour c){
	//std::cout << "3"<< std::endl;
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
	//std::cout << "4"<< std::endl;
	drawLine(window, tri.v0(), tri.v1(), c);

	drawLine(window, tri.v0(), tri.v2(), c);

	drawLine(window, tri.v1(), tri.v2(), c);



}

////////////////////////////////////////////////////////////////////////////////

int xyToTexture1D(int x, int y, TextureMap t){
	//std::cout << "5"<< std::endl;
	return y*t.width +x;

}

////////////////////////////////////////////////////////////////////////////////

void drawFilledTriangle(DrawingWindow &window, CanvasTriangle tri, Colour c, float depthArray[WIDTH][HEIGHT]){

	uint32_t colour = (255 << 24) + (int(c.red) << 16) + (int(c.green) << 8) + int(c.blue);

	//std::cout << depthArray[0][0]<< std::endl;

	CanvasPoint canvasP0 = tri.v0();
	CanvasPoint canvasP1 = tri.v1();
	CanvasPoint canvasP2 = tri.v2();
	CanvasPoint cv;

	//swap points so in increasing y order
	if(canvasP1.y<canvasP0.y){
 		std::swap(canvasP0, canvasP1);
	}
	if(canvasP2.y<canvasP1.y){
 		std::swap(canvasP2, canvasP1);
	}
	if(canvasP1.y<canvasP0.y){
 		std::swap(canvasP0, canvasP1);
	}

	//calculate new points canvas and texture if two points arent on same y
		//canvas
	int ydif02 = (int)canvasP2.y - (int)canvasP0.y;
	//std::cout<<ydifT02<<std::endl;
	if(canvasP1.y != canvasP2.y && canvasP0.y != canvasP1.y){
		std::vector<float> canvas0to2 = interpolateSingleFloat(canvasP0.x, canvasP2.x, ydif02+1);
		cv = CanvasPoint((int)canvas0to2.at(canvasP1.y-canvasP0.y), canvasP1.y);
	}
	if(canvasP1.y == canvasP0.y){
		cv = canvasP0;
	}
	if(canvasP1.y == canvasP2.y){
		cv = canvasP2;
	}
	cv.depth = (canvasP0.depth + ((canvasP1.y-canvasP0.y)/(canvasP2.y-canvasP0.y)) * abs(canvasP2.depth-canvasP0.depth));
	if(cv.x>canvasP1.x){
		std::swap(canvasP1, cv);
	}

	//fill top triangle if top isnt flat
	if(canvasP0.y != canvasP1.y){
		float ydif0to1 = abs(canvasP1.y-canvasP0.y);
		std::vector<float> canvas0tocv = interpolateSingleFloat(canvasP0.x, cv.x, ydif0to1+1);
		std::vector<float> canvas0to1 = interpolateSingleFloat(canvasP0.x, canvasP1.x, ydif0to1+1);

		std::vector<float> depth0tocv = interpolateSingleFloat(canvasP0.depth, cv.depth, ydif0to1+1);
		std::vector<float> depth0to1 = interpolateSingleFloat(canvasP0.depth, canvasP1.depth, ydif0to1+1);

		for(int i = 0; i<ydif0to1; i++){
			std::vector<float> depthAcross = interpolateSingleFloat(depth0tocv[i], depth0to1[i], (int)canvas0to1.at(i)-(int)canvas0tocv.at(i)+1);
			for(int x = (int)canvas0tocv.at(i); x<= (int)canvas0to1.at(i); x++){
				if (x >= HEIGHT || (int)round(canvasP0.y+i)  >= WIDTH || x < 0 || (int)round(canvasP0.y+i) < 0){}
				//else if(1/depthAcross.at(x-(int)canvas0tocv.at(i)) > depthArray[x][(int)round(canvasP0.y+i)]){
					//depthArray[x][(int)round(canvasP0.y+i)] = 1/depthAcross.at(x-(int)canvas0tocv.at(i));
					else{
					window.setPixelColour(x, canvasP0.y +i, colour);
				}
			}
		}
	}

	//fill bottom triangle if bottom isnt flat
	if(canvasP2.y != canvasP1.y){
		float ydif1to2 = abs(canvasP2.y-canvasP1.y);
		std::vector<float> canvascvto2 = interpolateSingleFloat(cv.x, canvasP2.x, ydif1to2+1);
		std::vector<float> canvas1to2 = interpolateSingleFloat(canvasP1.x, canvasP2.x, ydif1to2+1);

		std::vector<float> depthcvto2 = interpolateSingleFloat(cv.depth, canvasP2.depth, ydif1to2+1);
		std::vector<float> depth1to2 = interpolateSingleFloat(canvasP1.depth, canvasP2.depth, ydif1to2+1);

		for(int i = 0; i<ydif1to2; i++){
			std::vector<float> depthAcross = interpolateSingleFloat(depthcvto2[i], depth1to2[i], (int)canvas1to2.at(i)-(int)canvascvto2.at(i)+1);
			for(int x = (int)canvascvto2.at(i); x<= (int)canvas1to2.at(i); x++){
				if (x >= HEIGHT || (int)round(canvasP1.y+i)  >= WIDTH || x < 0 || (int)round(canvasP1.y+i) < 0){}
				//else if(1/depthAcross.at(x-(int)canvascvto2.at(i)) > depthArray[x][(int)round(canvasP1.y+i)]){
					//depthArray[x][(int)round(canvasP1.y+i)] = 1/depthAcross.at(x-(int)canvascvto2.at(i));
					else{
					//std::cout<<depthArray[x][(int)round(canvasP1.y+i)]<<std::endl;
					window.setPixelColour(x, canvasP1.y +i, colour);
				}

			}
		}
	}

}

// void drawFilledTriangle(DrawingWindow &window, CanvasTriangle tri, Colour c,float (&depthArray)[WIDTH][HEIGHT]){
// 	//std::cout << "6"<< std::endl;
//
// 	uint32_t colour = (255 << 24) + (int(c.red) << 16) + (int(c.green) << 8) + int(c.blue);
//
// 	CanvasPoint canvasP0 = tri.v0();
// 	CanvasPoint canvasP1 = tri.v1();
// 	CanvasPoint canvasP2 = tri.v2();
// 	CanvasPoint cv;
//
// 	//swap points so in increasing y order
// 	if(canvasP1.y<canvasP0.y){
//  		std::swap(canvasP0, canvasP1);
// 	}
// 	if(canvasP2.y<canvasP1.y){
//  		std::swap(canvasP2, canvasP1);
// 	}
// 	if(canvasP1.y<canvasP0.y){
//  		std::swap(canvasP0, canvasP1);
// 	}
//   std::cout << "1"<< std::endl;
//
// 	//calculate new points canvas and texture if two points arent on same y
// 		//canvas
// 	int ydif02 = (int)canvasP2.y - (int)canvasP0.y;
//
// 	if(canvasP1.y != canvasP2.y && canvasP0.y != canvasP1.y){
//
// 		std::vector<float> canvas0to2 = interpolateSingleFloat(canvasP0.x, canvasP2.x, ydif02);
// 		cv = CanvasPoint((int)canvas0to2.at(canvasP1.y-canvasP0.y), canvasP1.y);
// 	}
// 	if(canvasP1.y == canvasP0.y){
// 		cv = canvasP0;
// 	}
// 	if(canvasP1.y == canvasP2.y){
// 		cv = canvasP2;
// 	}
// 	std::cout << "1.5"<< std::endl;
//
// 	std::vector<float> ds = interpolateSingleFloat(canvasP0.depth, canvasP2.depth, int(canvasP2.y-canvasP0.y)+1);
// 	cv.depth = ds.at(floor(canvasP1.y-canvasP0.y));
//
// 	if(cv.x>canvasP1.x){
// 		std::swap(canvasP1, cv);
// 	}
//
// 	std::cout << "2"<< std::endl;
//
// 	/*//Getting new point
//
// 	std::vector<float> xs = interpolateSingleFloat(tri.v0().x, tri.v2().x, int(tri.v2().y-tri.v0().y)+1);
// 	std::vector<float> ds = interpolateSingleFloat(tri.v0().depth, tri.v2().depth, int(tri.v2().y-tri.v0().y)+1);
// 	//std::cout<<"here2"<<std::endl;
// 	CanvasPoint CV = CanvasPoint(xs.at(floor(tri.v1().y-tri.v0().y)), tri.v1().y, ds.at(floor(tri.v1().y-tri.v0().y)));
// 	//xs.at(tri.v1().y-tri.v0().y)
// 	//std::cout<<"here3"<<std::endl;*/
//
// 	//fill top triangle if top isnt flat
// 	if(canvasP0.y != canvasP1.y){
// 		int ydif0to1 = abs(canvasP1.y-canvasP0.y);
// 		std::vector<float> canvas0tocv = interpolateSingleFloat(canvasP0.x, cv.x, ydif0to1);
// 		std::vector<float> canvasD0tocv = interpolateSingleFloat(canvasP0.depth, cv.depth, ydif0to1);
// 		std::vector<float> canvas0to1 = interpolateSingleFloat(canvasP0.x, canvasP1.x, ydif0to1);
// 		std::vector<float> canvasD0to1 = interpolateSingleFloat(canvasP0.depth, canvasP1.depth, ydif0to1);
//
// 		for(int i = 0; i<ydif0to1; i++){
//
//
// 			std::vector<float> LineD = interpolateSingleFloat((int)canvasD0tocv.at(i), (int)canvasD0to1.at(i), (int)abs(canvas0to1.at(i)-(int)canvas0tocv.at(i))+1);
//
// 			for(int x = (int)canvas0tocv.at(i); x<= (int)canvas0to1.at(i); x++){
// 				if (x >= HEIGHT || (int)round(canvasP0.y+i)  >= WIDTH || x < 0 || (int)round(canvasP0.y+i) < 0){}
// 				else if(1/LineD.at(x-(int)canvas0tocv.at(i)) > depthArray[x][(int)round(canvasP0.y+i)]){
// 					depthArray[x][(int)round(canvasP0.y+i)] = 1/LineD.at(x-(int)canvas0tocv.at(i));
// 					window.setPixelColour(x, canvasP0.y +i, colour);
//
// 				}
// 			}
// 		}
// 	}
// 	std::cout << "3"<< std::endl;
// /*
// 	//Fill top triangle
// 	int yDif = int(CV.y-tri.v0().y);
// 	std::vector<float> xs0CV = interpolateSingleFloat(tri.v0().x, CV.x, yDif);
// 	std::vector<float> xs0CVDs = interpolateSingleFloat(tri.v0().depth, CV.depth, yDif);
// 	std::vector<float> xs01 = interpolateSingleFloat(tri.v0().x, tri.v1().x, yDif);
// 	std::vector<float> xs01Ds = interpolateSingleFloat(tri.v0().depth, tri.v1().depth, yDif);
// 	for(int y = 0; y<yDif; y++){
// 		if(xs0CV.at(y)>xs01.at(y)){
// 			std::swap(xs0CV, xs01);
// 		}
//
// 		std::vector<float> across = interpolateSingleFloat(xs0CV.at(y), xs01.at(y), abs(xs01.at(y)-xs0CV.at(y))+2);
// 		std::vector<float> acrossDs = interpolateSingleFloat(xs0CVDs.at(y), xs01Ds.at(y), abs(xs01.at(y)-xs0CV.at(y))+2);
//
// 		for(int i = 0; i<across.size();i++){
//
// 			if ((int)across.at(i) >= HEIGHT || (int)round(tri.v0().y+y)  >= WIDTH || (int)across.at(i) < 0 || (int)round(tri.v0().y+y) < 0){}
// 			else if(1/acrossDs.at(i) > depthArray[(int)across.at(i)][(int)round(tri.v0().y+y)]){
// 				//std::cout<<"17"<<std::endl;
// 				depthArray[(int)across.at(i)][(int)round(tri.v0().y+y)] = 1/acrossDs.at(i);
// 				//std::cout<<"18"<<std::endl;
// 				window.setPixelColour(floor(across.at(i)), round(tri.v0().y+y), colour);
//
// 			}
// 		}
// 	}*/
//
// /*
// 	std::vector<float> line = interpolateSingleFloat(CV.x, tri.v1().x, abs(tri.v1().x-CV.x)+2);
//
// 	std::vector<float> lineDs = interpolateSingleFloat(CV.depth, tri.v1().depth, abs(tri.v1().x-CV.x)+2);
// //std::cout<<"2-4"<<std::endl;
// 	for(int i = 0; i<line.size();i++){
// 		if ((int)line.at(i) >= WIDTH || ((int)round(tri.v1().y) >=  HEIGHT || (int)line.at(i) < 0 || (int)round(tri.v1().y) < 0)){
//
// 		}
// 		else if(1/lineDs.at(i) > depthArray[(int)line.at(i)][(int)round(tri.v1().y)]){
// 			//std::cout<<"2-7"<<std::endl;
// 		//	depthArray[(int)line.at(i)][(int)round(tri.v1().y)] = 1/lineDs.at(i);
// 			window.setPixelColour(floor(line.at(i)), CV.y, colour);
// 			//std::cout<<"2-8"<<std::endl;
// 			window.setPixelColour(floor(line.at(i)), CV.y-1, colour);
//
// 		}
//
// 	}*/
//
// 	//fill bottom triangle if bottom isnt flat
// 	if(canvasP2.y != canvasP1.y){
// 		int ydif1to2 = (int)abs(canvasP2.y-canvasP1.y);
//
// 		std::vector<float> canvascvto2 = interpolateSingleFloat(cv.x, canvasP2.x, ydif1to2);
// 		std::vector<float> canvasDcvto2 = interpolateSingleFloat(cv.depth, canvasP2.depth, ydif1to2);
// 		std::vector<float> canvas1to2 = interpolateSingleFloat(canvasP1.x, canvasP2.x, ydif1to2);
// 		std::vector<float> canvasD1to2 = interpolateSingleFloat(canvasP1.depth, canvasP2.depth, ydif1to2);
//
//
//
// 		for(int i = 0; i<ydif1to2; i++){
//
// 			//std::vector<float> Line = interpolateSingleFloat((int)canvascvto2.at(i), (int)canvas1to2.at(i), (int)abs(canvas1to2.at(i)-(int)canvascvto2.at(i))+1);
// 			std::cout << "3.5"<< std::endl;
// 			std::cout << ydif1to2<< std::endl;
// 			std::vector<float> LineD = interpolateSingleFloat((int)canvasDcvto2.at(i), (int)canvasD1to2.at(i), (int)abs(canvas1to2.at(i)-(int)canvascvto2.at(i))+1);
// 			std::cout << (int)round(canvasP1.y+i)<< std::endl;
//
//
// 			for(int x = (int)canvascvto2.at(i); x<= (int)canvas1to2.at(i); x++){
// 				//std::cout << LineD.at(x-(int)canvascvto2.at(i))<< std::endl;
// 				std::cout << "3.6"<< std::endl;
// 				if (x >= HEIGHT || ( (int)round(canvasP1.y+i) >=  WIDTH || x < 0 || (int)round(canvasP1.y+i) < 0) ){
// 					//std::cout<<"3-2"<<std::endl;
// 				}
// 				else if(1/LineD.at(x-(int)canvascvto2.at(i)) > depthArray[x][(int)round(canvasP1.y+i)]){
// 					std::cout << "3.7"<< std::endl;
// 					depthArray[x][(int)round(canvasP1.y+i)] = 1/LineD.at(x-(int)canvascvto2.at(i));
// 					window.setPixelColour(x, canvasP1.y +i, colour);
// 					std::cout << "3.8"<< std::endl;
//
// 				}
//
// 			}
// 		}
// 	}
// 	std::cout << "4"<< std::endl;
//
// /*
// 	//fill bottom triangle
//
// 	int yDif2 = int(tri.v2().y-CV.y)+1;
//
// 	std::vector<float> xsCV2 = interpolateSingleFloat(CV.x,tri.v2().x, yDif2);
// 	std::vector<float> xsCV2Ds = interpolateSingleFloat(CV.depth,tri.v2().depth, yDif2);
// 	std::vector<float> xs12 = interpolateSingleFloat(tri.v1().x, tri.v2().x, yDif2);
// 	std::vector<float> xs12Ds = interpolateSingleFloat(tri.v1().depth, tri.v2().depth, yDif2);
// 	for(int y = 0; y<yDif2; y++){
// 		//std::cout<<"3-3"<<std::endl;
// 		if(xsCV2.at(y)>xs12.at(y)){
// 			std::swap(xsCV2, xs12);
// 		}
// 		std::vector<float> across = interpolateSingleFloat(xsCV2.at(y), xs12.at(y), abs(xs12.at(y)-xsCV2.at(y))+2);
// 		std::vector<float> acrossDs = interpolateSingleFloat(xsCV2Ds.at(y), xs12Ds.at(y), abs(xs12.at(y)-xsCV2.at(y))+2);
// 		for(int i = 0; i<across.size();i++){
//
// 			if ((int)across.at(i) >= HEIGHT || ( (int) round(tri.v1().y+y) >=  WIDTH || (int)across.at(i) < 0 || (int)round(tri.v1().y+y) < 0) ){
// 				//std::cout<<"3-2"<<std::endl;
// 			}
// 			else if(1/acrossDs.at(i) > depthArray[(int)across.at(i)][(int)round(tri.v1().y+y)]){
// 				depthArray[(int)across.at(i)][(int)round(tri.v1().y+y)] = 1/acrossDs.at(i);
// 				window.setPixelColour((int) floor(across.at(i)), round(CV.y+y), colour);
// 			}
// 		}
// 	}*/
//
// }


////////////////////////////////////////////////////////////////////////////////


std::vector<glm::vec3> interpolateThreeElementValues(glm::vec3 start, glm::vec3 end, int steps){
	//std::cout << "7"<< std::endl;
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
	//std::cout << "8"<< std::endl;
	initalCamera = initalCamera * toRotate;
}

////////////////////////////////////////////////////////////////////////////////

void modifyVectorRotation (glm::mat3 &rotation, glm::mat3 newRotation){
	rotation = rotation + newRotation;
}


void modifyCameraPosition(glm::vec3 &initalCamera,glm::vec3 toAdd){
	//std::cout << "9"<< std::endl;
	initalCamera.x = initalCamera.x + toAdd.x;
	initalCamera.y = initalCamera.y + toAdd.y;
	initalCamera.z = initalCamera.z + toAdd.z;
}


////////////////////////////////////////////////////////////////////////////////

void vec3ToImagePlane(std::vector<ModelTriangle> vs, DrawingWindow &window,glm::vec3 &initalCamera, glm::mat3 rotation){
	//std::cout << "10"<< std::endl;

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
		//std::cout<<"3-7"<<std::endl;

		for(int i = 0; i<3;i++){

			//std::cout<<"3-8"<<std::endl;
			glm::vec3 v = mt.vertices[i];
			v = v * rotation;

					//calculate u

		v = v - cameraPos;
		x = planeMulti * focal * ((v[0]/v[2])*-1) + (WIDTH/2);
		y = planeMulti * focal * (v[1]/v[2]) + (HEIGHT/2);
		//std::cout<<v[2]<<std::endl;
		cp = CanvasPoint(x,y, v[2]*-1);

		temp[i] = cp;

	//	uint32_t colour = (255 << 24) + (int(c.red) << 16) + (int(c.green) << 8) + int(c.blue);
		//std::cout<<"5-1"<<std::endl;

	//	window.setPixelColour(x, y, colour);
		}

		//std::cout<<"4-0"<<std::endl;

		//out.push_back(temp);

		//std::cout<<"4-1"<<std::endl;
		float depthArray[WIDTH][HEIGHT];
		//std::cout<<"4-2"<<std::endl;
		for (int y =0;y<WIDTH;y++){
			for(int x =0;x<HEIGHT;x++){
				depthArray[y][x] = 0;
			}
		}

		// for(float* f:depthArray){
		// 	f = 0;
		// }

		drawFilledTriangle(window, temp, mt.colour, depthArray);

	}
}

////////////////////////////////////////////////////////////////////////////////

void drawTexturedTriangle(DrawingWindow &window, CanvasTriangle tri){

	TextureMap texture = TextureMap("texture.ppm");

	tri.v0().texturePoint = {195, 5};
	tri.v1().texturePoint = {395, 380};
	tri.v2().texturePoint = {65, 330};

	TexturePoint textureP0 = tri.v0().texturePoint;
	TexturePoint textureP1 = tri.v1().texturePoint;
	TexturePoint textureP2 = tri.v2().texturePoint;
	TexturePoint tv;

	CanvasPoint canvasP0 = tri.v0();
	CanvasPoint canvasP1 = tri.v1();
	CanvasPoint canvasP2 = tri.v2();
	CanvasPoint cv;

	//swap points so in increasing y order
	if(canvasP1.y<canvasP0.y){
 		std::swap(canvasP0, canvasP1);
		std::swap(textureP0, textureP1);
	}
	if(canvasP2.y<canvasP1.y){
 		std::swap(canvasP2, canvasP1);
		std::swap(textureP2, textureP1);
	}
	if(canvasP1.y<canvasP0.y){
 		std::swap(canvasP0, canvasP1);
		std::swap(textureP0, textureP1);
	}

	//calculate new points canvas and texture if two points arent on same y
		//canvas
	int ydif02 = (int)canvasP2.y - (int)canvasP0.y;
	int ydifT02 = (int)textureP2.y - (int)textureP0.y;
	//std::cout<<ydifT02<<std::endl;
	if(canvasP1.y != canvasP2.y && canvasP0.y != canvasP1.y){

		std::vector<float> canvas0to2 = interpolateSingleFloat(canvasP0.x, canvasP2.x, ydif02);
		cv = CanvasPoint((int)canvas0to2.at(canvasP1.y-canvasP0.y), canvasP1.y);
	}
	if(canvasP1.y == canvasP0.y){
		cv = canvasP0;
	}
	if(canvasP1.y == canvasP2.y){
		cv = canvasP2;
	}
	if(cv.x>canvasP1.x){
		std::swap(canvasP1, cv);
	}
		//texture
	if(textureP1.y != textureP2.y && textureP0.y != textureP1.y){
		//
		std::vector<float> texture0to2 = interpolateSingleFloat(textureP0.x, textureP2.x, ydifT02);
		tv = TexturePoint((int)texture0to2.at((canvasP1.y/ydif02)*ydifT02), textureP0.y+(canvasP1.y/ydif02)*ydifT02);
	}
	if(textureP1.y == textureP0.y){
		tv = textureP0;
	}
	if(textureP1.y == textureP2.y){
		tv = textureP2;
	}
	if(tv.x>textureP1.x){
		std::swap(textureP1, tv);
	}


	//fill top triangle if top isnt flat
	if(canvasP0.y != canvasP1.y){
		float ydif0to1 = abs(canvasP1.y-canvasP0.y);
		float ydifT0to1 = abs(textureP1.y-textureP0.y);
		std::vector<float> canvas0tocv = interpolateSingleFloat(canvasP0.x, cv.x, ydif0to1);
		std::vector<float> texture0totv = interpolateSingleFloat(textureP0.x, tv.x, ydif0to1);
		std::vector<float> canvas0to1 = interpolateSingleFloat(canvasP0.x, canvasP1.x, ydif0to1);
		std::vector<float> texture0to1 = interpolateSingleFloat(textureP0.x, textureP1.x, ydif0to1);

		for(int i = 0; i<ydif0to1; i++){
			if(i == ydif0to1-1){
				std::cout<<(int)(textureP0.y+i*((textureP1.y-textureP0.y)/ydif0to1))<<std::endl;
			}

			std::vector<float> textureLine = interpolateSingleFloat((int)texture0totv.at(i), (int)texture0to1.at(i), (int)abs(canvas0to1.at(i)-(int)canvas0tocv.at(i))+1);
			for(int x = (int)canvas0tocv.at(i); x<= (int)canvas0to1.at(i); x++){

				window.setPixelColour(x, canvasP0.y +i, xyToTexture1D((int)textureLine.at(x-(int)canvas0tocv.at(i)), (int)(textureP0.y+i*((textureP1.y-textureP0.y)/ydif0to1)), texture));

			}
		}
	}

	//fill bottom triangle if bottom isnt flat
	if(canvasP2.y != canvasP1.y){
		float ydif1to2 = abs(canvasP2.y-canvasP1.y);
		float ydifT1to2 = abs(textureP2.y-textureP1.y);
		std::vector<float> canvascvto2 = interpolateSingleFloat(cv.x, canvasP2.x, ydif1to2);
		std::vector<float> texturetvto2 = interpolateSingleFloat(tv.x, textureP2.x, ydif1to2);
		std::vector<float> canvas1to2 = interpolateSingleFloat(canvasP1.x, canvasP2.x, ydif1to2);
		std::vector<float> texture1to2 = interpolateSingleFloat(textureP1.x, textureP2.x, ydif1to2);

		for(int i = 0; i<ydif1to2; i++){
			if(i == ydif1to2-1){
				std::cout<<(int)(textureP1.y+i*((textureP2.y-textureP1.y)/ydif1to2))<<std::endl;
			}
			std::vector<float> textureLine = interpolateSingleFloat((int)texturetvto2.at(i), (int)texture1to2.at(i), (int)abs(canvas1to2.at(i)-(int)canvascvto2.at(i))+1);
			for(int x = (int)canvascvto2.at(i); x<= (int)canvas1to2.at(i); x++){

				window.setPixelColour(x, canvasP1.y +i, xyToTexture1D((int)textureLine.at(x-(int)canvascvto2.at(i)), (int)(textureP1.y+i*((textureP2.y-textureP1.y)/ydif1to2)), texture));

			}
		}
	}


}



////////////////////////////////////////////////////////////////////////////////


void draw(DrawingWindow &window) {
	//std::cout << "12"<< std::endl;
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
	//std::cout << "13"<< std::endl;
	// Function for performing animation (shifting artifacts or moving the camera)

}


////////////////////////////////////////////////////////////////////////////////



void handleEvent(SDL_Event event, DrawingWindow &window,glm::vec3 &initalCamera, glm::mat3 &rotation ) {
	//std::cout << "14"<< std::endl;
	if (event.type == SDL_KEYDOWN) {

		if (event.key.keysym.sym == SDLK_LEFT) std::cout << "LEFT" << std::endl;

		else if (event.key.keysym.sym == SDLK_RIGHT) std::cout << "RIGHT" << std::endl;

		else if (event.key.keysym.sym == SDLK_UP) std::cout << "UP" << std::endl;

		else if (event.key.keysym.sym == SDLK_DOWN) std::cout << "DOWN" << std::endl;

		else if (event.key.keysym.sym == SDLK_u) drawStrokedTriangle(window, CanvasTriangle(CanvasPoint(rand()%WIDTH,rand()%HEIGHT), CanvasPoint(rand()%WIDTH,rand()%HEIGHT), CanvasPoint(rand()%WIDTH,rand()%HEIGHT)), Colour(rand()%256,rand()%256,rand()%256));

		//else if (event.key.keysym.sym == SDLK_f) drawFilledTriangle(window, CanvasTriangle(CanvasPoint(rand()%WIDTH,rand()%HEIGHT), CanvasPoint(rand()%WIDTH,rand()%HEIGHT), CanvasPoint(rand()%WIDTH,rand()%HEIGHT)), Colour(rand()%256,rand()%256,rand()%256));

		else if (event.key.keysym.sym == SDLK_a) modifyCameraPosition(initalCamera,glm::vec3(0.1, 0, 0));

		else if (event.key.keysym.sym == SDLK_d) modifyCameraPosition(initalCamera,glm::vec3(-0.1, 0, 0));

		else if (event.key.keysym.sym == SDLK_o) modifyCameraPosition(initalCamera,glm::vec3(0.0, 0.1, 0));

		else if (event.key.keysym.sym == SDLK_p) modifyCameraPosition(initalCamera,glm::vec3(0.0, -0.1, 0));

		else if (event.key.keysym.sym == SDLK_w) modifyCameraPosition(initalCamera,glm::vec3(0.0, 0, 0.1));

		else if (event.key.keysym.sym == SDLK_s) modifyCameraPosition(initalCamera,glm::vec3(0.0, 0, -0.1));

		//rotation
		else if (event.key.keysym.sym == SDLK_z) modifyVectorRotation(rotation, glm::mat3(1, 0, 0, 0, cos(0.1), -sin(0.1), 0, sin(0.1), cos(0.1)));

		else if (event.key.keysym.sym == SDLK_x) modifyVectorRotation(rotation, glm::mat3(-1, 0, 0, 0, -cos(0.1), sin(0.1), 0, -sin(0.1), -cos(0.1)));

		else if (event.key.keysym.sym == SDLK_c) modifyVectorRotation(rotation, glm::mat3(cos(0.1), 0, sin(0.1), 0, 1, 0, -sin(0.1), 0, cos(0.1)));

		else if (event.key.keysym.sym == SDLK_v) modifyVectorRotation(rotation, glm::mat3(-cos(0.1), 0, -sin(0.1), 0, -1, 0, sin(0.1), 0, -cos(0.1)));

		else if (event.key.keysym.sym == SDLK_b) modifyVectorRotation(rotation, glm::mat3(cos(0.1), -sin(0.1), 0, sin(0.1), cos(0.1), 0, 0, 0, 1));

		else if (event.key.keysym.sym == SDLK_n) modifyVectorRotation(rotation, glm::mat3(-cos(0.1), sin(0.1), 0, -sin(0.1), -cos(0.1), 0, 0, 0, -1));

	} else if (event.type == SDL_MOUSEBUTTONDOWN) window.savePPM("output.ppm");

}



////////////////////////////////////////////////////////////////////////////////



int main(int argc, char *argv[]) {
	//std::cout << "15"<< std::endl;
	DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);

	SDL_Event event;



	//std::vector<float> result;

	//result = interpolateSingleFloat(2.2, 8.5, 7);

	std::vector<glm::vec3> result = interpolateThreeElementValues(glm::vec3 (1, 4, 9.2), glm::vec3 (4, 1, 9.8), 4);

	// for(size_t i=0; i<result.size(); i++) std::cout << result[i].x << " ";
	//
	// for(size_t i=0; i<result.size(); i++) std::cout << result[i].y << " ";
	//
	// for(size_t i=0; i<result.size(); i++) std::cout << result[i].z << " ";

	//std::cout << std::endl;



	std::vector<ModelTriangle> modelTriangles = readOBJFile();

	glm::vec3 initalCamera  = glm::vec3(0.0, 0.0, 3.0);
	glm::mat3 rotation = glm::mat3(
   1, 0, 0, // first column (not row!)
   0, 1, 0, // second column
   0, 0, 1  // third column
	);

	while (true) {

		// We MUST poll for events - otherwise the window will freeze !

		if (window.pollForInputEvents(event)) handleEvent(event, window,initalCamera, rotation);

		update(window);

		// rotation = rotation * rotation;

		vec3ToImagePlane(modelTriangles, window ,initalCamera, rotation);



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
