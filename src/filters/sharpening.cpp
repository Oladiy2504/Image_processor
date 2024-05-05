#include <cstddef>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include <filters/matrix_filter.h>
#include <filters/registry.h>
#include <image/color.h>

namespace image_processor::filters {

namespace {
const float SHARPENING_MATRIX_CENTER = 5.0f;
}  // namespace

class Sharpening : public MatrixFilter {
public:
    class Factory : public Filter::Factory {
    public:
        std::unique_ptr<Filter> Construct(const std::vector<std::string>& parameters) override {
            if (!parameters.empty()) {
                throw std::invalid_argument("Sharpening filter doesn't expect any arguments");
            }
            return std::make_unique<Sharpening>();
        }
    };

public:
    Sharpening() : matrix_{{0, -1, 0}, {-1, SHARPENING_MATRIX_CENTER, -1}, {0, -1, 0}} {
    }

    image::Image Apply(const image::Image& image) override {
        image::Image applied_image{image.GetWidth(), image.GetHeight()};
        for (size_t row = 0; row < image.GetHeight(); ++row) {
            for (size_t col = 0; col < image.GetWidth(); ++col) {
                applied_image(row, col) = ApplyMatrix(image, row, col, matrix_);
            }
        }
        return applied_image;
    }

private:
    image_processor::utils::Matrix<float> matrix_;
};

REGISTER_FILTER("sharp", Sharpening::Factory)

}  // namespace image_processor::filters