#include "area_optimizer.h"

#include <queue>
#include <iostream>
#include <fstream>
using namespace std;

vector<int> dx = {0, 0, 1, -1};
vector<int> dy = {1, -1, 0, 0};

AreaOptimizer::AreaOptimizer(int width, int height, vector<Point> sources, vector<float> weights) {
    this->_width = width;
    this->_height = height;
    this->_n_sources = sources.size();
    this->_sources = sources;
    this->_weights = weights;
    this->_table = vector<vector<int>>(height, vector<int>(width, -1));
}

void AreaOptimizer::_fill_areas() {
    _cell_sum = vector<pair<int, Point>>(_n_sources, {0, {0, 0}});

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
                area -= 1/_weights[index]; // Order from small to large
                _cell_sum[index].first += 1;
                _cell_sum[index].second.x += x;
                _cell_sum[index].second.y += y;
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
    for (int i = 0; i < _n_sources; ++i) {
        auto [n, p] = _cell_sum[i];
        if (n > 0)
            _sources[i] = {p.x/n, p.y/n};
        else
            _sources[i] = {rand() % _width, rand() % _height};
        
    }
}

void AreaOptimizer::run_iteration() {
    _fill_areas();
    _expand();
}

void AreaOptimizer::save_to_csv(const string& filename) {
    // Open file
    ofstream file(filename);
    for (const auto& row : _table) {
        for (int i = 0; i < row.size(); i++) {
            file << row[i];
            if (i < row.size() - 1) {
                file << ",";
            }
        }
        file << endl;
    }
    file.close();
    cout << "Table saved to " << filename << endl;
}