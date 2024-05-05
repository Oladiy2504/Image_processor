#include "args.h"

#include <stdexcept>
#include <string_view>
#include <vector>

namespace image_processor::args {

namespace {

bool IsValidFilterName(std::string_view name) {
    return name.size() > 2 && name.starts_with('-');
}

}  // namespace

Args::Filter::Filter(std::string_view name, std::vector<std::string>&& parameters)
    : name_(name), parameters_(std::move(parameters)) {
}

std::string_view Args::Filter::Name() const {
    return name_;
}

Args::Args(int argc, const char* argv[]) : input_file_(), output_file_(), filters_() {
    if (argc < 3) {
        throw std::runtime_error(
            "USAGE: image_processor [INPUT_FILE] [OUTPUT_FILE] [-[FILTER_1_NAME] [FILTER_1_PARAM_1] [FILTER_1_PARAM_2]"
            "...]...");
    }
    input_file_ = argv[1];
    output_file_ = argv[2];
    for (size_t i = 3; i < static_cast<size_t>(argc);) {
        std::string_view filter_name = argv[i];
        if (!IsValidFilterName(filter_name)) {
            throw std::runtime_error("Invalid filter name [" + std::string(filter_name) + "]");
        }
        ++i;
        std::vector<std::string> params;
        for (; i < static_cast<size_t>(argc) && !IsValidFilterName(argv[i]); ++i) {
            params.push_back(argv[i]);
        }
        filters_.emplace_back(filter_name.substr(1), std::move(params));
    }
}

const std::vector<std::string>& Args::Filter::Parameters() const {
    return parameters_;
}

std::string_view Args::InputFile() const {
    return input_file_;
}

std::string_view Args::OutputFile() const {
    return output_file_;
}

const std::vector<Args::Filter>& Args::Filters() const {
    return filters_;
}

}  // namespace image_processor::args