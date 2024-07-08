#ifndef __AREA_OPTIMIZER_H
#define __AREA_OPTIMIZER_H

#include <vector>
#include <string>

struct Point {
    int x, y;
};

class AreaOptimizer {
    int _width, _height, _n_sources;
    std::vector<Point> _sources;
    std::vector<float> _weights;
    std::vector<std::vector<int>> _table;
    std::vector<std::pair<int, Point>> _cell_sum;

    void _fill_areas();
    void _expand();

public:
    AreaOptimizer(int width, int height, std::vector<Point> sources, std::vector<float> weights);
    void run_iteration();
    void save_to_csv(const std::string& filename);
};

#endif