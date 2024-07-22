#include "ortho_area_optimizer.h"
#include "types.h"
#include <chrono>
#include <iomanip>
#include <iostream>
#include <matplot/matplot.h>
#include <matplot/util/keywords.h>
#include <sstream>
#include <thread>
using namespace std;
using namespace matplot;

void save_iteration_as_image(AreaOptimizer &optimizer, int iteration,
                             const std::string &identifier) {
  // Create a new figure
  auto f = figure(true);
  f->size(800, 600);

  // Get the data from the optimizer
  std::vector<std::vector<int>> data = optimizer.get_table();

  // Debug: print size of the data
  std::cout << "Data size: " << data.size() << " x "
            << (data.empty() ? 0 : data[0].size()) << std::endl;

  // Check if data is valid
  if (data.empty() || data[0].empty()) {
    std::cerr << "Error: Data is empty" << std::endl;
    return;
  }

  // Plot using the data
  try {
    auto im = imagesc(data);
    gca()->color_box_range(0, optimizer.get_weights().size() - 1);
    colorbar();
  } catch (const std::exception &e) {
    std::cerr << "Error during plotting: " << e.what() << std::endl;
    return;
  }

  // Get the sources before running the iteration
  std::vector<Vector2D> sources_before = optimizer.get_sources();

  // Run the iteration
  optimizer.run_iteration();

  // Get the sources after running the iteration
  std::vector<Vector2D> sources_after = optimizer.get_sources();

  // Plot the sources before and after with scatter plots and connect them with
  // arrows
  hold(on);
  vector<double> source_before_x, source_before_y;
  for (size_t i = 0; i < sources_before.size(); ++i) {
    source_before_x.push_back(sources_before[i].x);
    source_before_y.push_back(sources_before[i].y);
  }
  scatter(source_before_y, source_before_x);
  vector<double> source_after_x, source_after_y;
  for (size_t i = 0; i < sources_after.size(); ++i) {
    source_after_x.push_back(sources_after[i].x);
    source_after_y.push_back(sources_after[i].y);
  }
  scatter(source_after_y, source_after_x);

  for (size_t i = 0; i < sources_before.size(); ++i) {
    double x_before = sources_before[i].x;
    double y_before = sources_before[i].y;
    double x_after = sources_after[i].x;
    double y_after = sources_after[i].y;

    // Draw an arrow from the source before to the source after
    auto arrow = gca()->arrow(y_before, x_before, y_after, x_after);
    arrow->color("k");
    arrow->line_width(1.5);
  }

  // Set title
  std::stringstream ss;
  ss << identifier << " - Iteration " << std::setw(4) << std::setfill('0')
     << iteration;
  title(ss.str());

  // Save the figure
  ss.str("");
  ss << identifier << '_' << std::setw(4) << std::setfill('0') << iteration
     << ".png";
  save(ss.str());
}

enum SOURCES { FEW, MANY };
enum LAYOUT { STRICT, RANDOM };
enum WEIGHTS { SAME, DIFFERENT };

string get_name(SOURCES s, LAYOUT l, WEIGHTS w) {
  string name = "";
  if (s == FEW)
    name += "f";
  else
    name += "m";
  if (l == STRICT)
    name += "s";
  else
    name += "r";
  if (w == SAME)
    name += "s";
  else
    name += "d";
  return name;
}

void run_test(int n, int num_iterations, SOURCES s, LAYOUT l, WEIGHTS w) {
  string name = get_name(s, l, w);
  cout << "Running test " << name << "..." << endl;
  vector<Vector2D> sources;
  vector<double> weights;
  if (s == FEW) {
    for (int i = 0; i < 2; ++i) {
      for (int j = 0; j < 2; ++j) {
        if (l == STRICT)
          sources.push_back({i * (n / 2) + n / 4, j * (n / 2) + n / 4});
        else if (l == RANDOM)
          sources.push_back({rand() % n, rand() % n});
      }
    }
  } else {
    for (int i = 0; i < 4; ++i) {
      for (int j = 0; j < 4; ++j) {
        if (l == STRICT)
          sources.push_back({i * (n / 4) + n / 8, j * (n / 4) + n / 8});
        else if (l == RANDOM)
          sources.push_back({rand() % n, rand() % n});
      }
    }
  }
  if (w == SAME)
    weights = vector<double>(sources.size(), 1);
  else {
    weights = vector<double>(sources.size());
    for (unsigned int i = 0; i < sources.size(); ++i) {
      weights[i] = 1 + rand() % 9;
    }
  }

  OrthoAreaOptimizer optimizer(n, n, n/10, sources, weights);
  for (int i = 0; not optimizer.is_converged() and i < num_iterations; ++i) {
    optimizer.run_iteration();
    save_iteration_as_image(optimizer, i, name);
  }

  // We wait because otherwise it won't read the files
  this_thread::sleep_for(chrono::seconds(1));

  // Commands for generating palette, GIF, and MP4
  ostringstream cmd_palette, cmd_gif, cmd_mp4;

  // Generate palette
  cmd_palette << "ffmpeg -framerate 10 -i " << name
              << "_%04d.png -vf \"scale=640:-1:flags=lanczos,palettegen\" -y "
                 "palette.png";

  // Generate GIF
  cmd_gif << "ffmpeg -framerate 10 -i " << name
          << "_%04d.png -i palette.png -lavfi "
             "\"scale=640:-1:flags=lanczos[x];[x][1:v]paletteuse\" -y "
          << name << ".gif";

  // Generate MP4
  cmd_mp4 << "ffmpeg -framerate 10 -i " << name
          << "_%04d.png -c:v libx264 -vf \"scale=640:-1\" -pix_fmt yuv420p -y "
          << name << ".mp4";

  // Execute commands
  cout << "Generating palette..." << endl;
  int result_palette = system(cmd_palette.str().c_str());
  if (result_palette == 0) {
    cout << "Successfully generated palette." << endl;

    cout << "Generating GIF..." << endl;
    int result_gif = system(cmd_gif.str().c_str());
    if (result_gif == 0) {
      cout << "Successfully created GIF: " << name << ".gif" << endl;
    } else {
      cerr << "Error creating GIF. FFmpeg returned: " << result_gif << endl;
    }
  } else {
    cerr << "Error generating palette. FFmpeg returned: " << result_palette
         << endl;
  }

  cout << "Generating MP4..." << endl;
  int result_mp4 = system(cmd_mp4.str().c_str());
  if (result_mp4 == 0) {
    cout << "Successfully created MP4: " << name << ".mp4" << endl;
  } else {
    cerr << "Error creating MP4. FFmpeg returned: " << result_mp4 << endl;
  }
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    cerr << "Usage: " << argv[0] << " <number_of_iterations>" << endl;
    return 1;
  }

  int num_iterations = stoi(argv[1]);

  cout << "Start running with " << num_iterations << " iterations..." << endl;

  int n = 256;
  for (int i = 0; i < 2; ++i)
    for (int j = 0; j < 2; ++j)
      for (int k = 0; k < 2; ++k)
        run_test(n, num_iterations, SOURCES(i), LAYOUT(j), WEIGHTS(k));
}

/*
int main() {
  int n = 32;
  AreaOptimizer optimizer(n, n, n/16, {{0,0}, {n-1,n-1}},
                          {1, 3});
  optimizer.run_iteration();
  //print_colored_table(optimizer.get_table());
  save_iteration_as_image(optimizer, 1, "test");
}
*/
