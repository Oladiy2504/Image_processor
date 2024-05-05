#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include <image/color.h>
#include <filters/registry.h>

namespace image_processor::filters {

class Negative : public Filter {
public:
    class Factory : public Filter::Factory {
    public:
        std::unique_ptr<Filter> Construct(const std::vector<std::string>& parameters) override {
            if (!parameters.empty()) {
                throw std::invalid_argument("Negative filter doesn't expect any arguments");
            }
            return std::make_unique<Negative>();
        }
    };

public:
    image::Image Apply(const image::Image& image) override {
        image::Image applied_image{image.GetWidth(), image.GetHeight()};
        for (size_t row = 0; row < image.GetHeight(); ++row) {
            for (size_t col = 0; col < image.GetWidth(); ++col) {
                auto pixel = image(row, col);
                applied_image(row, col) = {1 - pixel.r, 1 - pixel.g, 1 - pixel.b};
            }
        }
        return applied_image;
    }
};

REGISTER_FILTER("neg", Negative::Factory)

}  // namespace image_processor::filters