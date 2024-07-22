
#include "ortho_area_optimizer.h"
#include "list.h"

#include <assert.h>
#include <cstdlib>
#include <queue>
using namespace std;

OrthoAreaOptimizer::OrthoAreaOptimizer(int width, int height, int limit,
                             vector<Vector2D> sources, vector<double> weights) {
  _width = width;
  _height = height;
  _limit = limit;
  _table = vector<vector<int>>(width, vector<int>(height, -1));
  _edge_tables = vector<vector<DirectionVec>>(
      width, vector<DirectionVec>(height, DirectionVec(4, nullptr)));
  _n_regions = sources.size();
  _regions = vector<Region>(_n_regions);
  for (int i = 0; i < _n_regions; ++i)
    _regions[i] = {sources[i], {0, 0}, 0, weights[i], {}};
  _converged = false;
}

void OrthoAreaOptimizer::_fill_areas() {
  priority_queue<pair<double, int>> pq;
  for (int i = 0; i < _n_regions; ++i) {
    Region &r = _regions[i];
    // Add edges
    for (int k = 0; k < 4; ++k) {
      Edge e{(Direction)(k), r.source, 1};
      _add_edge(i, e);
    }
    // We push the negative area to have a min heap
    if (_regions[i].area != 0)
      pq.push({-r.area / r.weight, i});
  }

  // Main loop
  while (not pq.empty()) {
    auto [_, r_index] = pq.top();
    pq.pop();

    Region &r = _regions[r_index];

    // Check if there are valid edges
    if (not r.edge_list.empty()) {
      Node<Edge> *e_ptr = _select_edge(r_index);
      _expand_edge(r_index, e_ptr);
      pq.push({-r.area / r.weight, r_index});
    }
  }
}

void OrthoAreaOptimizer::_correct_centroids() {
  for (Region &r : _regions) {
    if (r.area > 0)
      r.source = {r.cell_sum.x / r.area, r.cell_sum.y / r.area};
    else
      r.source = {rand() % _width, rand() % _height};
  }
}

Edge expand_edge_aux(const Edge &e) {
  return {e.dir, e.source + e.normal(), e.length};
}

void OrthoAreaOptimizer::_expand_edge(int r_index, Node<Edge> *e_ptr) {
  // cout << "Expanding edge region " << r_index << ": ";
  // print_edge_info(e_ptr->data);
  Edge e = expand_edge_aux(e_ptr->data);
  _delete_edge(r_index, e_ptr);
  _add_edge(r_index, e);
  _add_edge(r_index, {ROTATE_COUNTERCLOCKWISE(e.dir), e.source, 1});
  _add_edge(r_index, {ROTATE_CLOCKWISE(e.dir), e.end(), 1});
}

bool OrthoAreaOptimizer::_out_of_bounds(Vector2D pos) {
  return pos.x < 0 or pos.x >= _width or pos.y < 0 or pos.y >= _height;
}

vector<Edge> OrthoAreaOptimizer::_break_up_edge(const Edge &e) {
  vector<Edge> v_edges;

  Edge current_edge;
  bool valid = false;
  for (Vector2D pos : _iterate_edge(e)) {
    Vector2D front = pos + e.normal();
    if (_out_of_bounds(front) or _table[front.x][front.y] != -1) {
      if (valid) {
        v_edges.push_back(current_edge);
        valid = false;
      }
    } else {
      if (not valid) {
        current_edge = {e.dir, pos, 1};
        valid = true;
      } else
        ++current_edge.length;
    }
  }

  if (valid)
    v_edges.push_back(current_edge);

  return v_edges;
}

void OrthoAreaOptimizer::_add_edge(int r_index, Edge e) {
  // cout << "Adding edge to region " << r_index << ": ";
  // print_edge_info(e);
  Region &r = _regions[r_index];
  // Paint the cells and update the region
  for (Vector2D pos : _iterate_edge(e)) {
    if (_table[pos.x][pos.y] == -1) {
      _table[pos.x][pos.y] = r_index;
      r.cell_sum += pos;
      ++r.area;
    }
  }

  // Break up edges that will now be blocked
  for (Vector2D pos : _iterate_edge(expand_edge_aux(e))) {
    int front_r = _table[pos.x][pos.y];
    Node<Edge> *front_e_ptr =
        _edge_tables[pos.x][pos.y][ROTATE_OPPOSITE(e.dir)];
    if (front_r != -1 and front_e_ptr != nullptr) {
      if (front_e_ptr == nullptr) {
        // cout << "Error! Expanding into region " << front_r << endl;
        // cout << "Region edges:" << endl;
        // FOR_EACH_NODE(_regions[front_r].edge_list, node) {
        //   print_edge_info(node->data);
        // }
      }
      assert(front_e_ptr != nullptr);
      Edge front_edge = front_e_ptr->data;
      _delete_edge(front_r, front_e_ptr);
      for (Edge edge : _break_up_edge(front_edge))
        _add_edge_table(front_r, edge);
    }
  }

  // Bind edge to connected edges
  Vector2D left_pos = e.source + ROTATE_OPPOSITE(e.dir);
  Node<Edge> *l_node = _edge_tables[left_pos.x][left_pos.y][e.dir];
  if (not _out_of_bounds(left_pos) and
      _table[left_pos.x][left_pos.y] == r_index and l_node != nullptr) {
    e.source = l_node->data.source;
    e.length += l_node->data.length;
    _delete_edge(r_index, l_node);
  }

  Vector2D right_pos = e.end() + e.dir;
  Node<Edge> *r_node = _edge_tables[right_pos.x][right_pos.y][e.dir];
  if (not _out_of_bounds(right_pos) and
      _table[right_pos.x][right_pos.y] == r_index and r_node != nullptr) {
    e.length += r_node->data.length;
    _delete_edge(r_index, r_node);
  }

  // Add the expandable edges to the region
  for (Edge edge : _break_up_edge(e))
    _add_edge_table(r_index, edge);
}

