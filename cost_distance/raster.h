//
// Created by root on 05/03/20.
//


#ifndef UNTITLED_RASTER_H
#define UNTITLED_RASTER_H

#include <string>
using namespace std;

#include "gdal.h"
#include "gdal_priv.h"
#include "gdalwarper.h"
#include "ogr_spatialref.h"
#include "ogr_geometry.h"

class Raster {
public:
    float** read_tif_matrix(string name,int &m, int &n, int &scale);
    void print_raster(float **mtt,int m, int n);
    void matrix_to_tiff(float** output_raster, int rows, int cols, int count);
private:

};

#endif //UNTITLED_RASTER_H
