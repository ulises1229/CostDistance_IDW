#include "Tratamiento_datos.h"
#include "common.h"
Tratamiento_datos::Tratamiento_datos(short int rows,short int cols){}
Tratamiento_datos::~Tratamiento_datos() {}
//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
void Tratamiento_datos::reset_Matrix(float **mat){
    for(int i=0; i<ROWS; i++)
        for(int j=0; j<COLS; j++)
            mat[i][j]=-1.0;
}
//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------ 
void Tratamiento_datos::print_Matrix(float **mat){
    printf("------------------Printing matrix...\n");
    for(int i=0; i<ROWS; i++){
        for(int j=0; j<COLS; j++)
            printf(" %f", mat[i][j]);
        printf("\n");
    }
}
//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
void Tratamiento_datos::IDW(float necesidad, float **fricc, float **biomass, float **final,short int x_init, short int y_init ){
	float recogido=0;//el valor de la biomassa recogida por el momento
    map <float, cell> biomass_disponible ;// se crea el mapa para guardar los lugares explorados en donde podríamos recoger la biomasa
	while(recogido<=necesidad){
		movimientos(x_init, y_init, biomass, final,biomass_disponible);//desde la ubicacion de la exploracion busca cuales son los posibles movimientos que puede hacer
        map<float, cell >::iterator p = --biomass_disponible.end(); //accedemos al movimiento posible que tenga la mayor biomasa
        final[p->second.x][p->second.y]= 1/pow( fricc[p->second.x][p->second.y]  - fricc[x_init][y_init] ,2);//guardamos en el mapa final el valor de friccion que corresponde
        recogido+= p->first; //sumamos la biomasa recogida
        x_init = p->second.x; //guardamos nueva coordenada x
        y_init= p->second.y; //guardamos nueva coordenada y
        biomass_disponible.erase (p);//eliminamos el elemento que ya se usó
	}
}
//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
void Tratamiento_datos::movimientos(short int x, short int y, float **biomass, float **final, map <float, cell> biomass_disponible ){
	short int x_tmp=0, y_tmp= 0;
    cell posicion;//se crea la estructura
	for(short int mov=1; mov<=8; mov++ ){//son 8 los posibles movimientos que puede hacer
		switch(mov) { //para cada movimiento se guardan las coordenas temporales x y  aunque no sufran cambios
			case 1: 
				x_tmp=x +1;
				y_tmp=y;
				break;
			case 2: 
				x_tmp= x  +1 ;
				y_tmp= y + 1;
				break;
			case 3: 
				x_tmp= x ;
				y_tmp=y+1;
				break;
			case 4: 
				x_tmp= x -1;
				y_tmp= y+1;
				break;
			case 5: 
				x_tmp= x  -1;
				y_tmp=y;
				break;
			case 6: 
				x_tmp= x -1;
				y_tmp=y-1;
				break;
			case 7: 
				x_tmp= x ;
				y_tmp=y-1;
				break;
			case 8: 
				x_tmp= x +1;
				y_tmp=y-1;
				break;
			default: 
				break;
		}
        if(x_tmp>0 && x_tmp<=ROWS && y_tmp>0 && y_tmp<=COLS && final[x_tmp][y_tmp]==-1.0){//confirman que se pueda hacer el movimiento
            posicion.x = x_tmp;//guarda la posición en x del movimiento
            posicion.y = y_tmp;//guarda la posición en y del movimiento
            biomass_disponible.insert(pair<float, cell>(biomass[x_tmp][y_tmp], posicion));//se guarda en el mapa, biomass como llave y la estructura con la ubicacion x y
        }
	}
}
//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------