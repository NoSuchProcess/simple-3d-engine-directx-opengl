#ifndef S3DE_GLOBALS_H
	#include "../globals.h"
#endif

#ifndef S3DE_TERRAIN_H
#define S3DE_TERRAIN_H

class Terrain : public Component
{
public:
	Terrain(const std::vector<wxString> &textureImageFiles, int size, int octaves, float redistribution);
	Terrain();
	~Terrain() {}

private:
	int                   octaves;
	float                 redistribution;
	int                   size;
	std::vector<wxString> textureImageFiles;

private:
	void create(int size, int octaves, float redistribution);

public:
	int   Octaves();
	void  Resize(int size, int octaves, float redistribution);
	float Redistribution();
	int   Size();

};

#endif
