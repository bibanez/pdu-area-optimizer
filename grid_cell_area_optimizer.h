#ifndef __GRID_CELL_AREA_OPTIMIZER_H
#define __GRID_CELL_AREA_OPTIMIZER_H

#include "area_optimizer.h"
using RegionIndices = std::vector<std::vector<int>>;

class GridCellAreaOptimizer : public AreaOptimizer {
  int _width, _height, _n_sources;
  std::vector<Vector2D> _sources;
  std::vector<double> _weights;
  std::vector<std::vector<int>> _table;
  bool _converged;

  /**
   *Pre: none\n
   *Post: `_table` contains the result of the weighted fill algorithm
   **/
  void _fill_areas();

  /**
   *Pre: `_table` is not empty\n
   *Post: `_sources` is updated to contain the new centroids for each region
   **/
  void _expand();

public:
  GridCellAreaOptimizer(int width, int height, std::vector<Vector2D> sources,
                        std::vector<double> weights);
  void run_iteration() override;
  std::vector<int> get_areas() override;
  std::vector<double> get_weights() override;
  std::vector<Vector2D> get_sources() override;
  RegionIndices get_table() override;
  bool is_converged() override;
};

#endif
