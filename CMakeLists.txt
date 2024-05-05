include_directories(src)

set(CMAKE_CXX_STANDARD 20)

add_executable(
    image_processor
    image_processor.cpp

    src/io/reader.cpp
    src/io/writer.cpp

    src/utils/matrix.cpp

    src/image/color.cpp
    src/image/image.cpp

    src/args/args.cpp

    src/image_format/format.cpp
    src/image_format/bmp.cpp

    src/filters/registry.cpp
    src/filters/filter.cpp
    src/filters/crop.cpp
    src/filters/grayscale.cpp
    src/filters/negative.cpp
    src/filters/matrix_filter.cpp
    src/filters/sharpening.cpp
    src/filters/edge_detection.cpp
)
