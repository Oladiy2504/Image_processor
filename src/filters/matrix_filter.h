#include <cstddef>
#include <vector>

#include <filters/registry.h>
#include <image/color.h>
#include <image/image.h>
#include <utils/matrix.h>

namespace image_processor::filters {

image::Color ApplyMatrix(const image::Image& image, size_t row, size_t col,
                         const image_processor::utils::Matrix<float>& matrix);
std::vector<size_t> MakeRowBounds(const image::Image& image, size_t row);
std::vector<size_t> MakeColBounds(const image::Image& image, size_t col);

class MatrixFilter : public Filter {
public:
    MatrixFilter() : matrix_{3, 3, 0} {
    }

protected:
    image_processor::utils::Matrix<float> matrix_;
};

}  // namespace image_processor::filters