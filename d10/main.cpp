#include <fstream>
#include <filesystem>
#include <iostream>
#include <vector>

using Maze = std::vector<std::vector<char>>;
using Point = std::pair<int32_t, int32_t>;

enum Direction {
    N = 0,
    S,
    E,
    W,
    X
};

struct Tile {
    Point p;
    char pipe;
};

constexpr char NS = '|';
constexpr char EW = '-';
constexpr char NE = 'L';
constexpr char NW = 'J';
constexpr char SW = '7';
constexpr char SE = 'F';

std::vector<std::vector<Tile>> loops {};
int32_t find_loop_length(Maze const &m, Tile const &start, Direction const initial_dir) {
    
    Direction direction {initial_dir};
    Tile current_tile {start};
    int32_t loop_length {0};

    loops.push_back(std::vector<Tile>());
    while (current_tile.pipe != 'S') {
        ++loop_length;
        switch (direction) {
            case N:
                if (current_tile.p.first == 0) {
                    return 0;
                }
                if (m[current_tile.p.first - 1][current_tile.p.second] == 'S') {
                    current_tile.pipe = 'S';
                    ++loop_length;
                    break;
                }
                if (m[current_tile.p.first - 1][current_tile.p.second] == NS) {
                    direction = N;
                } else if (m[current_tile.p.first - 1][current_tile.p.second] == SW) {
                    direction = W;
                } else if (m[current_tile.p.first - 1][current_tile.p.second] == SE) {
                    direction = E;
                } else {
                    return 0; // not a loop, not sure if possible
                }
                current_tile.pipe = m[current_tile.p.first - 1][current_tile.p.second];
                current_tile.p.first -= 1;
                loops.back().push_back(current_tile);
                break;
            case S:
                if (current_tile.p.first == m.size() - 1) {
                    return 0;
                }
                if (m[current_tile.p.first + 1][current_tile.p.second] == 'S') {
                    current_tile.pipe = 'S';
                    ++loop_length;
                    break;
                }
                if (m[current_tile.p.first + 1][current_tile.p.second] == NS) {
                    direction = S;
                } else if (m[current_tile.p.first + 1][current_tile.p.second] == NW) {
                    direction = W;
                } else if (m[current_tile.p.first + 1][current_tile.p.second] == NE) {
                    direction = E;
                } else {
                    return 0; // not a loop, not sure if possible
                }
                current_tile.pipe = m[current_tile.p.first + 1][current_tile.p.second];
                current_tile.p.first += 1;
                loops.back().push_back(current_tile);
                break;
            case E:
                if (current_tile.p.second == m[0].size() - 1) {
                    return 0;
                }
                if (m[current_tile.p.first][current_tile.p.second + 1] == 'S') {
                    current_tile.pipe = 'S';
                    ++loop_length;
                    break;
                }
                if (m[current_tile.p.first][current_tile.p.second + 1] == EW) {
                    direction = E;
                } else if (m[current_tile.p.first][current_tile.p.second + 1] == SW) {
                    direction = S;
                } else if (m[current_tile.p.first][current_tile.p.second + 1] == NW) {
                    direction = N;
                } else {
                    return 0; // not a loop, not sure if possible
                }
                current_tile.pipe = m[current_tile.p.first][current_tile.p.second + 1];
                current_tile.p.second += 1;
                loops.back().push_back(current_tile);
                break;
            case W:
                if (current_tile.p.second == 0) {
                    return 0;
                }
                if (m[current_tile.p.first][current_tile.p.second - 1] == 'S') {
                    current_tile.pipe = 'S';
                    ++loop_length;
                    break;
                }
                if (m[current_tile.p.first][current_tile.p.second - 1] == EW) {
                    direction = W;
                } else if (m[current_tile.p.first][current_tile.p.second - 1] == SE) {
                    direction = S;
                } else if (m[current_tile.p.first][current_tile.p.second - 1] == NE) {
                    direction = N;
                } else {
                    return 0; // not a loop, not sure if possible
                }
                current_tile.pipe = m[current_tile.p.first][current_tile.p.second - 1];
                current_tile.p.second -= 1;
                loops.back().push_back(current_tile);
                break;
            default:
                break;
        }
    }

    return loop_length / 2;
}

bool check_if_enclosed(Maze &m, Point const &p) {
    std::cout << p.first << " " << p.second << std::endl;
    Point new_point {p};

    if (m[p.first][p.second] == 'E') return true;
    if (m[p.first][p.second] == 'C') return true;
    if (p.first == 0 || p.second == 0 || p.first == m.size() - 1 || p.second == m.size() - 1) {
        return false;
    }
    m[p.first][p.second] = 'C';

    if (m[p.first + 1][p.second] != 'X') {
        if (!check_if_enclosed(m, Point(p.first + 1, p.second))) {
            return false;
        }
    }
    if (m[p.first - 1][p.second] != 'X') {
        if (!check_if_enclosed(m, Point(p.first - 1, p.second))) {
            return false;
        }
    }
    if (m[p.first][p.second + 1] != 'X') {
        if (!check_if_enclosed(m, Point(p.first, p.second + 1))) {
            return false;
        }
    }
    if (m[p.first][p.second - 1] != 'X') {
        if (!check_if_enclosed(m, Point(p.first, p.second - 1))) {
            return false;
        }
    }
    m[p.first][p.second] = 'E';
    return true;
}

int32_t find_enclosed_area(Maze &m) {
    int32_t area {};
    for (int32_t i = 1; i < m.size() - 1; ++i) {
        for (int32_t j = 1; j < m[0].size() - 1; ++j) {
            if (m[i][j] != 'X') continue;
            m[i][j] = 'C';
            if (check_if_enclosed(m, Point(i, j))) {
                ++area;
            }
        }
    }
    return area;
}

int main() {
    std::filesystem::path file_path {"/home/ttalik/tmp/cpp/aoc2023/d10/input.txt"};
    std::fstream file_handle {file_path};

    Maze m {};

    std::string line {};
    int32_t line_index {};
    int32_t column_index {};
    Point start {};
    while (std::getline(file_handle, line)) {
        m.push_back(std::vector<char>());
        column_index = 0;
        for (auto const ch: line) {
            if (ch == 'S') {
                start.first = line_index;
                start.second = column_index;
            }
            m[line_index].push_back(ch);
            ++column_index;
        }
        ++line_index;
    }

    int32_t max {};
    for (auto dir: {N, S, E, W}) {
        max = std::max(max, find_loop_length(m, Tile {start, 'X'}, dir));
    }
    std::cout << max << std::endl;

    for (auto const &loop: loops) {
        std::cout << "length " << loop.size() << "\n";
    }
    for (auto const &tile: loops[1]) {
        m[tile.p.first][tile.p.second] = 'X';
    }
    for (auto const &line: m) {
        for (auto ch: line) {
            std::cout << ch;
        }
        std::cout << std::endl;
    }
    std::cout << start.first << " " << start.second << std::endl;
    std::cout << find_enclosed_area(m) << std::endl;
    for (auto const &line: m) {
        for (auto ch: line) {
            std::cout << ch;
        }
        std::cout << std::endl;
    }
    return 0;
}
