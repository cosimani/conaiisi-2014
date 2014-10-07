#include "ventana.h"
#include "ui_ventana.h"
#include <QMessageBox>
#include <QDebug>
#include <QKeyEvent>

#include "hiloCaptura.h"
#include "escena.h"



Ventana::Ventana(QWidget *parent) : QWidget(parent), ui(new Ui::Ventana)  {
    ui->setupUi(this);

    escena = new Escena(this);
    hiloCaptura = new HiloCaptura(this);

    ui->layoutEscena->addWidget(escena, 0, 0, 1, 1, Qt::AlignLeft | Qt::AlignTop);
    escena->setFocus();  // Para que el Escape dentro de Escena sea capturada y poder cerrar la aplicacion

    connect(ui->pbCambiarCamara, SIGNAL(clicked()), hiloCaptura, SLOT(slot_cambiarCamara()));

    this->slot_encenderCamara();
}

Ventana::~Ventana()  {
    delete ui;
    delete hiloCaptura;
}

void Ventana::slot_encenderCamara()  {
    escena->iniciarTemporizador();
    hiloCaptura->start();
    escena->setFocus();
}

void Ventana::slot_apagarCamara()  {
    escena->detenerTemporizador();
    hiloCaptura->finalizarHilo();
    escena->setFocus();
}


/**
  * Metodo que se ejecuta antes de cerrar la ventana (por ejemplo, cuando pulsamos X)
  */
void Ventana::closeEvent(QCloseEvent *)  {
    this->slot_apagarCamara();
    hiloCaptura->wait();  // Esperamos que el hilo finalice nates de cerrar la aplicacion.
    qDebug() << "closeEvent";
}

void Ventana::keyPressEvent(QKeyEvent *keyEvent)  {
    int key = keyEvent->key();

    switch(key)  {
    case Qt::Key_Escape:
        this->slot_apagarCamara();
        hiloCaptura->wait();  // Esperamos que el hilo finalice nates de cerrar la aplicacion.
        this->close();
        break;
    default: ;
    }
}
