//
// Created by alejandro on 22/03/20.
//

#ifndef TEST_RASTER_COST_DIST_H
#define TEST_RASTER_COST_DIST_H

using namespace std;

class Raster_cost_dist {
public:

    typedef struct position{

        int x;
        int y;
        float val_fricc;

        }
        position;


    float** cost_distance(int **inicio,float **fricc,int m, int n,position array[]);
    int movimientos(int pos_x,int pos_y,float costo_acumulado,float **fricc,int m, int n,position array[]);
    int movimientos_init(int pos_x,int pos_y,float costo_acumulado,float **fricc,int m, int n,position array[]);
    float** raster_cost(position array[],float **map_cost,int count);
    void print_matrix(float **mapa,int m, int n);
private:
};


#endif //TEST_RASTER_COST_DIST_H
