#include "area_optimizer.h"
#include <iostream>
#include <iomanip>
#include <sstream>
using namespace std;

void save_iteration_to_csv(AreaOptimizer& optimizer, int iteration) {
    stringstream ss;
    ss << "csv_files/output_" << setw(4) << setfill('0') << iteration << ".csv";
    optimizer.save_to_csv(ss.str());
}

int main() {
    cout << "Start running..." << endl;

    int n = 1024;
    vector<Point> sources;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            sources.push_back({rand()%n, rand()%n});
        }
    }
    vector<float> weights(16, 1);

    AreaOptimizer optimizer(n, n, sources, weights);
    for (int i = 0; i < 10000; ++i) {
        optimizer.run_iteration();
        if (i % 100 == 0) {
            save_iteration_to_csv(optimizer, i);
        }
    }
    optimizer.save_to_csv("output_final.csv");
    cout << "Done!" << endl;
}
