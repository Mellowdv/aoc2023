#include <algorithm>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <numeric>
#include <vector>

using namespace std::filesystem;

struct Race {
    int64_t time;
    int64_t distance;
};

struct Range {
    int64_t from;
    int64_t to;
};

Range find_winning_range(Race const &r) {
    Range range {};

    int64_t calculated_distance {};
    int64_t current_speed {};
    int64_t remaining_time {};
    for (int64_t t = 0; t <= r.time; ++t) {
        remaining_time = r.time - t;
        current_speed = t;
        calculated_distance = current_speed * remaining_time;
        if (calculated_distance > r.distance) {
            range.from = t;
            break;
        }
    }

    for (int64_t t = r.time; t > 0; --t) {
        remaining_time = r.time - t;
        current_speed = t;
        calculated_distance = current_speed * remaining_time;
        if (calculated_distance > r.distance) {
            range.to = t;
            break;
        }
    }

    return range;
}

int main() {
    
    path file_path {"/home/ttalik/tmp/cpp/aoc2023/d6/input.txt"};
    std::fstream file_handle {file_path};
    std::vector<Race> races {};
    for (int i = 0; i < 4; ++i) races.push_back(Race());

    std::string line {};
    std::getline(file_handle, line);
    std::sscanf(line.data(), "%*s %ld %ld %ld %ld", &races.at(0).time, &races.at(1).time, &races.at(2).time, &races.at(3).time);
    std::getline(file_handle, line);
    std::sscanf(line.data(), "%*s %ld %ld %ld %ld", &races.at(0).distance, &races.at(1).distance, &races.at(2).distance, &races.at(3).distance);

    std::vector<Range> ranges {};
    ranges.resize(4);
    std::transform(races.begin(), races.end(), ranges.begin(), find_winning_range);

    for (auto range: ranges) {
        std::cout << range.from << " " << range.to << "\n";
    }
    auto result = std::transform_reduce(ranges.begin(), ranges.end(), 1, std::multiplies{}, [] (auto range) { return range.to - range.from + 1; });
    std::cout << result << std::endl;

    Race longest = { .time = 49'877'895, .distance = 356'137'815'021'882 };
    auto longest_range = find_winning_range(longest);
    std::cout << longest_range.to - longest_range.from + 1 << std::endl;

    return 0;
}
