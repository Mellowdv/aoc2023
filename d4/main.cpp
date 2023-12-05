#include <cctype>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <string>
#include <unordered_set>
#include <ranges>

using namespace std::filesystem;

std::array<int32_t, 199> card_winning_numbers;
std::array<int32_t, 199> card_values;

int32_t get_matching_number_count(std::string_view const &card, int32_t const card_index) {
    if (card_winning_numbers[card_index] != -1) {
        return card_winning_numbers[card_index];
    }

    std::unordered_set<int32_t> card_numbers;
    
    auto iter {card.begin()};
    while (*iter != ':') {
        ++iter;
    }
    for (; *iter != '|'; ++iter) {
        if (std::isdigit(*iter)) {
            card_numbers.insert(std::atoi(iter));
            while (std::isdigit(*iter)) {
                ++iter;
            }
            continue;
        }
    }
  
    int32_t matching_num_count {};
    for (; iter < card.end(); ++iter) {
        if (std::isdigit(*iter)) {
            if (card_numbers.contains(std::atoi(iter))) {
                ++matching_num_count;
            }
            while (std::isdigit(*iter)) {
                ++iter;
            }
            continue;
        }
    }
    return matching_num_count;
}

int32_t find_card_points(std::string_view const &card, int32_t const card_index) {

    int32_t matching_num_count {get_matching_number_count(card, card_index)};
    card_winning_numbers[card_index] = matching_num_count;
    int32_t points {};
    if (matching_num_count >= 1) {
        points = 1;
        while (matching_num_count > 1) {
            points *= 2; 
            --matching_num_count;
        }
    }
    return points;
}

int32_t find_number_of_scratchcards() {
    auto get_card_value = [] (auto &&get_card_value, int32_t const card_index) {
        std::cout << "Called with index " << card_index << "\n";
        if (card_values[card_index] != -1) {
            return card_values[card_index];
        }
        int32_t won_cards {card_winning_numbers[card_index]};
        std::cout << "Has " << won_cards << "\n";
        int32_t card_value {won_cards};
        for (int i = 1; i <= won_cards; ++i) {
            card_value += get_card_value(get_card_value, card_index + i); 
        }
        card_values[card_index] = card_value;
        return card_value;
    };

    int32_t result {};
    for (int i = 0; i < 199; ++i) {
        result += get_card_value(get_card_value, i);
    }
    return result;
}

int main() {
    card_winning_numbers.fill(-1);
    card_values.fill(-1);

    path file_path {"/home/t.talik/Programming/aoc2023/d4/input.txt"};
    std::fstream file_handle {file_path};
    
    std::string line {};
    int32_t sum {};
    int32_t card_index {};
    while (std::getline(file_handle, line)) {
        sum += find_card_points(line, card_index);
        ++card_index;
    }
    std::cout << sum << "\n";
    std::cout << find_number_of_scratchcards() + 199;

    return 0;
}
