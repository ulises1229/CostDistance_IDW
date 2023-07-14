/*
 * costDistance.cpp
 *
 *  Created on: 04/10/2017
 *      Author: lanase
 */

#include "distance.h"


	// Utility method for comparing two cells
	static bool operator<(const costDistance::cell& a, const costDistance::cell& b){
		if (a.distance == b.distance){
			if (a.x != b.x)
				return (a.x < b.x);
			else
				return (a.y < b.y);
		}
		return (a.distance < b.distance);
	}

	bool costDistance::isInsideGrid(int i, int j){
		return (i >= 0 && i < ROW && j >= 0 && j < COL && friction[i][j] >= 0);
	}


	set<costDistance::cell>costDistance::vecinos(int origen_x, int origen_y){
		set<cell>distancias;
		float** dis;
		dis = new float*[this->ROW];
		for(int i = 0; i< ROW; ++i)
			dis[i] = new float[COL];
		// initializing distance array by INT_MAX
		for (int i = 0; i < ROW; i++)
			for (int j = 0; j < COL; j++){
				dis[i][j] = INT_MAX;
			}

		// direction arrays for simplification of getting
		// neighbour
		int dx[] = { -1, -1, 0, 1, 1, 1, 0,-1 };
		int dy[] = {  0,  1, 1, 1, 0, -1, -1,-1 };

		set<cell> st;

		// insert (0, 0) cell with 0 distance
		st.insert(cell(origen_x, origen_y, 0));

		// initialize distance of (0, 0) with its grid value
		dis[origen_x][origen_y] = friction[origen_x][origen_y];

		// get the cell with minimum distance and delete
		// it from the set
		cell k = *st.begin();
		st.erase(st.begin());
		int x, y;
		// looping through all neighbours
		//cout << friction[origen_x][origen_y] << endl;
		for (int i = 0; i < 8; i++){
			x = k.x + dx[i];
			y = k.y + dy[i];
			//cout << "x = " << x << " y = " << y << endl;
			// if not inside boundry, ignore them
			//cout << friction[x][y] << endl;
			if (!isInsideGrid(x, y) || friction[x][y] == 99999){
				//cout << "no inside grid" << endl;
				continue;
			}

			if(i % 2 == 0){//par = no es diagonal
				//cout << "N" << endl;
				dis[x][y] = (dis[k.x][k.y] + friction[x][y])/2;
			}
			else{
				//cout << "D" << endl;
				dis[x][y] = 1.414214*(dis[k.x][k.y] + friction[x][y])/2;
			}
			distancias.insert(cell(x, y, dis[x][y]));
			active_raster[x][y] = true;
			cost_distance[x][y] = dis[x][y];//son los bordes, no hay necesidad de verificar si es menor al INT_MAX
		}

		cost_distance[origen_x][origen_y] = 0;

		for(int m=0; m<this->ROW; m++)
			delete[] dis[m];

		st.clear();
		return distancias;
	}


	void costDistance::acumulados(set<cell> active_costs, int origen_x, int origen_y, bool diagonals_cost_more){
			/*extern char is_usable;
			int exp;

			if(is_usable == 'y')
				exp = intervals * 4;
			else
				exp = intervals * 8;

			xMin = xMin - exp;
			yMin = yMin - exp;
			xMax = xMax + exp;
			yMax = yMax + exp;

			//cout << xMin << " - " << xMax << " - " << yMin << " - " << yMax << endl;

			if(xMin < 0)
				xMin = 0;

			if(yMin < 0)
				yMin = 0;

			if(xMax >= ROW)
				xMax = ROW - 1;

			if(yMax >= COL)
				yMax = COL - 1;*/


			int dx[] = { -1, -1, 0, 1, 1, 1, 0,-1 };
			int dy[] = {  0,  1, 1, 1, 0, -1, -1,-1 };
			int cont = 1;
			while(!active_costs.empty()){
				cell k = *active_costs.begin();//inicio con la distancia menor
				active_costs.erase(active_costs.begin());

				//cout << "Estoy en: " << k.x << " - " << k.y << " - " << k.distance << endl;
				//if(k.x >= xMin && k.x <= xMax && k.y >= yMin && k.y <= yMax){
					cont++;
					int x, y;
					for (int i = 0; i < 8; i++){
						x = k.x + dx[i];
						y = k.y + dy[i];
						//cout << x << ", " << y << " " << active_raster[x][y]<< " - " << biomass[x][y] << endl;
						set<float>distancias;
						if(isInsideGrid(x,y)) {
							if(i % 2 != 0){// si es movimiento diagonal
								//cout << "Diagonal" << " "<< x << ", " << y << " " << friction[x][y] << endl;
								if((x != origen_x || y != origen_y) && active_raster[x][y] == 0){
									float dist=0;
									int cont_x = k.x; int cont_y = k.y;
									int cont_x2 = k.x; int cont_y2 = k.y;
									int caminos = 1;
									//set<float>distancias;
									for(int z = 0; z < 8; z=z+2){

										int mov_x = rutas[i][z];
										int mov_y = rutas[i][z+1];

										dist += (friction[cont_x+mov_x][cont_y+mov_y] + friction[cont_x2][cont_y2])/2;
										//cout << mov_x << " " << mov_y << " " << "friction[cont_x+mov_x][cont_y+mov_y] = "<< friction[cont_x+mov_x][cont_y+mov_y]
										//<< " friction[cont_x2][cont_y2] = "<< friction[cont_x2][cont_y2] << " /2 =  " << dist << endl;
										if(caminos == 2 || caminos == 4){
											//cout << dist+cost_distance[k.x][k.y] << endl;
											distancias.insert(dist+cost_distance[k.x][k.y]);
											cont_x = cont_x2 = k.x;
											cont_y = cont_y2 = k.y;
											caminos = 0;
											dist = 0;
										}else{
											cont_x2 = cont_x+mov_x;
											cont_y2 = cont_y+mov_y;
											cont_x = cont_x+mov_x;
											cont_y = cont_y+mov_y;
										}
										caminos++;
									}
									distancias.insert(1.414214 * ((friction[k.x][k.y]+friction[x][y])/2) + cost_distance[k.x][k.y] );
									float minimo = *distancias.begin();
									//cout << minimo << endl;
									if(minimo < cost_distance[x][y] && minimo >= 0){
										//cout << "Inserté " << x << ", " << y << " - " << minimo << endl;
										cost_distance[x][y] = minimo;
										active_costs.insert(cell(x,y,minimo));
									}
								}
							}
							else{
								//cout << "Normal" << " "<< x << ", " << y << " " << friction[x][y] << endl;
								float dist = 0;
								if((x != origen_x || y != origen_y) && active_raster[x][y] == 0 && isInsideGrid(x,y)){
									dist = ((friction[k.x][k.y] + friction[x][y])/2) + cost_distance[k.x][k.y];
									if(dist < cost_distance[x][y] && dist >= 0){
										//cout << "Inserté " << x << ", " << y << " - " << dist << endl;
										cost_distance[x][y] = dist;
										active_costs.insert(cell(x,y,dist));
									}
								}
							}
						}
					}
					active_raster[k.x][k.y] = true;
				//}
				}


			active_costs.clear();
			//pushbroom(cost_distance, origen_x, origen_y, 1);
		}

	void costDistance::cost_dist_func()
	{
		std::vector<std::pair<int, int>> pairs;
		cout << "reservo" << endl;
		bool is_first_time = true;
		int n = ROW - 1;
		int m = COL - 1;
		int sources[4][2] = {
			{0, 0},
			{n, m},
			{n, 0},
			{0, m}
		};
		init_index_queue = 0;
		size_queue = 0;
		end_index_queue = 0;
		cout << "reservo" << endl;
		pairs.push_back( make_pair(0,0) );
		active_raster[0][0] = 1;
		cout << "reservo" << endl;
		int count = 0;
//		while( (pairs.size() > 0) )
		while( count < pairs.size() )
		{
				cout << "Iteracion: " << count << endl;
//				cout << "Tamaño de la cola: " << pairs.size() << endl;
//			int i = queue[init_index_queue][0]; //obtengo coordenadas de la casilla sobre la cual trabajar
//			int j = queue[init_index_queue][1];
				int i = pairs.at(count).first;
				int j = pairs.at(count).second;
				cout << "Calculando vecinos de: " << i << " " << j << "COST DISTANCE: " << endl;
				if(isInsideGrid(i, j))
				{
					calculate_neighbors(i, j); //Realizo el calculo de los vecinos
				}
				//Añado los siguientes tres elementos a la cola

//			push_in_queue(i+1, j); // casilla lateral derecha
//			push_in_queue(i, j+1); // casilla inferior
//			push_in_queue(i+1, j+1); // casilla esquina inferior derecha
//			erase_from_queue();
//			pairs.erase(pairs.begin())

				int i1 = i;
				int j1 = j+1;
				int i2 = i+1;
				int j2 = j;
				int i3 = i2;
				int j3 = j1;
				cout << "Posibles vecinos a insertar: " << endl;
				cout << i1 << " y " << j1 << endl;
				cout << i2 << " y " << j2 << endl;
				cout << i3 << " y " << j3 << endl;
				cout << "Repetidos: " << endl;
				if(isInsideGrid(i1, j1))
				{
					if ( !isPresentInList(i1, j1) ) {
						pairs.push_back( make_pair(i1, j1) );
						active_raster[i1][j1] = 1;
					}
				}
				if(isInsideGrid(i2, j2))
				{
					if ( !isPresentInList(i2, j2) ) {
						pairs.push_back( make_pair(i2, j2) );
						active_raster[i2][j2] = 1;
					}
				}
				if(isInsideGrid(i3, j3))
				{
					if ( !isPresentInList(i3, j3) ) {
						pairs.push_back( make_pair(i3, j3) );
						active_raster[i3][j3] = 1;
					}
				}
				cout << "Current list: " << endl;
				for( int i=0; i < pairs.size(); i++ )
				{
					cout << "[ " << pairs.at(i).first << ", " << pairs.at(i).second << " ] " << endl;
				}
//CAMBIO A MATRIZ DE EXPLORACION BINARIA

				count++;
		}
	}

	void costDistance::calculate_neighbors(int i, int j)
	{
		float current_distance = 0;
		float neighbour_distance = 0;
		int dx[] = { -1, -1, 0, 1, 1, 1, 0,-1 };
		int dy[] = {  0,  1, 1, 1, 0, -1, -1,-1 };
			if(cost_distance[i][j] ==  INT_MAX){
				current_distance = friction[i][j];
			}
			else{
				current_distance = cost_distance[i][j];
			}
			for (int mov = 0; mov < 8; mov++){
				int x = i + dx[mov];
				int y = j + dy[mov];
				if( isInsideGrid(x, y) ){
					cout << "VALOR DE COSTO DISTANCIA EN [" << i << ", " << j << "] : " << cost_distance[x][y] << endl;
					if(cost_distance[x][y] ==  INT_MAX){
						neighbour_distance = friction[x][y];
					}
					else{
						neighbour_distance = cost_distance[x][y];
					}
					if (mov % 2 != 0){  //Diagonal movement
						neighbour_distance = sqrt(2)*(neighbour_distance + friction[i][j]);	//Calcula el CD del vecino
					}
					else{ //Cross movement
						neighbour_distance = neighbour_distance + friction[i][j];
					}
					current_distance = min(current_distance, neighbour_distance); //Con el valor calculado verifica para actualizar el minimo del actual
					neighbour_distance = min(neighbour_distance, cost_distance[x][y]); //Actualiza el minimo del vecino que acaba de calcular
					cost_distance[x][y] = neighbour_distance; //Lo setea en la casilla del CD del vecino
				}
			}
			cost_distance[i][j] = current_distance; //añade la distancia final minima
}

