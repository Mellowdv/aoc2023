#include <chrono>
#include <vector>
#include <string>
#include <fstream>
#include <filesystem>
#include <iostream>

void transpose_clockwise(std::vector<std::string> &grid) {
    for (int i = 0; i < grid.size(); ++i) {
        for (int j = i + 1; j < grid[0].size(); ++j) {
            std::swap(grid[i][j], grid[j][i]);
        }
    }

    for (int i = 0; i < grid.size(); ++i) {
        int low = 0, high = grid[0].size() - 1;
        while (low < high) {
            std::swap(grid[i][low], grid[i][high]);
            low++;
            high--;
        }
    }
}

int64_t solve(std::vector<std::string> &grid) {
    int64_t constexpr TAKEN = std::numeric_limits<int64_t>::max(); 
    int64_t sum {};
    std::vector<int64_t> last_empty_pos {};
    for (auto i {0U}; i < grid[0].size(); ++i) { last_empty_pos.push_back(TAKEN); } 

    for (int64_t outer_index {}; outer_index < grid.size(); ++outer_index) {
        auto const &row = grid[outer_index];
        for (int64_t index {}; index < row.size(); ++index) {
            if (row[index] == '.') {
                if (outer_index < last_empty_pos[index]) {
                    last_empty_pos[index] = outer_index;
                }
            } else if (row[index] == '#') {
                last_empty_pos[index] = TAKEN;
            } else {
                if (last_empty_pos[index] != TAKEN) {
                    auto tmp = last_empty_pos[index];
                    grid[tmp][index] = 'O'; 
                    grid[outer_index][index] = '.';

                    auto load = grid.size() - tmp;
                    sum += load;
                    if (tmp + 1 < grid.size()) {
                        last_empty_pos[index] = tmp + 1;
                    }
                } else {
                    auto load = grid.size() - outer_index;
                    sum += load;
                }
            }
        }
    }
    return sum;
}

enum class DIRECTIONS : uint8_t {
    NORTH,
    WEST,
    SOUTH,
    EAST
};

int64_t solve_p2(std::vector<std::string> &grid) {

    std::array<DIRECTIONS, 4> directions {DIRECTIONS::NORTH, DIRECTIONS::WEST, DIRECTIONS::SOUTH, DIRECTIONS::EAST};
    int64_t cycles {0};
    int64_t sum {0};

    int64_t current_load {0};
    using Point_list = std::vector<std::pair<int8_t, int8_t>>;
    std::vector<Point_list> master_list {};

    int64_t final_load {0};
    int64_t max_cycles {1'000'000'000};
    while (cycles < max_cycles) {

        for (auto const dir: directions) {
            solve(grid);
            transpose_clockwise(grid);
        }
        ++cycles;

        current_load = 0;
        Point_list current_cycle_points {};
        for (int64_t row = 0; row < grid.size(); ++row) {
            for (int64_t column = 0; column < grid[0].size(); ++column) {
                if (grid[row][column] == 'O') {
                    current_cycle_points.push_back({row, column});
                    current_load += grid.size() - row;
                }
            }
        }
        int64_t cnt {0};
        if (max_cycles == 1'000'000'000) {
            for (auto const &list: master_list) {
                cnt++;
                if (list == current_cycle_points) {
                    int64_t adjusted_max_cycles = cycles + ((max_cycles - cnt) % (cycles - cnt));
                    max_cycles = adjusted_max_cycles;
                    break;
                }
            }
            master_list.push_back(current_cycle_points);
        }
        final_load = current_load;
    }
    
    return final_load;
}

int main() {
    auto const begin{std::chrono::high_resolution_clock::now()};
    std::filesystem::path file_path {"/home/t.talik/Programming/cpp/aoc2023/d14/input.txt"};
    std::fstream file_handle {file_path};
    std::string line {};

    int64_t sum {};
    std::vector<std::string> grid {};
    while (std::getline(file_handle, line)) {
        grid.push_back(line);
    }
    auto grid2 = grid;

    sum = solve(grid); 
    std::cout << sum << "\n";

    std::cout << solve_p2(grid2) << "\n";
    auto const end{std::chrono::high_resolution_clock::now()};
    std::cout << (end - begin).count() << " ns\n";
    return 0;
}
