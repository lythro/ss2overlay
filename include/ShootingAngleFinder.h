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

	// calculate the power needed to hit at dx,dy at given angle
	static int calculatePower(int dx, int dy, float angle, int &miss );

	// find the position of the guns endpoint
	// (needed for more accuracy, the bullet does not come
	//  from the center of the tank)
	static QPoint findGunEndpoint(QPixmap* pixmap, QPoint origin, QColor color);


//private:
	static vector<QPoint> findAnglePoints(QPixmap* pixmap, QPoint origin);
};

#endif
