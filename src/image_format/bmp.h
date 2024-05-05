#pragma once

#include <image/image.h>
#include "image_format/format.h"

namespace image_processor::image_format {

class BMP : public ImageFormat {
public:
    image::Image Load(std::string_view filename) override;
    void Save(const image::Image& image, std::string_view filename) override;
};

}  // namespace image_processor::image_format