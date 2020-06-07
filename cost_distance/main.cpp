#include <iostream>
#include "raster.h"
#include <stdio.h>
#include <stdlib.h>
#include "raster_cost_dist.h"
#include <string>
#include <map>
#include <time.h>
#include <ctime>
#include <math.h>
#include <vector>

typedef struct local{

    int x;
    int y;

}
        local;

using namespace std;

int M,N,M_i,N_i,cell_null,total_local;   //m es la fila y n la coumna
float** fric_map;
float** map_cost_dist;
float** inicio;
float** map_local;


int main() {
    Raster objrast;
    Raster_cost_dist objcost;
    int scale;
    // accedemos al mapa de fricción
    fric_map = objrast.read_tif_matrix("/home/alejandro/CLionProjects/cost_distance/fricc_w(1).tif", M, N, scale,cell_null);

    // accedemos a las localidades
    map_local = objrast.read_tif_matrix("/home/alejandro/CLionProjects/cost_distance/Locs_c.tif", M_i, N_i,scale,cell_null);
    //objcost.print_matrix(inicio,M_i,N_i);

    //obtenemos el numero de comunidades
    int num_com = objrast.contar_comunidades(map_local,M_i,N_i,cell_null);

    // guardamos las localidades en un mapa para ordenarlas
    map<int, Raster::local> localidades;
    objrast.leer_localidades(map_local,M_i,N_i,localidades,cell_null,num_com);

    // guardamos los requisitos de las comunidades en un mapa
    map<int, float> bio_local;
    objrast.carga_requisitos("/home/alejandro/CLionProjects/cost_distance/fwuse_W01.csv",bio_local);

    Raster_cost_dist::position datos[100];

    clock_t start = clock();
    //objcost.print_matrix(fric_map,M,N);
    int i = 0;
    // leemos 10 comunidades que tienen requerimientos de biomasa diferentes a cero
    while(i<10){
        std::map<int,float>::iterator bio = bio_local.begin();
        if(bio->second!=0) {
            i++;
            clock_t start = clock();
            std::map<int, Raster::local>::iterator lugar = localidades.find(bio->first);
            printf("\nx: %d  y: %d   localidad: %d \n", lugar->second.x, lugar->second.y, lugar->first);
            map_cost_dist = objcost.cost_distance(lugar->second.x, lugar->second.y, fric_map, M, N, datos);
            objrast.matrix_to_tiff(map_cost_dist, M, N, lugar->second.num_local);
            clock_t end = clock();
            double duration = (double) (end - start) / (double) CLOCKS_PER_SEC;
            printf("tiempo_cost_distance:\n%lf seconds \n%lf milliseconds\n", duration, duration * 1000);

        }
        bio_local.erase(bio);
    }
    //objcost.print_matrix(map_cost_dist,M,N);
    clock_t end = clock();
    double duration = (double)(end - start) / (double) CLOCKS_PER_SEC;
    printf("tiempo_cost_distance_total:\n%lf seconds \n%lf milliseconds\n", duration, duration*1000);

}

