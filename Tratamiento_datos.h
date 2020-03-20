#include "common.h"
#ifndef VECTORIZED_MATRIX_VECTORIZED_H
#define VECTORIZED_MATRIX_VECTORIZED_H
#endif 

class Tratamiento_datos{
public:
    typedef struct cell{
        short int x;
        short int y;
    }
        cell;
    Tratamiento_datos(short int rows,short int cols);
    ~Tratamiento_datos();

	void reset_Matrix(float **mat);
    void print_Matrix(float **mat);
    void IDW(float necesidad, float **fricc, float **biomass, float **final,short int x_init, short int y_init );
    void movimientos(short int x, short int y, float **biomass, float **final, map <float, cell> biomass_disponible );
private:
    short int ROWS,COLS;
};