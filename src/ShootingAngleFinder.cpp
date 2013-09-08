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
		if (x < 5) continue;
		if (x > width-5) break;

		for (int dy = -range; dy < range; dy++)
		{
			// skip my user-name-area! evil white name!
			if ( -20 < dx && dx < 20 )
				if ( -40 < dy && dy < -25 )
					continue;

			//
			int y = origin.y() + dy;
			if (y < 30) continue; // cut the top - titlebar!
			if (y > height-5) break;

			QPoint p( x, y );
			QRgb rgb = img.pixel( p );
			QColor c( rgb );


			int sum = c.red() + c.green() + c.blue();

			int drg = c.red() - c.green();
			drg = drg < 0 ? drg * -1 : drg;

			int dgb = c.green() - c.blue();
			dgb = dgb < 0 ? dgb * -1 : dgb;

			int drb = c.red() - c.blue();
			drb = drb < 0 ? drb * -1 : drb;

			if ((sum > 200 & drg < 30 && dgb < 30 && drb < 30)
			 || (sum > 300 & drg < 50 && dgb < 50 && drb < 70))
					v.push_back( p );
		}
	}

	return v;
}
