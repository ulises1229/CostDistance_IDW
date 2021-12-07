/*
 * main.cpp
 *
 *  Created on: 17/08/2017
 *      Author: lanase
 */

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
//#include <float.h>
#include <vector>
#include <iterator>
#include <map>
#include "DisplayImage.cpp"
#include "distance.h"
//#include "gdal.cpp"
//#include "exploracion.h" //A*
//#include "tree.h" //Nodos
//#include "dijkstra.cpp" //Dijkstra
//#include "bellford.cpp" //Bellman-Ford
#include <tclap/CmdLine.h>
#include <omp.h>

struct centroid{
  int x;
  int y;
};

char is_usable;

void compare_matrices(float ** m_friction, float ** m_fricged, int r, int c)
{
  int count = 0;
  ofstream ofs("matrix_diff.txt", std::ofstream::app);
  for(int i = 0; i < r; i++)
  {
    for(int j = 0; j < c; j++)
    {
//      if((m_friction[i][j] > m_fricged[i][j]) && (m_fricged[i][j] >= 0 && m_friction[i][j] >= 0))
//      if( (m_fricged[i][j] >= 0 && m_friction[i][j] >= 0) )
//      {
        ofs << "("<< i <<","<< j << "), F=" << setprecision(10) << m_friction[i][j] << ", G = " << m_fricged[i][j] << ", " << m_friction[i][j] - m_fricged[i][j] << endl;
        count++;
//      }
    }
  }
  ofs << count;
  ofs.close();
}

int main(int argc, const char** argv) {
  string map_friction, map_locality, is_r, diagonals_cm, map_fric_gdal;
  bool is_relative = false;
  bool diagonals_cost_more = false;
  //Command line messages
  TCLAP::CmdLine cmd("Command description message", ' ', "1");
  TCLAP::ValueArg<std::string> friction("f","friction","Absolute path to friction_map.tif",true,"/path/to/image.tif","string");
  TCLAP::ValueArg<std::string> locality("l","locality","Absolute path to locality_map.tif",true,"/path/to/image.tif","string");
  TCLAP::ValueArg<std::string> gdal("g","gdal","Absolute path to gdal_map.tif",true,"/path/to/image.tif","string");
  TCLAP::ValueArg<std::string> diagonals_cost("d","diagonals_cost_more","If true, diagonals have extra cost.",true,"true or false","string");
  TCLAP::ValueArg<std::string> is_rel("r","relative","Determines if the friction on map is relative",true,"true or false","string");
  // Add arguments to command line output
  cmd.add(friction);
  cmd.add(locality);
  cmd.add(gdal);
  cmd.add(diagonals_cost);
  cmd.add(is_rel);
  // Parse the argv array.
  cmd.parse( argc, argv );
  // Get the value parsed by each arg.
  map_friction = friction.getValue();
  map_locality = locality.getValue();
  map_fric_gdal = gdal.getValue();
  is_r= is_rel.getValue();
  diagonals_cm = diagonals_cost.getValue();

  //Check if friction is relative
  if (is_r == "true" || is_r == "t")
    is_relative = true;

  if (diagonals_cm == "true" || diagonals_cm == "t")
    diagonals_cost_more = true;
  //Printing the input values
  cout << "Friction Map: " << map_friction << endl;
  cout << "Loc Map: " << map_locality << endl;

  //Is usable set value
  is_usable = 'n';
  // Declare image object
  Display_image di;
  // Import friction and points
  // Import biomass and friction  
	clock_t begin = clock();
  float** m_friction = di.tiff_to_matrix_gdal(map_friction, 1);
  float** m_locality = di.tiff_to_matrix_gdal(map_locality, 2);
  float** m_fricged = di.tiff_to_matrix_gdal(map_fric_gdal, 3);

  //cout << "Done" << endl;
	cout << di.epsg << endl;
	di.reproject_coords(map_friction);
	//exit(0);
	clock_t end = clock();
  int rows = di.ROWS;
  int cols = di.COLS;

	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	cout << "TIFF to matrix = " << elapsed_secs << " secs." << endl;

  //Generate vector of localities
  vector<centroid> localities;
  for(int i=0; i<rows; i++)
  {
    for(int j=0; j<cols; j++)
    {
      cout << m_locality[i][j] << endl;
      if(m_locality[i][j]==1)
        localities.push_back({i, j});
//      cout << "(" << i << "," << j << ")";
    }
  }
 
  //Function distance is declared
  costDistance d;
  d.COL = cols;
  d.ROW = rows;
  double min = di.adfMinMax[0];
  double max = di.adfMinMax[1];
  
  //Func dist is called from the localities vector
  cout << localities.size() << endl;
  for(int k = 0; k < localities.size(); k++)
  {
    cout << "dnjnd" << endl;
    int coord_x = localities.at(k).x;
    int coord_y = localities.at(k).y;

    clock_t begin_cd = clock();
    
    d.inicio_cost_distance(m_friction, coord_x, coord_y, di.firstX, di.firstY, di.projection, is_relative, diagonals_cost_more);

    //d.inicio_cost_distance(d.cost_distance, coord_x, coord_y, di.projection, is_relative, max, min, true);
    /*for (int i = 0; i < 25; i++) {
      d.inicio_cost_distance(d.cost_distance, coord_x, coord_y, di.projection, is_relative, max, min, true);
    }*/

    clock_t end_cd = clock();
    double elapsed_secs_cd = double(end_cd - begin_cd) / CLOCKS_PER_SEC;
    cout << "Cost Distance time = " << elapsed_secs_cd << " secs." << endl;

    clock_t begin3 = clock();
    di.matrix_to_tiff(d.cost_distance, rows, cols, k+1);
    clock_t end3 = clock();
    double elapsed_secs3 = double(end3 - begin3) / CLOCKS_PER_SEC;
    cout << "Creating the final route image took " << elapsed_secs3 << " secs." << endl;

    compare_matrices(d.cost_distance, m_fricged, rows, cols);
    d.freeMem();
  }
}
