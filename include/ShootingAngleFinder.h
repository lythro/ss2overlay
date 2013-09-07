#ifndef SHOOTINGANGLEFINDER_H_
#define SHOOTINGANGLEFINDER_H_

#include <QPoint>
#include <QPixmap>

#include <vector>
using std::vector;

class ShootingAngleFinder
{
public:
	// finds the shooting-angel from the given pixmap by evaluating
	// pixel colors around the origin
	static float findAngle(QPixmap* pixmap, QPoint origin, bool &ok);

//private:
	static vector<QPoint> findAnglePoints(QPixmap* pixmap, QPoint origin);
};

#endif
