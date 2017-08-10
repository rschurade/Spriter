#include "Spriter.h"

#include <QBitMap>
#include <QDebug>
#include <QFileDialog>
#include <QLineEdit>
#include <QKeyEvent>
#include <QSettings>
#include <QJsonDocument>
#include <QVariant>
#include <QVariantMap>
#include <QTreeWidgetItem>
#include <QPainter>
#include <QPushButton>

Spriter::Spriter(QWidget *parent)
	: QMainWindow(parent),
	m_pixmapLoaded( false )
{
	ui.setupUi(this);

	ui.lineEditR->setValidator( new QIntValidator(0, 255, this) );
	ui.lineEditG->setValidator( new QIntValidator(0, 255, this) );
	ui.lineEditB->setValidator( new QIntValidator(0, 255, this) );
	ui.lineEditA->setValidator( new QIntValidator(0, 255, this) );

	connect( ui.sliderR, &QSlider::sliderMoved, this, &Spriter::onSliderR );
	connect( ui.sliderG, &QSlider::sliderMoved, this, &Spriter::onSliderG );
	connect( ui.sliderB, &QSlider::sliderMoved, this, &Spriter::onSliderB );
	connect( ui.sliderA, &QSlider::sliderMoved, this, &Spriter::onSliderA );

	connect( ui.lineEditR, &QLineEdit::textEdited, this, &Spriter::onLineEditR );
	connect( ui.lineEditG, &QLineEdit::textEdited, this, &Spriter::onLineEditG );
	connect( ui.lineEditB, &QLineEdit::textEdited, this, &Spriter::onLineEditB );
	connect( ui.lineEditA, &QLineEdit::textEdited, this, &Spriter::onLineEditA );

	connect( ui.tableWidgetSprite, &QTableWidget::cellClicked, this, &Spriter::onCellClicked );
	connect( ui.treeWidget, &QTreeWidget::itemClicked, this, &Spriter::onTreeClicked );

	connect( ui.actionLoad, &QAction::triggered, this, &Spriter::onLoad );
	connect( ui.actionLoadPNG, &QAction::triggered, this, &Spriter::onLoadPNG );
	connect( ui.actionSave, &QAction::triggered, this, &Spriter::onSave );

	connect( ui.sliderLevelDef, &QSlider::valueChanged, this, &Spriter::onLevelDefSelected );
	connect( ui.tableWidgetLevelDef, &QTableWidget::cellChanged, this, &Spriter::onItemLevelDefChanged );

	connect( ui.sliderLevel, &QSlider::valueChanged, this, &Spriter::paint5x5 );
	connect( ui.sliderYMax, &QSlider::valueChanged, this, &Spriter::paint5x5 );

	connect( ui.pushButtonCommit, &QPushButton::clicked, this, &Spriter::onButtonCommit );
	connect( ui.pushButtonCancel, &QPushButton::clicked, this, &Spriter::onButtonCancel );
	connect( ui.pushButtonSave, &QPushButton::clicked, this, &Spriter::onSave );

	ui.labelColor->setAutoFillBackground( true );

	QColor c( 255, 255, 255 );
	for( int i = 0; i < 10; ++i )
	{
		m_colorPresets.append( c );
		QTableWidgetItem *newItem = new QTableWidgetItem;
		ui.tableWidgetColors->setItem( 0, i, newItem );
	}
	for( int i = 0; i < 32; ++i )
	{
		for ( int k = 0; k < 32; ++k )
		{
			QTableWidgetItem *newItem = new QTableWidgetItem;
			ui.tableWidgetSprite->setItem( i, k, newItem );
		}
	}

	QSettings settings( "./settings", QSettings::IniFormat );

	for( int i = 0; i < 250; ++i )
	{
		if( settings.contains( "def" + QString::number( i ) ) )
		{
			m_ids5x5.append( settings.value( "def" + QString::number( i ) ).toString() );
		}
		else
		{
			m_ids5x5.append( "" );
		}
	}
	onLevelDefSelected( 0 );


	
	if( settings.contains( "preset0" ) ) m_colorPresets.replace( 0, settings.value( "preset0" ).value<QColor>() );
	if( settings.contains( "preset1" ) ) m_colorPresets.replace( 1, settings.value( "preset1" ).value<QColor>() );
	if( settings.contains( "preset2" ) ) m_colorPresets.replace( 2, settings.value( "preset2" ).value<QColor>() );
	if( settings.contains( "preset3" ) ) m_colorPresets.replace( 3, settings.value( "preset3" ).value<QColor>() );
	if( settings.contains( "preset4" ) ) m_colorPresets.replace( 4, settings.value( "preset4" ).value<QColor>() );
	if( settings.contains( "preset5" ) ) m_colorPresets.replace( 5, settings.value( "preset5" ).value<QColor>() );
	if( settings.contains( "preset6" ) ) m_colorPresets.replace( 6, settings.value( "preset6" ).value<QColor>() );
	if( settings.contains( "preset7" ) ) m_colorPresets.replace( 7, settings.value( "preset7" ).value<QColor>() );
	if( settings.contains( "preset8" ) ) m_colorPresets.replace( 8, settings.value( "preset8" ).value<QColor>() );
	if( settings.contains( "preset9" ) ) m_colorPresets.replace( 9, settings.value( "preset9" ).value<QColor>() );
	for( int i = 0; i < 10; ++i )
	{
		ui.tableWidgetColors->item( 0, i )->setBackgroundColor( m_colorPresets[i] );
	}
	recallPreset( 0 );
	
	if( settings.contains( "jsonPath" ) )
	{
		loadAndParseJSON( settings.value( "jsonPath" ).toString() );
	}
	if( settings.contains( "pngPath" ) )
	{
		QPixmap pm;
		m_pixmapLoaded = pm.load( settings.value( "pngPath" ).toString() );
		m_pixmap = pm;
		if( m_pixmapLoaded )
		{
			extractSprites();
		}
	}

	paint5x5( 0 );
}

