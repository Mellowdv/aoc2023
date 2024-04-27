#include <fstream>
#include <iostream>
#include <queue>
#include <unordered_set>
#include <vector>
#include <memory>
#include <chrono>

using namespace std;

// #define FILE "example.txt"
// int ROWS = 13;
// int COLS = 13;

#define FILE "/home/t.talik/Programming/aoc2023/d17/input.txt"
constexpr int ROWS = 141;
constexpr int COLS = 141;

void printMap(const std::unique_ptr<std::unique_ptr<int[]>[]> &map);
void parseMap(std::unique_ptr<std::unique_ptr<int[]>[]> &map);
int getLeastHeatLoss(const std::unique_ptr<std::unique_ptr<int[]>[]> &map, int minSteps = 1, int maxSteps = 3);
void runPart1();
void runPart2();

struct Step {
    int x;
    int y;
    int dir;
    int heatLoss;
    int steps;
};

int N = 1;
int E = 2;
int S = 3;
int W = 4;

int main(int argc, char *argv[]) {
    runPart1();
    runPart2();
    return 0;
}

void runPart1() {
    cout << "AoC 2023 Day 17 Part 1" << endl;
    const auto start = chrono::high_resolution_clock::now();

    auto map = std::make_unique<std::unique_ptr<int[]>[]>(ROWS);
    for (int i = 0; i < ROWS; ++i) {
        map[i] = std::make_unique<int[]>(COLS);
    }

    parseMap(map);

    // printMap(map);

    const int result = getLeastHeatLoss(map);

    const auto finish = chrono::high_resolution_clock::now();
    const chrono::duration<double, milli> elapsed = finish - start;

    cout << "Result: " << result << ((result != 102 && result != 916) ? " FAILING" : " PASSING") << endl;
    cout << "Elapsed: " << elapsed.count() << " ms" << endl;
};

void runPart2() {
    cout << "AoC 2023 Day 17 Part 2" << endl;
    const auto start = chrono::high_resolution_clock::now();

    auto map = std::make_unique<std::unique_ptr<int[]>[]>(ROWS);
    for (int i = 0; i < ROWS; ++i) {
        map[i] = std::make_unique<int[]>(COLS);
    }

    parseMap(map);

    // printMap(map);

    const int result = getLeastHeatLoss(map, 4, 10);

    const auto finish = chrono::high_resolution_clock::now();
    const chrono::duration<double, milli> elapsed = finish - start;

    cout << "Result: " << result << ((result != 94 && result != 1067) ? " FAILING" : " PASSING") << endl;
    cout << "Elapsed: " << elapsed.count() << " ms" << endl;
}

void printMap(const std::unique_ptr<std::unique_ptr<int[]>[]> &map) {
    cout << "----" << endl;

    for (int y = 0; y < ROWS; ++y) {
        for (int x = 0; x < COLS; ++x) {
            cout << map[y][x];
        }
        cout << endl;
    }
    cout << "----" << endl;
}


struct CompareSteps {
    bool operator()(Step const &a, Step const &b) const {
        return b.heatLoss < a.heatLoss;
    }
};

int cacheKey(Step &step) {
    auto &[x, y, dir, heatLoss, steps] = step;

    // return (x << (8 + 3 + 4)) | (y << (3 + 4)) | (dir << 4) | steps; // safer
    return (((x + 1) * 37 + (y + 1) * 17) * 5 + dir) * 9 + steps;
}


int getLeastHeatLoss(const std::unique_ptr<std::unique_ptr<int[]>[]> &map, const int minSteps, const int maxSteps) {
    constexpr int endX = COLS - 1;
    constexpr int endY = ROWS - 1;

    Step startingEast = {0, 0, E, 0, 0};
    Step staringSouth = {0, 0, S, 0, 0};

    priority_queue<Step, vector<Step>, CompareSteps> q;
    q.push(startingEast);
    q.push(staringSouth);

    unordered_set<int> visited;
    visited.insert(cacheKey(staringSouth));
    visited.insert(cacheKey(startingEast));

    while (!q.empty()) {
        const auto [x, y, dir, heatLoss, steps] = q.top();
        q.pop();

        // cout << "X: " << x << " y: " << y << " dir: " << dir << " steps: " << steps << " heatLoss: " << heatLoss << endl;
        if (x == endX && y == endY) {
            if (steps < minSteps) continue;
            return heatLoss;
        }

        std::tuple<int, int, int> nextSteps[3];

        if (dir == N) {
            nextSteps[0] = {x, y - 1, N};
            nextSteps[1] = {x - 1, y, W};
            nextSteps[2] = {x + 1, y, E};
        }
        if (dir == E) {
            nextSteps[0] = {x + 1, y, E};
            nextSteps[1] = {x, y + 1, S};
            nextSteps[2] = {x, y - 1, N};
        }
        if (dir == S) {
            nextSteps[0] = {x, y + 1, S};
            nextSteps[1] = {x - 1, y, W};
            nextSteps[2] = {x + 1, y, E};
        }
        if (dir == W) {
            nextSteps[0] = {x - 1, y, W};
            nextSteps[1] = {x, y + 1, S};
            nextSteps[2] = {x, y - 1, N};
        }

        for (const auto [nx, ny, nDir]: nextSteps) {
            if (nx < 0 || ny < 0 || nx > endX || ny > endY) continue;
            if (steps > maxSteps - 1 && dir == nDir) continue;
            if (steps < minSteps && dir != nDir) continue;

            // const int nPrio = heatLoss + map[ny][nx] + (endX - nx + endY - ny);

            Step next = {
                    nx,
                    ny,
                    nDir,
                    (heatLoss + map[ny][nx]),
                    (dir == nDir ? steps + 1 : 1)};

            if (auto key = cacheKey(next); visited.find(key) == visited.end()) {
                visited.insert(key);
                q.push(next);
            }
        }
    }

    return -1;
}

void parseMap(std::unique_ptr<std::unique_ptr<int[]>[]> &map) {
    ifstream file(FILE);

    if (!file.is_open()) {
        cout << "Can't open: " << FILE << endl;
        exit(1);
    }

    string line;
    int mr = 0;

    while (getline(file, line)) {
        int mc = 0;
        for (const char &c: line) {
            map[mr][mc] = c - '0';
            mc++;
        }
        mr++;
    }

    file.close();
}
