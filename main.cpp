#include <QApplication>

#include "ventana.h"
#include "configuracion.h"

#include <QDebug>
#include <QDesktopWidget>

int main( int argc, char** argv )  {
    QApplication a(argc, argv);

    Configuracion::obtenerConfiguracion();

    Ventana ventana;
    ventana.show();
    ventana.move(500, 0);

    int quit = a.exec();

    return quit;
}
