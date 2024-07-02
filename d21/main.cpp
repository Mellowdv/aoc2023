#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <queue>

constexpr int64_t GARDEN_SIZE = 131;
constexpr int64_t MAX_STEPS = 64;
constexpr char ROCK = '#';
constexpr char FREE = '.';
constexpr char START = 'S';

/*
 * Part I
 * It is sufficient to find the shortest paths to each point
 * To see why think about this situation:
 * A. You must make exactly 4 steps
 * B. You've already made 1 step (you have 3 left)
 * C. Can you ever finish on the square that you are currently standing on?
 * Answ. No, by exhaustive search: if you make 1 step off, 1 step on then you have to step off
 * If you make 2 steps off, then you have to make 2 steps back to get back, but you only have 1 left
 * This means that it takes an even amount of steps to go back to the point you're on.
 * Since our MAX_STEPS is even and a shortest path to a point is odd, even - odd = odd and so we can't make even steps to get back.
 *
 * Part II
 * Now the garden is infinite (it repeats) and the MAX_STEPS is 26501365.
 * No idea how to solve it. Seeing how there are 26 million steps to consider, we would need 26 million steps in all directions minimum.
 * This gives us a 52 million by 52 million array, this by itself means that there are 15 zeroes in the area of the produced array.
 * Clearly we can't do that to the RAM or the CPU...
 * This means there must be some trick to figuring this out.
 * Perhaps some form of "virtual" BFS can be performed?
 * Let's not create those huge matrices, but instead only store the points really visited in an unordered_map?
 * But won't this also require 52 million x 52 million slots - ROCKs? That's ridiculous number. It won't fit in memory...
 * This would require 21632000 GB of RAM. Wow...
 * So there must be some way to bypass all this nonsense.
 *
 * Instead... Do BFS with S as the starting point.
 * Then do BFS with all points on the perimeter of the original garden.
 * For every BFS round that's done, get all the paths that could be reached within 26501365 steps
 * (This time around it's the ones you can reach in odd number of steps. To see why:
 * A. You must make exactly 5 steps
 * B. You've already taken 2 steps (you have 3 left)
 * C. Can you ever finish on the square you are on right now?
 * Answ. No, if you have to step off, that's -1 step, step back on -1 step, step back off - 0 steps left
 * If you make 2 steps off, then you have to make 1 back, you're not on the target square.
 * In other words, Odd - even = odd, and you can't go back to the square in odd number of steps.)
 * Now this only gives us the paths in a 3 by 3 square.
 * But if we can get the paths in a 3 by 3 square of maps, can we not get them in an arbitrary square?
 * Or in other words, if we know that it takes let's say 6 steps to reach from right to left of the garden
 * then to do that twice takes 12 steps.
 *
 * Or in even otherer words, whenever we reach the left edge, we can just look up the result from BFS performed on the right side
 * and then add the length of the path so far to everything in that BFS. Eventually we will encounter a BFS where 0 solutions are available.
 * This means we can stop looking in that direction
 * Still seems big, we'd have a BFS for every point on the perimeter, so 131 + 131 + 129 + 129 = 520 BFS maps.
 * 520 x 16 KB is still just 8 320 KB, or 8 MB. Nothing compared to the one above... Well, let's see if it even works out.
 * Aha, triple that for the other arrays of visited and path_lengths.
 * Well it can be calculated at least. Doesn't even take that long, noticable amount of time, but not huge.
 */

