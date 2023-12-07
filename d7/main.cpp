#include <algorithm>
#include <fstream>
#include <filesystem>
#include <array>
#include <iostream>
#include <vector>

constexpr int32_t JOKER = 9;

enum Hand_type {
    high_card,
    one_pair,
    two_pair,
    three_of_a_kind,
    full_house,
    four_of_a_kind,
    five_of_a_kind
};

struct Hand {
    std::string char_representation;
    Hand_type hand_type;
    int64_t bid;
};
using Hands = std::vector<Hand>;

int8_t get_rank_index(char c) {
    switch (c) {

        case '2':
            return 0;
        case '3':
            return 1;
        case '4':
            return 2;
        case '5':
            return 3;
        case '6':
            return 4;
        case '7':
            return 5;
        case '8':
            return 6;
        case '9':
            return 7;
        case 'T':
            return 8;
        case 'J':
            return 9;
        case 'Q':
            return 10;
        case 'K':
            return 11;
        case 'A':
            return 12;
        default:
            return 0;
    }
}

int8_t get_rank_index_jokers(char c) {
    switch (c) {

        case '2':
            return 0;
        case '3':
            return 1;
        case '4':
            return 2;
        case '5':
            return 3;
        case '6':
            return 4;
        case '7':
            return 5;
        case '8':
            return 6;
        case '9':
            return 7;
        case 'T':
            return 8;
        case 'J':
            return 9;
        case 'Q':
            return 10;
        case 'K':
            return 11;
        case 'A':
            return 12;
        default:
            return 0;
    }
}

Hand_type determine_hand_type(std::string_view const &hand) {
    std::array<int8_t, 13> aux_array;
    aux_array.fill(0);
    for (auto c: hand) {
        aux_array[get_rank_index(c)]++;
    }
    bool found_three {false};
    int8_t found_two {};
    for (auto n: aux_array) {
        std::cout << n << " ";
    }
    std::cout << "\n";
    for (auto num: aux_array) {
        switch (num) {
            case 5:
                return five_of_a_kind;
            case 4:
                return four_of_a_kind;
            case 3:
                found_three = true;
                continue;
            case 2:
                found_two++;
                continue;
            default:
                continue;
        }
    }

    if (found_three && found_two) {
        return full_house;
    } else if (found_three) {
        return three_of_a_kind;
    } else if (found_two == 2) {
        return two_pair;
    } else if (found_two == 1) {
        return one_pair;
    }

    return high_card;
}

void sort_cards(Hands &hands) {
    auto comparator = [] (Hand const &lhs, Hand const &rhs)
    {
        if (lhs.hand_type < rhs.hand_type) {
            return true;
        } else if (lhs.hand_type > rhs.hand_type) {
            return false;
        }

        auto lhs_iter {lhs.char_representation.begin()};
        auto rhs_iter {rhs.char_representation.begin()};

        while (lhs_iter != lhs.char_representation.end()) {
            if (get_rank_index(*lhs_iter) < get_rank_index(*rhs_iter)) {
                return true;
            } else if (get_rank_index(*lhs_iter) > get_rank_index(*rhs_iter)) {
                return false;
            } else {
                ++lhs_iter;
                ++rhs_iter;
            }
        }
        return true;
    };
    std::sort(hands.begin(), hands.end(), comparator);
}

int64_t determine_result(Hands const &hands) {
    int64_t sum {};
    for (int32_t i = 0; i < hands.size(); ++i) {
        sum += (hands.at(i).bid * (i + 1));
    }
    return sum;
}

int main() {

    std::filesystem::path file_path {"/home/t.talik/Programming/aoc2023/d7/input.txt"};
    std::fstream file_handle {file_path};

    Hands hands {};
    std::string line;
    while (std::getline(file_handle, line)) {
        std::string char_representation;
        int64_t bid;

        std::stringstream sstream {line};
        sstream >> char_representation >> bid;
        Hand h { .char_representation = std::string(char_representation), .bid = bid};
        std::cout << "Hand: " << h.char_representation << " determined to have type: ";
        h.hand_type = determine_hand_type(h.char_representation);
        std::cout << h.hand_type << "\n";
        hands.push_back(h);
    }
    sort_cards(hands);
    for (auto h: hands) {
        std::cout << h.char_representation << "\n";
    }
    std::cout << determine_result(hands) << std::endl;
    
    return 0;
}
