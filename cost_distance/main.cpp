#include <iostream>
#include "raster.h"
#include <stdio.h>
#include <stdlib.h>
#include "raster_cost_dist.h"
#include <string>
#include <map>

typedef struct prub{

    int x;
    int y;

}
        prub;

using namespace std;

int M,N,M_i,N_i;   //m es la fila y n la coumna
float** fric_map;
float** map_cost_dist;
float** inicio;

int origen[3][2]={{1,1},{1,1},{1,1}};
float prueba[6][6]={{1,3,4,4,3,2},
                    {4,6,2,3,7,6},
                    {5,8,7,5,6,6},
                    {1,4,5,-9,5,1},
                    {4,7,5,-9,2,6},
                    {1,2,2,1,3,4}};

int main() {
    struct prub array_1;
    Raster objrast;
    Raster_cost_dist objcost;
    int scale;
    fric_map = objrast.read_tif_matrix("/home/alejandro/CLionProjects/test/Haiti_fricc_Clip.tif", M, N, scale);
    //fric_map = objrast.read_tif_matrix("/home/alejandro/CLionProjects/test/cmake-build-debug/cost_distance_output_1.tiff", M, N);
    //objrast.print_raster(fric_map,M,N);
    printf("\n scala = %d \n",scale);
    objcost.print_matrix(fric_map,M,N);

    inicio = objrast.read_tif_matrix("/home/alejandro/CLionProjects/test/haiti_cost_dinamica_clip_30.tif", M_i, N_i,scale);
    printf("\n scala = %d \n",scale);
    objcost.print_matrix(inicio,M_i,N_i);

    printf("\n x %d", N);
    printf("\n y  %d", M);

    map<int, prub > costos;
    srand(time(NULL));
    for (int i = 100; i >= 0; i--) {
        array_1.x=i;
        array_1.y=rand()%10;
        costos[rand()%20+7]=array_1;
        //costos.insert(pair<int,>((rand() % 20) + 7,i));
    }

    for (int i = 7; i < 20; i++) {
        //printf("\n %d   %d", costos[i].x,costos[i].y);
    }

    std::map<int,prub>::iterator it = costos.begin();
    int a = it->second.x;
    printf("\n%d\n",a);
   // std::cout << it->first << " => " << it->second << '\n';
    //map<int,int>::iterator p = costos.begin();
    //printf("\n%d\n yaaa",costos.begin());

    float**pruebas = new float*[6];
    int**orig = new int*[3];
    for(int i=0;i<6;i++)
        pruebas[i] = new float[6];
    for(int i=0;i<3;i++)
        orig[i] = new int[2];

    for(int i=0;i<6;i++){
        for(int j=0;j<6;j++){
            pruebas[i][j]=prueba[i][j];
        }
    }
    for(int i=0;i<3;i++){
        for(int j=0;j<2;j++){
            orig[i][j]=origen[i][j];
        }
    }
    Raster_cost_dist::position datos[100];

    map_cost_dist = objcost.cost_distance(orig ,pruebas ,6,6,datos);
    objcost.print_matrix(map_cost_dist,6,6);

    //objcost.print_matrix(fric_map,M,N);
    map_cost_dist = objcost.cost_distance(orig ,fric_map ,M,N,datos);
    objcost.print_matrix(map_cost_dist,M,N);

    objrast.matrix_to_tiff(map_cost_dist,M,N,8);

    //generando un inicio
    float **prubas_origen = new float*[M];
    for(int i=0;i<M;i++)
        prubas_origen[i] = new float[N];

    prubas_origen[1][1] = 250;
    objrast.matrix_to_tiff(fric_map,M,N,7);
}

