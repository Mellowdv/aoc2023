#include <iostream>
#include <filesystem>
#include <fstream>
#include <ranges>
#include <vector>
#include <array>

enum class OP: uint8_t {
    RM,
    ADD,
};

int64_t hash_it_baby(std::string_view str) {
    auto iter {str.begin()};
    int64_t current_sum {0};
    int64_t sum {0};
    for (; iter < str.end(); ++iter) {
        current_sum = 0;
        if (*iter == '\n') continue;
        while (*iter != ',' && iter < str.end()) {
            current_sum += *iter;
            current_sum *= 17;
            current_sum %= 256;
            ++iter;
        }
        sum += current_sum;
    }
    return sum;
}

std::tuple<std::string_view, OP, int64_t> process_instruction(std::string_view const in) {
    std::string_view to_hash {};
    OP op {};
    int64_t len {};
    
    auto iter {in.begin()};
    while (iter < in.end()) {
        if (*iter == '\n') continue;
        if (!std::isalpha(*iter)) {
            to_hash = std::string_view(in.begin(), iter);
            if (*iter == '=') {
                op = OP::ADD;
                ++iter;
                len = std::atoi(iter);
                return {to_hash, op, len};
            } else {
                op = OP::RM;
                return {to_hash, op, len};
            }
        }
        ++iter;
    }
    return {to_hash, op, len};
}

void print_boxes(std::array<std::vector<std::pair<std::string_view, int64_t>>, 256> const &boxes) {
    int64_t box_count {0};
    for (auto const &box: boxes) {
        if (box.size() == 0) {
            ++box_count;
            continue;
        }
        std::cout << "BOX " << box_count << ": ";
        for (auto const [label, length]: box) {
            std::cout << "[" << label << " " << length << "] ";
        }
        std::cout << std::endl;
        ++box_count;
    }
}

int64_t follow_the_HASHMAP(std::string_view input) {
    std::array<std::vector<std::pair<std::string_view, int64_t>>, 256> boxes {};
    for (auto const &ins: std::views::split(input, ',')) {
        auto [to_hash, op, len] = process_instruction(std::string_view(ins));
        int64_t hashed = hash_it_baby(to_hash);
        auto it = std::ranges::find_if(boxes[hashed], [&to_hash] (std::pair<std::string_view, int64_t> const kv) { return kv.first == to_hash; });
        if (op == OP::RM) {
            if (it == boxes[hashed].end()) continue;
            boxes[hashed].erase(it);
            continue;
        }
        if (it == boxes[hashed].end()) {
            boxes[hashed].push_back({to_hash, len});
        } else {
            it->second = len;
        }
        //print_boxes(boxes);
    }

    int64_t base {0};
    int64_t running_sum {0};
    for (int64_t box_idx = 0; box_idx < 256; ++box_idx) {
        base = box_idx + 1;
        for (int64_t inner_idx = 0; inner_idx < boxes[box_idx].size(); ++inner_idx) {
            running_sum += base * (inner_idx + 1) * boxes[box_idx][inner_idx].second;
        }
    }
    return running_sum;
}

int32_t main() {
    std::string test {"HASH"};
    std::string test2 {"rn=1,cm-,qp=3,cm=2,qp-,pc=4,ot=9,ab=5,pc-,pc=6,ot=7"};
    std::filesystem::path file_path {"/home/t.talik/Programming/cpp/aoc2023/d15/input.txt"};
    std::fstream file_handle {file_path};
    std::string input {};
    std::getline(file_handle, input);
    std::cout << hash_it_baby(test) << std::endl;
    std::cout << hash_it_baby(test2) << std::endl;
    std::cout << hash_it_baby(input) << std::endl;
    std::cout << follow_the_HASHMAP(test2) << std::endl;
    std::cout << follow_the_HASHMAP(input) << std::endl;
    return 0;
}
