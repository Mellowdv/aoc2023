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

int64_t count_valid_arrangements(std::string_view const &record, std::vector<int64_t> const &contiguous_groups, int64_t group_index, Dp &dp) {

    // If we're past the last group index, that means we've completed all the passes
    if (group_index >= contiguous_groups.size()) return dp[contiguous_groups.size() - 1][record.size() - 1];

    // else we need to do a pass over the string_view looking for the current group
    int64_t current_group_size {contiguous_groups[group_index]};
    int64_t chunk {0};
    for (int i = 0; i < record.size(); ++i) {
        // try to look for chunks long enough to fit the group
        // if '.' encountered we have to reset the chunk
        if (record[i] != '.') {
            ++chunk;
        } else {
            chunk = 0;
            // Copy the previous results if we haven't encountered a new chunk
            dp[group_index][i] = dp[group_index][i - 1];
        }

        // chunk long enough to contain our group
        if (chunk >= current_group_size) {
            int64_t region_start {i - current_group_size};
            if ((region_start < 0) || (record[region_start] != '#')) {
                // this means that the chunk is valid from the left
                if ((i + 1) >= record.size() || record[i + 1] != '#') {
                    // this means the chunk is valid from the right
                    if (i == 0 || dp[group_index][i - 1] == 0) {
                        bool const group_and_region_index_valid = group_index > 0 && (i - chunk >= 0);
                        if (group_and_region_index_valid && dp[group_index - 1][i - chunk] == 0) {
                            dp[group_index][i] = 0;
                        } else {
                            dp[group_index][i] = 1;
                        }
                    } else {
                        dp[group_index][i] = dp[group_index][i - 1] + 1;
                    }
                    std::cout << "[LOG] Valid chunk found from " << region_start << " to " << i << "\n";
                } else {
                    // chunk invalid
                    // copy previous data
                    std::cout << "[LOG] Chunk invalid from the right " << i << "\n";
                    dp[group_index][i] = i > 0 ? dp[group_index][i - 1] : 0;
                    continue;
                }
            } else {
                    std::cout << "[LOG] Chunk invalid from the left " << i << "\n";
                    dp[group_index][i] = i > 0 ? dp[group_index][i - 1] : 0;
                    continue;
            }
        } else {
            // Copy previous results if chunk not long enough to keep information about previous best
            dp[group_index][i] = i > 0 ? dp[group_index][i - 1] : 0;
        }
    }

    std::cout << "==== DEBUG ====\n";
    print_dp(dp);
    std::cout << "===============\n";
    return count_valid_arrangements(record, contiguous_groups, group_index + 1, dp);
}

int64_t process_line(std::string const &input) {
    std::vector<int64_t> contiguous_groups;
    std::string record;

    std::stringstream buf {input};
    
    buf >> record;
    std::cout << record << std::endl;
    
    int64_t tmp;
    while (buf >> tmp) {
        contiguous_groups.push_back(tmp);
        std::cout << contiguous_groups.back() << std::endl;
        // eat ',' or '\n'
        char ch {};
        buf >> ch;
    }

    Dp dp(contiguous_groups.size() + 1, std::vector<int64_t>(record.size() + 1, 0));
    for (int i = 0; i < dp.size(); ++i) {
        dp[i][0] = 1;
    }
    return count_valid_arrangements(record, contiguous_groups, 0, dp);
}

int main() {
    std::filesystem::path file_path {"/home/ttalik/tmp/cpp/aoc2023/d12/input.txt"};
    std::fstream file_handle {file_path};

    std::string line {};
    while (std::getline(file_handle, line)) {
        std::cout << process_line(line) << std::endl;
    }
    return 0;
}
