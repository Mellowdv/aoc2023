#include <filesystem>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <vector>

static bool log {false};

using ArchivedRecord = std::pair<std::string, int64_t>;
struct ArchivedRecordHash {
    std::size_t operator()(const ArchivedRecord& ar) const noexcept {
        std::size_t hash = std::hash<std::string>{} (ar.first);
        return hash;
    }
};

using Memo = std::unordered_map<ArchivedRecord, int64_t, ArchivedRecordHash>;

void log_comment(std::string_view const &msg) {
    if (!log) return;
    std::cout << msg << "\n";
}

int64_t count_arrangements(
        std::string_view const &record,
        std::string_view::iterator pos,
        std::vector<int64_t> const &contiguous_groups,
        int64_t group_index,
        Memo &memo) {

    int64_t total {0};
    if (group_index >= contiguous_groups.size()) {
        //log_comment("Reached last group");
        for (auto it = pos; it < record.end(); ++it) {
            if (*it == '#') return 0;
        }
        return 1;
    }

    if (pos < record.end()) {
        auto key = ArchivedRecord {std::string(pos, record.end()), group_index};
        if (memo.contains(key)) {
            //log_comment("Accessed cache :)");
            return memo[key];
        }
    }
    // If there are less slots remaining than we need, this is failed, return 0
    auto group_size = contiguous_groups.at(group_index);
    if (std::distance(pos, record.end()) < group_size) return 0;
    // If the current pos is '.', then we can continue, there's nothing for us here.        
    if (*pos == '.') {
        return count_arrangements(record, pos + 1, contiguous_groups, group_index, memo);
    }

    // If the current pos is '?' or '#' we need to do something
    // Set up window to be checked
    auto left_bound {pos};
    auto right_bound {pos + group_size};

    // Right bound is not guaranteed so needs to be checked, left is guaranteed because we skip all '.'s
    int8_t valid_count {0};
    if (*right_bound != '#') {
        for (auto it = pos; it < right_bound; ++it) {
            if (*it == '.') {
                break;
            } else {
                valid_count++;
            }
        }
    }

    // '?' is treated as '#'
    // If the current window fits the group, then check next group (? treated as '#' case)
    if (valid_count >= group_size) {
        total += count_arrangements(record, pos + (group_size + 1), contiguous_groups, group_index + 1, memo);
    }
    
    // '?' is treated as '.'
    // In cases when pos was '?' we also need to check the case where it's treated as '.'
    if (*pos == '?') {
        total += count_arrangements(record, pos + 1, contiguous_groups, group_index, memo);
    }
    memo.insert({ArchivedRecord {std::string(pos, record.end()), group_index}, total});

    return total;
}

int64_t process_line(std::string const &input) {
    std::vector<int64_t> contiguous_groups;
    std::string tmp_record;

    std::stringstream buf {input};
    
    buf >> tmp_record;
    std::string_view record {tmp_record};
    
    int64_t tmp;
    while (buf >> tmp) {
        contiguous_groups.push_back(tmp);
        // eat ',' or '\n'
        char ch {};
        buf >> ch;
    }

    Memo memo {};
    auto val = count_arrangements(record, record.begin(), contiguous_groups, 0, memo);
    return val;
}

int64_t process_line2(std::string const &input) {
    std::vector<int64_t> contiguous_groups;
    std::string tmp_record;

    std::stringstream buf {input};
    
    buf >> tmp_record;
    std::string unfolded_record = tmp_record;
    for (int i = 0; i < 4; ++i) {
        unfolded_record += ("?" + tmp_record); 
    }
    std::string_view record {unfolded_record};
    
    int64_t tmp;
    std::vector<int64_t> contiguous_groups_unfolded;
    while (buf >> tmp) {
        contiguous_groups.push_back(tmp);
        // eat ',' or '\n'
        char ch {};
        buf >> ch;
    }
    for (int j = 0; j < 5; ++j) {
        for (auto group: contiguous_groups) contiguous_groups_unfolded.push_back(group);
    }

    Memo memo {};
    auto val = count_arrangements(record, record.begin(), contiguous_groups_unfolded, 0, memo);
    return val;
}

int main() {
    log = true;
    std::filesystem::path file_path {"/home/t.talik/Programming/cpp/aoc2023/d12/input.txt"};
    std::fstream file_handle {file_path};

    std::string line {};
    int64_t sum {};
    int64_t sum2 {};
    while (std::getline(file_handle, line)) {
        auto tmp {process_line(line)};
        auto tmp2 {process_line2(line)};
        sum += tmp;
        sum2 += tmp2;
    }
    std::cout << "Sum is... " << sum << "\n";
    std::cout << "Sum2 is... " << sum2 << "\n";
    return 0;
}
