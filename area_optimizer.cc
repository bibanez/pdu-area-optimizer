#include "area_optimizer.h"

#include <cstdlib>
#include <queue>
using namespace std;

vector<int> dx = {0, 0, 1, -1};
vector<int> dy = {1, -1, 0, 0};

AreaOptimizer::AreaOptimizer(int width, int height, vector<Point> sources,
                             vector<float> weights) {
  this->_width = width;
  this->_height = height;
  this->_n_sources = sources.size();
  this->_sources = sources;
  this->_weights = weights;
  this->_converged = false;
}

void AreaOptimizer::_fill_areas() {
  _table = vector<vector<int>>(_height, vector<int>(_width, -1));

  vector<queue<Point>> queues(_n_sources);
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

void AreaOptimizer::_expand() {
  vector<pair<int, Point>> cell_sum(_n_sources, {0, {0, 0}});

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

void AreaOptimizer::run_iteration() {
  vector<Point> old_sources = _sources;
  if (not _converged) {
    _fill_areas();
    _expand();
    if (_sources == old_sources)
      _converged = true;
  }
}

bool AreaOptimizer::converged() { return _converged; }

/**
 * Pre: -
 * Post: Returns the number of cells occupied by a given source
 **/
vector<int> AreaOptimizer::get_areas() {
  vector<int> area(_n_sources, 0);
  for (int x = 0; x < _width; ++x) {
    for (int y = 0; y < _height; ++y) {
      ++area[_table[x][y]];
    }
  }
  return area;
}

vector<Point> AreaOptimizer::get_sources() { return _sources; }

/**
 * Pre: -
 * Post: Returns the weights associated to each source
 **/
vector<float> AreaOptimizer::get_weights() { return _weights; }

vector<vector<int>> AreaOptimizer::get_table() { return _table; }
