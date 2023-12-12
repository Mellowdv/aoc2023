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

std::vector<Tile> loop {};
int32_t find_loop_length(Maze const &m, Tile const &start, Direction const initial_dir) {
    
    Direction direction {initial_dir};
    Tile current_tile {start};
    int32_t loop_length {0};

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
                loop.push_back(current_tile);
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
                loop.push_back(current_tile);
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
                loop.push_back(current_tile);
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
                loop.push_back(current_tile);
                break;
            default:
                break;
        }
    }

    return loop_length / 2;
}

enum Side {
    In,
    Out
};

enum Pointing {
    Unknown,
    Up,
    Down
};

int32_t find_enclosed_area(Maze &m) {
    int32_t area {};
    Side side {Out};   
    Pointing pointing {Unknown};
    for (int32_t i = 0; i < m.size(); ++i) {
        side = Out;
        pointing = Unknown;
        for (int32_t j = 0; j < m[0].size(); ++j) {
            if (m[i][j] == '-') continue;
            if (m[i][j] == '.' && side == In) {
                ++area;
            }
            if (m[i][j] == '|') {
                if (side == Out) {
                    side = In;
                } else {
                    side = Out;
                }
                continue;
            }

            if (m[i][j] == 'F') {
                pointing = Down;
                continue;
            } else if (m[i][j] == 'L') {
                pointing = Up;
                continue;
            }

            if (m[i][j] == '7') {
                if (pointing == Down) {
                    continue;
                } else {
                    if (side == Out) {
                        side = In;
                    } else {
                        side = Out;
                    }
                    continue;
                }
            }

            if (m[i][j] == 'J') {
                if (pointing == Up) {
                    continue;
                } else {
                    if (side == Out) {
                        side = In;
                    } else {
                        side = Out;
                    }
                    continue;
                }
            }
        }
    }
    return area;
}

int main() {
    std::filesystem::path file_path {"/home/t.talik/Programming/aoc2023/d10/input.txt"};
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

    int32_t length;
    for (auto dir: {N, S, E, W}) {
        length = find_loop_length(m, Tile {start, 'X'}, dir);
        if (length > 0) {
            break;
        }
    }
    std::cout << length << "\n";

    for (auto &line: m) {
        for (auto &ch: line) {
            ch = '.';
        }
    }
    for (auto const &tile: loop) {
        m[tile.p.first][tile.p.second] = tile.pipe;
    }
    m[start.first][start.second] = '|';
    std::cout << find_enclosed_area(m) << "\n";
    return 0;
}