void costDistance::push_in_queue(int x, int y)
{
	cout << "entro al metodo con X y Y: " << x << " " << y <<endl;
	if( isInsideGrid(x, y) )
	{
		queue[end_index_queue][0] = x;
		queue[end_index_queue][1] = y;
		end_index_queue++;
		size_queue++;
	}
}

void costDistance::erase_from_queue()
{
	queue[init_index_queue][0] = 0;
	queue[init_index_queue][1] = 0;
	init_index_queue++;
	size_queue--;
}

	void costDistance::pushbroom() {
		int m = ROW - 1; //Renglones
		int n = COL - 1; //Columnas
		int dx[] = { -1, -1, 0, 1, 1, 1, 0,-1 };
		int dy[] = {  0,  1, 1, 1, 0, -1, -1,-1 };
		int sources[4][2] = {
			{0, 0},
			{n, m},
			{n, 0},
			{0, m}
		};

		int contX = 1;
		int contY = 0;

		for(int j = sources[0][1]; j < n; j++) { //Esq. Sup. Izquierda (1)
			for (int i = sources[0][0]; j < m - 1; j++) {
				for (int auxR = 0; auxR < contX; auxR++) {
					for (int auxC = 0; auxC < contY; auxC++) {
						for (int mov = 0; mov < 8; mov++) {
							int x = i + dx[mov];
							int y = j + dy[mov];
							cost_distance[i][j] = INT_MAX;
							if (isInsideGrid(x, y)) {
								cost_distance[i][j];
							}
						}
					}
				}
			}
		}
	}

	void costDistance::pushbroom_old() {
		int n = ROW - 1;
		int m = COL - 1;
		int dx[] = { -1, -1, 0, 1, 1, 1, 0,-1 };
		int dy[] = {  0,  1, 1, 1, 0, -1, -1,-1 };
		int sources[4][2] = {
			{0, 0},
			{n, m},
			{n, 0},
			{0, m}
		};

		/*float min_cost_hv = INT_MAX;
		float min_cost_diag = INT_MAX;*/
		cost_distance[0][0] = friction[0][0]; //cambiar 0,0 por las coords de la primer celda válida
		cost_distance[n][m] = friction[n][m];
		cost_distance[n][0] = friction[n][0];
		cost_distance[0][m] = friction[0][m];


		for(int i = sources[0][0]; i < n; i++) { //Esq. Sup. Izquierda (1)
			for (int j = sources[0][1]; j < m - 1; j++) {
				if (friction[i][j] >= 0) {
					cout << "***** Origen: " << i << ", " << j << endl;
					for (int mov = 0; mov < 8; mov++) {
						int x = i + dx[mov];
						int y = j + dy[mov];
						if( isInsideGrid(x, y) ) {
							if (mov % 2 != 0)
							{ //Diagonal movement
								/*if(cost_distance[i][j]==0)
								{
									cost_distance[i][j] = INT_MAX; //Inicializamos el x actual, los destinos estan en cero
								}
								if(cost_distance[x][y]==0)
								{
									cost_distance[x][y] = friction[x][y];
								}
								cout << "CD del vecino (DIAGONAL)" << x << ", " << y << ": " << cost_distance[x][y] << endl;
								cout << "Frict del origen " << i << ", " << j << ": " << friction[i][j] << endl;
								min_cost_diag = min(min_cost_diag, sqrt(2)*(cost_distance[x][y] + friction[i][j]));
								cout << "Mínimo es: " << min_cost_diag << endl;
								cout << endl;*/

								float distVal = 0;
								float minDist = cost_distance[x][y];
								int adjMovX = i;
								int adjMovY = j;
								//int cont_x2 = i;
								//int cont_y2 = j;
								int movCount = 1;

								for (int z = 0; z < 8; z = z + 2) {
									int nextMovX = rutas[mov][z];
									int nextMovY = rutas[mov][z + 1];

									distVal += friction[adjMovX][adjMovY];
									//distVal += (friction[adjMovX + nextMovX][adjMovY + nextMovY] + friction[adjMovX][adjMovY]) / 2;
									//distVal += friction[adjMovX + nextMovX][adjMovY + nextMovY] + cost_distance[adjMovX][adjMovY];

									if (movCount == 2 || movCount == 4) {
										if (distVal < minDist) {
											minDist = distVal + cost_distance[i][j];
										}
										adjMovX = i;
										adjMovY = j;
										movCount = 0;
										distVal = 0;
									} else {
										adjMovX = adjMovX + nextMovX;
										adjMovY = adjMovY + nextMovY;
									}
									movCount++;
								}

								distVal = sqrt(2) * ( friction[x][y] + cost_distance[i][j] );
								//distVal = sqrt(2) * ( ((friction[i][j] + friction[x][y]) / 2 )+ cost_distance[i][j] );
								//distVal = sqrt(2) * ( friction[x][y] + cost_distance[i][j] );

								if (distVal < minDist) {
									minDist = distVal;
								}

								if (minDist < cost_distance[x][y] && minDist >= 0) {
									cost_distance[x][y] = minDist;
								}

							}
							else
							{ //Cross movement
								/*if(cost_distance[i][j]==0)
								{
									cost_distance[i][j] = INT_MAX; //Inicializamos el x actual, los destinos estan en cero
								}
								if(cost_distance[x][y]==0)
								{
									cost_distance[x][y] = friction[x][y];
								}
								cout << "CD del vecino (ADYACENTE)" << x << ", " << y << ": " << cost_distance[x][y] << endl;
								cout << "Frict del origen " << i << ", " << j << ": " << friction[i][j] << endl;
								min_cost_hv = min(min_cost_hv, cost_distance[x][y] + friction[i][j]);
								cout << "Mínimo es: " << min_cost_hv << endl;
								cout << endl;*/

								float distVal = 0;
								if ( isInsideGrid(x, y) ) {
									distVal = friction[x][y] + cost_distance[i][j];
									//distVal = ((friction[i][j] + friction[x][y]) / 2) + cost_distance[i][j];
									//distVal = cost_distance[i][j] + friction[x][y];
									if (distVal < cost_distance[x][y] && distVal >= 0) {
										cost_distance[x][y] = distVal;
									}
								}
							}
						}
					}
					/*min_cost_diag = min(cost_distance[i][j], min_cost_diag);
					min_cost_hv = min(cost_distance[i][j], min_cost_hv);
					cost_distance[i][j] = min(min_cost_hv, min_cost_diag);
					cout << "VALOR DE COST DISTANCE EN " << i << ", " << j << ": "<< cost_distance[i][j] << endl;
					cout << endl;
					cout << endl;
					min_cost_hv = INT_MAX;
					min_cost_diag = INT_MAX;*/
				}
				//exit(0);
			}
		}
		/*
		for (int i = sources[1][0]; i >= 0; i--) { //Esq. Inf. Derecha (2)
			for (int j = sources[1][1]; j >= 0; j--) {
				for (int mov = 0; mov < 8; mov++) {
					int x = i + dx[mov];
					int y = j + dy[mov];
					if( isInsideGrid(x, y) ) {
						if (mov % 2 == 0)
						{ //Diagonal movement
							/*if(cost_distance[i][j]==0)
							{
								cost_distance[i][j] = INT_MAX; //Inicializamos el x actual, los destinos estan en cero
							}
							if(cost_distance[x][y]==0)
							{
								cost_distance[x][y] = friction[x][y];
							}
							min_cost_diag = min(min_cost_diag, sqrt(2)*(cost_distance[x][y] + friction[i][j]));*/
/*
							float distVal = 0;
							float minDist = cost_distance[x][y];
							int adjMovX = i;
							int adjMovY = j;
							//int cont_x2 = i;
							//int cont_y2 = j;
							int movCount = 1;

							for (int z = 0; z < 8; z = z + 2) {
								int nextMovX = rutas[mov][z];
								int nextMovY = rutas[mov][z + 1];
								distVal += (friction[adjMovX + nextMovX][adjMovY + nextMovY] + friction[adjMovX][adjMovY]) / 2;
								//distVal += friction[adjMovX + nextMovX][adjMovY + nextMovY] + cost_distance[adjMovX][adjMovY];

								if (movCount == 2 || movCount == 4) {
									if (distVal < minDist) {
										minDist = distVal + cost_distance[i][j];
									}
									adjMovX = i;
									adjMovY = j;
									movCount = 0;
									distVal = 0;
								} else {
									adjMovX = adjMovX + nextMovX;
									adjMovY = adjMovY + nextMovY;
								}
								movCount++;
							}

							distVal = sqrt(2) * ( ( (friction[i][j] + friction[x][y]) / 2 ) + cost_distance[i][j] );
							//distVal = sqrt(2) * ( friction[x][y] + cost_distance[i][j] );

							if (distVal < minDist) {
								minDist = distVal;
							}

							if (minDist < cost_distance[x][y] && minDist >= 0) {
								cost_distance[x][y] = minDist;
							}

						}
						else
						{ //Cross movement
							/*if(cost_distance[i][j]==0)
							{
								cost_distance[i][j] = INT_MAX; //Inicializamos el x actual, los destinos estan en cero
							}
							if(cost_distance[x][y]==0)
							{
								cost_distance[x][y] = friction[x][y];
							}
							min_cost_hv = min(min_cost_hv, cost_distance[x][y] + friction[i][j]);*/
/*
							float distVal = 0;
							if ( isInsideGrid(x, y) ) {
								distVal = ( (friction[i][j] + friction[x][y]) / 2 )+ cost_distance[i][j];
								//distVal = cost_distance[i][j] + friction[x][y];
								if (distVal < cost_distance[x][y] && distVal >= 0) {
									cost_distance[x][y] = distVal;
								}
							}

						}
					}
				}
				/*min_cost_diag = min(cost_distance[i][j], min_cost_diag);
				min_cost_hv = min(cost_distance[i][j], min_cost_hv);
				cost_distance[i][j] = min(min_cost_hv, min_cost_diag);
				min_cost_hv = 0;
				min_cost_diag = 0;*//*
			}
		}

*/

		/* for (int i = sources[3][0]; i < n; i++) { //Esq. Sup. Derecha (3)
			for (int j = sources[3][1]; j >= 0; j--) {
				for (int mov = 0; mov < 8; mov++) {
					int x = i + dx[mov];
					int y = j + dy[mov];
					if( isInsideGrid(x, y) ) {
						if (mov % 2 == 0)
						{ //Diagonal movement
							if(cost_distance[i][j]==0)
							{
								cost_distance[i][j] = INT_MAX; //Inicializamos el x actual, los destinos estan en cero
							}
							if(cost_distance[x][y]==0)
							{
								cost_distance[x][y] = friction[x][y];
							}
							min_cost_diag = min(min_cost_diag, sqrt(2)*(cost_distance[x][y] + friction[i][j]));
/**
							float distVal = 0;
							float minDist = cost_distance[x][y];
							int adjMovX = i;
							int adjMovY = j;
							//int cont_x2 = i;
							//int cont_y2 = j;
							int movCount = 1;

							for (int z = 0; z < 8; z = z + 2) {
								int nextMovX = rutas[mov][z];
								int nextMovY = rutas[mov][z + 1];
								//distVal += (friction[adjMovX + nextMovX][adjMovY + nextMovY] + friction[adjMovX][adjMovY]) / 2;
								distVal += friction[adjMovX + nextMovX][adjMovY + nextMovY] + cost_distance[adjMovX][adjMovY];

								if (movCount == 2 || movCount == 4) {
									if (distVal < minDist) {
										minDist = distVal + cost_distance[i][j];
									}
									adjMovX = i;
									adjMovY = j;
									movCount = 0;
									distVal = 0;
								} else {
									adjMovX = adjMovX + nextMovX;
									adjMovY = adjMovY + nextMovY;
								}
								movCount++;
							}

							//distVal = sqrt(2) * ( (friction[i][j] + friction[x][y]) / 2 + cost_distance[i][j] );
							distVal = sqrt(2) * ( friction[x][y] + cost_distance[i][j] );

							if (distVal < minDist) {
								minDist = distVal;
							}

							if (minDist < cost_distance[x][y] && minDist >= 0) {
								cost_distance[x][y] = minDist;
							}

						}
						else
						{ //Cross movement
							if(cost_distance[i][j]==0)
							{
								cost_distance[i][j] = INT_MAX; //Inicializamos el x actual, los destinos estan en cero
							}
							if(cost_distance[x][y]==0)
							{
								cost_distance[x][y] = friction[x][y];
							}
							min_cost_hv = min(min_cost_hv, cost_distance[x][y] + friction[i][j]);*/
/**
							float distVal = 0;
							if ( isInsideGrid(x, y) ) {
								//distVal = (friction[i][j] + friction[x][y]) / 2 + cost_distance[i][j];
								distVal = cost_distance[i][j] + friction[x][y];
								if (distVal < cost_distance[x][y] && distVal >= 0) {
									cost_distance[x][y] = distVal;
								}
							}
*/
						/*}
					}
				}
				min_cost_diag = min(cost_distance[i][j], min_cost_diag);
				min_cost_hv = min(cost_distance[i][j], min_cost_hv);
				cost_distance[i][j] = min(min_cost_hv, min_cost_diag);
				min_cost_hv = 0;
				min_cost_diag = 0;
			}
		}

		for (int i = sources[2][0]; i >= 0; i--) { //Esq. Inf. Izquierda (4)
			for (int j = sources[2][1]; j < m; j++) {
				for (int mov = 0; mov < 8; mov++) {
					int x = i + dx[mov];
					int y = j + dy[mov];
					if( isInsideGrid(x, y) ) {
						if (mov % 2 == 0)
						{ //Diagonal movement
							if(cost_distance[i][j]==0)
							{
								cost_distance[i][j] = INT_MAX; //Inicializamos el x actual, los destinos estan en cero
							}
							if(cost_distance[x][y]==0)
							{
								cost_distance[x][y] = friction[x][y];
							}
							min_cost_diag = min(min_cost_diag, sqrt(2)*(cost_distance[x][y] + friction[i][j]));
/**
							float distVal = 0;
							float minDist = cost_distance[x][y];
							int adjMovX = i;
							int adjMovY = j;
							//int cont_x2 = i;
							//int cont_y2 = j;
							int movCount = 1;

							for (int z = 0; z < 8; z = z + 2) {
								int nextMovX = rutas[mov][z];
								int nextMovY = rutas[mov][z + 1];
								//distVal += (friction[adjMovX + nextMovX][adjMovY + nextMovY] + friction[adjMovX][adjMovY]) / 2;
								distVal += friction[adjMovX + nextMovX][adjMovY + nextMovY] + cost_distance[adjMovX][adjMovY];

								if (movCount == 2 || movCount == 4) {
									if (distVal < minDist) {
										minDist = distVal + cost_distance[i][j];
									}
									adjMovX = i;
									adjMovY = j;
									movCount = 0;
									distVal = 0;
								} else {
									adjMovX = adjMovX + nextMovX;
									adjMovY = adjMovY + nextMovY;
								}
								movCount++;
							}

							//distVal = sqrt(2) * ( (friction[i][j] + friction[x][y]) / 2 + cost_distance[i][j] );
							distVal = sqrt(2) * ( friction[x][y] + cost_distance[i][j] );

							if (distVal < minDist) {
								minDist = distVal;
							}

							if (minDist < cost_distance[x][y] && minDist >= 0) {
								cost_distance[x][y] = minDist;
							}

						}
						else
						{ //Cross movement
							if(cost_distance[i][j]==0)
							{
								cost_distance[i][j] = INT_MAX; //Inicializamos el x actual, los destinos estan en cero
							}
							if(cost_distance[x][y]==0)
							{
								cost_distance[x][y] = friction[x][y];
							}
							min_cost_hv = min(min_cost_hv, cost_distance[x][y] + friction[i][j]);*/
/**
							float distVal = 0;
							if ( isInsideGrid(x, y) ) {
								//distVal = (friction[i][j] + friction[x][y]) / 2 + cost_distance[i][j];
								distVal = cost_distance[i][j] + friction[x][y];
								if (distVal < cost_distance[x][y] && distVal >= 0) {
									cost_distance[x][y] = distVal;
								}
							}
*/
						/*}
					}
				}
				min_cost_diag = min(cost_distance[i][j], min_cost_diag);
				min_cost_hv = min(cost_distance[i][j], min_cost_hv);
				cost_distance[i][j] = min(min_cost_hv, min_cost_diag);
				min_cost_hv = 0;
				min_cost_diag = 0;
			}
		}*/
	}


	void costDistance::inicio_cost_distance(float** grid, int srcX, int srcY, int firstX, int firstY, double projection, bool is_relative, bool diagonals_cost_more) {
		//cout << COL << " " << ROW << endl;
		this->friction = new float*[this->ROW];
		this->cost_distance = new float*[this->ROW];
		this->active_raster = new bool*[this->ROW];
		for(int i = 0; i< ROW; ++i){
			this->friction[i] = new float[COL];
			this->cost_distance[i] = new float[COL];
			this->active_raster[i] = new bool[COL];
		}
		for(int x = 0; x < ROW; x++){
			for(int y = 0; y < COL; y++){
				/* if(grid[x][y] == 999999)
					grid[x][y] = -9999; */

				if (is_relative)
					this->friction[x][y] = grid[x][y] * projection;
				else
					this->friction[x][y] = grid[x][y];

				this->active_raster[x][y] = false;
				this->cost_distance[x][y] = INT_MAX;
			}
		}

		this->queue = new int*[ROW * COL * 1000]; //Declara

		for(int z = 0; z < ROW*COL*3; z++)
		{
			this->queue[z] = new int[2];
			this->queue[z][0] = 0;
			this->queue[z][1] = 0;
		}

		// the position of a cell that you want to display its neighbours
		cout << srcX << " - " << srcY << endl;
		//active_raster[srcX][srcY] = 1;
		cost_distance[srcX][srcY] = 0;

		//se obtienen los vecinos proximos al origen y sus distancias calculadas. ordenas de menor a mayor
		//set<cell> distancias = vecinos(srcX,srcY);
//		cost_distance[srcX][srcY] = 0;

		/*set <cell> :: iterator itr;
		for (itr = distancias.begin(); itr != distancias.end(); ++itr){
			cout << (*itr).x << ", " << (*itr).y << " - dist =  " << (*itr).distance << endl;
		}

		exit(0);*/
		//acumulados(distancias, srcX, srcY, diagonals_cost_more);//metodo para calcular demas vecinos.
		cout << "Antes de Pushbroom" << endl;
//		pushbroom();
		cost_dist_func();

		for(int r=0; r<ROW; r++){
			for(int c=0; c<COL; c++){
				if(cost_distance[r][c] == INT_MAX)
					cost_distance[r][c] = -9999;

			}
		}
	}

	void costDistance::freeMem() {
		for(int m=0; m < ROW; m++){
			delete[] cost_distance[m];
			delete[] friction[m];
			delete[] active_raster[m];
		}
	}

	float costDistance::min(float a, float b) {
		if( a <= b )
			return a;
		else
			return b;
	}

	bool costDistance::isPresentInList(int i, int j)
	{
		/*bool exists = false;
		for( int k=0; k < pairs.size(); k++ )
		{
			if( ( pairs.at(k).first== i ) && ( pairs.at(k).second == j ) )
			{
				exists = true;
				break;
			}
		}*/
		return active_raster[i][j];
	}