Spriter::~Spriter()
{
	QSettings settings( "./settings", QSettings::IniFormat );
	settings.setValue( "preset0", m_colorPresets[0] );
	settings.setValue( "preset1", m_colorPresets[1] );
	settings.setValue( "preset2", m_colorPresets[2] );
	settings.setValue( "preset3", m_colorPresets[3] );
	settings.setValue( "preset4", m_colorPresets[4] );
	settings.setValue( "preset5", m_colorPresets[5] );
	settings.setValue( "preset6", m_colorPresets[6] );
	settings.setValue( "preset7", m_colorPresets[7] );
	settings.setValue( "preset8", m_colorPresets[8] );
	settings.setValue( "preset9", m_colorPresets[9] );
	for( int i = 0; i < 250; ++i )
	{
		settings.setValue( "def" + QString::number( i ), m_ids5x5[i] );
	}
}

void Spriter::onLevelDefSelected( int level )
{
	ui.tableWidgetLevelDef->clear();
	for( int i = 0; i < 10; ++i )
	{
		for ( int k = 0; k < 5; ++k )
		{
			QString id = m_ids5x5[k + i * 5 + level * 50];
			QTableWidgetItem *newItem = new QTableWidgetItem( id );
			ui.tableWidgetLevelDef->setItem( i, k, newItem );
		}
	}
}

void Spriter::onItemLevelDefChanged( int row, int column )
{
	m_ids5x5.replace( column + row * 5 + ui.sliderLevelDef->value() * 50, ui.tableWidgetLevelDef->item( row, column )->text() );
	paint5x5( ui.sliderLevel->value() );
}

void Spriter::onSliderR( int val )
{
	ui.lineEditR->setText( QString::number( val ) );
	setEditColor();
}
void Spriter::onSliderG( int val )
{
	ui.lineEditG->setText( QString::number( val ) );
	setEditColor();
}
void Spriter::onSliderB( int val )
{
	ui.lineEditB->setText( QString::number( val ) );
	setEditColor();
}
void Spriter::onSliderA( int val )
{
	ui.lineEditA->setText( QString::number( val ) );
	setEditColor();
}

void Spriter::onLineEditR( QString text )
{
	ui.sliderR->setValue( text.toInt() );
	setEditColor();
}
void Spriter::onLineEditG( QString text )
{
	ui.sliderG->setValue( text.toInt() );
	setEditColor();
}
void Spriter::onLineEditB( QString text )
{
	ui.sliderB->setValue( text.toInt() );
	setEditColor();
}
void Spriter::onLineEditA( QString text )
{
	ui.sliderA->setValue( text.toInt() );
	setEditColor();
}

