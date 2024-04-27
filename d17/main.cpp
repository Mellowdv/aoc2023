#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <queue>
#include <unordered_set>
#include <format>

int32_t constexpr SZ = 141;
int32_t constexpr ASCII_OFFSET = 48;
using City_Blocks = std::array<std::array<int64_t, SZ>, SZ>;

// Hash combinator stolen from Boost to hash the Path_Node struct
template <class T>
inline void hash_combine(std::size_t &s, const T &v)
{
  std::hash<T> h;
  s ^= h(v) + 0x9e3779b9 + (s << 6) + (s >> 2);
}

enum Directions {
    RIGHT,
    LEFT,
    UP,
    DOWN
};

struct Path_Node {
    int16_t row;
    int16_t col;
    Directions dir;
    int8_t steps_made;
    int64_t heat_loss;

    bool operator>(Path_Node const &rhs) const {
        return this->heat_loss > rhs.heat_loss;
    }

    bool operator==(Path_Node const &rhs) const {
        return this->row == rhs.row &&
               this->col == rhs.col &&
               this->dir == rhs.dir &&
               this->steps_made == rhs.steps_made &&
               this->heat_loss == rhs.heat_loss;
    }

    std::string make_key() const {
        std::size_t seed = 0;
        //hash_combine(seed, this->row);
        //hash_combine(seed, this->col);
        //hash_combine(seed, this->dir);
        //hash_combine(seed, this->steps_made);
        //hash_combine(seed, this->heat_loss);
        //return ((this->col * 29) + (this->row * 23)) + this->dir * 19 + this->steps_made;
        //return seed;
        return std::format("{} {} {} {}", this->row, this->col, static_cast<int8_t>(this->dir), this->steps_made);
    }
};

struct Path_Node_Hash {
    std::size_t operator()(Path_Node const& p_n) const {
        std::size_t seed = 0;
        //hash_combine(seed, p_n.row);
        //hash_combine(seed, p_n.col);
        //hash_combine(seed, p_n.dir);
        //hash_combine(seed, p_n.steps_made);
        //hash_combine(seed, p_n.heat_loss);
        return ((p_n.col * 29) + (p_n.row * 23)) + p_n.dir * 19 + p_n.steps_made;
        //return (((p_n.col + 1) * 37 + (p_n.row + 1) * 17) * 5 + p_n.dir) * 9 + p_n.steps_made;
        //return seed;
    }
};

// Keys are [current_direction][turn_direction] (clockwise/counter-clockwise)
static std::array<std::array<Directions, 2>, 4> lookup_new_direction {
    {
        { Directions::DOWN, Directions::UP },
        { Directions::UP, Directions::DOWN },
        { Directions::RIGHT, Directions::LEFT },
        { Directions::LEFT, Directions::RIGHT },
    }
};

inline std::pair<int16_t, int16_t> move(int16_t rows, int16_t cols, Directions dir) {
    switch (dir) {
        case Directions::RIGHT: {
            ++cols;
            break;
        }
        case Directions::LEFT: {
            --cols;
            break;
        }
        case Directions::UP: {
            --rows;
            break;
        }
        case Directions::DOWN: {
            ++rows;
            break;
        }
    }
    return { rows, cols };
}

inline bool check_bounds(int16_t rows, int16_t cols) {
    return rows >= 0 && rows < SZ && cols >= 0 && cols < SZ;
}

int64_t get_optimal_path(City_Blocks const &cb, int32_t min_steps, int32_t max_steps) {

    Path_Node right { 0, 0, Directions::RIGHT, 0, 0 };
    Path_Node down { 0, 0, Directions::DOWN, 0, 0 };

    std::array<std::array<std::array<std::array<bool, 11>, 4>, SZ>, SZ> visited_map;
    for (auto &row: visited_map) {
        for (auto &col: row) {
            for (auto &dir: col) {
                for (auto &visited: dir) {
                    visited = false;
                }
            }
        }
    };
    // Prepare prio queue and set
    std::priority_queue<Path_Node, std::vector<Path_Node>, std::greater<Path_Node>> q;
    q.push(right);
    q.push(down);
    visited_map[right.row][right.col][right.dir][right.steps_made] = true;
    visited_map[down.row][down.col][down.dir][down.steps_made] = true;

    uint64_t max_queue_size {0};
    while (!q.empty()) {
        max_queue_size = std::max(max_queue_size, q.size());
        Path_Node c_n {q.top()};
        q.pop();
        
        if (c_n.row == SZ - 1 && c_n.col == SZ - 1) {
            std::cout << "Max queue size is: " << max_queue_size << "\n";
            if (c_n.steps_made < min_steps) continue;
            return c_n.heat_loss;
        }
        
        if (c_n.steps_made < max_steps) {
            auto const [n_rows, n_cols] = move(c_n.row, c_n.col, c_n.dir);
            if (check_bounds(n_rows, n_cols)) {
                Path_Node n_straight {
                    n_rows,
                    n_cols,
                    c_n.dir,
                    static_cast<int8_t>(c_n.steps_made + 1),
                    c_n.heat_loss + cb[n_rows][n_cols] }; 
                if (!visited_map[n_straight.row][n_straight.col][n_straight.dir][n_straight.steps_made]) {
                //if (!visited.contains(n_straight.make_key())) {
                    q.push(n_straight);
                    visited_map[n_straight.row][n_straight.col][n_straight.dir][n_straight.steps_made] = true;
                    //visited.insert(n_straight.make_key());
                }
            }
        }
        if (c_n.steps_made < min_steps) continue;
        for (auto const dir: { Directions::RIGHT, Directions::LEFT } ) {
            //std::cout << "Lookup input: " << c_n.dir << "x" << dir << "\n";
            //std::cout << "Lookup output: " << lookup_new_direction[c_n.dir][dir] << "\n";
            auto const [n_rows, n_cols] = move(c_n.row, c_n.col, lookup_new_direction[c_n.dir][dir]);
            if (check_bounds(n_rows, n_cols)) {
                Path_Node n_new_dir {
                    n_rows,
                    n_cols,
                    lookup_new_direction[c_n.dir][dir],
                    1,
                    c_n.heat_loss + cb[n_rows][n_cols]
                };
                if (!visited_map[n_new_dir.row][n_new_dir.col][n_new_dir.dir][n_new_dir.steps_made]) {
                //if (!visited.contains(n_new_dir.make_key())) {
                    q.push(n_new_dir);
                    visited_map[n_new_dir.row][n_new_dir.col][n_new_dir.dir][n_new_dir.steps_made] = true;
                    //visited.insert(n_new_dir.make_key());
                }
            }
        }
    }

    return -1;
}

int32_t main() {
    City_Blocks city_blocks;
    //std::filesystem::path file_path {"/home/t.talik/Programming/aoc2023/d17/input.txt"};
    std::filesystem::path file_path {"/home/t.talik/Programming/cpp/aoc2023/d17/input.txt"};
    std::fstream file_handle {file_path};
    std::string input {};
    for (int rows = 0; rows < SZ && std::getline(file_handle, input); ++rows) {
        for (int cols = 0; cols < SZ; ++cols) {
            city_blocks[rows][cols] = input.at(cols) - ASCII_OFFSET;
        }
    }

    std::cout << get_optimal_path(city_blocks, 1, 3) << std::endl;
    std::cout << get_optimal_path(city_blocks, 4, 10) << std::endl;
    /*for (auto &row: city_blocks) {
        for (auto &col: row) {
            std::cout << col;
        }
        std::cout << std::endl;
    }*/
    return 0;
}
