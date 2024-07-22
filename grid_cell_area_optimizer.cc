#include "grid_cell_area_optimizer.h"

#include <cstdlib>
#include <queue>
using namespace std;

vector<int> dx = {0, 0, 1, -1};
vector<int> dy = {1, -1, 0, 0};

GridCellAreaOptimizer::GridCellAreaOptimizer(int width, int height,
                                             vector<Vector2D> sources,
                                             vector<double> weights) {
  _width = width;
  _height = height;
  _n_sources = sources.size();
  _sources = sources;
  _weights = weights;
  _converged = false;
}

void GridCellAreaOptimizer::_fill_areas() {
  _table = vector<vector<int>>(_height, vector<int>(_width, -1));

  vector<queue<Vector2D>> queues(_n_sources);
  for (int i = 0; i < _n_sources; ++i) {
    queues[i].push(_sources[i]);
  }
  priority_queue<pair<double, int>> pq;
  for (int i = 0; i < _n_sources; ++i) {
    pq.push({0, i});
  }

  while (!pq.empty()) {
    auto [area, index] = pq.top();
    pq.pop();

    auto &q = queues[index];
    if (!q.empty()) {
      auto [x, y] = q.front();
      q.pop();
      if (0 <= x && x < _width && 0 <= y && y < _height && _table[x][y] == -1) {
        _table[x][y] = index;
        area -= 1 / _weights[index]; // Order from small to large
        for (int i = 0; i < 4; ++i) {
          int nx = x + dx[i];
          int ny = y + dy[i];
          q.push({nx, ny});
        }
      }
      pq.push({area, index});
    }
  }
}

void GridCellAreaOptimizer::_expand() {
  vector<pair<int, Vector2D>> cell_sum(_n_sources, {0, {0, 0}});

  for (int x = 0; x < _width; ++x) {
    for (int y = 0; y < _height; ++y) {
      int index = _table[x][y];
      cell_sum[index].first++;
      cell_sum[index].second.x += x;
      cell_sum[index].second.y += y;
    }
  }

  for (int i = 0; i < _n_sources; ++i) {
    auto [n, p] = cell_sum[i];
    if (n > 0)
      _sources[i] = {p.x / n, p.y / n};
    else
      _sources[i] = {rand() % _width, rand() % _height};
  }
}

void GridCellAreaOptimizer::run_iteration() {
  if (not _converged) {
    vector<Vector2D> old_sources = get_sources();
    _fill_areas();
    _expand();
    if (old_sources == get_sources())
      _converged = true;
  }
}

vector<int> GridCellAreaOptimizer::get_areas() {
  vector<int> area(_n_sources, 0);
  for (int x = 0; x < _width; ++x) {
    for (int y = 0; y < _height; ++y) {
      ++area[_table[x][y]];
    }
  }
  return area;
}

vector<double> GridCellAreaOptimizer::get_weights() { return _weights; }

vector<Vector2D> GridCellAreaOptimizer::get_sources() { return _sources; }

RegionIndices GridCellAreaOptimizer::get_table() { return _table; }

bool GridCellAreaOptimizer::is_converged() { return _converged; }
