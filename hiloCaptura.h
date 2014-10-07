#ifndef HILOCAPTURA_H
#define HILOCAPTURA_H

#include <QThread>
#include <QImage>
#include <QStringList>

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/photo/photo.hpp>

#include <vector>

#include "ventana.h"

#include "configuracion.h"

#include "faceDetector.h"
#include "cara.h"

#include "ventanavalores.h"

#define CORRECCION_BOCA_ABIERTA 1.2
#define CORRECCION_LINEA_SUPERIOR_BOCA 1.1
#define COMP_Lab 1


using namespace std;
using namespace cv;

class Ventana;

class HiloCaptura : public QThread  {
    Q_OBJECT

public:
    HiloCaptura(QObject * parent = 0);
    virtual ~HiloCaptura();
    void finalizarHilo();

private:
    Ventana * ventana;
    bool activo;
    int camaraActiva;

    VideoCapture * cap;  // Conexion con la Webcam

    Mat frame;  // Con este objeto se trabajara para cada frame de la camara, tanto para kinect como webcam

    Mat croppedImage;

    FaceDetector detectorCara;
    vector<Cara> faces;

    VentanaValores ventanaValores;

    float umbralCannyMin, umbralCannyMax, tamanoApertura;
    bool L2Gradiente;

    float angulo(Point p1, Point p2);
    Point inferiorDe(Point ojo, float ang, bool caso);
    Point puntoNuevo(Point ojo, float ang, bool caso, int dist);

protected:
    void run();

private slots:

public slots:
    void slot_cambiarCamara();

signals:
    void signal_distanciaActualizada(int);

};

#endif // HILOCAPTURA_H
