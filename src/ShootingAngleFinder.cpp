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


	QColor compare( 255, 255, 255 );

	vector<QPoint> v;

	int mindist = 100000000;

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

			int dr = c.red() - compare.red();
			int dg = c.green() - compare.green();
			int db = c.blue() - compare.blue();

			int dist = dr*dr + dg*dg + db*db;

			mindist = dist < mindist ? dist : mindist;

			//if (c == compare)
			if (dist < 100)
			{
				v.push_back( p );
			}
		}
	}

	cout << " mindist: " << mindist << endl;

	return v;
}