void Spriter::setEditColor()
{
	int r = ui.sliderR->value();
	int g = ui.sliderG->value();
	int b = ui.sliderB->value();
	int a = ui.sliderA->value();
	m_currentColor.setRed( r );
	m_currentColor.setGreen( g );
	m_currentColor.setBlue( b );
	m_currentColor.setAlpha( a );
	
	ui.labelColor->setStyleSheet( "QLabel { background-color: " + m_currentColor.name() + " }" );
}

void Spriter::keyPressEvent( QKeyEvent* event )
{
	if( event->modifiers() & Qt::ControlModifier )
	{
		int r = ui.sliderR->value();
		int g = ui.sliderG->value();
		int b = ui.sliderB->value();
		int a = ui.sliderA->value();
		QColor c( r, g, b );

		switch ( event->key() )
		{
			case Qt::Key_1:
				ui.tableWidgetColors->item( 0, 0 )->setBackgroundColor( c );
				m_colorPresets.replace( 0, c );
				break;
			case Qt::Key_2:
				ui.tableWidgetColors->item( 0, 1 )->setBackgroundColor( c );
				m_colorPresets.replace( 1, c );
				break;
			case Qt::Key_3:
				ui.tableWidgetColors->item( 0, 2 )->setBackgroundColor( c );
				m_colorPresets.replace( 2, c );
				break;
			case Qt::Key_4:
				ui.tableWidgetColors->item( 0, 3 )->setBackgroundColor( c );
				m_colorPresets.replace( 3, c );
				break;
			case Qt::Key_5:
				ui.tableWidgetColors->item( 0, 4 )->setBackgroundColor( c );
				m_colorPresets.replace( 4, c );
				break;
			case Qt::Key_6:
				ui.tableWidgetColors->item( 0, 5 )->setBackgroundColor( c );
				m_colorPresets.replace( 5, c );
				break;
			case Qt::Key_7:
				ui.tableWidgetColors->item( 0, 6 )->setBackgroundColor( c );
				m_colorPresets.replace( 6, c );
				break;
			case Qt::Key_8:
				ui.tableWidgetColors->item( 0, 7 )->setBackgroundColor( c );
				m_colorPresets.replace( 7, c );
				break;
			case Qt::Key_9:
				ui.tableWidgetColors->item( 0, 8 )->setBackgroundColor( c );
				m_colorPresets.replace( 8, c );
				break;
			case Qt::Key_0:
				ui.tableWidgetColors->item( 0, 9 )->setBackgroundColor( c );
				m_colorPresets.replace( 9, c );
				break;
		}
	}
	else
	{
		switch ( event->key() )
		{
			case Qt::Key_1: recallPreset( 0 ); break;
			case Qt::Key_2: recallPreset( 1 ); break;
			case Qt::Key_3: recallPreset( 2 ); break;
			case Qt::Key_4: recallPreset( 3 ); break;
			case Qt::Key_5: recallPreset( 4 ); break;
			case Qt::Key_6: recallPreset( 5 ); break;
			case Qt::Key_7: recallPreset( 6 ); break;
			case Qt::Key_8: recallPreset( 7 ); break;
			case Qt::Key_9: recallPreset( 8 ); break;
			case Qt::Key_0: recallPreset( 9 ); break;
		}
	}
}

void Spriter::recallPreset( int pos )
{
	QColor c = m_colorPresets[pos];
	m_currentColor = c;
	ui.sliderR->setValue( c.red() );
	ui.sliderG->setValue( c.green() );
	ui.sliderB->setValue( c.blue() );
	ui.sliderA->setValue( c.alpha() );
	ui.lineEditR->setText( QString::number( c.red() ) );
	ui.lineEditG->setText( QString::number( c.green() ) );
	ui.lineEditB->setText( QString::number( c.blue() ) );
	ui.lineEditA->setText( QString::number( c.alpha() ) );
	ui.labelColor->setStyleSheet( "QLabel { background-color: " + m_currentColor.name() + " }" );
}

