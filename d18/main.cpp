#include <algorithm>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <queue>
#include <string>
#include <array>
#include <unordered_set>

using map = std::array<std::array<char, 360>, 310>;
using visited_map = std::array<std::array<bool, 360>, 310>;

struct Point {
    int32_t x;
    int32_t y;

    bool operator==(Point const &rhs) const {
        return this->x == rhs.x && this->y == rhs.y;
    }
};

template <class T>
inline void hash_combine(std::size_t &s, const T &v) {
    std::hash<T> h;
    s ^= h(v) + 0x9e3779b9 + (s << 6) + (s >> 2);
}
struct Point_Hash {
    std::size_t operator()(Point const &p) const {
        std::size_t seed {0};
        hash_combine(seed, p.x);
        hash_combine(seed, p.y);
        return seed;
    }
};

inline bool check_bounds(int16_t rows, int16_t cols) {
    return rows >= 0 && rows < 300 && cols >= 0 && cols < 356;
}

int64_t flood_fill(map &m, visited_map &visit_map, Point const &p) {
    // If out of bounds or already visited, return 0;
    if (!check_bounds(p.y, p.x) || visit_map[p.y][p.x]) {
        return 0;
    }
    // If any '#' encountered, stop filling, as this is the border
    if (m[p.y][p.x] == '#') {
        visit_map[p.y][p.x] = true;
        return 1;
    }

    visit_map[p.y][p.x] = true;
    m[p.y][p.x] = '#';
    
    return 1 + flood_fill(m, visit_map, {p.x + 1, p.y})
             + flood_fill(m, visit_map, {p.x - 1, p.y})
             + flood_fill(m, visit_map, {p.x, p.y - 1})
             + flood_fill(m, visit_map, {p.x, p.y + 1});
}

int64_t shoelace_formula(std::vector<int64_t> &x_coords, std::vector<int64_t> &y_coords) {

    std::reverse(x_coords.begin() + 1, x_coords.end());
    std::reverse(y_coords.begin() + 1, y_coords.end());

    int64_t left_sum {0};
    int64_t right_sum {0};

    for (int idx = 0; idx < x_coords.size(); ++idx) {
        int64_t y_idx = (idx + 1) % x_coords.size();
        left_sum += x_coords[idx] * y_coords[y_idx];
        right_sum += x_coords[y_idx] * y_coords[idx];
    }
    return std::abs(left_sum - right_sum) / 2;
}

std::pair<char, int64_t> convert_hex(std::string_view str) {
    auto dir_it = str.end() - 2;
    std::stringstream ss;
    ss << std::hex << std::string(str.begin() + 2, str.end() - 2);
    int64_t num {0};
    ss >> num;
    std::cout << *dir_it << " " << num << "\n";
    return { *dir_it, num };
}

int main() {
    std::filesystem::path file_path{"/home/t.talik/Programming/cpp/aoc2023/d18/input.txt"};
    std::fstream file_handle{file_path};
    std::string line;
    std::vector<int64_t> x_coords {};
    std::vector<int64_t> x_coords2 {};
    std::vector<int64_t> y_coords {};
    std::vector<int64_t> y_coords2 {};

    int64_t current_x {0};
    int64_t current_y {0};
    int64_t current_x2 {0};
    int64_t current_y2 {0};

    x_coords.push_back(current_x);
    x_coords2.push_back(current_x2);
    y_coords.push_back(current_y);
    y_coords2.push_back(current_y2);
    int64_t trench_length {0};
    int64_t trench_length2 {0};
    while (std::getline(file_handle, line)) {
        std::stringstream sstream {line};
        char dir {};
        sstream >> dir;
        int32_t num {};
        sstream >> num;

        std::string hex_to_convert {};
        sstream >> hex_to_convert;
        auto const [dir2, num2] = convert_hex(hex_to_convert);
        switch (dir) {
            case 'R':
                current_x += num;
                break;
            case 'L':
                current_x -= num;
                break;
            case 'U':
                current_y += num;
                break;
            case 'D':
                current_y -= num;
                break;
        }
        switch (dir2) {
            case '0':
                current_x2 += num2;
                break;
            case '2':
                current_x2 -= num2;
                break;
            case '3':
                current_y2 += num2;
                break;
            case '1':
                current_y2 -= num2;
                break;
        }
        x_coords.push_back(current_x);
        y_coords.push_back(current_y);
        x_coords2.push_back(current_x2);
        y_coords2.push_back(current_y2);
        trench_length += num;
        trench_length2 += num2;
    }
    assert(x_coords.size() == y_coords.size());
    // Use Pick's theorem to get interior points: I = A + 1 + Perimeter / 2
    int64_t result = shoelace_formula(x_coords, y_coords) + 1 + (trench_length / 2);
    int64_t result2 = shoelace_formula(x_coords2, y_coords2) + 1 + (trench_length2 / 2);
    std::cout << result << std::endl;
    std::cout << result2 << std::endl;
    return 0;
}
