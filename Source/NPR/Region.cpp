#include "Region.hpp"

int Region::contor = 0;
const int Region::threshold = 50;

Region::Region()
{
	this->index = Region::contor;
	Region::contor++;
	this->colorRegion = colorPixel(0, 0, 0);
	this->numberPixel = 0;
}

Region::Region(colorPixel pixel)
{
	this->index = Region::contor;
	Region::contor++;
	this->colorRegion = pixel;
	this->numberPixel = 1;
}

Region::~Region()
{
}

bool Region::matchColor(colorPixel color)
{
	if (std::abs(std::get<0>(this->colorRegion) - std::get<0>(color)) +
		std::abs(std::get<1>(this->colorRegion) - std::get<1>(color)) +
		std::abs(std::get<2>(this->colorRegion) - std::get<2>(color)) < Region::threshold
		)
		return true;

	return false;
}

void Region::updateRegionColor(colorPixel color)
{
	unsigned int red = (std::get<0>(this->colorRegion) * this->numberPixel + std::get<0>(color)) / (this->numberPixel + 1);
	unsigned int green = (std::get<1>(this->colorRegion) * this->numberPixel + std::get<1>(color)) / (this->numberPixel + 1);
	unsigned int blue = (std::get<2>(this->colorRegion) * this->numberPixel + std::get<2>(color)) / (this->numberPixel + 1);

	this->colorRegion = colorPixel(red, green, blue);
	this->numberPixel++;
}