#ifndef __AREA_OPTIMIZER_H
#define __AREA_OPTIMIZER_H

#include <vector>

struct Point {
    int x, y;

    // operator for comparison
    bool operator==(const Point &p) const {
        return x == p.x && y == p.y;
    }
};

class AreaOptimizer {
    int _width, _height, _n_sources;
    std::vector<Point> _sources;
    std::vector<float> _weights;
    std::vector<std::vector<int>> _table;

    bool _converged;

    void _fill_areas();
    void _expand();

public:
    AreaOptimizer(int width, int height, std::vector<Point> sources, std::vector<float> weights);
    void run_iteration();
    std::vector<int> get_areas();
    std::vector<float> get_weights();
    std::vector<Point> get_sources();
    std::vector<std::vector<int>> get_table();
    bool converged();
};

#endif
