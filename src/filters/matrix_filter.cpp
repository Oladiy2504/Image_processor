#include "matrix_filter.h"

#include <algorithm>

namespace image_processor::filters {

std::vector<size_t> MakeRowBounds(const image::Image& image, size_t row) {
    std::vector<size_t> row_bounds{row, row, row + 1};
    if (row == image.GetHeight() - 1) {
        row_bounds[2] = row;
    }
    if (row != 0) {
        row_bounds[0] = row - 1;
    }
    return row_bounds;
}

std::vector<size_t> MakeColBounds(const image::Image& image, size_t col) {
    std::vector<size_t> col_bounds{col, col, col + 1};
    if (col == image.GetWidth() - 1) {
        col_bounds[2] = col;
    }
    if (col != 0) {
        col_bounds[0] = col - 1;
    }
    return col_bounds;
}

image::Color ApplyMatrix(const image::Image& image, size_t row, size_t col,
                         const image_processor::utils::Matrix<float>& matrix) {
    image::Color applied_color;
    std::vector<size_t> row_bounds = MakeRowBounds(image, row);
    std::vector<size_t> col_bounds = MakeColBounds(image, col);
    for (size_t i = 0; i < matrix.GetHeight(); ++i) {
        for (size_t j = 0; j < matrix.GetWidth(); ++j) {
            auto pixel = image(row_bounds[i], col_bounds[j]);
            applied_color = applied_color + matrix(i, j) * pixel;
        }
    }
    applied_color.r = std::min(1.0f, std::max(0.0f, applied_color.r));
    applied_color.g = std::min(1.0f, std::max(0.0f, applied_color.g));
    applied_color.b = std::min(1.0f, std::max(0.0f, applied_color.b));
    return applied_color;
}

}  // namespace image_processor::filters