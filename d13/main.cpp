#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>
#include <chrono>

int64_t process_landscape(std::vector<std::string> const &landscape, int64_t max_differences) {

    int64_t differences {};
    for (int16_t column = 0; column < landscape[0].size() - 1; ++column) {
        differences = 0;
        int16_t symmetry_left_candidate = column, symmetry_right_candidate = column + 1;
        for (;
             (symmetry_left_candidate >= 0) && (symmetry_right_candidate < landscape[0].size());
             --symmetry_left_candidate, ++symmetry_right_candidate) {
            for (auto const &line: landscape) {
                if (line[symmetry_left_candidate] == line[symmetry_right_candidate]) {
                    continue;
                } else {
                    ++differences;
                    if (differences > max_differences) {
                        break;
                    }
                }
            }
            if (differences > max_differences) {
                break;
            }
        }
        if (differences == max_differences) {
            //std::cout << "Column wise\n";
            return column + 1; 
        }
    }

    for (int16_t row = 0; row < landscape.size(); ++row) {
        differences = 0;
        int16_t symmetry_top_candidate = row, symmetry_bot_candidate = row + 1;
        for (;
             (symmetry_top_candidate >= 0) && (symmetry_bot_candidate < landscape.size());
             --symmetry_top_candidate, ++symmetry_bot_candidate) {
            auto const &top {landscape[symmetry_top_candidate]};
            auto const &bot {landscape[symmetry_bot_candidate]};
            for (int16_t idx = 0; idx < top.size(); ++idx) {
                if (top[idx] == bot[idx]) {
                    continue;
                } else {
                    ++differences;
                    if (differences > max_differences) {
                        break;
                    }
                }
            }
            if (differences > max_differences) {
                break;
            }
        }
        if (differences == max_differences) {
            ////std::cout << "Row wise\n";
            return (row + 1) * 100;
        }
    }
    return -1;
}

int main() {
    auto const begin{std::chrono::high_resolution_clock::now()};
    std::filesystem::path file_path {"/home/t.talik/Programming/cpp/aoc2023/d13/input.txt"};
    std::fstream file_handle {file_path};
    std::string line {};

    int64_t sum {};
    int64_t sum2 {};
    std::vector<std::string> landscape;
    while (std::getline(file_handle, line)) {
        if (line == "") {
            for (auto const &line: landscape) {
                //std::cout << line << "\n";
            }
            auto const tmp = process_landscape(landscape, 0);
            auto const tmp2 = process_landscape(landscape, 1);
            //std::cout << tmp << "\n";
            sum += tmp;
            sum2 += tmp2;
            landscape.clear();
        } else {
            landscape.push_back(line);
        }
    }
    std::cout << sum << "\n";
    std::cout << sum2 << "\n";
    auto const end{std::chrono::high_resolution_clock::now()};
    std::cout << (end - begin).count() << " ns\n";
    return 0;
}
