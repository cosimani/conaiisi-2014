#ifndef VENTANAVALORES_H
#define VENTANAVALORES_H

#include <QWidget>

namespace Ui {
class VentanaValores;
}

class VentanaValores : public QWidget
{
    Q_OBJECT

public:
    explicit VentanaValores(QWidget *parent = 0);
    ~VentanaValores();
    Ui::VentanaValores *ui;

private:

};

#endif // VENTANAVALORES_H
