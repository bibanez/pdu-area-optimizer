#include "area_optimizer.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
using namespace std;

void save_iteration_to_csv(AreaOptimizer& optimizer, int iteration) {
    stringstream ss;
    ss << "csv_files/output_" << setw(4) << setfill('0') << iteration << ".csv";
    optimizer.save_to_csv(ss.str());
}

void save_stats_to_csv(const vector<float> &weight, const vector<int> &area, const string &filename) {
    if (weight.size() != area.size()) {
        cerr << "Error: weight and area vectors must have the same size." << endl;
        return;
    }

    float weight_sum = 0;
    for (float w : weight) weight_sum += w;
    float area_sum = 0;
    for (int a : area) area_sum += a;
    
    ofstream outfile(filename);
    if (!outfile.is_open()) {
        cerr << "Error: Unable to open file " << filename << endl;
        return;
    }

    // Write CSV header
    outfile << "Source,w/weight_sum,a/area_sum" << endl;

    // Write data for each source
    for (size_t i = 0; i < weight.size(); ++i) {
        outfile << i << ","
                << fixed << setprecision(6) << weight[i] / weight_sum << ","
                << fixed << setprecision(6) << static_cast<float>(area[i]) / area_sum << endl;
    }

    outfile.close();
    cout << "Stats saved to " << filename << endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <number_of_iterations>" << endl;
        return 1;
    }
    
    int num_iterations = stoi(argv[1]);
    
    cout << "Start running with " << num_iterations << " iterations..." << endl;

    int n = 256;
    // Few sources same weight strict layout
    {
        vector<Point> sources;
        for (int i = 0; i < 2; ++i) {
            for (int j = 0; j < 2; ++j) {
                sources.push_back({i*(n/2)+n/4, j*(n/2)+n/4});
            }
        }
        vector<float> weights(4, 1);

        AreaOptimizer optimizer(n, n, sources, weights);
        for (int i = 0; i < num_iterations; ++i) {
            optimizer.run_iteration();
        }
        optimizer.save_to_csv("table_fss.csv");
        save_stats_to_csv(weights, optimizer.get_areas(), "stats_fss.csv");
        cout << "Done! fss" << endl;
    }

    // Few sources same weight random layout
    {
        vector<Point> sources;
        for (int i = 0; i < 2; ++i) {
            for (int j = 0; j < 2; ++j) {
                sources.push_back({rand()%n, rand()%n});
            }
        }
        vector<float> weights(4, 1);

        AreaOptimizer optimizer(n, n, sources, weights);
        for (int i = 0; i < num_iterations; ++i) {
            optimizer.run_iteration();
        }
        optimizer.save_to_csv("table_fsr.csv");
        save_stats_to_csv(weights, optimizer.get_areas(), "stats_fsr.csv");
        cout << "Done! fsr" << endl;
    }
    
    // Few sources different weight strict layout
    {
        vector<Point> sources;
        for (int i = 0; i < 2; ++i) {
            for (int j = 0; j < 2; ++j) {
                sources.push_back({i*(n/2)+n/4, j*(n/2)+n/4});
            }
        }
        vector<float> weights(4);
        for (int i = 0; i < 4; ++i) {
            weights[i] = 1+rand()%9;
        }

        AreaOptimizer optimizer(n, n, sources, weights);
        for (int i = 0; i < num_iterations; ++i) {
            optimizer.run_iteration();
        }
        optimizer.save_to_csv("table_fds.csv");
        save_stats_to_csv(weights, optimizer.get_areas(), "stats_fds.csv");
        cout << "Done! fds" << endl;
    }

    // Few sources different weight random layout
    {
        vector<Point> sources;
        for (int i = 0; i < 2; ++i) {
            for (int j = 0; j < 2; ++j) {
                sources.push_back({rand()%n, rand()%n});
            }
        }
        vector<float> weights(4);
        for (int i = 0; i < 4; ++i) {
            weights[i] = 1+rand()%9;
        }

        AreaOptimizer optimizer(n, n, sources, weights);
        for (int i = 0; i < num_iterations; ++i) {
            optimizer.run_iteration();
        }
        optimizer.save_to_csv("table_fdr.csv");
        save_stats_to_csv(weights, optimizer.get_areas(), "stats_fdr.csv");
        cout << "Done! fdr" << endl;
    }

    // Many sources same weight strict layout
    {
        vector<Point> sources;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                sources.push_back({i*(n/4)+n/8, j*(n/4)+n/8});
            }
        }
        vector<float> weights(16, 1);

        AreaOptimizer optimizer(n, n, sources, weights);
        for (int i = 0; i < num_iterations; ++i) {
            optimizer.run_iteration();
        }
        optimizer.save_to_csv("table_mss.csv");
        save_stats_to_csv(weights, optimizer.get_areas(), "stats_mss.csv");
        cout << "Done! mss" << endl;
    }

    // Many sources same weight random layout
    {
        vector<Point> sources;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                sources.push_back({rand()%n, rand()%n});
            }
        }
        vector<float> weights(16, 1);

        AreaOptimizer optimizer(n, n, sources, weights);
        for (int i = 0; i < num_iterations; ++i) {
            optimizer.run_iteration();
        }
        optimizer.save_to_csv("table_msr.csv");
        save_stats_to_csv(weights, optimizer.get_areas(), "stats_msr.csv");
        cout << "Done! msr" << endl;
    }

    // Many sources different weight strict layout
    {
        vector<Point> sources;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                sources.push_back({i*(n/4)+n/8, j*(n/4)+n/8});
            }
        }
        vector<float> weights(16);
        for (int i = 0; i < 16; ++i) {
            weights[i] = 1+rand()%9;
        }

        AreaOptimizer optimizer(n, n, sources, weights);
        for (int i = 0; i < num_iterations; ++i) {
            optimizer.run_iteration();
        }
        optimizer.save_to_csv("table_mds.csv");
        save_stats_to_csv(weights, optimizer.get_areas(), "stats_mds.csv");
        cout << "Done! mds" << endl;
    }

    // Many sources different weight random layout
    {
        vector<Point> sources;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                sources.push_back({rand()%n, rand()%n});
            }
        }
        vector<float> weights(16);
        for (int i = 0; i < 16; ++i) {
            weights[i] = 1+rand()%9;
        }

        AreaOptimizer optimizer(n, n, sources, weights);
        for (int i = 0; i < num_iterations; ++i) {
            optimizer.run_iteration();
        }
        optimizer.save_to_csv("table_mdr.csv");
        save_stats_to_csv(weights, optimizer.get_areas(), "stats_mdr.csv");
        cout << "Done! mdr" << endl;
    }

    return 0;
}

