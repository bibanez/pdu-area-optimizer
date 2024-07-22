#include "types.h"

bool Vector2D::operator==(const Vector2D &other) const {
  return (x == other.x) and (y == other.y);
}

Vector2D Vector2D::operator+(const Vector2D &other) const {
  return Vector2D{x + other.x, y + other.y};
}

Vector2D Vector2D::operator-(const Vector2D &other) const {
  return Vector2D{x - other.x, y - other.y};
}

Vector2D Vector2D::operator*(const Vector2D &other) const {
  return Vector2D{x * other.x, y * other.y};
}

Vector2D Vector2D::operator+(const Direction dir) const {
  Vector2D result = *this;
  switch (dir) {
  case Direction::UP:
    result.y++;
    break;
  case Direction::DOWN:
    result.y--;
    break;
  case Direction::LEFT:
    result.x--;
    break;
  case Direction::RIGHT:
    result.x++;
    break;
  }
  return result;
}

Vector2D &Vector2D::operator+=(const Vector2D &other) {
  x += other.x;
  y += other.y;
  return *this;
}

Vector2D &Vector2D::operator-=(const Vector2D &other) {
  x -= other.x;
  y -= other.y;
  return *this;
}

Vector2D &Vector2D::operator*=(const int n) {
  x *= n;
  y *= n;
  return *this;
}

Vector2D &Vector2D::operator+=(const Direction dir) {
  switch (dir) {
  case Direction::UP:
    y++;
    break;
  case Direction::DOWN:
    y--;
    break;
  case Direction::LEFT:
    x--;
    break;
  case Direction::RIGHT:
    x++;
    break;
  }
  return *this;
}

Direction Edge::normal() const { return (Direction)((dir + 3) % 4); }

Vector2D Edge::end() const {
  Vector2D result = source;
  switch (dir) {
  case Direction::UP:
    result.y += length - 1;
    break;
  case Direction::DOWN:
    result.y -= length - 1;
    break;
  case Direction::LEFT:
    result.x -= length - 1;
    break;
  case Direction::RIGHT:
    result.x += length - 1;
    break;
  }
  return result;
}
