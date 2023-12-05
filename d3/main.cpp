#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>

using namespace std::filesystem;
using DataIt = std::string_view::iterator;

bool check_line(DataIt start, DataIt end, int16_t const line_length) {
    auto iter {start};

    while (iter <= end) {
        if (*iter != '.' && !std::isdigit(*iter)) {
            return true;
        }
        ++iter;
    }
    return false;
}

bool check_previous_line(std::string_view const &number, int16_t const line_length) {
    auto const range_begin {number.begin() - (line_length + 1)};
    auto const range_end {number.end() - line_length};
    
    auto iter {range_begin};

    while (iter <= range_end) {
        if (*iter != '.' && !std::isdigit(*iter)) {
            return true;
        }
        ++iter;
    }
    return false;
}

bool check_next_line(std::string_view const &number, int16_t const line_length) {
    auto const range_begin {number.begin() + (line_length - 1)};
    auto const range_end {number.end() + line_length};
    
    auto iter {range_begin};

    while (iter <= range_end) {
        if (*iter != '.' && !std::isdigit(*iter)) {
            return true;
        }
        ++iter;
    }
    return false;
}

int32_t get_sum_of_engine_parts(std::string_view const &data, int16_t const line_length) {

    auto const start_of_data {data.begin()};
    auto const end_of_data {data.end()};

    auto iter = start_of_data;

    int32_t sum {};
    for (auto iter = start_of_data; iter < end_of_data; ++iter) {
        if (std::isdigit(*iter)) {
            auto number_start {iter};
            while (std::isdigit(*iter)) {
                ++iter;
            }
            if (number_start != start_of_data && *(number_start - 1) != '.' && !std::isdigit(*iter)) {
                sum += std::atoi(number_start);
                continue;
            }
            if (*iter != '.' && !std::isdigit(*iter)) {
                sum += std::atoi(number_start);
                continue;
            }
            if ((number_start - line_length) >= start_of_data) {
                if (check_previous_line(std::string_view {number_start, iter}, line_length)) {
                    sum += std::atoi(number_start);
                }
            }
            if ((number_start + line_length) < end_of_data) {
                if (check_next_line(std::string_view {number_start, iter}, line_length)) {
                    sum += std::atoi(number_start);
                }
            }
        }

    }
    return sum;
}

DataIt find_next_gear(std::string_view const &data, DataIt iter, int16_t const line_length) {

    auto data_end {data.end()};
    while (iter != data_end && *iter != '*') {
        ++iter;
    }
    return iter;
}

void get_line_gear_result(std::vector<int64_t> &found_nums, DataIt line_iter) {
    auto get_left_num = [] (DataIt iter) {
        while (std::isdigit(*iter)) {
            --iter;
        }
        ++iter;
        return std::atoi(iter);
    };

    if (std::isdigit(*line_iter)) {
        found_nums.push_back(get_left_num(line_iter));
    } else {
        if (std::isdigit(*(line_iter - 1))) {
            found_nums.push_back(get_left_num(line_iter - 1));
        }
        if (std::isdigit(*(line_iter + 1))) {
            found_nums.push_back(std::atoi(line_iter + 1));
        }
    }
}

int32_t get_gear_product(std::string_view const &data, DataIt iter, int16_t const line_length) {

    std::vector<int64_t> found_nums {};
    get_line_gear_result(found_nums, iter);

    auto prev_line_iter = iter - line_length;
    get_line_gear_result(found_nums, prev_line_iter);

    auto next_line_iter = iter + line_length;
    get_line_gear_result(found_nums, next_line_iter);

    if (found_nums.size() < 2) {
        return 0;
    }
    if (found_nums.size() > 2) {
        return 0;
    }
    return found_nums.at(0) * found_nums.at(1);
}

int64_t get_sum_of_gear_products(std::string_view const &data, int16_t const line_length) {
    auto iter = find_next_gear(data, data.begin(), line_length);
    int64_t sum {};

    while (iter != data.end()) {
        auto gear_product = get_gear_product(data, iter, line_length);
        sum += gear_product;
        iter = find_next_gear(data, ++iter, line_length);
    }
    return sum;
}

int main() {

    path file_path {"/home/t.talik/Programming/aoc2023/d3/input.txt"};
    std::fstream file_handle {file_path};
    std::stringstream sstream;

    std::string line;
    while (std::getline(file_handle, line)) {
        sstream << line;
    }
    int16_t const line_length = 140;

    std::string data {sstream.str()};
    std::cout << get_sum_of_engine_parts(data, line_length) << "\n";
    std::cout << get_sum_of_gear_products(data, line_length) << "\n";

    return 0;
}
