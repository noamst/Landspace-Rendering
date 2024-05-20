#include "Triangle.h"

Triangle::Triangle() {

}
Triangle::Triangle(const Triangle& br) {
	vertices[0] = br.vertices[0];
	vertices[1] = br.vertices[1];
	vertices[2] = br.vertices[2];
	IsMarked = br.IsMarked;
}
Triangle::Triangle(PointColor v1, PointColor v2, PointColor v3) {
	vertices[0] = v1;
	vertices[1] = v2;
	vertices[2] = v3;
	IsMarked = false;
}

PointColor Triangle::getVertice(int i) {
	return vertices[i];
}
void Triangle::setVertice(float x , float y , float z,uchar color,int i) {
	vertices[i] = PointColor(x, y, z, color);
}
PointColor * Triangle::getVerticeAddress(int i) {
	return vertices + i;
}

void Triangle::setMark(bool Ismarked) {
	IsMarked = Ismarked;
}
bool Triangle::getMark() {
	return IsMarked;

}