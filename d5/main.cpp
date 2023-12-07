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

struct Range {
    int64_t min;
    int64_t max;
};

using Ranges = std::vector<Range>;
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

Ranges get_next_mapped_ranges(MappingContainer const &mc, Ranges &ranges_to_map) {
    for (auto range_to_map: ranges_to_map) { 
        for (auto const &m: mc) {
            if (range_to_map.min >= m.min && range_to_map.max < m.max) {
                range_to_map = Range(range_to_map.min - m.diff, range_to_map.max - m.diff);
                break;
            } else if (range_to_map.min >= m.min && range_to_map.max >= m.max) {

            }
        }
    }
}

Ranges get_location_range(Pipeline const &pl, Ranges &seed_ranges) {
    Ranges mapped_ranges = seed_ranges;
    for (auto const &mc: pl) {
        mapped_ranges = get_next_mapped_ranges(mc, mapped_ranges);
    }
    return mapped_ranges;
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

    // Part 1
    int64_t min {INT64_MAX};
    for (auto seed: seed_numbers) {
        min = std::min(min, get_location(pl, seed));
    }
    std::cout << min << "\n";

    // Part 2
    min = INT64_MAX;
    bool check_range {false};
    int64_t current_seed;
    for (auto seed: seed_numbers) {
        if (check_range) {
            std::cout << "Checking range " << seed << "\n";
            for (int64_t range = seed; range > 0; --range) {
                min = std::min(min, get_location(pl, current_seed + range));
            }
            check_range = false;
        } else {
            current_seed = seed;
            check_range = true;
        }
    }
    std::cout << min << "\n";

    return 0;
}
