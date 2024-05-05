#include "bmp.h"

#include <io/writer.h>
#include <image/image.h>
#include <io/reader.h>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>

namespace image_processor::image_format {

namespace {

constexpr uint32_t KDefaultOffset = 54;
constexpr uint32_t KDefaultHeaderSize = 40;
constexpr uint16_t KDefaultBitsPerPixel = 24;
constexpr int32_t KDefaultHorizontal = 11811;
constexpr int32_t KDefaultVertical = 11811;

struct FileHeader {
    char header_field[2] = {'B', 'M'};
    uint32_t file_size = 0;
    uint16_t reserved[2] = {0, 0};
    uint32_t offset = KDefaultOffset;
};

struct BitmapInfoHeader {
    uint32_t header_size = KDefaultHeaderSize;

    struct Size {
        int32_t width = 0;
        int32_t height = 0;
    } size = {};

    uint16_t color_planes = 1;
    uint16_t bits_per_pixel = KDefaultBitsPerPixel;
    uint32_t compression_method = 0;
    uint32_t bitmap_data_size = 0;

    struct Resolution {
        int32_t horizontal = KDefaultHorizontal;
        int32_t vertical = KDefaultVertical;
    } resolution = {};

    struct Colors {
        uint32_t total = 0;
        uint32_t important = 0;
    } colors = {};
};

struct Color24bits {
    uint8_t b;
    uint8_t g;
    uint8_t r;
    static constexpr size_t KBitsPerPixel = sizeof(uint8_t) * 3;
};

size_t GetRowSize(size_t image_width) {
    return (Color24bits::KBitsPerPixel * image_width + (sizeof(uint32_t) - 1)) / sizeof(uint32_t) * 4;
}

FileHeader LoadFileHeader(io::Reader& reader) {
    auto file_header = FileHeader{};
    reader >> file_header.header_field[0] >> file_header.header_field[1] >> file_header.file_size >>
        file_header.reserved[0] >> file_header.reserved[1] >> file_header.offset;
    if (file_header.header_field[0] != 'B' || file_header.header_field[1] != 'M') {
        throw std::runtime_error("Unsupported file format");
    } else if (file_header.file_size < file_header.offset) {
        throw std::runtime_error("File size must be greater than offset");
    }
    return file_header;
}

BitmapInfoHeader LoadInfoHeader(io::Reader& reader) {
    BitmapInfoHeader info_header;
    reader >> info_header.header_size >> info_header.size.width >> info_header.size.height >>
        info_header.color_planes >> info_header.bits_per_pixel >> info_header.compression_method >>
        info_header.bitmap_data_size >> info_header.resolution.horizontal >> info_header.resolution.vertical >>
        info_header.colors.total >> info_header.colors.important;
    if (info_header.size.height < 0) {
        info_header.size.height = -info_header.size.height;
    }
    if (info_header.color_planes != 1 || info_header.compression_method != 0 ||
        (info_header.size.width * info_header.size.width * Color24bits::KBitsPerPixel != info_header.bitmap_data_size &&
         info_header.bitmap_data_size != 0)) {
        throw std::runtime_error("Invalid info header");
    }
    return info_header;
}

void LoadRowImage(io::Reader& reader, image::Image& image, size_t row) {
    for (size_t col = 0; col < image.GetWidth(); ++col) {
        Color24bits pixel_data{};
        reader >> pixel_data.b >> pixel_data.g >> pixel_data.r;
        image(row, col) = image::Color{
            .r = static_cast<float>(pixel_data.r) / std::numeric_limits<uint8_t>::max(),
            .g = static_cast<float>(pixel_data.g) / std::numeric_limits<uint8_t>::max(),
            .b = static_cast<float>(pixel_data.b) / std::numeric_limits<uint8_t>::max(),
        };
    }
    size_t bytes_read = (Color24bits::KBitsPerPixel / sizeof(uint8_t)) * image.GetWidth();
    size_t row_size = GetRowSize(image.GetWidth());
    for (; bytes_read < row_size; ++bytes_read) {
        uint8_t dummy = 0;
        reader >> dummy;
    }
}

image::Image LoadImage(io::Reader& reader, const BitmapInfoHeader& info_header) {
    image::Image image{static_cast<size_t>(info_header.size.width), static_cast<size_t>(info_header.size.height)};
    for (size_t row = info_header.size.height - 1; row != std::numeric_limits<size_t>::max(); --row) {
        LoadRowImage(reader, image, row);
    }
    return image;
}

void SaveFileHeader(io::Writer& writer, const FileHeader& file_header) {
    writer << file_header.header_field[0] << file_header.header_field[1] << file_header.file_size
           << file_header.reserved[0] << file_header.reserved[1] << file_header.offset;
}

void SaveInfoHeader(io::Writer& writer, const BitmapInfoHeader info_header) {
    writer << info_header.header_size << info_header.size.width << info_header.size.height << info_header.color_planes
           << info_header.bits_per_pixel << info_header.compression_method << info_header.bitmap_data_size
           << info_header.resolution.horizontal << info_header.resolution.vertical << info_header.colors.total
           << info_header.colors.important;
}

void SaveRowImage(io::Writer& writer, const image::Image& image, size_t row) {
    for (size_t col = 0; col < image.GetWidth(); ++col) {
        const auto lower = 0.0f;
        const float upper = std::numeric_limits<uint8_t>::max();
        image::Color pixel = image(row, col) * upper;
        Color24bits pixel_data{
            .b = static_cast<uint8_t>(std::clamp(pixel.b, lower, upper)),
            .g = static_cast<uint8_t>(std::clamp(pixel.g, lower, upper)),
            .r = static_cast<uint8_t>(std::clamp(pixel.r, lower, upper)),
        };
        writer << pixel_data.b << pixel_data.g << pixel_data.r;
    }
    size_t bytes_written = (Color24bits::KBitsPerPixel / sizeof(uint8_t)) * image.GetWidth();
    size_t row_size = GetRowSize(image.GetWidth());
    for (; bytes_written < row_size; ++bytes_written) {
        uint8_t dummy{};
        writer << dummy;
    }
}

void SaveImage(io::Writer& writer, const image::Image& image) {
    for (size_t row = image.GetHeight() - 1; row != std::numeric_limits<size_t>::max(); --row) {
        SaveRowImage(writer, image, row);
    }
}

}  // namespace

image::Image BMP::Load(std::string_view filename) {
    io::Reader reader(filename);
    LoadFileHeader(reader);
    const auto info_header = LoadInfoHeader(reader);
    return LoadImage(reader, info_header);
}

void BMP::Save(const image::Image& image, std::string_view filename) {
    io::Writer writer(filename);
    FileHeader file_header{};
    BitmapInfoHeader info_header{};
    info_header.bitmap_data_size = GetRowSize(image.GetWidth() * image.GetHeight());
    info_header.size.width = static_cast<int32_t>(image.GetWidth());
    info_header.size.height = static_cast<int32_t>(image.GetHeight());
    file_header.file_size = info_header.bitmap_data_size + file_header.offset;
    SaveFileHeader(writer, file_header);
    SaveInfoHeader(writer, info_header);
    SaveImage(writer, image);
}

}  // namespace image_processor::image_format