void Spriter::onCellClicked( int row, int column )
{
	QColor color = m_currentColor;
	if( ui.pushButtonDelete->isChecked() )
	{
		color = QColor( 0, 0, 0, 0 );
		
	}
	
	if( ui.radioButton1->isChecked() )
	{
		ui.tableWidgetSprite->item( row, column )->setBackgroundColor( color );
	}
	else if( ui.radioButton3->isChecked() )
	{
		for( int x = column - 1; x < column + 2; ++x  )
		{
			for( int y = row - 1; y < row + 2; ++y  )
			{
				ui.tableWidgetSprite->item( qMin( 31, qMax( y, 0 ) ), qMin( 31, qMax( x, 0 ) ) )->setBackgroundColor( color );
			}
		}
		
	}
	else if( ui.radioButton5->isChecked() )
	{
		for( int x = column - 2; x < column + 3; ++x  )
		{
			for( int y = row - 2; y < row + 3; ++y  )
			{
				ui.tableWidgetSprite->item( qMin( 31, qMax( y, 0 ) ), qMin( 31, qMax( x, 0 ) ) )->setBackgroundColor( color );
			}
		}

	}

}

void Spriter::onLoad()
{
	qDebug() << "onLoad()";
	QString filter( "all files (*.*);;json (*.json)" );

	QString path;
	QSettings settings( "./settings", QSettings::IniFormat );
	if( settings.contains( "jsonPath" ) )
	{
		path = settings.value( "jsonPath" ).toString();
	}

	QFileDialog* fd = new QFileDialog( this, "Open File", path, filter );
	fd->setFileMode( QFileDialog::ExistingFile );
	fd->setAcceptMode( QFileDialog::AcceptOpen );

	QStringList fileNames;
	if ( fd->exec() )
	{
		fileNames = fd->selectedFiles();
	}
	delete fd;
	if( !fileNames.empty() )
	{
		settings.setValue( "jsonPath", fileNames.first() );
		loadAndParseJSON( fileNames.first() );
	}
}

void Spriter::onLoadPNG()
{
	qDebug() << "onLoad()";
	QString filter( "all files (*.*);;PNG (*.png)" );

	QString path;
	QSettings settings( "./settings", QSettings::IniFormat );
	if( settings.contains( "pngPath" ) )
	{
		path = settings.value( "pngPath" ).toString();
	}

	QFileDialog* fd = new QFileDialog( this, "Open File", path, filter );
	fd->setFileMode( QFileDialog::ExistingFile );
	fd->setAcceptMode( QFileDialog::AcceptOpen );

	QStringList fileNames;
	if ( fd->exec() )
	{
		fileNames = fd->selectedFiles();
	}
	delete fd;
	if( !fileNames.empty() )
	{
		settings.setValue( "pngPath", fileNames.first() );
		QPixmap pm;
		m_pixmapLoaded = pm.load( fileNames.first() );
		m_pixmap = pm;
		if( m_pixmapLoaded )
		{
			extractSprites();
		}
	}
}

void Spriter::onSave()
{
	QSettings settings( "./settings", QSettings::IniFormat );
	QString path = settings.value( "pngPath" ).toString();
	
	QPainter* painter = new QPainter( &m_pixmap );
	int posX = ui.lineEditPosX->text().toInt();
	int posY = ui.lineEditPosY->text().toInt();
	int w = ui.lineEditSizeX->text().toInt();
	int h = ui.lineEditSizeY->text().toInt();
	
	//painter->eraseRect( posX, posY, w, h );
	painter->setCompositionMode( QPainter::CompositionMode_Source );
	painter->fillRect( posX, posY, w, h, Qt::transparent );
	painter->drawPixmap( posX, posY, m_currentSprite.pixmaps[m_currentRot] );
	delete painter;
	qDebug() << path;
	m_pixmap.save( path, "PNG" );


	m_spriteDefs.clear();
	m_sprites.clear();

	if( settings.contains( "jsonPath" ) )
	{
		loadAndParseJSON( settings.value( "jsonPath" ).toString() );
	}
	if( settings.contains( "pngPath" ) )
	{
		QPixmap pm;
		m_pixmapLoaded = pm.load( settings.value( "pngPath" ).toString() );
		m_pixmap = pm;
		if( m_pixmapLoaded )
		{
			extractSprites();
		}
	}

	paint5x5( 0 );
}

