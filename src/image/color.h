#pragma once

namespace image_processor::image {

struct Color {
    float r = 0.0f;
    float g = 0.0f;
    float b = 0.0f;
};

Color operator+(const Color& a, const Color& b);
Color operator-(const Color& a, const Color& b);
Color operator*(const Color& a, const Color& b);
Color operator*(const Color& c, float a);
Color operator*(float a, const Color& c);
Color operator/(const Color& c, float a);

namespace {

const Color BLACK = Color{0, 0, 0};
const Color WHITE = Color{1, 1, 1};

}  // namespace

}  // namespace image_processor::image