void OrthoAreaOptimizer::_delete_edge(int r_index, Node<Edge> *e_ptr) {
  Region &r = _regions[r_index];
  Edge &e = e_ptr->data;
  // cout << "Deleting edge from region " << r_index << ": ";
  // print_edge_info(e);
  // Unpaint the edge_table cells
  for (Vector2D pos : _iterate_edge(e))
    _edge_tables[pos.x][pos.y][e.dir] = nullptr;
  r.edge_list.delete_node(e_ptr);
}

Node<Edge> *OrthoAreaOptimizer::_select_edge(int r_index) {
  Region &r = _regions[r_index];
  Node<Edge> *max_ptr = nullptr;
  int max_dist = -1;
  Node<Edge> *max_ptr_under = nullptr;
  int max_dist_under = -1;
  int max_length_under = -1;
  FOR_EACH_NODE(r.edge_list, node) {
    Edge new_edge = expand_edge_aux(node->data);

    // Calculate new centroid
    Vector2D cell_sum = r.cell_sum;
    int n = r.area;
    for (Vector2D pos : _iterate_edge(new_edge)) {
      cell_sum += pos;
      n++;
    }
    Vector2D centroid = {cell_sum.x / n, cell_sum.y / n};
    Vector2D diff = centroid - r.source;
    float new_dist = diff.x * diff.x + diff.y * diff.y;

    // Update max if applicable
    if (new_dist < max_dist or max_dist < 0) {
      max_ptr = node;
      max_dist = new_dist;
    }
    if (node->data.length < _limit and node->data.length >= max_length_under and
        (new_dist < max_dist_under or max_dist_under < 0)) {
      max_ptr_under = node;
      max_dist_under = new_dist;
      max_length_under = node->data.length;
    }
  }
  if (max_dist < 0)
    return max_ptr_under;
  return max_ptr;
}

void OrthoAreaOptimizer::_add_edge_table(int r_index, const Edge &e) {
  Region &r = _regions[r_index];
  Node<Edge> *e_ptr = r.edge_list.push_back(e);
  for (Vector2D pos : _iterate_edge(e))
    _edge_tables[pos.x][pos.y][e.dir] = e_ptr;
}

vector<int> OrthoAreaOptimizer::get_areas() {
  vector<int> areas(_n_regions);
  for (int i = 0; i < _n_regions; ++i)
    areas[i] = _regions[i].area;
  return areas;
}

vector<double> OrthoAreaOptimizer::get_weights() {
  vector<double> weights(_n_regions);
  for (int i = 0; i < _n_regions; ++i)
    weights[i] = _regions[i].weight;
  return weights;
}

vector<Vector2D> OrthoAreaOptimizer::get_sources() {
  vector<Vector2D> sources = vector<Vector2D>(_n_regions);
  for (int i = 0; i < _n_regions; ++i)
    sources[i] = _regions[i].source;
  return sources;
}

vector<Vector2D> OrthoAreaOptimizer::_iterate_edge(const Edge &e) {
  vector<Vector2D> positions;
  if (e.source.x < 0 or e.source.x >= _width or e.source.y < 0 or
      e.source.y >= _height)
    return {};

  if (e.dir == UP) {
    if (e.end().y >= _height)
      return {};
    for (int y = e.source.y; y < e.source.y + e.length; ++y)
      positions.push_back({e.source.x, y});
  } else if (e.dir == DOWN) {
    if (e.end().y < 0)
      return {};
    for (int y = e.source.y; y > e.source.y - e.length; --y)
      positions.push_back({e.source.x, y});
  } else if (e.dir == LEFT) {
    if (e.end().x < 0)
      return {};
    for (int x = e.source.x; x > e.source.x - e.length; --x)
      positions.push_back({x, e.source.y});
  } else if (e.dir == RIGHT) {
    if (e.end().x >= _width)
      return {};
    for (int x = e.source.x; x < e.source.x + e.length; ++x)
      positions.push_back({x, e.source.y});
  }
  return positions;
}

void OrthoAreaOptimizer::_clear_structures() {
  // Clear tables
  for (int i = 0; i < _width; ++i) {
    for (int j = 0; j < _height; ++j) {
      for (int k = 0; k < 4; ++k) {
        _edge_tables[i][j][k] = nullptr;
      }
      _table[i][j] = -1;
    }
  }

  // Clear regions
  for (Region &r : _regions) {
    r.area = 0;
    r.cell_sum = {0, 0};
    assert(r.edge_list.empty());
  }
}

void OrthoAreaOptimizer::run_iteration() {
  if (not _converged) {
    vector<Vector2D> old_sources = get_sources();
    _clear_structures();
    _fill_areas();
    _correct_centroids();
    if (old_sources == get_sources())
      _converged = true;
  }
}

RegionIndices OrthoAreaOptimizer::get_table() { return _table; }

bool OrthoAreaOptimizer::is_converged() { return _converged; }
