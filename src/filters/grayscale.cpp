#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include <filters/registry.h>
#include <image/color.h>

namespace image_processor::filters {

namespace {
const float RED_TO_GRAY = 0.299f;
const float GREEN_TO_GRAY = 0.587f;
const float BLUE_TO_GRAY = 0.114f;
}  // namespace

class GrayScale : public Filter {
public:
    class Factory : public Filter::Factory {
    public:
        std::unique_ptr<Filter> Construct(const std::vector<std::string>& parameters) override {
            if (!parameters.empty()) {
                throw std::invalid_argument("Grayscale filter doesn't expect any arguments");
            }
            return std::make_unique<GrayScale>();
        }
    };

public:
    image::Image Apply(const image::Image& image) override {
        image::Image applied_image{image.GetWidth(), image.GetHeight()};
        for (size_t row = 0; row < image.GetHeight(); ++row) {
            for (size_t col = 0; col < image.GetWidth(); ++col) {
                auto pixel = image(row, col);
                float color = RED_TO_GRAY * pixel.r + GREEN_TO_GRAY * pixel.g + BLUE_TO_GRAY * pixel.b;
                applied_image(row, col) = {color, color, color};
            }
        }
        return applied_image;
    }
};

REGISTER_FILTER("gs", GrayScale::Factory)

}  // namespace image_processor::filters