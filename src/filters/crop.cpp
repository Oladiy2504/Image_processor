#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <filters/filter.h>
#include <filters/registry.h>

namespace image_processor::filters {

namespace {

size_t CastParams(std::string_view s) {
    std::stringstream sstream(s.data());
    size_t result = 0;
    sstream >> result;
    return result;
}

}  // namespace

class Crop : public Filter {
public:
    class Factory : public Filter::Factory {
    public:
        std::unique_ptr<Filter> Construct(const std::vector<std::string>& parameters) override {
            if (parameters.size() != 2) {
                throw std::invalid_argument("Crop filter expects two arguments: -crop [WIDTH] [HEIGHT]");
            }
            const size_t width = CastParams(parameters[0]);
            const size_t height = CastParams(parameters[1]);
            if (width == 0 || height == 0) {
                throw std::runtime_error("Incorrect crop params");
            }
            return std::make_unique<Crop>(width, height);
        }
    };

public:
    Crop(size_t width, size_t height) : width_(width), height_(height) {
    }

    image::Image Apply(const image::Image& image) override {
        height_ = height_ > image.GetHeight() ? image.GetHeight() : height_;
        width_ = width_ > image.GetWidth() ? image.GetWidth() : width_;
        image::Image applied_image{width_, height_};
        for (size_t row = 0; row < height_; ++row) {
            for (size_t col = 0; col < width_; ++col) {
                applied_image(row, col) = image(row, col);
            }
        }
        return applied_image;
    }

private:
    size_t width_;
    size_t height_;
};

REGISTER_FILTER("crop", Crop::Factory)

}  // namespace image_processor::filters