#include <chrono>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <thread>

static const double PI = std::atan(1) * 4;

// UV resolution of torus
constexpr uint16_t kMaxU = 100;
constexpr uint16_t kMaxV = 100;

constexpr double kMainRadius = 2;
constexpr double kThicknessRadius = 1;

// Z coordinate of the center
constexpr double kDistance = 6;

// Rotation speeds in radians per second
constexpr double kXSpeed = 0.3;
constexpr double kYSpeed = -0.2;

// Screen size
constexpr uint8_t kWidth = 80;
constexpr uint8_t kHeight = 22;

// The width to height ratio of a console character
constexpr double kCharacterSizeRatio = 0.5;

struct Point {
  Point operator - (const Point& rhs) const;
  Point operator / (double rhs) const;
  Point Normalize() const;
  double Dot(const Point& rhs) const;
  Point Rotate(double ax, double ay) const;
  double x, y, z;
};

Point Point::operator - (const Point& rhs) const {
  return {
    .x = x - rhs.x,
    .y = y - rhs.y,
    .z = z - rhs.z,
  };
}

Point Point::operator / (const double rhs) const {
  return {
    .x = x / rhs,
    .y = y / rhs,
    .z = z / rhs,
  };
}

Point Point::Normalize() const {
  return *this / std::sqrt(x * x + y * y + z * z);
}

double Point::Dot(const Point& rhs) const {
  return x * rhs.x + y * rhs.y + z * rhs.z;
}

Point Point::Rotate(const double ax, const double ay) const {
  const double sax = std::sin(ax);
  const double cax = std::cos(ax);
  const Point p{
    .x = x,
    .y = z * sax + y * cax,
    .z = z * cax - y * sax,
  };
  const double say = std::sin(ay);
  const double cay = std::cos(ay);
  return {
    .x = p.x * cay - p.z * say,
    .y = p.y,
    .z = p.x * say + p.z * cay,
  };
}

// Directional light
const Point kLight{
  .x = -1,
  .y = -1,
  .z = 2,
};

int main() {
  constexpr double kXDelta = kXSpeed / 3;
  constexpr double kYDelta = kYSpeed / 3;
  constexpr double kHalfWidth = kWidth / 2.0;
  constexpr double kHalfHeight = kHeight / 2.0;
  const Point normalized_light = kLight.Normalize();
  std::cout << "\x1b[2J" << std::endl;
  double ax = 0, ay = 0;
  while (true) {
    char screen[kHeight][kWidth];
    double z_buffer[kHeight][kWidth];
    for (int i = 0; i < kHeight; ++i) {
      for (int j = 0; j < kWidth; ++j) {
        screen[i][j] = 32;
        z_buffer[i][j] = 0;
      }
    }
    std::cout << "\x1b[HHere, have a doughnut:" << std::endl;
    for (int u = 0; u < kMaxU; ++u) {
      const double a1 = PI * 2.0 * (double)u / (double)kMaxU;
      const double sa1 = std::sin(a1);
      const double ca1 = std::cos(a1);
      for (int v = 0; v < kMaxV; ++v) {
        const double a2 = PI * 2.0 * (double)v / (double)kMaxV;
        const double sa2 = std::sin(a2);
        const double ca2 = std::cos(a2);
        const Point p{
          .x = ca1 * (kMainRadius + ca2 * kThicknessRadius),
          .y = sa2 * kThicknessRadius,
          .z = sa1 * (kMainRadius + ca2 * kThicknessRadius),
        };
        const Point q = p.Rotate(ax, ay);
        const double z = 1 / (kDistance + q.z);
        const double x = q.x * kHalfWidth * z;
        const double y = q.y * kCharacterSizeRatio * kHalfWidth * z;
        const int i = std::round(kHalfHeight - y);
        const int j = std::round(x + kHalfWidth);
        if (i >= 0 && i < kHeight &&
            j >= 0 && j < kWidth &&
            z > z_buffer[i][j])
        {
          const Point c{
            .x = ca1 * kMainRadius,
            .y = 0,
            .z = sa1 * kMainRadius,
          };
          const Point n = (q - c.Rotate(ax, ay)) / kThicknessRadius;
          const double brightness = std::acos(n.Dot(normalized_light)) / PI;
          screen[i][j] = ".,-~:;=!*#$@@"[(int)std::floor(brightness * 12)];
          z_buffer[i][j] = z;
        }
      }
    }
    for (int i = 0; i < kHeight; ++i) {
      std::cout.write(screen[i], kWidth);
      std::cout.put('\n');
    }
    std::cout << std::flush;
    ax += kXDelta;
    ay += kYDelta;
    std::this_thread::sleep_for(std::chrono::milliseconds(33));
  }
}
