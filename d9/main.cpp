#include <fstream>
#include <filesystem>
#include <iostream>
#include <vector>

bool check_if_all_zeroes(std::vector<int32_t> const &seq) {
    for (auto const num: seq) {
        if (num != 0) return false;
    }
    return true;
}

std::pair<int32_t, int32_t> find_next_and_prev_value(std::vector<int32_t> const &sequence) {
    auto iter {sequence.begin()};
    if (*iter == 0) {
        if (check_if_all_zeroes(sequence)) {
            return std::pair(0, 0);
        }
    }

    std::vector<int32_t> new_sequence {};
    while (iter < sequence.end() - 1) {
        new_sequence.push_back(*(iter + 1) - *iter);
        ++iter;
    }

    for (auto const num: new_sequence) {
        std::cout << num << " ";
    }
    std::cout << "\n";

    auto extrapolated_results {find_next_and_prev_value(new_sequence)};
    return std::pair((*iter) + extrapolated_results.first, *sequence.begin() - extrapolated_results.second);
}

int main() {

    std::filesystem::path file_path {"/home/t.talik/Programming/aoc2023/d9/input.txt"};
    std::fstream file_handle {file_path};

    std::string line {};
    int32_t sum {};
    int32_t prev_sum {};
    while (std::getline(file_handle, line)) {
        std::stringstream sstream {line};
        std::vector<int32_t> sequence {};
        int32_t tmp {};
        while (sstream >> tmp) {
            sequence.push_back(tmp);
        }

        auto results {find_next_and_prev_value(sequence)};
        std::cout << results.first << std::endl; 
        sum += results.first;
        prev_sum += results.second;
    }
    std::cout << sum << std::endl;
    std::cout << prev_sum << std::endl;
    return 0;
}
