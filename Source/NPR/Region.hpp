#ifndef REGION_HPP
#define REGION_HPP

#include <vector>
#include <algorithm>
#include <set>
#include <tuple>

#include <Core/Engine.h>

typedef std::tuple<unsigned char, unsigned char, unsigned char> colorPixel;

#define WHITE_PIXEL	colorPixel(255, 255, 255)

class Region
{
public:
	Region();
	Region(colorPixel pixel);
	~Region();

	static int contor;
	static const int threshold;
	int index;
	colorPixel colorRegion;
	int numberPixel;

	bool matchColor(colorPixel color);
	void updateRegionColor(colorPixel color);
};

#endif // REGION_HPP
