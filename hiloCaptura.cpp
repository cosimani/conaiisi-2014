#include "hiloCaptura.h"
#include "util.h"
#include "escena.h"
#include <QDebug>

#include "ui_ventanavalores.h"


using namespace cv;

HiloCaptura::HiloCaptura(QObject * parent) : activo(false), camaraActiva(0)
{
    this->ventana = (Ventana *)parent;

    if (Configuracion::tipoCamara.contains("webcam1", Qt::CaseInsensitive))  {
        cap = new VideoCapture(0);qDebug() << "webcam1";
        cap->set(CV_CAP_PROP_FRAME_WIDTH, RESOLUTION_W);
        cap->set(CV_CAP_PROP_FRAME_HEIGHT, RESOLUTION_H);
    }
    else if (Configuracion::tipoCamara.contains("webcam2", Qt::CaseInsensitive))  {
        cap = new VideoCapture(1);qDebug() << "webcam2";
        cap->set(CV_CAP_PROP_FRAME_WIDTH, RESOLUTION_W);
        cap->set(CV_CAP_PROP_FRAME_HEIGHT, RESOLUTION_H);
    }
    else if (Configuracion::tipoCamara.contains("webcam3", Qt::CaseInsensitive))  {
        cap = new VideoCapture(2);
        cap->set(CV_CAP_PROP_FRAME_WIDTH, RESOLUTION_W);
        cap->set(CV_CAP_PROP_FRAME_HEIGHT, RESOLUTION_H);
    }

    this->frame.create(Size(RESOLUTION_W, RESOLUTION_H), CV_8UC3);

    ventanaValores.show();
    ventanaValores.move(0, 0);

}

HiloCaptura::~HiloCaptura()  {
    delete cap;
}


void HiloCaptura::slot_cambiarCamara()  {
    this->finalizarHilo();
    this->wait(500);  // Esperamos 500 milisegundos maximo para que finalice el hilo y podamos cambiar de camara

    if (camaraActiva == 2)
        camaraActiva = 0;
    else if (camaraActiva == 1)
        camaraActiva = 2;
    else if (camaraActiva == 0)
        camaraActiva = 1;

//    (this->camaraActiva == 0) ? this->camaraActiva = 2 : this->camaraActiva = 0;
    this->start();
}

