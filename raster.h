//
// Created by otavio on 05/03/20.
//
#include "common.h"

#ifndef UNTITLED_RASTER_H
#define UNTITLED_RASTER_H


class Raster {
public:
    void read_tif_matrix(string file, float** costos, short int ROWS, short int COLS);
private:
    short int ROWS,COLS;
};

#endif //UNTITLED_RASTER_H