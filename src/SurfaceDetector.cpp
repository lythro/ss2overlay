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
		if (origin.x()-5 < x && x < origin.x()+5)
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
			if (tval - val > 120)
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
	 * if first point unknown:
	 * set it to have the same value as the first valid point.
	 * just setting it to height causes trouble with outer-bound-bouncing
	 */
	if (surface[0] == -1)
	{
		for (int i = 1; i < surface.size(); i++)
		{
			if (surface[i] != -1)
			{
				surface[0] = surface[i];
				break;
			}
		}
	}

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
				float m = ((float)(surface[x] - lefty)) / ((float)(x-1 - leftx));
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

	/* fix the right edge just like the left */
	if (in_hole)
	{
		int y = height;
		for (int x = surface.size()-1; x >= 0; x--)
		{
			if (surface[x] != -1)
			{
				y = surface[x];
			}
		}

		int x = surface.size()-1;
		while (surface[x] == -1)
		{
			surface[x--] = y;
		}
	}

	return surface;
}
