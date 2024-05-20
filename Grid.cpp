#include "Grid.h"
#include "PointColor.h"

Grid::Grid() {
	grid = NULL;
	squareWidth1 = 0;
	squareHeight1 = 0 ;
	
}
Mat Grid::getImage() {
	return image;
}
Grid::Grid(const Grid& other) {
	image = other.image;
	squareHeight1 = other.squareHeight1;
	squareWidth1 = other.squareWidth1;
	widthsquare1 = other.widthsquare1;
	heightsquare1 = other.heightsquare1;
	Triangle** otherT = other.grid;
	grid = new Triangle * [2 * squareHeight1 * squareWidth1 + 1];
	int i = 0;
	while (otherT[i] != NULL) {
		
		grid[i] = new Triangle(*otherT[i]);
		i = i + 1;

	}
	grid[i] = NULL;
}

Vec3b convolve(int i, int j,int coeffiecient,Mat image) {
	Vec3b ConvolvedColor = image.at<Vec3b>(i, j);
	if (((i < image.rows - coeffiecient) && (0 < i - coeffiecient)) 
		|| ((j < image.cols - coeffiecient) && (0 < j - coeffiecient))){
	  for (int k = i - coeffiecient; i < i + coeffiecient-1; k++) {
		  for (int m = j - coeffiecient; j < j + coeffiecient-1; m++) {
			  Vec3b color = image.at<Vec3b>(k,m);
			  ConvolvedColor = (1 / ((2*coeffiecient+1) * (2 * coeffiecient + 1)) * color);
		  }
	  }
	}
	else {
		ConvolvedColor = image.at<Vec3b>(i, j);
	}
	return ConvolvedColor;
}


float Grid::CalcHeight(uchar color) {
	
	
 	return (float)(color/2);
 
}

Triangle** Grid::makeGrid(float squareWidth, float squareHeight, Mat image, int orgWidth, int  orgHeight) {

	Triangle** grid = new Triangle * [2 * squareHeight * squareWidth + 1];
	float widthSquare = (image.cols / squareWidth);
	float heightSquare = (image.rows / squareHeight);
	float widthsquare1 = (image.cols / squareWidth);
	float heightsquare1 = (image.rows / squareHeight);
	float proportionWidth = 1;//squareWidth / orgWidth;
	float proportionHeight = 1;//squareHeight / orgHeight;
	int k = 0;
	for (int i = 0; i < (squareHeight); i++) {
		for (int j = 0; j < (squareWidth); j++) {
			int fixedRow = (i + 1) * heightSquare;
			int fixedCol = (j + 1) * widthSquare;
			if ((i + 1) * heightSquare >= image.rows) {
				fixedRow = image.rows - 1;
			}
			if ((j + 1) * widthSquare >= image.cols) {
				fixedCol = image.cols - 1;
			}
			// bottom triangle top point BGR (Vector[0]:Blue,Vector[1]:Green,Vector[2]:Red)
			uchar color = image.at<uchar>(i * heightSquare, j * widthSquare);
			//Vec3b color = convolve(i * heightSquare, j * widthSquare,1,image);
			PointColor point1 = PointColor(i *heightSquare, j *widthSquare , CalcHeight(color), color);
			// bottom triangle left base point
			uchar color1 = image.at<uchar>(fixedRow, (j) * widthSquare);
			//color = convolve(i * heightSquare, (j + 1) * widthSquare, 1, image);
			PointColor point2 = PointColor((i+1)* heightSquare,j* widthSquare, CalcHeight(color1), color1);
			// bottom triangle right base point
			uchar color2 = image.at<uchar>(fixedRow, fixedCol);
			//color = convolve((i + 1) * heightSquare, (j + 1) * widthSquare, 1, image);
			PointColor point3 = PointColor((i + 1) * heightSquare, (j + 1) * widthSquare, CalcHeight(color2), color2);
			grid[k] = new Triangle(point1, point2, point3);
			k = k + 1;
			// upper triangle top point
			PointColor point4 = PointColor(point1.getV1(), point1.getV2(), point1.getV3(),point1.getColor());
			// upper triangle right top point
			uchar color3 = image.at<uchar>((i) * heightSquare, fixedCol);
			//color = convolve((i + 1) * heightSquare, (j)*widthSquare, 1, image);
			PointColor point5 = PointColor((i) * heightSquare, (j + 1) * widthSquare, CalcHeight(color3), color3);
			// bottom triangle right base point
			PointColor point6 = PointColor(point3.getV1(), point3.getV2(), point3.getV3(), point3.getColor());
			grid[k] = new Triangle(point4, point5, point6);
			k =k+ 1;

		}
		
	}
	grid[k] = NULL;

	return grid;



}


void Grid::KillGrid()
 { 
	int k = 0;
    while (grid[k] != NULL) {
		delete grid[k];
		k++;	
	}

 }
Grid::Grid(int squareWidth, int squareHeight, Mat img, int screenWidth, int  screenHeight) {
	squareWidth1 = squareWidth;
	squareHeight1 = squareHeight;
	image = img;
	grid = makeGrid(squareWidth, squareHeight,img, screenWidth, screenHeight);
}

Triangle** Grid::getTriangle() {
	return grid; 
}

int Grid::getWidth() {
	return squareWidth1;
}
int Grid::getHeight() {
	return squareHeight1;
}