std::array<std::array<int64_t, GARDEN_SIZE>, GARDEN_SIZE> bfs(std::array<std::array<char, GARDEN_SIZE>, GARDEN_SIZE> garden, std::pair<int64_t, int64_t> start) {

    std::array<std::array<bool, GARDEN_SIZE>, GARDEN_SIZE> visited_matrix;
    std::array<std::array<int64_t, GARDEN_SIZE>, GARDEN_SIZE> path_lengths;
    for (auto &row: visited_matrix) {
        std::fill(row.begin(), row.end(), false);
    }
    for (auto &row: path_lengths) {
        std::fill(row.begin(), row.end(), std::numeric_limits<int64_t>::max());
    }
    path_lengths[start.first][start.second] = 0;

    // straight BFS...
    std::queue<std::pair<int64_t, int64_t>> q;
    q.push(start);
    visited_matrix[start.first][start.second] = true;

    while (!q.empty()) {

        std::pair<int64_t, int64_t> current_plot = q.front();
        q.pop();

        int64_t current_length = path_lengths[current_plot.first][current_plot.second];
        // NORTH
        if (current_plot.first > 0) {
            std::pair<int64_t, int64_t> candidate_plot = {current_plot.first - 1, current_plot.second};
            if (!visited_matrix[candidate_plot.first][candidate_plot.second] &&
                    garden[candidate_plot.first][candidate_plot.second] != ROCK) {
                if (current_length + 1 < path_lengths[candidate_plot.first][candidate_plot.second]) {
                    path_lengths[candidate_plot.first][candidate_plot.second] = current_length + 1;
                    visited_matrix[candidate_plot.first][candidate_plot.second] = true;
                    q.push(candidate_plot);
                }
            }
        }
    
        // SOUTH
        if (current_plot.first < GARDEN_SIZE - 1) {
            std::pair<int64_t, int64_t> candidate_plot = {current_plot.first + 1, current_plot.second};
            if (!visited_matrix[candidate_plot.first][candidate_plot.second] &&
                    garden[candidate_plot.first][candidate_plot.second] != ROCK) {
                if (current_length + 1 < path_lengths[candidate_plot.first][candidate_plot.second]) {
                    path_lengths[candidate_plot.first][candidate_plot.second] = current_length + 1;
                    visited_matrix[candidate_plot.first][candidate_plot.second] = true;
                    q.push(candidate_plot);
                }
            }
        }

        // WEST
        if (current_plot.second > 0) {
            std::pair<int64_t, int64_t> candidate_plot = {current_plot.first, current_plot.second - 1};
            if (!visited_matrix[candidate_plot.first][candidate_plot.second] &&
                    garden[candidate_plot.first][candidate_plot.second] != ROCK) {
                if (current_length + 1 < path_lengths[candidate_plot.first][candidate_plot.second]) {
                    path_lengths[candidate_plot.first][candidate_plot.second] = current_length + 1;
                    visited_matrix[candidate_plot.first][candidate_plot.second] = true;
                    q.push(candidate_plot);
                }
            }
        }

        // EAST
        if (current_plot.second < GARDEN_SIZE - 1) {
            std::pair<int64_t, int64_t> candidate_plot = {current_plot.first, current_plot.second + 1};
            if (!visited_matrix[candidate_plot.first][candidate_plot.second] &&
                    garden[candidate_plot.first][candidate_plot.second] != ROCK) {
                if (current_length + 1 < path_lengths[candidate_plot.first][candidate_plot.second]) {
                    path_lengths[candidate_plot.first][candidate_plot.second] = current_length + 1;
                    visited_matrix[candidate_plot.first][candidate_plot.second] = true;
                    q.push(candidate_plot);
                }
            }
        }
    }

    int64_t num_of_reachable_plots {0};
    for (int i = 0; i < GARDEN_SIZE; ++i) {
        for (int j = 0; j < GARDEN_SIZE; ++j) {
            if (path_lengths[i][j] <= MAX_STEPS && path_lengths[i][j] % 2 == 0) {
                garden[i][j] = 'O';
                num_of_reachable_plots++;
            }
        }
    }
    std::cout << num_of_reachable_plots;
    return path_lengths;
}

