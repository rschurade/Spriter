#pragma once

#include <QString>
#include <QPixmap>
#include <QMap>

struct SpriteValues
{
	SpriteValues() {
		x = 0;
		y = 0;
		dimX = 0;
		dimY = 0;
		xOffset = 0;
		yOffset = 0;
		flipHorizontal = false;
		flipVertical = false;
		opacity = 1.0;
	}

	int x;
	int y;
	int dimX;
	int dimY;
	char xOffset;
	char yOffset;
	float opacity;
	bool flipHorizontal;
	bool flipVertical;
	QString effect;
};

struct Sprite
{
	Sprite() {
	}
	Sprite( const Sprite& other )
	{
		id = other.id;
		
		pixmaps = other.pixmaps;
		values = other.values;
	}

	QString id;
	
	
	QMap<QString, SpriteValues> values;
	QMap<QString, QPixmap> pixmaps;
};