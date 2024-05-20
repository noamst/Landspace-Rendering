#include "PointColor.h"


PointColor::PointColor() {
	x = 0; 
	y = 0;
	z = 0;
}

PointColor::PointColor(float v1, float v2, float v3, uchar color) {
	x = v1;
	y = v2;
	z = v3;
	BGRColor = color;
}

float PointColor::getV1() {
	return x;
}
float PointColor::getV2() {
	return y;
}
float PointColor::getV3() {
	return z;
}
uchar PointColor::getColor() {
	return BGRColor;
}
Vec3b PointColor::getColorRGB() {
	return colorRGB;
}
void PointColor::setColorRGB(Vec3b pickColor) {
	colorRGB = pickColor;
}
void PointColor::setColor(uchar pickColor) {
	BGRColor = pickColor;
}

void PointColor::setV1(float x1) {
	x = x1;
}

void PointColor::setV2(float y1) {
	y = y1;
}

void PointColor::setV3(float z1) {
	z = z1;
}


