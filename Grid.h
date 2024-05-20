#pragma once
#include "Triangle.h"
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

using namespace cv;

class Grid
{
  private : 
	Triangle** grid;
	int squareWidth1;
	int heightsquare1;
	int widthsquare1;
	int squareHeight1;
	Mat image;

  public :
	  Grid();
	  void KillGrid();
	  Grid(const Grid &other);
	  Grid(int squareWidth, int squareHeight , Mat img, int screenWidth, int  screenHeight);
	  float CalcHeight(uchar color);
	  int getWidth();
	  int getHeight();
	  Mat getImage();
	  Triangle** getTriangle();
	  Triangle** makeGrid(float squareWidth , float squareHeight , Mat img , int screenWidth , int  screenHeight);

	  

};


