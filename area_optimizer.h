#ifndef __AREA_OPTIMIZER_H
#define __AREA_OPTIMIZER_H

#include "types.h"
#include <vector>

using DirectionVec = std::vector<Node<Edge>*>;
using RegionIndices = std::vector<std::vector<int>>;

class AreaOptimizer {
  int _width, _height, _n_regions, _limit;
  bool _converged;
  std::vector<Region> _regions;
  RegionIndices _table;
  std::vector<std::vector<DirectionVec>> _edge_tables;

  /**
   *Pre: none\n
   *Post: `_table` and `_edge_tables` are cleared. `_regions` is reset
   **/
  void _clear_structures();

  /**
   *Pre: `_regions` is reset, `_table` and `_edge_tables` are empty (filled with
   *-1)\n
   *Post: `_table` is painted with the index of each region
   **/
  void _fill_areas();

  /**
   *Pre: every region in `_regions` has its cell_sum and area correctly
   *calculated\n
   *Post: The source for every region is moved to its centroid
   **/
  void _correct_centroids();

  /**
   *Pre: none\n
   *Post: Returns a vector with the positions of the edge ordered from source to
   *end. All positions are inside the table, so if the edge is invalid, it will
   *return an empty vector
   **/
  std::vector<Vector2D> _iterate_edge(const Edge &e);

  /**
   *Pre: `r_index` is a valid index of `_regions` and has expandable edges\n
   *Post: Returns the index of the edge that should be expanded with the
   *criteria that it moves the centroid closest to the source
   **/
  Node<Edge> *_select_edge(int r_index);

  /**
   *Pre: `r_index` is a valid index of `_regions` and `e_index` is a valid
   *edge\n
   *Post: The edge is expanded in the direction of the source. `_table` is
   *painted with the new region
   **/
  void _expand_edge(int r_index, Node<Edge> *e_ptr);

  /**
   *Pre: `r_index` is a valid index of `_regions` and `e_index` is a valid
   *edge of the region\n
   *Post: The edge is deleted from the edge table of the region
   **/
  void _delete_edge(int r_index, Node<Edge> *e_ptr);

  /**
   *Pre: `r_index` is a valid index of `_regions`\n
   *Post: Region `r_index` contains the edge `e`. Keep in mind that it will only
   *have the expandable edges in the edge map
   **/
  void _add_edge(int r_index, Edge e);

  /**
   *Pre: `r_index` is a valid index of `_regions` and `e` is expandable\n
   *Post: The edge is added to the edge table of the region
   **/
  void _add_edge_table(int r_index, const Edge &e);

  /**
   *Pre: none\n
   *Post: Returns a vector with a list of expandable edges obtained from `e`
   **/
  std::vector<Edge> _break_up_edge(const Edge &e);

  /**
   *Pre: none\n
   *Post: Returns true if position is inside the table, false otherwise
   **/
  bool _out_of_bounds(Vector2D pos);

public:
  /**
   *Pre: width and height are > 0, limit is > 0 and proportional to width and
   *height, sources and weights have the same size\n Post: AreaOptimizer is
   *instantiated with the correct parameters
   **/
  AreaOptimizer(int width, int height, int limit, std::vector<Vector2D> sources,
                std::vector<double> weights);

  void run_iteration();
  std::vector<int> get_areas();
  std::vector<double> get_weights();
  std::vector<Vector2D> get_sources();
  std::vector<std::vector<int>> get_table();
  bool is_converged();
};

#endif
