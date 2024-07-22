#ifndef __AREA_OPTIMIZER_H
#define __AREA_OPTIMIZER_H

#include "types.h"
#include <vector>

class AreaOptimizer {
public:
  virtual ~AreaOptimizer() = default;
  virtual void run_iteration() = 0;

  /**
   * Pre: -
   * Post: Returns the number of cells occupied by a given source
   **/
  virtual std::vector<int> get_areas() = 0;

  /**
   * Pre: -
   * Post: Returns the weights associated to each source
   **/
  virtual std::vector<double> get_weights() = 0;
  virtual std::vector<Vector2D> get_sources() = 0;
  virtual std::vector<std::vector<int>> get_table() = 0;
  virtual bool is_converged() = 0;
};

#endif