void Spriter::loadAndParseJSON( QString url )
{
	QFile file( url );
	file.open( QIODevice::ReadOnly | QIODevice::Text );
	QString content = file.readAll();
	file.close();
	QJsonDocument sd = QJsonDocument::fromJson( content.toUtf8() );
	QVariantList spriteList = sd.toVariant().toList();

	ui.treeWidget->clear();
	m_spriteDefs.clear();

	for( auto spriteVariant : spriteList )
	{
		QVariantMap sm = spriteVariant.toMap();
		if ( sm.contains( "ID" ) )
		{
			QString id = sm.value( "ID" ).toString();
			m_spriteDefs.insert( id, sm );
			QTreeWidgetItem* twi = new QTreeWidgetItem( { id } );
			ui.treeWidget->addTopLevelItem( twi );
			if( sm.contains( "Rotations" ) )
			{
				QVariantList rl = sm.value( "Rotations" ).toList();
				for( auto rv : rl )
				{
					QVariantMap rm = rv.toMap();
					if( rm.contains( "Rotation" ) )
					{
						QString rot = rm.value( "Rotation" ).toString();
						QTreeWidgetItem* twic = new QTreeWidgetItem( { rot } );
						twi->addChild( twic );
					}
				}
			}
		}
	}
}

void Spriter::extractSprites()
{
	if( !m_pixmapLoaded ) return;

	for( auto sv : m_spriteDefs )
	{
		QVariantMap sm = sv.toMap();

		if ( !sm.contains( "Rotations" ) )
		{
			// Sprite with just one direction
			QString id = sm.value( "ID" ).toString();
			Sprite s;
			s.id = id;
			SpriteValues sv;


			QString rect = sm.value( "SourceRectangle" ).toString();
			QString offset = sm.value( "Offset" ).toString();

			QStringList rl = rect.split( " " );
			if ( rl.size() == 4 )
			{
				sv.x = rl[0].toInt();
				sv.y = rl[1].toInt();
				sv.dimX = rl[2].toInt();
				sv.dimY = rl[3].toInt();
			}
			QStringList ol = offset.split( " " );
			if ( ol.size() == 2 )
			{
				sv.xOffset = ol[0].toInt();
				sv.yOffset = ol[1].toInt();
			}
			extractPixmap( s, sv, "" );
			m_sprites.insert( s.id, s );
		}
		else
		{
			QString id = sm.value( "ID" ).toString();
			Sprite s;
			s.id = id;
			
			QVariantList rotList = sm.value( "Rotations" ).toList();
			
			for( auto rotEntry : rotList )
			{
				SpriteValues sv;
				QString offset = sm.value( "Offset" ).toString();
				QStringList ol = offset.split( " " );
				if ( ol.size() == 2 )
				{
					sv.xOffset = ol[0].toInt();
					sv.yOffset = ol[1].toInt();
				}

				QVariantMap rem = rotEntry.toMap();
				QString suffix = rem.value( "Rotation" ).toString();
				//qDebug() << s.id << suffix;
				QString rect = rem.value( "SourceRectangle" ).toString();
				sv.flipHorizontal = rem.value( "FlipHorizontal" ).toBool();
				sv.flipVertical = rem.value( "FlipVertical" ).toBool();

				QStringList rl = rect.split( " " );
				if ( rl.size() == 4 )
				{
					sv.x = rl[0].toInt();
					sv.y = rl[1].toInt();
					sv.dimX = rl[2].toInt();
					sv.dimY = rl[3].toInt();
				}

				QString roffset = rem.value( "Offset" ).toString();
				QStringList rol = roffset.split( " " );
				if ( rol.size() == 2 )
				{
					sv.xOffset = rol[0].toInt();
					sv.yOffset = rol[1].toInt();
				}
				
				extractPixmap( s, sv, suffix );

				QImage img = s.pixmaps[suffix].toImage();
				QPixmap p2 = QPixmap::fromImage( img.mirrored( sv.flipHorizontal, sv.flipVertical ) );
				s.pixmaps.insert( suffix, p2 );
			}
			m_sprites.insert( s.id, s );
		}
	}
}

void Spriter::extractPixmap( Sprite& sprite, SpriteValues sv, QString rotation )
{
	QPixmap p = m_pixmap.copy( sv.x, sv.y, sv.dimX, sv.dimY );
	p.setMask( p.createMaskFromColor( QColor( 255, 0, 255 ), Qt::MaskInColor ) );
	//p.save( "content/" + name + ".png", "PNG");

	if( rotation != "" )
	{
		sprite.pixmaps.insert( rotation, p );
		sprite.values.insert( rotation, sv );
	}
	else
	{
		sprite.pixmaps.insert( "FL", p );
		sprite.pixmaps.insert( "FR", p );
		sprite.pixmaps.insert( "BL", p );
		sprite.pixmaps.insert( "BR", p );
		sprite.values.insert( "FL", sv );
		sprite.values.insert( "FR", sv );
		sprite.values.insert( "BL", sv );
		sprite.values.insert( "BR", sv );
	}


}

