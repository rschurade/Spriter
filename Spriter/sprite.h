#pragma once

#include <QString>
#include <QPixmap>
#include <QMap>

struct Sprite
{
	Sprite() {
	}
	Sprite( const Sprite& other )
	{
		id = other.id;
		x = other.x;
		y = other.y;
		dimX = other.dimX;
		dimY = other.dimY;
		xOffset = other.xOffset;
		yOffset = other.yOffset;
		opacity = other.opacity;
		effect = other.effect;
		pixmaps = other.pixmaps;
		spriteID = other.spriteID;
	}

	QString id;
	int x;
	int y;
	int dimX;
	int dimY;
	char xOffset;
	char yOffset;
	float opacity;
	QString effect;
	QMap<QString, QPixmap> pixmaps;

	unsigned int spriteID;

};