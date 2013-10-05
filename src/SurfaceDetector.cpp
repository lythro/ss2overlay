#include "SurfaceDetector.h"

#include <QImage>
#include <QColor>
#include <QRgb>

#include <vector>
using std::vector;

vector<int> SurfaceDetector::extractSurface( QPixmap& pic, QPoint origin )
{
	QImage img = pic.toImage();

	int width = img.size().width();
	int height = img.size().height();

	// find the surface-height at every x-coordinate.
	// luckily the game allows no caves etc.
	
	vector<int> surface;

	for (int x = 1; x < width; x++)
	{
		if (origin.x()-10 < x && x < origin.x()+10)
		{
			// ignore some space around the player!
			surface.push_back( origin.y() + 2 );
			continue;
		}

		QRgb rgb = img.pixel( QPoint( x, 0 ) );
		QColor color( rgb );

		int val = color.red() + color.green() + color.blue();

		bool found = false;
		for (int y = 0; y < height; y++)
		{
			QRgb trgb = img.pixel( QPoint( x, y ) );
			QColor tcolor( trgb );

			if (tcolor.red() == 255 &&
				tcolor.green() == 255 &&
				tcolor.blue() == 0) continue;

			int tval = tcolor.red() + tcolor.green() + tcolor.blue();

			// no need for sign-check,
			// searching from the sky to the bottom
			// sky: black, ground: blue
			if (tval - val > 170)
			{
				// check if blue-part is greater then the rest
				if (!( tcolor.blue() - 50 > tcolor.green() &&
					   tcolor.blue() - 50 > tcolor.red()))
				{
					// if not - no found.
					val = tval;
					continue;
				}

				surface.push_back( y );
				found = true;
				break;
			}
			val = tval;
		}

		if (!found) surface.push_back( -1 );
	}

	
	/*
	 * there will be holes at steep hills
	 * identified by "-1" value.
	 * we will mend these holes by using a 
	 * linear approximation between the outer points
	 * 
	 * if first point unknown: just set it to be lowest ground possible.
	 * there shouldn't be to many missing points,
	 * so the first approximation will be very steep and unnoticable
	 */
	if (surface[0] == -1) surface[0] = height;

	/* find holes */
	int leftx = 0;
	int lefty = surface[0];

	bool in_hole = false;
	for (int x = 0; x < width; x++)
	{
		// if we run into a hole
		if (surface[x] == -1)
		{
			// notice it
			in_hole = true;
		}
		else
		{
			// if we found some ground
			// check: is there a hole to mend?
			if (in_hole)
			{
				float m = (surface[x] - lefty) / (x-1 - leftx);
				int b = lefty;

				int i = x-1;
				while (surface[i] == -1)
				{
					surface[i] = (int)(m * (i - leftx) + b + 0.5);
					i--;
				}

				in_hole = false;
			}

			leftx = x;
			lefty = surface[x];
		}
	}

	return surface;
}
