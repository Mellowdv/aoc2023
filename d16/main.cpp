#include <algorithm>
#include <filesystem>
#include <fstream>
#include <array>
#include <iostream>
#include <optional>
#include <queue>

enum Direction: int8_t {
    UP,
    DOWN,
    RIGHT,
    LEFT,
};

struct Beam {
    Direction dir;
    int8_t row;
    int8_t col;
};

// 0 == '/'
// 1 == '\'
constexpr static std::array<std::array<Direction, 4>, 2> reflection_table =
{{ 
    { Direction::RIGHT, Direction::LEFT, Direction::UP, Direction::DOWN },
    { Direction::LEFT, Direction::RIGHT, Direction::DOWN, Direction::UP },
}};

Direction reflect(Beam &b, char const mirror) {
    if (mirror == '/') return reflection_table[0][b.dir];
    return reflection_table[1][b.dir];
}

std::optional<std::pair<Beam, Beam>> split(Beam &b, char const splitter) {
    if ((splitter == '|') && (b.dir > Direction::DOWN)) {
        return std::make_optional(std::make_pair(Beam(Direction::UP, b.row, b.col), Beam(Direction::DOWN, b.row, b.col)));
    } else if ((splitter == '-') && (b.dir < Direction::RIGHT)) {
        return std::make_optional(std::make_pair(Beam(Direction::LEFT, b.row, b.col), Beam(Direction::RIGHT, b.row, b.col)));
    }
    return std::nullopt;
}

constexpr int8_t SIZE = 110;

void move_beam(Beam &b) {
    switch (b.dir) {
        case Direction::RIGHT:
            b.col++;
            break;
        case Direction::LEFT:
            b.col--;
            break;
        case Direction::DOWN:
            b.row++;
            break;
        case Direction::UP:
            b.row--;
            break;
    }
}
using Tab = std::array<std::array<std::array<uint8_t, 4>, SIZE>, SIZE>;

void simulate_beam(
        std::array<std::array<char, SIZE>, SIZE> const &field,
        Beam &b,
        std::vector<std::pair<int8_t, int8_t>> &energized_fields,
        std::queue<Beam> &bq,
        Tab &t) {

    while ((b.row < SIZE) && (b.col < SIZE) && (b.row >= 0) && (b.col >= 0)) {
        if (t[b.row][b.col][b.dir] != 0) { 
            bq.pop();
            return;
        }
        energized_fields.push_back({b.row, b.col});
        t[b.row][b.col][b.dir] = 1;
        char current_tile = field[b.row][b.col];
        if ((current_tile == '/') || (current_tile == '\\')) {
            b.dir = reflect(b, current_tile);
        } else if ((current_tile == '|') || (current_tile == '-')) {
            auto const result = split(b, current_tile);
            if (result.has_value()) {
                auto [b1, b2] = result.value();
                move_beam(b1);
                move_beam(b2);
                bq.pop();
                bq.push(b1);
                bq.push(b2);
                return;
            }
        }
        move_beam(b);
    }
    bq.pop();
}
                

int64_t find_energized_tiles(std::array<std::array<char, SIZE>, SIZE> const &field, Beam start_beam) {
    std::queue<Beam> beam_queue {};
    beam_queue.push(start_beam);
    std::vector<std::pair<int8_t, int8_t>> energized_fields {};
    Tab tab;
    for (auto &first: tab) {
        for (auto &second: first) {
            for (auto &dir: second) {
                dir = 0;
            }
        }
    }

    while (!beam_queue.empty()) {
        Beam b = beam_queue.front();
        Direction dir = b.dir;
        simulate_beam(field, b, energized_fields, beam_queue, tab);
    }

    std::sort(energized_fields.begin(), energized_fields.end());
    auto const it = std::unique(energized_fields.begin(), energized_fields.end());
    energized_fields.erase(it, energized_fields.end());
    return energized_fields.size();
}

std::array<std::array<char, SIZE>, SIZE> build_field(std::fstream &fh) {
    std::array<std::array<char, SIZE>, SIZE> field {};
    std::string line {};
    int64_t row {0};
    while (std::getline(fh, line)) {
        if (line == "") break;
        for (int64_t col = 0; col < SIZE; ++col) {
            field[row][col] = line[col];
        }
        ++row;
    }
    return field;
}

int main() {
    auto const begin{std::chrono::high_resolution_clock::now()};
    //std::filesystem::path file_path {"/home/t.talik/Programming/cpp/aoc2023/d16/input.txt"};
    std::filesystem::path file_path {"/home/ttalik/tmp/cpp/aoc2023/d16/input.txt"};
    std::fstream file_handle {file_path};
    auto const field {build_field(file_handle)};
    std::cout << find_energized_tiles(field, Beam(Direction::RIGHT, 0, 0)) << std::endl;

    int64_t max {0};
    for (int idx = 0; idx < SIZE; ++idx) {
        max = std::max(max, find_energized_tiles(field, Beam(Direction::RIGHT, idx, 0)));
        max = std::max(max, find_energized_tiles(field, Beam(Direction::LEFT, idx, SIZE - 1)));
        max = std::max(max, find_energized_tiles(field, Beam(Direction::DOWN, 0, idx)));
        max = std::max(max, find_energized_tiles(field, Beam(Direction::UP, SIZE - 1, idx)));
    }
    std::cout << "The max is: " << max << std::endl;

    auto const end{std::chrono::high_resolution_clock::now()};
    std::cout << (end - begin).count() << " ns" << std::endl;
    return 0;
}