void Spriter::onTreeClicked( QTreeWidgetItem* twi, int column )
{
	QString text = twi->text( column );
	m_currentRot = "FL";
	if( text == "FL" || text == "FR" || text == "BL" || text == "BR" )
	{
		m_currentRot = text;
		text = twi->parent()->text( column );
	}
	qDebug() << text << m_currentRot;

	m_currentSprite = m_sprites[text];
	QPixmap pm = m_currentSprite.pixmaps[m_currentRot];
	QImage img = pm.toImage();
	ui.labelPreview->setPixmap( pm );
	ui.labelPreview->setMaximumSize( m_currentSprite.values[m_currentRot].dimX, m_currentSprite.values[m_currentRot].dimY );
	ui.labelPreview->setMinimumSize( m_currentSprite.values[m_currentRot].dimX, m_currentSprite.values[m_currentRot].dimY );
		
	ui.labelPreview2->setMaximumSize( m_currentSprite.values[m_currentRot].dimX * 2, m_currentSprite.values[m_currentRot].dimY * 2 );
	ui.labelPreview2->setMinimumSize( m_currentSprite.values[m_currentRot].dimX * 2, m_currentSprite.values[m_currentRot].dimY * 2 );
	ui.labelPreview2->setPixmap( pm.scaled( m_currentSprite.values[m_currentRot].dimX * 2, m_currentSprite.values[m_currentRot].dimY * 2 )  );

	ui.lineEditID->setText( text );
	ui.lineEditRot->setText( m_currentRot );

	
				
	bool flipX = m_currentSprite.values[m_currentRot].flipHorizontal;
	bool flipY = m_currentSprite.values[m_currentRot].flipVertical;
	ui.checkBoxFlipX->setChecked( flipX );
	ui.checkBoxFlipY->setChecked( flipY );

	ui.lineEditPosX->setText( QString::number( m_currentSprite.values[m_currentRot].x ) );
	ui.lineEditPosY->setText( QString::number( m_currentSprite.values[m_currentRot].y ) );
	ui.lineEditSizeX->setText( QString::number( m_currentSprite.values[m_currentRot].dimX ) );
	ui.lineEditSizeY->setText( QString::number( m_currentSprite.values[m_currentRot].dimY ) );
	ui.lineEditOffsetX->setText( QString::number( m_currentSprite.values[m_currentRot].xOffset ) );
	ui.lineEditOffsetY->setText( QString::number( m_currentSprite.values[m_currentRot].yOffset ) );

	for( int i = 0; i < 32; ++i )
	{
		for ( int k = 0; k < 32; ++k )
		{
			ui.tableWidgetSprite->item( i, k )->setBackgroundColor( Qt::white );
		}
	}

	for ( int x = 0; x < img.size().width(); ++x )
	{
		for ( int y = 0; y < img.size().height(); ++y )
		{
			QColor col = img.pixelColor( x, y );
			ui.tableWidgetSprite->item( y, x )->setBackgroundColor( col );
		}
	}
}

