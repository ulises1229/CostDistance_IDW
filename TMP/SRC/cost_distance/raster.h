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
#include <map>

class Raster {
public:
    typedef struct local{

        int x;
        int y;
        int num_local;

    }
            local;

    float** read_tif_matrix(string name,int &m, int &n, int &scale,int &cell_null);
    void print_raster(float **mtt,int m, int n);
    void matrix_to_tiff(float** output_raster, int rows, int cols, int count);
    void leer_localidades(float **map_local,int m, int n, map<int,local> &local_ord,int cell_null,int num_local);
    int contar_comunidades(float **mapa_local, int m, int n,int cell_null);
    int no_row(string name);
    void carga_requisitos(string name,map <int, float> &req_map);
private:

};

#endif //UNTITLED_RASTER_H
