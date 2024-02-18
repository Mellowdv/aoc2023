#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

using Dp = std::vector<std::vector<int64_t>>;

void print_dp(Dp const &dp) {
    for (auto vec: dp) {
        for (auto num: vec) {
            std::cout << num << " ";
        }
        std::cout << "\n";
    }
}

bool should_lock_group_in(std::string_view const &record, std::string_view::iterator it, int64_t group_size) {
    int64_t found_hashes {};
    for (int idx = 0; idx < group_size && it < record.end(); ++idx) {
        if (*it == '#') ++found_hashes;
        ++it;
    }
    if (found_hashes == group_size) return true;
    return false;
}

int64_t count_arrangements(std::string_view const &record, std::vector<int64_t> const &contiguous_groups, int64_t group_index, Dp &dp) {

    if (group_index >= contiguous_groups.size()) {
        std::cout << "Ran out of groups\n";
        return dp.at(contiguous_groups.size()).at(record.size() - 1);
    }
    std::cout << "Counting arrangements for " << contiguous_groups.at(group_index) << "\n";
    
    int64_t valid_arrangements {};
    int64_t group_size {contiguous_groups.at(group_index)};
    int64_t current_chunk_length {};
    for (auto it = record.begin(); it < record.end(); ++it) {
        if (*it != '.') {
            ++current_chunk_length; 
        } else {
            current_chunk_length = 0;
        }

        auto left_bound {it - group_size};
        auto right_bound {it + 1};
        bool is_group_valid {(left_bound < record.begin() || *left_bound != '#') && (right_bound == record.end() || *right_bound != '#')};
        if ((current_chunk_length >= group_size) && is_group_valid) {
            auto current_pos = std::distance(record.begin(), it);
            // In case we found a chunk of '#'s exactly as long as our group, we shouldn't look any further really
            if (should_lock_group_in(record, it, group_size)) {
                dp[group_index + 1].at(current_pos) = dp[group_index][current_pos - group_size];
                int64_t tmp = dp[group_index + 1][current_pos];
                while (current_pos < record.size()) {
                    dp[group_index + 1].at(current_pos) = tmp;
                    ++current_pos;
                }
                break;
            }

            if (dp[group_index][current_pos - group_size] != 0) {
                valid_arrangements += dp[group_index][current_pos - group_size]; 
                dp[group_index + 1].at(current_pos) += valid_arrangements;
            }
        }
    }
    return count_arrangements(record, contiguous_groups, group_index + 1, dp);
}

int64_t process_line(std::string const &input) {
    std::vector<int64_t> contiguous_groups;
    std::string tmp_record;

    std::stringstream buf {input};
    
    buf >> tmp_record;
    std::string record {"." + tmp_record};
    std::cout << record << std::endl;
    
    int64_t tmp;
    while (buf >> tmp) {
        contiguous_groups.push_back(tmp);
        std::cout << contiguous_groups.back() << std::endl;
        // eat ',' or '\n'
        char ch {};
        buf >> ch;
    }

    Dp dp {contiguous_groups.size() + 1, std::vector<int64_t>(record.size())};
    for (auto &base_slot: dp.at(0)) {
        base_slot = 1;
    }
    auto val = count_arrangements(record, contiguous_groups, 0, dp);
    print_dp(dp);
    return val;
}

int main() {
    std::filesystem::path file_path {"/home/t.talik/Programming/aoc2023/d12/input.txt"};
    std::fstream file_handle {file_path};

    std::string line {};
    int64_t sum {};
    while (std::getline(file_handle, line)) {
        auto tmp {process_line(line)};
        std::cout << tmp << std::endl;
        sum += tmp;
    }
    std::cout << "Sum is... " << sum << "\n";
    return 0;
}
