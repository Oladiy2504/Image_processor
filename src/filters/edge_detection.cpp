#include <cstddef>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <filters/matrix_filter.h>
#include <filters/registry.h>
#include <image/color.h>

namespace image_processor::filters {

namespace {
const float ED_MATRIX_CENTER = 4.0f;

float CastParams(std::string_view s) {
    std::stringstream sstream(s.data());
    float result = -1;
    sstream >> result;
    return result;
}

}  // namespace

class EdgeDetection : public MatrixFilter {
public:
    class Factory : public Filter::Factory {
    public:
        std::unique_ptr<Filter> Construct(const std::vector<std::string>& parameters) override {
            if (parameters.size() != 1) {
                throw std::invalid_argument("Edge Detection filter expects only one argument: -edge [THRESHOLD]");
            }
            const float threshold = CastParams(parameters[0]);
            if (threshold == -1) {
                throw std::runtime_error("Incorrect edge detection params");
            }
            return std::make_unique<EdgeDetection>(threshold);
        }
    };

public:
    explicit EdgeDetection(float threshold)
        : matrix_{{0, -1, 0}, {-1, ED_MATRIX_CENTER, -1}, {0, -1, 0}}, threshold_(threshold) {
    }

    image::Image Apply(const image::Image& image) override {
        auto factory = image_processor::filters::Registry::Instance().Get("gs");
        auto filter = factory({});
        image::Image grayscale_image = filter->Apply(image);
        image::Image applied_image = {image.GetWidth(), image.GetHeight()};
        for (size_t row = 0; row < image.GetHeight(); ++row) {
            for (size_t col = 0; col < image.GetWidth(); ++col) {
                applied_image(row, col) = ApplyMatrix(grayscale_image, row, col, matrix_);
                if (applied_image(row, col).r > threshold_) {
                    applied_image(row, col) = image_processor::image::WHITE;
                } else {
                    applied_image(row, col) = image_processor::image::BLACK;
                }
            }
        }
        return applied_image;
    }

private:
    image_processor::utils::Matrix<float> matrix_;
    float threshold_;
};

REGISTER_FILTER("edge", EdgeDetection::Factory)

}  // namespace image_processor::filters