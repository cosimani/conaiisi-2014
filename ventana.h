#ifndef VENTANA_H
#define VENTANA_H

#include <QWidget>

class Escena;
class HiloCaptura;

namespace Ui {
    class Ventana;
}

class Ventana : public QWidget
{
    Q_OBJECT

public:
    explicit Ventana(QWidget *parent = 0);
    ~Ventana();

    HiloCaptura* hiloCaptura;
    Escena * escena;

    Ui::Ventana *ui;

protected:
    void closeEvent(QCloseEvent *);
    void keyPressEvent(QKeyEvent *);

private slots:
    void slot_encenderCamara();
    void slot_apagarCamara();
};

#endif // VENTANA_H
