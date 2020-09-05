#ifndef POINT_LOADER_H
#define POINT_LOADER_H

#include "glm/glm.hpp"

class PointLoader
{
public:
	PointLoader();
	~PointLoader();

	virtual void loadFromFile(const char* filename) {};

	//glm::vec3 parseLine();

private:
	//glm::vec3 mVerticies[];

};

class PointLoader3D : public PointLoader
{
public:
	PointLoader3D();

	virtual void loadFromFile(const char* filename);
private:
	
};



/*
 PLY - Polygon file format.

 Format description: http://paulbourke.net/dataformats/ply/

ply
format ascii 1.0           { ascii/binary, format version number }
comment made by Greg Turk  { comments keyword specified, like all lines }
comment this file is a cube
element vertex 8           { define "vertex" element, 8 of them in file }
property float x           { vertex contains float "x" coordinate }
property float y           { y coordinate is also a vertex property }
property float z           { z coordinate, too }
element face 6             { there are 6 "face" elements in the file }
property list uchar int vertex_index { "vertex_indices" is a list of ints }
end_header                 { delimits the end of the header }
0 0 0                      { start of vertex list }
0 0 1
0 1 1
0 1 0
1 0 0
1 0 1
1 1 1
1 1 0
4 0 1 2 3                  { start of face list }
4 7 6 5 4
4 0 4 5 1
4 1 5 6 2
4 2 6 7 3
4 3 7 4 0

*/
class PointLoaderPLY : public PointLoader
{
public:
	PointLoaderPLY();

	virtual void loadFromFile(const char* filename);
private:

};

#endif // !POINT_LOADER_H