#pragma once
#include "PointColor.h"
class Triangle
{
  private:
	   PointColor vertices[3];
	   bool IsMarked; 
  public :
	  Triangle();
	  Triangle(const Triangle & br);
	  //~Triangle();
	  Triangle(PointColor v1, PointColor v2, PointColor v3);
	  PointColor getVertice(int i);
	  PointColor * getVerticeAddress(int i);
	  void setVertice(float x, float y, float z,uchar color,int i);
	  void setMark(bool isMarked);
	  bool getMark();
};

