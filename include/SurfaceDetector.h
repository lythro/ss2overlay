#ifndef SURFACEDETECTOR_H_
#define SURFACEDETECTOR_H_

#include <QPixmap>
#include <QPoint>

#include <vector>
#include <iostream>

using std::vector;
using std::cout;
using std::endl;


class SurfaceDetector
{
public:
	// index == x coordinate, value == y coordinate
	// ignore a space around the player position
	// because the name and gun might be difficult
	static vector<int> extractSurface( QPixmap& pic, QPoint origin );
};

#endif
