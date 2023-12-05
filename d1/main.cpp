#include <cstdlib>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <iterator>
#include <string>
#include <string_view>
#include <array>
#include <optional>

using namespace std::filesystem;

int32_t retrieve_number(std::string_view const line) {
    bool continue_left {true};
    bool continue_right {true};

    auto left_iter {line.begin()};
    auto right_iter {line.end() - 1};
    while (continue_left || continue_right) {
        if (continue_left) {
            if (std::isdigit(*left_iter)) {
                continue_left = false;
            } else {
                ++left_iter;
            }
        }

        if (continue_right) {
            if (std::isdigit(*right_iter)) {
                continue_right = false;
            } else {
                --right_iter;
            }
        }

        if (right_iter == left_iter) break;
    }

    if ((right_iter == left_iter) && !std::isdigit(*right_iter)) return 0;
    
    std::string num_as_string {*left_iter, *right_iter};
    return std::stoi(num_as_string);
}

static std::array<std::string, 10> const digits { 
    "zero",
    "one",
	"two",
	"three",
	"four",
	"five",
	"six",
	"seven",
	"eight",
	"nine"
};

std::optional<char> check_if_substring_is_digit(std::string_view const window) {
    // Manual checks :(
    auto window_start = window.begin();
    if (window.size() < 3) return std::nullopt;
    std::string three_chars {window_start, window_start + 3};
    std::string four_chars  = window.size() < 4 ? "" : std::string(window_start, window_start + 4);
    std::string five_chars = window.size() < 5 ? "" : std::string(window_start, window_start + 5);

    if (three_chars == digits[1]) {
        return std::optional('1');
    } else if (three_chars == digits[2]) {
        return std::optional('2');
    } else if (three_chars == digits[6]) {
        return std::optional('6');
    } else if (four_chars == digits[0]) {
        return std::optional('0');
    } else if (four_chars == digits[4]) {
        return std::optional('4');
    } else if (four_chars == digits[5]) {
        return std::optional('5');
    } else if (four_chars == digits[9]) {
        return std::optional('9');
    } else if (five_chars == digits[3]) {
        return std::optional('3');
    } else if (five_chars == digits[7]) {
        return std::optional('7');
    } else if (five_chars == digits[8]) {
        return std::optional('8');
    } else {
        return std::nullopt;
    }
}

int32_t retrieve_number2(std::string_view const line) {

    char left_digit {'0'};
    char right_digit {'0'};
    bool continue_left {true};
    bool continue_right {true};

    auto left_iter {line.begin()};
    auto right_iter {line.end() - 1};
    while (continue_left || continue_right) {
        if (continue_left) {
            if (std::isdigit(*left_iter)) {
                continue_left = false;
                left_digit = *left_iter;
            } else {
                // check the left side window
                auto result = check_if_substring_is_digit(std::string_view {left_iter});
                if (result.has_value()) {
                    continue_left = false;
                    left_digit = result.value();
                } else {
                    ++left_iter;
                }
            }
        }

        if (continue_right) {
            if (std::isdigit(*right_iter)) {
                continue_right = false;
                right_digit = *right_iter;
            } else {
                auto result = check_if_substring_is_digit(std::string_view {right_iter});
                if (result.has_value()) {
                    continue_right = false;
                    right_digit = result.value();
                } else {
                    --right_iter;
                }
            }
        }

        if (right_iter == left_iter) {
            left_digit = *left_iter;
            right_digit = *right_iter;
            break;
        }
    }
    
    std::string num_as_string {left_digit, right_digit};
    return std::stoi(num_as_string);
}

int main() {

    path input_path {"/home/t.talik/Programming/cpp/aoc23/d1/input.txt"};
    std::fstream file_handle {input_path};
    std::string input_line {};


    int32_t sum {};
    int32_t sum2 {};
    while (std::getline(file_handle, input_line)) {
        sum += retrieve_number(input_line);
        sum2 += retrieve_number2(input_line);
    }
    std::cout << sum << "\n";
    std::cout << sum2 << "\n";
    return 0;
}
