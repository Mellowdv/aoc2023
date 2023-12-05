#include <fstream>
#include <filesystem>
#include <iostream>
#include <ranges>
#include <string>
#include <string_view>

constexpr int32_t RED_MAX = 12;
constexpr int32_t GREEN_MAX = 13;
constexpr int32_t BLUE_MAX = 14;

using namespace std::filesystem;
struct Round {
    int32_t red;
    int32_t blue;
    int32_t green;
};



bool is_round_valid(std::string_view const round) {
    for (const auto color : std::views::split(round, ',')) {
        int32_t number = std::stoi(std::string(color.begin(), color.end()));
        if (std::string_view(color.end() - 3, color.end()) == "red") {
            if (number <= RED_MAX) continue;
            return false;
        }
        if (std::string_view(color.end() - 4, color.end()) == "blue") {
            if (number <= BLUE_MAX) continue;
            return false;
        }
        if (std::string_view(color.end() - 5, color.end()) == "green") {
            if (number <= GREEN_MAX) continue;
            return false;
        }
    }
    return true;
}

void update_game_maximums(Round &maximums, std::string_view const round) {
    for (const auto color : std::views::split(round, ',')) {
        int32_t number = std::stoi(std::string(color.begin(), color.end()));
        if (std::string_view(color.end() - 3, color.end()) == "red") {
            if (number > maximums.red) maximums.red = number;
        }
        if (std::string_view(color.end() - 4, color.end()) == "blue") {
            if (number > maximums.blue) maximums.blue = number;
        }
        if (std::string_view(color.end() - 5, color.end()) == "green") {
            if (number > maximums.green) maximums.green = number;
        }
    }
}

int32_t get_power_of_set(std::string_view const input_line) {
    Round maximums {};
    auto left_iter = input_line.begin();
    while (*left_iter != ':') {
        ++left_iter;
    }
    left_iter += 2;

    for (const auto round : std::views::split(std::string_view(left_iter), ';')) {
        update_game_maximums(maximums, std::string_view(round.begin(), round.end()));
    }
    return maximums.red * maximums.blue * maximums.green;
}

bool is_game_valid(std::string_view const input_line) {

    auto left_iter = input_line.begin();
    while (*left_iter != ':') {
        ++left_iter;
    }
    left_iter += 2;

    for (const auto round : std::views::split(std::string_view(left_iter), ';')) {
        if (is_round_valid(std::string_view(round.begin(), round.end()))) continue;
        return false;
    }
    return true;
}

int main() {

    path input_path {"/home/t.talik/Programming/cpp/aoc23/d2/input.txt"};
    std::fstream file_handle {input_path};
    std::string input_line {};

    int32_t index {1};
    int32_t sum {};
    int32_t sum2 {};
    while (std::getline(file_handle, input_line)) {
        if (is_game_valid(input_line)) sum += index;
        sum2 += get_power_of_set(input_line);
        ++index;
    }
    std::cout << sum << "\n";
    std::cout << sum2 << "\n";
    return 0;
}
