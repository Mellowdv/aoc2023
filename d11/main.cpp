#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>
#include <array>

constexpr uint64_t ROWS {140};
constexpr int8_t UNCHECKED {-1};
constexpr int8_t EMPTY {1};
constexpr int8_t NOT_EMPTY {0};

using Galaxy = std::pair<int64_t, int64_t>;
using Galaxy_list = std::vector<Galaxy>;
using Image = std::vector<std::vector<char>>;

static std::array<int8_t, ROWS> column_cache {};

bool check_if_row_empty(Image const &i, uint64_t row_index) {
    for (auto const c: i[row_index]) {
        if (c != '.') {
            return false;
        }
    }
    return true;
}

bool check_if_column_empty(Image const &i, uint64_t column_index) {
    if (column_cache[column_index] != UNCHECKED) {
        return column_cache[column_index] == EMPTY;
    }

    for (uint64_t row_index {0}; row_index < i.size(); ++row_index) {
        if (i[row_index][column_index] != '.') {
            column_cache[column_index] = NOT_EMPTY;
            return false;
        }
    }
    column_cache[column_index] = EMPTY;
    return true;
}

Galaxy_list find_galaxies(Image const &i, int64_t expansion_rate) {
    Galaxy_list gl {};
    uint64_t rows_from_the_start {};
    uint64_t columns_from_the_start {};
    uint64_t row_index {};
    uint64_t column_index {};
    for (; row_index < i.size(); ++row_index, ++rows_from_the_start) {
        std::cout << "Row idx: " << row_index << " Adjusted: " << rows_from_the_start << "\n";
        if (check_if_row_empty(i, row_index)) {
            rows_from_the_start += expansion_rate;
            continue;
        }
        column_index = 0;
        columns_from_the_start = 0;
        for (; column_index < i[row_index].size(); ++column_index, ++columns_from_the_start) {
            if (i[row_index][column_index] == '#') {
                gl.push_back(Galaxy(rows_from_the_start, columns_from_the_start));
                continue;
            }
            if (check_if_column_empty(i, column_index)) {
                columns_from_the_start += expansion_rate;
            }
        }
    }

    return gl;
}

int64_t find_shortest_path(Galaxy const &g1, Galaxy const &g2) {
    int64_t x1 = g1.first;
    int64_t x2 = g2.first;
    int64_t y1 = g1.second;
    int64_t y2 = g2.second;

    return std::abs(x1 - x2) + std::abs(y1 - y2);
}

int64_t find_distances_for_all_pairs(Galaxy_list const &gl) {
    int64_t sum {};
    for (auto i {0}; i < gl.size(); ++i) {
        for (auto j = i + 1; j < gl.size(); ++j) {
            sum += find_shortest_path(gl[i], gl[j]); 
        }
    }
    return sum;
}

int main() {
    std::filesystem::path file_path {"/home/t.talik/Programming/aoc2023/d11/input.txt"};
    std::fstream file_handle {file_path};
    column_cache.fill(UNCHECKED);    
    
    std::string line {};
    Image i {};
    i.resize(ROWS);
    uint64_t line_index {};
    while (std::getline(file_handle, line)) {
        std::for_each(line.begin(), line.end(), [&line_index, &i] (auto ch) {
            i[line_index].push_back(ch);
        });
        ++line_index;
    }

    for (auto const &l: i) {
        for (auto const ch: l) {
            std::cout << ch;
        }
        std::cout << "\n";
    }
    Galaxy_list gl {find_galaxies(i, 1)};
    for (const auto &g: gl) {
        std::cout << "Row: " << g.first << " Column: " << g.second << "\n";
    }

    std::cout << find_distances_for_all_pairs(gl) << std::endl;  
    Galaxy_list gl_expanded {find_galaxies(i, 999'999)};
    std::cout << find_distances_for_all_pairs(gl_expanded) << std::endl;
    return 0;
}