std::array<std::array<int64_t, GARDEN_SIZE * 3>, GARDEN_SIZE * 3> big_bfs(std::array<std::array<char, GARDEN_SIZE * 3>, GARDEN_SIZE * 3> &garden, std::pair<int64_t, int64_t> start) {

    std::array<std::array<bool, GARDEN_SIZE * 3>, GARDEN_SIZE * 3> visited_matrix;
    std::array<std::array<int64_t, GARDEN_SIZE * 3>, GARDEN_SIZE * 3> path_lengths;
    for (auto &row: visited_matrix) {
        std::fill(row.begin(), row.end(), false);
    }
    for (auto &row: path_lengths) {
        std::fill(row.begin(), row.end(), std::numeric_limits<int64_t>::max());
    }
    path_lengths[start.first][start.second] = 0;

    // straight BFS...
    std::queue<std::pair<int64_t, int64_t>> q;
    q.push(start);
    visited_matrix[start.first][start.second] = true;

    while (!q.empty()) {

        std::pair<int64_t, int64_t> current_plot = q.front();
        q.pop();

        int64_t current_length = path_lengths[current_plot.first][current_plot.second];
        // NORTH
        if (current_plot.first > 0) {
            std::pair<int64_t, int64_t> candidate_plot = {current_plot.first - 1, current_plot.second};
            if (!visited_matrix[candidate_plot.first][candidate_plot.second] &&
                    garden[candidate_plot.first][candidate_plot.second] != ROCK) {
                if (current_length + 1 < path_lengths[candidate_plot.first][candidate_plot.second]) {
                    path_lengths[candidate_plot.first][candidate_plot.second] = current_length + 1;
                    visited_matrix[candidate_plot.first][candidate_plot.second] = true;
                    q.push(candidate_plot);
                }
            }
        }
    
        // SOUTH
        if (current_plot.first < GARDEN_SIZE - 1) {
            std::pair<int64_t, int64_t> candidate_plot = {current_plot.first + 1, current_plot.second};
            if (!visited_matrix[candidate_plot.first][candidate_plot.second] &&
                    garden[candidate_plot.first][candidate_plot.second] != ROCK) {
                if (current_length + 1 < path_lengths[candidate_plot.first][candidate_plot.second]) {
                    path_lengths[candidate_plot.first][candidate_plot.second] = current_length + 1;
                    visited_matrix[candidate_plot.first][candidate_plot.second] = true;
                    q.push(candidate_plot);
                }
            }
        }

        // WEST
        if (current_plot.second > 0) {
            std::pair<int64_t, int64_t> candidate_plot = {current_plot.first, current_plot.second - 1};
            if (!visited_matrix[candidate_plot.first][candidate_plot.second] &&
                    garden[candidate_plot.first][candidate_plot.second] != ROCK) {
                if (current_length + 1 < path_lengths[candidate_plot.first][candidate_plot.second]) {
                    path_lengths[candidate_plot.first][candidate_plot.second] = current_length + 1;
                    visited_matrix[candidate_plot.first][candidate_plot.second] = true;
                    q.push(candidate_plot);
                }
            }
        }

        // EAST
        if (current_plot.second < GARDEN_SIZE - 1) {
            std::pair<int64_t, int64_t> candidate_plot = {current_plot.first, current_plot.second + 1};
            if (!visited_matrix[candidate_plot.first][candidate_plot.second] &&
                    garden[candidate_plot.first][candidate_plot.second] != ROCK) {
                if (current_length + 1 < path_lengths[candidate_plot.first][candidate_plot.second]) {
                    path_lengths[candidate_plot.first][candidate_plot.second] = current_length + 1;
                    visited_matrix[candidate_plot.first][candidate_plot.second] = true;
                    q.push(candidate_plot);
                }
            }
        }
    }

    int64_t num_of_reachable_plots {0};
    for (int i = 0; i < GARDEN_SIZE; ++i) {
        for (int j = 0; j < GARDEN_SIZE; ++j) {
            if (path_lengths[i][j] <= MAX_STEPS && path_lengths[i][j] % 2 == 0) {
                garden[i][j] = 'O';
                num_of_reachable_plots++;
            }
        }
    }
    std::cout << num_of_reachable_plots;
    return path_lengths;
}

int32_t main() {

    std::filesystem::path file_path {"input.txt"};
    std::fstream file_handle {file_path};

    std::string line {};
    std::array<std::array<char, GARDEN_SIZE>, GARDEN_SIZE> garden;

    int64_t row = 0;
    std::pair<int64_t, int64_t> start {};
    while (std::getline(file_handle, line)) {
        for (int64_t column = 0; column < GARDEN_SIZE; ++column) {
            garden[row][column] = line[column];
            if (line[column] == 'S') {
                start.first = row;
                start.second = column;
            }
        }
        ++row;
    }

    std::array<std::array<int64_t, GARDEN_SIZE>, GARDEN_SIZE> path_lengths = bfs(garden, start);

    std::pair<int64_t, int64_t> scaled_up_start;
    // Build a 3x3 square of maps
    std::array<std::array<char, GARDEN_SIZE * 3>, GARDEN_SIZE * 3> scaled_up_garden;
    for (int32_t scaled_up_row = 0; scaled_up_row < GARDEN_SIZE * 3; ++scaled_up_row) {
        for (int32_t scaled_up_column = 0; scaled_up_column < GARDEN_SIZE * 3; ++scaled_up_column) {
            scaled_up_garden[scaled_up_row][scaled_up_column] = garden[scaled_up_row % 131][scaled_up_column % 131];
            if (garden[scaled_up_row % 131][scaled_up_column % 131] == 'S') {
                if ((scaled_up_row > 131 && scaled_up_row < 262) &&
                        (scaled_up_column > 131 && scaled_up_column < 262)) {
                    // Nothing
                    std::cout << "Set the nonsense\n";
                    scaled_up_start.first = scaled_up_row;
                    scaled_up_start.second = scaled_up_column;
                } else {
                    garden[scaled_up_row][scaled_up_column] = '.';
                }
            }
        }
    }

    big_bfs(scaled_up_garden, scaled_up_start);
    for (auto &row: scaled_up_garden) {
        for (auto plot: row) {
            std::cout << plot;
        }
        std::cout << std::endl;
    }
    return 0;
}
