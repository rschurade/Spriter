#include "Spriter.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QCoreApplication::setOrganizationDomain( "ingnomia.de" );
	QCoreApplication::setApplicationName( "IngnomiaSpriter" );
	QCoreApplication::setApplicationVersion( "0.1.0" );


	Spriter w;
	w.show();
	return a.exec();
}
