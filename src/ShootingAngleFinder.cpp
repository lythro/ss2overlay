#include "ShootingAngleFinder.h"

#include <QImage>

#include <iostream>
using std::cout;
using std::endl;

float ShootingAngleFinder::findAngle(QPixmap* pixmap, QPoint origin, bool &ok)
{
		// TODO
		return 0.;
}


vector<QPoint> ShootingAngleFinder::findAnglePoints(QPixmap* pixmap, QPoint origin)
{
	int range = 200;

	QImage img = pixmap->toImage();

	int width = img.size().width();
	int height = img.size().height();

	vector<QPoint> v;


	for (int dx = -range; dx < range; dx++)
	{
		int x = origin.x() + dx;
		if (x < 0) continue;
		if (x >= width) break;

		for (int dy = -range; dy < range; dy++)
		{
			int y = origin.y() + dy;
			if (y < 0) continue;
			if (y >= height) break;

			QPoint p( x, y );
			QRgb rgb = img.pixel( p );
			QColor c( rgb );

			// well... which points do we accept?

		}
	}

	return v;
}
