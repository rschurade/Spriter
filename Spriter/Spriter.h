#pragma once

#include <sprite.h>

#include <QPixmap>

#include <QtWidgets/QMainWindow>
#include "ui_Spriter.h"


class Spriter : public QMainWindow
{
	Q_OBJECT

public:
	Spriter(QWidget *parent = Q_NULLPTR);
	~Spriter();

	void keyPressEvent( QKeyEvent* event );

private:
	Ui::SpriterClass ui;

	QColor m_currentColor;
	QList<QColor>m_colorPresets;

	void setEditColor();
	void recallPreset( int pos );

	void loadAndParseJSON( QString url );
	void extractSprites();
	void extractPixmap( Sprite& sprite, SpriteValues sv, QString rotation );

	void paint5x5( int levels );

	bool m_pixmapLoaded;
	QPixmap m_pixmap;

	QVariantMap m_spriteDefs;
	QMap<QString, Sprite> m_sprites;

	QList<QString> m_ids5x5;

	Sprite m_currentSprite;
	QString m_currentRot;

private slots:
	void onSliderR( int val );
	void onSliderG( int val );
	void onSliderB( int val );
	void onSliderA( int val );

	void onLineEditR( QString text );
	void onLineEditG( QString text );
	void onLineEditB( QString text );
	void onLineEditA( QString text );

	void onCellClicked( int row, int column );
	void onTreeClicked( QTreeWidgetItem* twi, int column );

	void onLevelDefSelected( int level );
	void onItemLevelDefChanged( int row, int column );

	void onButtonCommit();
	void onButtonCancel();


public slots:
	void onLoad();
	void onLoadPNG();
	void onSave();
};
