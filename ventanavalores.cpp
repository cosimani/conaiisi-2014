#include "ventanavalores.h"
#include "ui_ventanavalores.h"

VentanaValores::VentanaValores(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VentanaValores)
{
    ui->setupUi(this);
}

VentanaValores::~VentanaValores()
{
    delete ui;
}
