#pragma once
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
using namespace cv;
class PointColor
{
   private :
	   float x;
	   float y;
	   float z;
	   uchar BGRColor;
	   Vec3b colorRGB;
   public :
	   PointColor();
	   PointColor(float v1 , float v2 , float v3,uchar color);
	   float getV1();
	   float getV2();
	   float getV3();
	   uchar getColor();
	   Vec3b getColorRGB();
	   void setColorRGB(Vec3b pickColor);
	   void setColor(uchar pickColor);
	   void setV1(float x);
	   void setV2(float y);
	   void setV3(float z);
	   //float * getCords();

};

