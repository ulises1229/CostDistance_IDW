//
// Created by alejandro on 08/03/20.
//
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <array>
#include <memory>
#include <stdexcept>
#include <cstdio>
#include <algorithm>
#include <map>
#include <set>
#include "raster.h"
#include "gdal.h"
#include "gdal_priv.h"
#include "gdalwarper.h"
#include "ogr_spatialref.h"
#include "ogr_geometry.h"

using namespace std;

int ROWS,COLS;
float** fricc;
double adfGeoTransform[6];

float** Raster::read_tif_matrix(std::string file,int &m, int &n, int &scale)
{
    GDALDataset *dataset;
    char **MD;
    char *info;
    GDALAllRegister();
    string ds = file;
    dataset = (GDALDataset *) GDALOpen(ds.c_str(), GA_ReadOnly);
    GDALRasterBand  *poBand;

    poBand = dataset->GetRasterBand(1);

    int nXSize = poBand->GetXSize();
    int nYSize = poBand->GetYSize();

    m = nYSize;
    n = nXSize;

    dataset->GetGeoTransform( adfGeoTransform );
    scale = adfGeoTransform[1];

    ROWS = nYSize; COLS = nXSize;
    fricc = new float*[ROWS];
    for(int i = 0; i< ROWS; ++i) {
        //costosAux[i] = new int[COLS];
        fricc[i] = new float[COLS];
    }

    //GDALDataType dataType = poBand->GetRasterDataType();
    float *pBuf = new float[nYSize * nXSize];

    poBand->RasterIO(GF_Read, 0, 0, nXSize, nYSize, pBuf, nXSize, nYSize, GDT_Float32, 0, 0);


    int location;
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++)
        {
            location = (nXSize * (i)) + j;
            fricc[i][j] = *(pBuf+location);
        }
    }

    return fricc;
}

void Raster::print_raster(float **mtt, int m, int n) {
    for(int i=0; i< m; i++){
        for(int j =0; j < n; j++){
            printf(" %f ",mtt[i][j]);
        }
        printf("\n");
    }
}

void Raster::matrix_to_tiff(float **output_raster, int rows, int cols, int count) {
    GDALDataset *poDstDS;

    GDALDriver *poDriver;
    OGRSpatialReference oSRS;
    string fileName = "cost_distance_output_" + to_string(count) + ".tiff";

    poDriver = GetGDALDriverManager()->GetDriverByName("Gtiff");
    poDstDS = poDriver->Create( fileName.c_str(), cols, rows, 1, GDT_Float32, NULL);

    GDALRasterBand *poBand;
    float *pBuf = new float[rows * cols], maxVal = 0;
    for(int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            pBuf[i * cols + j] = output_raster[i][j];
            if(output_raster[i][j] < 0)
                pBuf[i * cols + j] = -9999;
            if(output_raster[i][j] > maxVal)
                maxVal = output_raster[i][j];
        }
    }

    poBand = poDstDS->GetRasterBand(1);
    poDstDS->GetRasterBand(1)->SetNoDataValue(-9999);
    poBand->RasterIO( GF_Write, 0, 0, cols, rows,
                      pBuf, cols, rows, GDT_Float32, 0, 0 );
    GDALClose( (GDALDatasetH) poDstDS );
    cout << fixed << "Max Val: " << maxVal << endl;
}
