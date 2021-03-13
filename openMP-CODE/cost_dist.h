#include "common.h"

typedef struct l_dist{
    int x;
    int y;
    float costo;
} l_dist;

class Cost_dist{
public:

    typedef struct position{
        int x;
        int y;
        float val_fricc;
    } position;

    float** cost_distance(int inicio_x,int inicio_y,float **fricc,int m, int n, position array[], map<double,l_dist> &costos, float **&map_cost);
    int movimientos(int pos_x,int pos_y,float costo_acumulado,float **fricc,int m, int n,position array[]);
    int movimientos_init(int pos_x,int pos_y,float costo_acumulado,float **fricc,int m, int n,position array[]);
    void raster_cost(position *array,float **map_cost,int count, map<double,l_dist> &costos,int &start);

private:
};


