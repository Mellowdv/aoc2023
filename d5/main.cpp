#include <fstream>
#include <filesystem>
#include <iostream>
#include <vector>
#include <ranges>

using namespace std::filesystem;

struct Mapping {
    int64_t min;
    int64_t max;
    int64_t diff;
};

using MappingContainer = std::vector<Mapping>;
using Pipeline = std::vector<MappingContainer>;
using DataIt = std::string_view::iterator;

MappingContainer build_mapping_container(std::fstream &file_handle) {
    MappingContainer mc;

    std::string line {};
    while (std::getline(file_handle,line)) {
        if (!std::isdigit(*line.begin())) {
            break;
        }
        auto nums = line | std::ranges::views::split(' ') | std::ranges::views::transform([] (auto &&range) {
                return std::atoll(range.data());});
        auto iter {nums.begin()};
        int64_t dest_min {*iter};
        int64_t source_min {*(++iter)};
        int64_t range {*(++iter)};
        mc.push_back(Mapping(source_min, source_min + range, source_min - dest_min));
    }
    return mc;
}

Pipeline build_pipeline(std::fstream &file_handle) {
    Pipeline pl;

    std::string line {};
    while (std::getline(file_handle, line)) {
        if (std::isalpha(*line.begin())) {
            pl.push_back(build_mapping_container(file_handle));
        }
    }
    return pl;
}

int64_t get_next_mapped_value(MappingContainer const &mc, int64_t value_to_map) {
    for (auto const &m: mc) {
        if (value_to_map >= m.min && value_to_map < m.max) {
            int64_t mapped_val = value_to_map - m.diff;
            //std::cout << "Mapped " << value_to_map << " to " << mapped_val << "\n";
            return value_to_map - m.diff;
        }
    }
    return value_to_map;
}

int64_t get_location(Pipeline const &pl, int64_t seed_number) {
    int64_t mapped_value = seed_number;
    for (auto const &mc: pl) {
       mapped_value = get_next_mapped_value(mc, mapped_value);
    }
    return mapped_value;
}

int main() {

    path file_path {"/home/t.talik/Programming/aoc2023/d5/input.txt"};
    std::fstream file_handle {file_path};
    std::string seeds {};
    std::getline(file_handle, seeds);
    auto iter {seeds.begin()};
    while (!std::isdigit(*iter)) {
        ++iter;
    }
    auto seed_numbers = std::string_view(iter, seeds.end()) | std::views::split(' ') | std::views::transform([] (auto &&range) {
            return std::atoll(range.data()); });

    Pipeline pl {build_pipeline(file_handle)};

    std::cout << "The size of the pipe " << pl.size() << "\n";
    for (auto mc: pl) {
        std::cout << "Size of mapping " << mc.size() << "\n";
        for (auto m: mc) {
            std::cout << "Mapping: " << m.min << " " << m.max << " " << m.diff << "\n";
        }
    }

    int64_t min {INT64_MAX};
    for (auto seed: seed_numbers) {
        min = std::min(min, get_location(pl, seed));
        std::cout << min << "\n";
    }
    std::cout << "\n";

    return 0;
}