void HiloCaptura::run()  {

    this->activo = true;

    if (Configuracion::tipoCamara.contains("kinect", Qt::CaseInsensitive))  {
//        if (!kinect->kinectDetected)  {
//            qDebug() << "La camara del Kinect no pudo ser iniciada!!";
//            return;
//        }
    }
    else  {  // Aqui para cualquier webcam
        if( !cap->isOpened() )  {
            qDebug() << "La camara con VideoCapture no pudo ser iniciada!!";
            return;
        }
    }

    while(activo)  {

//        QThread::msleep(20);

        if (Configuracion::tipoCamara.contains("kinect", Qt::CaseInsensitive))  {
//            kinect->getRGB(m_rgb);

//            Mat frame(Size(RESOLUTION_W, RESOLUTION_H), CV_8UC3, (void*)m_rgb.data());
//            this->frame = frame;
        }
        else  {  // Aqui para cualquier webcam
            cap->operator >>(frame);
        }

        vector<Rect> faces;
        faces = detectorCara.detectFacesRect(frame);

        if(faces.size() > 0)   {  // Encontramos una cara

            faces.at(0).height *= CORRECCION_BOCA_ABIERTA;  // Para bajar un poco mas la linea inferior para cuando se abre la boca

            if (ventanaValores.ui->cbLineas->isChecked())
                detectorCara.drawMultipleRect(faces, frame);  // Dibuja el Rectángulo de la cara
            Rect roiFace(faces.at(0).x, faces.at(0).y, faces.at(0).width, faces.at(0).height);
            Mat middleFace(frame, roiFace);

            this->frame = middleFace.clone(); // Trabajamos solo sobre la cara

            int anchoImagenROIFace = frame.cols;
            int altoImagenROIFace = frame.rows;

            // Rectangulos Grandes
            Rect rOjoIzquierdo(anchoImagenROIFace/10, altoImagenROIFace/CORRECCION_BOCA_ABIERTA/10,
                               anchoImagenROIFace*4/10, altoImagenROIFace/CORRECCION_BOCA_ABIERTA*5/10);            
            Rect rOjoDerecho(anchoImagenROIFace*5/10, altoImagenROIFace/CORRECCION_BOCA_ABIERTA/10,
                             anchoImagenROIFace*4/10, altoImagenROIFace/CORRECCION_BOCA_ABIERTA*5/10);
            Mat matOjoIzquierdo(frame, rOjoIzquierdo);
            Mat matOjoDerecho(frame, rOjoDerecho);

            if (ventanaValores.ui->cbLineas->isChecked())  {
                rectangle(frame, rOjoIzquierdo, Scalar( 0, 255, 0 ), 2, 8, 0);
                rectangle(frame, rOjoDerecho, Scalar( 0, 255, 0 ), 2, 8, 0);
            }

            // Rectangulos de Ojos
            vector<Rect> ojoIzquierdo = detectorCara.detectOjoIzquierdoRect(matOjoIzquierdo);
            if (ventanaValores.ui->cbLineas->isChecked())
                detectorCara.drawMultipleRect(ojoIzquierdo, matOjoIzquierdo);
            vector<Rect> ojoDerecho = detectorCara.detectOjoDerechoRect(matOjoDerecho);
            if (ventanaValores.ui->cbLineas->isChecked())
                detectorCara.drawMultipleRect(ojoDerecho, matOjoDerecho);

            // Calculamos lineas de borde
            Point centroOjoIzquierdo;
            if(ojoIzquierdo.size() > 0)  {
                centroOjoIzquierdo.x = anchoImagenROIFace/10 + ojoIzquierdo.at(0).x + ojoIzquierdo.at(0).width/4;
                centroOjoIzquierdo.y = altoImagenROIFace/10 + ojoIzquierdo.at(0).y + ojoIzquierdo.at(0).height/2;
            }



            Point centroOjoDerecho;
            if (ojoDerecho.size() > 0)  {
                centroOjoDerecho.x = anchoImagenROIFace*5/10 + ojoDerecho.at(0).x + ojoDerecho.at(0).width*3/4;
                centroOjoDerecho.y = altoImagenROIFace/10 + ojoDerecho.at(0).y +ojoDerecho.at(0).height/2;
            }

            float anguloEntreOjos;
            if (ojoIzquierdo.size() > 0 && ojoDerecho.size() > 0 )
                anguloEntreOjos = angulo(centroOjoIzquierdo, centroOjoDerecho);

            bool caso = (centroOjoIzquierdo.y < centroOjoDerecho.y);
            Point centroInferiorIzquierdo = inferiorDe(centroOjoIzquierdo, anguloEntreOjos, caso);
            Point centroInferiorDerecho = inferiorDe(centroOjoDerecho, anguloEntreOjos, caso);

            if (ventanaValores.ui->cbLineas->isChecked())
                if (ojoIzquierdo.size() > 0 && ojoDerecho.size() > 0)  {  // Dibuja las lineas verticales
                    line(frame, centroOjoIzquierdo, centroInferiorIzquierdo, Scalar(255, 255, 0), 2);
                    line(frame, centroOjoDerecho, centroInferiorDerecho, Scalar(255, 255, 0), 2);
                }


            // Borde de boca Superior
            Point bocaSupIzq = puntoNuevo(centroOjoIzquierdo, anguloEntreOjos, caso,
                                          altoImagenROIFace/CORRECCION_BOCA_ABIERTA/3);  // Por que es esa division por 3 ?
            bocaSupIzq.y /= CORRECCION_LINEA_SUPERIOR_BOCA;
            Point bocaSupDer = puntoNuevo(centroOjoDerecho, anguloEntreOjos, caso,
                                          altoImagenROIFace/CORRECCION_BOCA_ABIERTA/3);
            bocaSupDer.y /= CORRECCION_LINEA_SUPERIOR_BOCA;

            // Borde de boca Inferior
            Point bocaInfIzq = puntoNuevo(centroOjoIzquierdo, anguloEntreOjos, caso, altoImagenROIFace*4/7);  // Por que 4/7 ?
            Point bocaInfDer = puntoNuevo(centroOjoDerecho, anguloEntreOjos, caso, altoImagenROIFace*4/7);

            if (ventanaValores.ui->cbLineas->isChecked())
                if (ojoIzquierdo.size() > 0 && ojoDerecho.size() > 0)  {  // Dibuja los bordes superior e inferior de la boca
                    line(frame, bocaSupIzq, bocaSupDer, Scalar(255, 255, 0), 2);
                    line(frame, bocaInfIzq, bocaInfDer, Scalar(255, 255, 0), 2);
                }

            Point roiSupIzq;
            Point roiInfDer;

            if (bocaInfIzq.x < bocaSupIzq.x)  {
                roiSupIzq.x = bocaInfIzq.x;
                roiSupIzq.y = bocaSupIzq.y;
                roiInfDer.x = bocaSupDer.x;
                roiInfDer.y = bocaInfDer.y;
            }
            else  {
                roiSupIzq.x = bocaSupIzq.x;
                roiSupIzq.y = bocaSupDer.y;
                roiInfDer.x = bocaInfDer.x;
                roiInfDer.y = bocaInfIzq.y;
            }

            Rect roiBoca(roiSupIzq, roiInfDer);
            Mat boca(frame, roiBoca);

            if (ventanaValores.ui->cbRecortada->isChecked())
                this->frame = boca.clone();
        }

        if (ventanaValores.ui->cbActualizar->isChecked())  {
            umbralCannyMin = ventanaValores.ui->leUmbralMin->text().toFloat();
            umbralCannyMax = ventanaValores.ui->leUmbralMax->text().toFloat();
            tamanoApertura = ventanaValores.ui->leTamApertura->text().toFloat();
            L2Gradiente = ventanaValores.ui->cbL2Gradiente->isChecked();
        }

        if (ventanaValores.ui->cbLab->isChecked())  {
            cvtColor( frame, frame, CV_BGR2Lab );

            unsigned int cantidadComponentes = frame.cols*frame.rows*3;
            int sumaComponente_a = 0;

            QDebug deb = qDebug();

            for (unsigned int i=0 ; i<cantidadComponentes ; i+=3)  {
                sumaComponente_a += (frame.data[i+COMP_Lab] - 128);  // Sumamos el componente 'a' de cada pixel
//                deb << "i=" << i << frame.data[i+COMP_Lab] - 128 << " - ";
            }

            // Calculamos el promedio (segun paper que dice que es una forma de obtener el color de labio)
            int mediaComponente_a = sumaComponente_a / (frame.cols*frame.rows);

            // Un dato para el calculo de varianza
            int sumatoriaDatoMenosPromedioAlCuadrado = 0;

            for (unsigned int i=0 ; i<cantidadComponentes ; i+=3)  {
                sumatoriaDatoMenosPromedioAlCuadrado += ( (frame.data[i+COMP_Lab] - 128) - mediaComponente_a )^2;
            }

            double varianza = sumatoriaDatoMenosPromedioAlCuadrado / (frame.cols*frame.rows);

            qDebug() << "Promedio=" << mediaComponente_a << " Varianza=" << varianza;

            // El paper 'a real-time lip localization...' dice que el umbral para detectar labios es con esta formula
            float umbral = mediaComponente_a + varianza;

            // Creamos una imagen gris
            std::vector<uint8_t> labiosEnBinario;

            // Aqui vamos completando cada pixel gris con valores negros y blancos
            for (unsigned int i=0 ; i<cantidadComponentes ; i+=3)  {

                // Restamos 128 porque el componente "a" tiene valores negativos y el frame va de 0 a 255 solamente
                // Ver http://www.seas.upenn.edu/~bensapp/opencvdocs/ref/opencvref_cv.htm
                if ( (frame.data[i+COMP_Lab] - 128) > umbral )  {
                    labiosEnBinario.push_back(255);
                }
                else  {
                    labiosEnBinario.push_back(0);
                }
            }

            // Con ese vector pasamos a mat para luego llevar esa imagen en escala de grises a RGB
            Mat matLabiosEnBinario(Size(frame.cols, frame.rows), CV_8UC1, (void*)labiosEnBinario.data());
            cvtColor(matLabiosEnBinario, frame, CV_GRAY2BGR);

        }

        if (ventanaValores.ui->cbProcesar->isChecked())  {

            vector<vector<Point> > contours;
            vector<Vec4i> hierarchy;

            cvtColor( frame, frame, CV_BGR2GRAY );
            blur( frame, frame, Size(3,3) );

            Canny( frame, frame, umbralCannyMin, umbralCannyMax, tamanoApertura, L2Gradiente);
//            findContours( frame, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
            findContours( frame, contours, hierarchy, CV_RETR_TREE, CV_LINK_RUNS, Point(0, 0) );

            int min = ventanaValores.ui->leContornoMin->text().toInt();
            vector<vector<Point> >::iterator iterContour;
            double length;
            iterContour = contours.begin();

            while(iterContour != contours.end())  {

                length = arcLength((*iterContour), true);
                if (length < min)  {
                    iterContour = contours.erase(iterContour);
                }
                else
                    iterContour++;
            }

            Mat maskContornos = Mat::zeros( frame.size(), CV_8UC1 );
            for (unsigned int i=0 ; i<contours.size() ; i++)  {
                drawContours(maskContornos, contours, i, Scalar(255, 0, 0), 1, 8, hierarchy, 0, Point());
            }

            cvtColor(maskContornos, frame, CV_GRAY2RGB);
//            cvtColor(frame, frame, CV_GRAY2RGB);
        }

        ventana->escena->setImage(frame);

















//        vector<Rect> faces;
//        faces = detectorCara.detectFacesRect(frame);
////        detectorCara.drawMultipleRect(faces, frame);  // Metodo que dibujar el rectangulo de la cara

//        if(faces.size() > 0)  { // Si detectamos al menos una cara, recortamos la mitad de la primer cara encontrada
////             Rect roi(faces.at(0).x, faces.at(0).y + faces.at(0).height/2, faces.at(0).width, faces.at(0).height/2);
//             Rect roi(faces.at(0).x, faces.at(0).y, faces.at(0).width, faces.at(0).height);
//             Mat middleFace(frame, roi);
//             this->frame = middleFace.clone();

//             int anchoImagen = frame.cols;
////             qDebug() << "Ancho imagen: " << anchoImagen;
//             int altoImagen = frame.rows;
////             qDebug() << "Alto imagen: " << altoImagen;

//             Rect rOjoIzquierdo(anchoImagen/10, altoImagen*3/20, anchoImagen*2/5, altoImagen*2/5);
//             Mat matOjoIzquierdo(frame, rOjoIzquierdo);

//             if (ventanaValores.ui->cbLineas->isChecked())
//                rectangle(frame, rOjoIzquierdo, Scalar( 0, 255, 0 ), 2, 8, 0);
//             //frame = matOjoDerecho.clone();

//             vector<Rect> ojoIzquierdo;
//             ojoIzquierdo = detectorCara.detectOjoIzquierdoRect(matOjoIzquierdo);

//             if (ventanaValores.ui->cbLineas->isChecked())
//                detectorCara.drawMultipleRect(ojoIzquierdo, matOjoIzquierdo);  // Metodo que dibujar el rectangulo de la cara

//             Point centroOjoIzquierdo;
//             if(ojoIzquierdo.size() > 0)  { // Si detectamos al menos una cara, recortamos la mitad de la primer cara encontrada
//                 centroOjoIzquierdo.x = anchoImagen/10 + ojoIzquierdo.at(0).x + ojoIzquierdo.at(0).width/2;
//                 centroOjoIzquierdo.y = altoImagen*3/20 + ojoIzquierdo.at(0).y+ojoIzquierdo.at(0).height/2;
//                 if (ventanaValores.ui->cbLineas->isChecked())
//                    line(frame, centroOjoIzquierdo, Point(centroOjoIzquierdo.x, altoImagen-5), Scalar(255, 0, 0), 2);
//             }


//             Rect rOjoDerecho(anchoImagen-anchoImagen/2, altoImagen*3/20, anchoImagen*2/5, altoImagen*2/5);
//             Mat matOjoDerecho(frame, rOjoDerecho);
//             if (ventanaValores.ui->cbLineas->isChecked())
//                rectangle(frame, rOjoDerecho, Scalar( 0, 255, 0 ), 2, 8, 0);

//             vector<Rect> ojoDerecho;
//             ojoDerecho = detectorCara.detectOjoDerechoRect(matOjoDerecho);
//             if (ventanaValores.ui->cbLineas->isChecked())
//                detectorCara.drawMultipleRect(ojoDerecho, matOjoDerecho);  // Metodo que dibujar el rectangulo de la cara

//             Point centroOjoDerecho;
//             if (ojoDerecho.size() > 0)  {
//                 centroOjoDerecho.x = anchoImagen-anchoImagen/2 + ojoDerecho.at(0).x + ojoDerecho.at(0).width/2;
//                 centroOjoDerecho.y = altoImagen*3/20 + ojoDerecho.at(0).y+ojoDerecho.at(0).height/2;
//                 if (ventanaValores.ui->cbLineas->isChecked())
//                    line(frame, centroOjoDerecho, Point(centroOjoDerecho.x, altoImagen-5), Scalar(255, 0, 0), 2);
//             }

//             Point puntoIzquierdoTopBoca;
//             puntoIzquierdoTopBoca.x = centroOjoIzquierdo.x;
//             puntoIzquierdoTopBoca.y = (altoImagen - centroOjoIzquierdo.y)/2 + centroOjoIzquierdo.y;

//             Point puntoDerechoTopBoca;
//             puntoDerechoTopBoca.x = centroOjoDerecho.x;
//             puntoDerechoTopBoca.y = (altoImagen - centroOjoDerecho.y)/2 + centroOjoDerecho.y;

//             if (ventanaValores.ui->cbLineas->isChecked())
//                line(frame, puntoIzquierdoTopBoca, puntoDerechoTopBoca, Scalar(255, 0, 0), 2);

//             Point verticeTopIzqBoca = puntoIzquierdoTopBoca;
//             Point verticeTopDerBoca = puntoDerechoTopBoca;
//             Point verticeBottomIzqBoca;
//             Point verticeBottomDerBoca;

//             verticeBottomIzqBoca.x = puntoIzquierdoTopBoca.x;
//             verticeBottomIzqBoca.y = altoImagen;

//             verticeBottomDerBoca.x = puntoDerechoTopBoca.x;
//             verticeBottomDerBoca.y = altoImagen;

//             if (ventanaValores.ui->cbLineas->isChecked())  {
//                 line(frame, verticeTopIzqBoca, verticeTopDerBoca, Scalar(255, 0, 255), 2);
//                 line(frame, verticeTopDerBoca, verticeBottomDerBoca, Scalar(255, 0, 255), 2);
//                 line(frame, verticeBottomDerBoca, verticeBottomIzqBoca, Scalar(255, 0, 255), 2);
//                 line(frame, verticeBottomIzqBoca, verticeTopIzqBoca, Scalar(255, 0, 255), 2);
//             }

//             Rect roiBoca(verticeTopIzqBoca, verticeBottomDerBoca);
//             Mat boca(frame, roiBoca);
//             this->frame = boca.clone();
//        }

//        Mat canny_output;
//        vector<vector<Point> > contours;
//        vector<Vec4i> hierarchy;

//        if (ventanaValores.ui->cbActualizar->isChecked())  {
//            umbralCannyMin = ventanaValores.ui->leUmbralMin->text().toFloat();
//            umbralCannyMax = ventanaValores.ui->leUmbralMax->text().toFloat();
//            tamanoApertura = ventanaValores.ui->leTamApertura->text().toFloat();
//            L2Gradiente = ventanaValores.ui->cbL2Gradiente->isChecked();
//        }

//        if (ventanaValores.ui->cbProcesar->isChecked())  {
//            cvtColor( frame, frame, CV_BGR2GRAY );
//            blur( frame, frame, Size(3,3) );
////            medianBlur( frame, frame, 21 );

//            Canny( frame, frame, umbralCannyMin, umbralCannyMax, tamanoApertura, L2Gradiente);
//            findContours( frame, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

//            int min = 120;
//            vector<vector<Point> >::iterator iterContour;
//            double length;
//            iterContour = contours.begin();

//            while(iterContour != contours.end())  {
//                length = arcLength((*iterContour), false);
//                if (length < min)
//                    iterContour = contours.erase(iterContour);
//                else
//                    iterContour++;
//            }

//            Mat maskContornos = Mat::zeros( frame.size(), CV_8UC1 );
//            for (unsigned int i=0 ; i<contours.size() ; i++)  {
//                drawContours(maskContornos, contours, i, Scalar(255, 0, 0), 1, 8, hierarchy, 0, Point());
//            }

////        // maskContornos tiene un solo componente (no tiene R G y B). Por eso hacemos CV_GRAY2RGB para que
/// tenga 3 canales.
////        // Si sacamos esta funcion entonces la imagen real frame no sera modificada.
//            cvtColor(maskContornos, frame, CV_GRAY2BGR);
//            cvtColor(frame, frame, CV_BGR2Lab);
////            cvtColor(frame, frame, CV_GRAY2RGB);
//        }

//        ventana->escena->setImage(frame);
    }
}

float HiloCaptura::angulo(Point p1, Point p2)
{
    float adyacente = p2.x - p1.x;
    float opuesto;
    if (p2.y < p1.y) opuesto = p1.y - p2.y;
    else opuesto = p2.y - p1.y;
    float ang = atan(opuesto/adyacente);
//    qDebug() << "Angulo entre ojos en grados: " << ang*57.2957795 << "°";
    return ang;
}

Point HiloCaptura::inferiorDe(Point ojo, float ang, bool caso)
{
    int y = ojo.y + 400;
    int x;
    if(caso == 0) x = ojo.x + (tan(ang)*y);
    else x = ojo.x - (tan(ang)*y);
    return Point(x, y);
}

Point HiloCaptura::puntoNuevo(Point ojo, float ang, bool caso, int dist)
{
    int x;
    if (caso == 0) x =  ojo.x + sin(ang) * dist;
    else x =  ojo.x - sin(ang) * dist;
    int y = ojo.y + cos(ang) * dist;
    return Point(x, y);
}

void HiloCaptura::finalizarHilo()  {
    this->activo = false;
}

