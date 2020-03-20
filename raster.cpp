//
// Created by otavio on 05/03/20.
//
#include "raster.h"
#include "common.h"
void Raster::read_tif_matrix(string file,float** costos,  short int ROWS, short int COLS){
    GDALDataset *dataset;
    char **MD;
    char *info;
    GDALAllRegister();
    string ds = file;
    dataset = (GDALDataset *) GDALOpen(ds.c_str(), GA_ReadOnly);
    GDALRasterBand  *poBand;
    poBand   = dataset->GetRasterBand(1);
    int nXSize = poBand->GetXSize();
    int nYSize = poBand->GetYSize();
    printf("\n x %d",nXSize);
    printf("\n y  %d\n",nYSize);
    ROWS = nYSize;
    COLS = nXSize;
    costos = new float*[ROWS];
    for(int i = 0; i< ROWS; ++i) {
        //costosAux[i] = new int[COLS];
        costos[i] = new float[COLS];
    }
    //GDALDataType dataType = poBand->GetRasterDataType();
    float *pBuf = new float[nYSize * nXSize];
    poBand->RasterIO(GF_Read, 0, 0, nXSize, nYSize, pBuf, nXSize, nYSize, GDT_Float32, 0, 0);
    int location;
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++)
        {
            location = (nXSize * (i)) + j;
            costos[i][j] = *(pBuf+location);
        }
    }

    for(int i=0; i< 300; i++){
        for(int j =0; j < COLS; j++){
            printf(" %f ",costos[i][j]);
        }
        printf("\n");
    }
}
