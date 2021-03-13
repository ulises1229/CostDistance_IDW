#include "common.h"
#include "IDW.h"
#include "raster.h"
#include "cost_dist.h"
int main() {
    clock_t start_global = clock();//inicia tiempo global
    std::string str;
//------------------------------------------------------------------------variables
    int M,N,M_i,N_i;//M: fila y N: columna
    int i;
    int cell_null;//valor nulo comunidad
    int cell_null_2;//valor nulo friccion
    int scale;//escala del mapa
    int num_com=0;//numero de localidades en el mapa
    float exp=1.005;//exponente IDW
    int cont=0;//numero de localidades a explorar en el ciclo while
// ------------------------------------------------------------------------matrices
    float** fric_map; //mapa friccion
    float** map_cost_dist;//mapa costo distancia
    float** map_local;//mapa con localidades (ubicacion)
// ------------------------------------------------------------------------mapas
    map<int, Raster::local> localidades;
    map<int, float> bio_local;
    Raster::tiempo time_local;
    map<int, Raster::tiempo> time_ejecucion;
    map <double,l_dist> costos;//mapa costo distancia con los posibles movimientos
//------------------------------------------------------------------------objetos
    Raster objrast;
    IDW objIDW;
    Cost_dist objcost;
//-------------------------------------------------------------------------------------------------------carga de archivos
    fric_map = objrast.read_tif_matrix("/home/sperez/servidor_pruebas/modelos/Kenia/fricc_w.tif", M, N, scale,cell_null_2);
    printf("--------No. columnas: %d\n", M);
    printf("--------No. renglones: %d\n", N);
    // mapa localidades
    map_local = objrast.read_tif_matrix("/home/sperez/servidor_pruebas/modelos/Kenia/locs_5_int.tif", M_i, N_i,scale,cell_null);
    // guardamos los requisitos de las comunidades en un mapa
    objrast.carga_requisitos("/home/sperez/servidor_pruebas/modelos/Kenia/fwuse_5.csv",bio_local);
    //obtenemos el numero de comunidades
    num_com = objrast.contar_comunidades(map_local,M_i,N_i,cell_null);
    // guardamos las localidades en un mapa para ordenarlas
    objrast.leer_localidades(map_local,M_i,N_i,localidades,cell_null,num_com);
    cout << cell_null  << endl;
//-------------------------------------------------------------------------------------------------------valores iniciales
    Cost_dist::position datos[100];
    exp=1.005;
    float **IDW = new float *[M]; //matriz IDW para 1 localidad
    for ( i = 0; i < M; i++)
        IDW[i] = new float[N];
    objIDW.reset_Matrix(IDW, M, N, -1); //llena la matriz inicial del valor indicado
    std::map<int,float>::iterator bio; //iterados mapa requisitos localidad
    std::map<int,float>::iterator init;
    std::map<int,float>::iterator fin;
    std::map<int, Raster::local>::iterator lugar; //iterados mapa ubicacion localidades
    //-------------------------------------------------------------------------------------------------------inicia calculo modelos
    init= bio_local.begin();
    cout << "llave inicial " << init->first<<endl;
    fin = bio_local.end();
    cout << "llave final " << fin->first<<endl;
    int start=int(init->first);
    int end= int(fin->first);
    omp_set_num_threads(1);
#pragma omp parallel for private(i,bio,lugar,map_cost_dist,costos)
  //----------------------------------------------------------------------------
  //comparar mapas CD de la forma serial y paralelo
  //---------------------------------------------------------------------------
    for(i=start;i<=end;i++){
        //printf("main_%d\n",i);
        if(bio_local.find(i)!=bio_local.end()) {//que no encontró la llave
            bio = bio_local.find(i);
            if (bio->second != 0){
                if(localidades.find(i)!=localidades.end()) { //busca localidad para la que se tengan requisitos
                    lugar = localidades.find(bio->first);//buscar ubicacion de la localidad
                    //-------------------------------------------------------------------------------------------------------inicia costo ditancia
                    clock_t start_cost_dist = clock();//inicia tiempo cost_dist
                   //#pragma omp critical
                    costos.clear();
                    map_cost_dist = objcost.cost_distance(lugar->second.x, lugar->second.y, fric_map, M, N,datos,costos);
                    //printf("CD:fin\n");
                    clock_t end_cost_dist = clock();//termina tiempo
                    time_local.CD =(float) (end_cost_dist - start_cost_dist) /(float) CLOCKS_PER_SEC;//calcula tiempo de ejecucion

                    objrast.matrix_to_tiff(map_cost_dist, M, N,i,"serial_MP_mapa_cost_dist_");
                    //-------------------------------------------------------------------------------------------------------inicia IDW
                    clock_t start_IDW = clock();//inicia tiempo cost_dist
                    objIDW.IDW_test(bio->second,map_cost_dist,IDW,M,N,exp,cell_null);
                    clock_t end_IDW = clock();//termina tiempo
                    time_local.IDW =(float) (end_IDW - start_IDW) / (float) CLOCKS_PER_SEC;//calcula tiempo de ejecucion
                    //se guarda en el mapa la localidad que se exploró como llave y la estructura con los tiempos CD e IDW
                    time_ejecucion.insert(pair<int, Raster::tiempo>(bio->first, time_local));
                    cont++;//se suman las localidades calculadas
                    //cout << "exploracion: " << bio->first << " completa" << endl;
                }
            } else {
                cout << "no entró comunidad " << bio->first << endl;
            }
        }
        //bio_local.erase(bio);//borrar comunidad ya explorada
        //bio= bio_local.begin();
    }
    objrast.matrix_to_tiff(IDW, M, N,cont,"serial_MP_mapa_IDW_");//crea tiff de IDW de todas las localidades calculadas
    objrast.guardar_tiempos(time_ejecucion);
    clock_t end_global = clock();//termina tiempo
    double duration = (double)(end_global - start_global) / (double) CLOCKS_PER_SEC;//calcula tiempo de ejecucion
    printf("tiempo global: %lf segundos \n", duration);
    cout << "Listo\n" << endl;
    return 0;
}

