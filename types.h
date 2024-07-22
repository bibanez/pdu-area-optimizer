#ifndef __TYPES_H
#define __TYPES_H

#include "list.h"

#define ROTATE_CLOCKWISE(x) (Direction)((x+1)%4)
#define ROTATE_COUNTERCLOCKWISE(x) (Direction)((x+3)%4)
#define ROTATE_OPPOSITE(x) (Direction)((x+2)%4)
 
enum Direction { UP = 0, RIGHT = 1, DOWN = 2, LEFT = 3 };

struct Vector2D {
  int x, y;

  bool operator==(const Vector2D &other) const;
  Vector2D operator+(const Vector2D &other) const;
  Vector2D operator-(const Vector2D &other) const;
  Vector2D operator*(const Vector2D &other) const;
  Vector2D operator+(const Direction dir) const;
  Vector2D &operator+=(const Vector2D &other);
  Vector2D &operator-=(const Vector2D &other);
  Vector2D &operator*=(const int n);
  Vector2D &operator+=(const Direction dir);
};

struct Edge {
  Direction dir;
  Vector2D source;
  int length;

  Vector2D end() const;
  Direction normal() const;
};

/**
 *Every region is contiguous and has a weight proportional to the desired
 *area. The map of edges contains only the edges that can be expanded.
 **/
struct Region {
  Vector2D source, cell_sum;
  int area;
  double weight;
  DoubleLinkedList<Edge> edge_list;
};

#endif