void Spriter::paint5x5( int levels )
{
	QPixmap pm( 240, 240 );
	pm.fill( QColor( 0, 0, 0, 0 ) );

	QPainter* painter = new QPainter( &pm );

	int px = 0;
	int py = 0;
	int x0 = 120;
	int y0 = 120;
	
	int zMax = ui.sliderLevel->value() + 1;
	int yMax = ui.sliderYMax->value() + 1;
	for ( int z = 0; z < zMax; ++z )
	{
		// paint floors
		for ( int y = 0; y < yMax; ++y )
		{
			for ( int x = 0; x < 5; ++x )
			{
				px = x0 + 16 * x - 16 * y;
				py = y0 + 8 * y + 8 * x;
					
				
				int fId = 50 * z + 10 * y + x + 5;
				QString fIdS = m_ids5x5[fId];

				if( !fIdS.isEmpty() )
				{
					Sprite floorSprite = m_sprites[fIdS];
					painter->drawPixmap( px + floorSprite.values[m_currentRot].xOffset, py + floorSprite.values[m_currentRot].yOffset, floorSprite.pixmaps["FL"] );
				}

			}
		}
		y0 -= 16;

		// paint walls
		for ( int y = 0; y < yMax; ++y )
		{
			for ( int x = 0; x < 5; ++x )
			{
				px = x0 + 16 * x - 16 * y;
				py = y0 + 8 * y + 8 * x;

				int wId = 50 * z + 10 * y + x;
				QString wIdS = m_ids5x5[wId];
				if( !wIdS.isEmpty() )
				{
					QString orient = "FL";
					if ( wIdS.endsWith( "FR" ) ) { orient = "FR"; wIdS = wIdS.left( wIdS.size() - 2 ); }
					if ( wIdS.endsWith( "BR" ) ) { orient = "BR"; wIdS = wIdS.left( wIdS.size() - 2 ); }
					if ( wIdS.endsWith( "BL" ) ) { orient = "BL"; wIdS = wIdS.left( wIdS.size() - 2 ); }
					Sprite wallSprite = m_sprites[wIdS];
					painter->drawPixmap( px + wallSprite.values[orient].xOffset, py + wallSprite.values[orient].yOffset, wallSprite.pixmaps[orient] );
				}
			}
		}
		y0 -= 4;
	}
	
	ui.label5x5->setPixmap( pm.scaled( 480, 480 ) );
	
	delete painter;
}

void Spriter::onButtonCommit()
{
	QPixmap pm = m_currentSprite.pixmaps[m_currentRot];
	QImage img = pm.toImage();
	for ( int x = 0; x < img.size().width(); ++x )
	{
		for ( int y = 0; y < img.size().height(); ++y )
		{
			QColor col = ui.tableWidgetSprite->item( y, x )->backgroundColor();
			img.setPixelColor( x, y, col );
		}
	}
	pm = QPixmap::fromImage( img );
	m_currentSprite.pixmaps[m_currentRot] = pm;
	m_sprites.insert( m_currentSprite.id, m_currentSprite );

	ui.labelPreview->setPixmap( pm );
	ui.labelPreview->setMaximumSize( m_currentSprite.values[m_currentRot].dimX, m_currentSprite.values[m_currentRot].dimY );
	ui.labelPreview->setMinimumSize( m_currentSprite.values[m_currentRot].dimX, m_currentSprite.values[m_currentRot].dimY );

	ui.labelPreview2->setMaximumSize( m_currentSprite.values[m_currentRot].dimX * 2, m_currentSprite.values[m_currentRot].dimY * 2 );
	ui.labelPreview2->setMinimumSize( m_currentSprite.values[m_currentRot].dimX * 2, m_currentSprite.values[m_currentRot].dimY * 2 );
	ui.labelPreview2->setPixmap( pm.scaled( m_currentSprite.values[m_currentRot].dimX * 2, m_currentSprite.values[m_currentRot].dimY * 2 )  );

	paint5x5( 0 );
}

void Spriter::onButtonCancel()
{
	QPixmap pm = m_currentSprite.pixmaps[m_currentRot];
	QImage img = pm.toImage();
	ui.labelPreview->setPixmap( pm );
	ui.labelPreview->setMaximumSize( m_currentSprite.values[m_currentRot].dimX, m_currentSprite.values[m_currentRot].dimY );
	ui.labelPreview->setMinimumSize( m_currentSprite.values[m_currentRot].dimX, m_currentSprite.values[m_currentRot].dimY );

	ui.labelPreview2->setMaximumSize( m_currentSprite.values[m_currentRot].dimX * 2, m_currentSprite.values[m_currentRot].dimY * 2 );
	ui.labelPreview2->setMinimumSize( m_currentSprite.values[m_currentRot].dimX * 2, m_currentSprite.values[m_currentRot].dimY * 2 );
	ui.labelPreview2->setPixmap( pm.scaled( m_currentSprite.values[m_currentRot].dimX * 2, m_currentSprite.values[m_currentRot].dimY * 2 )  );

	for( int i = 0; i < 32; ++i )
	{
		for ( int k = 0; k < 32; ++k )
		{
			ui.tableWidgetSprite->item( i, k )->setBackgroundColor( Qt::white );
		}
	}

	for ( int x = 0; x < img.size().width(); ++x )
	{
		for ( int y = 0; y < img.size().height(); ++y )
		{
			QColor col = img.pixelColor( x, y );
			ui.tableWidgetSprite->item( y, x )->setBackgroundColor( col );
		}
	}
}