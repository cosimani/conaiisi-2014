#ifndef CARA_H
#define CARA_H

#include <opencv/cv.h>

using namespace cv;

class Cara  {
public:
    Cara()  {  }

    Point center;
    int diametro;
    int width;
    int height;
};

#endif // CARA_H
