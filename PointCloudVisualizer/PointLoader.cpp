#include "PointLoader.h"

#include <fstream>
#include <iostream>

PointLoader::PointLoader()
{
}


PointLoader::~PointLoader()
{
}

PointLoader3D::PointLoader3D()
{

}

void PointLoader3D::loadFromFile(const char* filename)
{
	std::ifstream file(filename);

	float x, y, z, temperature;

	for (size_t i = 0; i < 10; i++) {
		file >> x >> y >> z >> temperature;
		std::cout << x << " " << y << " " << z << " " << temperature << std::endl;
	}
}
