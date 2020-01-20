
/*
* distance.h
*
*  Created on: 15/03/2018
*      Author: lanase
*/

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <iterator>
#include <set>
#include <climits>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
using namespace std;

class costDistance {
  public:
  //structs
  struct cell2 {
  int x, y;
  cell2(int x, int y) :
      x(x), y(y) {}
  };

  struct cell {
  int x, y;
  float distance;
  cell(int x, int y, float distance) :
      x(x), y(y), distance(distance) {}
  };



  //methods
  bool isInsideGrid(int i, int j);
  inline set<cell>vecinos(int origen_x, int origen_y);
  inline void acumulados(set<cell> active_costs, int origen_x, int origen_y, bool diagonals_cost_more);
  inline void pushbroom_old();
  inline void pushbroom();

  //nuevos metodos
  void cost_dist_func();
  void calculate_neighbors(int i, int j);
  void erase_from_queue();
  void push_in_queue(int x, int y);

  void inicio_cost_distance(float** grid, int srcX, int srcY, int firstX, int firstY, double projection, bool is_relative, bool diagonals_cost_more);
  void freeMem();
  bool validate_limit(double max, double min, float friction);
  float min(float a, float b);
  bool isPresentInList(int i, int j);


  //variables
  int ROW, COL;
  int ** locations;
  float ** cost_distance;
  float ** friction;
  bool ** active_raster;

  //variables de la cola de nodos pendientes
  int ** queue;
  int init_index_queue;
  int end_index_queue;
  int size_queue;

  int rutas[8][8]{
    {0,0,0,0,0,0,0,0},
    {-1,0,0,1,0,1,-1,0},
    {0,0,0,0,0,0,0,0},
    {0,1,1,0,1,0,0,1},
    {0,0,0,0,0,0,0,0},
    {0,-1,1,0,1,0,0,-1},
    {0,0,0,0,0,0,0,0},
    {-1,0,0,-1,0,-1,-1,0},
  };

};
