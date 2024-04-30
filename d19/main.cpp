#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <ranges>
#include <chrono>

enum class Operation: int8_t {
    LESS,
    GREATER,
};

enum class Category: int8_t {
    X,
    M,
    A,
    S
};

enum class Special: int8_t {
    ACCEPT,
    REJECT
};

inline Category char_to_cat(char const ch) {
    switch (ch) {
        case 'x':
            return Category::X;
        case 'm':
            return Category::M;
        case 'a':
            return Category::A;
        case 's':
            return Category::S;
    }
    return Category::X;
}

inline Operation char_to_op(char const ch) {
    switch (ch) {
        case '<':
            return Operation::LESS;
        case '>':
            return Operation::GREATER;
    }
    return Operation::LESS;
}

using Range = std::pair<int64_t, int64_t>;
struct Ranges {
    Range x_range;
    Range m_range;
    Range a_range;
    Range s_range;
};

using Rule_Result = std::variant<bool, Special, std::string>;
using Rule = std::function<Rule_Result(int64_t)>;
using Workflow = std::vector<Rule>;
using Workflow_Map = std::unordered_map<std::string, Workflow>;
using Range_Rule_Result = std::tuple<Ranges, std::string, Ranges>;
using Range_Rule = std::function<Range_Rule_Result(Ranges)>;
using Range_Workflow = std::vector<Range_Rule>;
using Range_Workflow_Map = std::unordered_map<std::string, Range_Workflow>;

struct Parts {
    std::vector<int64_t> x;
    std::vector<int64_t> m;
    std::vector<int64_t> a;
    std::vector<int64_t> s;
};

static Parts p;

int64_t get_range_length(Range const &r) {
    return r.second - r.first + 1;
}

Rule_Result determine_result(std::string_view const &res) {
    if (res == "A") {
        return Special::ACCEPT;
    }
    if (res == "R") {
        return Special::REJECT;
    }
    return Rule_Result(std::string {res});
}

Range adjust_range(Range const &current_range, Range const &other_range) {
    Range r {};
    // Disjoint ranges, should just return a range of size 0 because everything will be rejected
    if ((current_range.second < other_range.first) || (current_range.first > other_range.second)) {
        std::cout << "Ranges completely disjoint!\n";
        return std::make_pair(0, 0);
    }

    // Partial overlap from the left or complete overlap, new range starts at deeper point
    if (current_range.first >= other_range.first) {
        r.first = current_range.first;
    } else {
        r.first = other_range.first;
    }

    // Partial overlap from the right or complete overlap, new range ends at the closer point
    if (current_range.second >= other_range.second) {
        r.second = other_range.second;
    } else {
        r.second = current_range.second;
    }
    return r;
}

Rule_Result rule(int64_t part_index, Category c, Operation o, int64_t value_to_compare_against, std::string const &result) {
    
    int64_t rating {0};
    switch (c) {
        case Category::X:
            rating = p.x[part_index];
            break;
        case Category::M:
            rating = p.m[part_index];
            break;
        case Category::A:
            rating = p.a[part_index];
            break;
        case Category::S:
            rating = p.s[part_index];
            break;
    }

    bool const passed = (o == Operation::LESS) ? (rating < value_to_compare_against) : (rating > value_to_compare_against); 

    return passed ? determine_result(result) : Rule_Result(passed);
}

Range_Rule_Result range_rule(Ranges ranges, Category c, Operation o, int64_t value_to_compare_against, std::string const &result) {
    Range passed_range {};
    Range failed_range {};

    Range current_range {};
    switch (c) {
        case Category::X:
            current_range = ranges.x_range;
            break;
        case Category::M:
            current_range = ranges.m_range;
            break;
        case Category::A:
            current_range = ranges.a_range;
            break;
        case Category::S:
            current_range = ranges.s_range;
            break;
    }

    switch (o) {
        case Operation::LESS:
            passed_range = std::make_pair(1, value_to_compare_against - 1);
            failed_range = std::make_pair(value_to_compare_against, 4000);
            break;
        case Operation::GREATER:
            passed_range = std::make_pair(value_to_compare_against + 1, 4000);
            failed_range = std::make_pair(1, value_to_compare_against);
            break;
    }

    Range adjusted_passed_range {adjust_range(current_range, passed_range)};
    Range adjusted_failed_range {adjust_range(current_range, failed_range)};

    Ranges passed_ranges = ranges;
    Ranges failed_ranges = ranges;
    switch (c) {
        case Category::X:
            passed_ranges.x_range = adjusted_passed_range;
            failed_ranges.x_range = adjusted_failed_range;
            break;
        case Category::M:
            passed_ranges.m_range = adjusted_passed_range;
            failed_ranges.m_range = adjusted_failed_range;
            break;
        case Category::A:
            passed_ranges.a_range = adjusted_passed_range;
            failed_ranges.a_range = adjusted_failed_range;
            break;
        case Category::S:
            passed_ranges.s_range = adjusted_passed_range;
            failed_ranges.s_range = adjusted_failed_range;
            break;
    }
    return std::make_tuple(passed_ranges, result, failed_ranges);
}

bool apply_rules(Workflow_Map &w_m, Workflow const &w, int64_t const part_index) {
    for (auto &r: w) {
        auto const result {r(part_index)};

        // If A or R, immediately return the results
        if (std::holds_alternative<Special>(result)) {
            if (std::get<Special>(result) == Special::ACCEPT) return true;
            return false;
        }

        // If the rule failed, move on to the next one
        if (std::holds_alternative<bool>(result)) {
            if (!std::get<bool>(result)) continue;
        }

        // If the rule result is another workflow, then we return the result of that workflow
        if (std::holds_alternative<std::string>(result)) {
            auto const it = w_m.find(std::get<std::string>(result));
            if (it == w_m.end()) { 
                std::cerr << "ERROR, WORKFLOW NOT FOUND!\n";
                std::cerr << "WORKFLOW REQUESTED: " << std::get<std::string>(result) << "\n";
                return false;
            }
            
            Workflow wf {w_m[std::get<std::string>(result)]};
            return apply_rules(w_m, wf, part_index);
        }

        return false;
    }
    return true;
}

int64_t apply_range_rules(Ranges rs, Range_Workflow_Map &w_m, Range_Workflow const &w, int64_t workflow_index) {
    // This yields 2 ranges
    auto [rs_passed, passed_string, rs_failed] = w[workflow_index](rs);
    
    int64_t passed_num {};
    if (passed_string == "A") {
        // Multiply and return the ranges as a single int64_t
        passed_num = get_range_length(rs_passed.x_range) * get_range_length(rs_passed.a_range) * get_range_length(rs_passed.m_range) * get_range_length(rs_passed.s_range);
        // Magic value, means that there was no second range, it's the end
        if (rs_failed.x_range.first == 9999) {
            return passed_num;
        }
    } else if (passed_string == "R") {
        // Reject all parts in this range, effectively sum increases by 0
        passed_num = 0;
        if (rs_failed.x_range.first == 9999) {
            return passed_num;
        }
    } else {
        const auto wf {w_m[passed_string]};
        passed_num = apply_range_rules(rs_passed, w_m, wf, 0);
        if (rs_failed.x_range.first == 9999) {
            return passed_num;
        }
    }

    int64_t failed_num {apply_range_rules(rs_failed, w_m, w, workflow_index + 1)};

    return passed_num + failed_num;
}

Rule process_string_rule(std::string_view const &str) {
    auto iter = std::find_if(str.begin(), str.end(), [] (auto it) { return it == '<' || it == '>'; });
    // Final operation, return rule immediately
    if (iter == str.end()) {
        std::string workflow {str.begin(), str.end()};
        Rule r {};
        if (workflow == "A") {
            r = [] (int64_t part_index) { return Special::ACCEPT; };
            return r;
        } else if (workflow == "R") {
            r = [] (int64_t part_index) { return Special::REJECT; };
            return r;
        } else {
            r = [workflow] (int64_t part_index) { return workflow; }; 
            return r;
        }
    }
    Category c {char_to_cat(*(iter - 1))};
    Operation o {char_to_op(*iter)};
    int64_t value_to_compare_against {std::atoll(iter + 1)};
    auto colon_iter = std::find_if(iter, str.end(), [] (auto it) { return it == ':'; });
    std::string const rule_result {colon_iter + 1, str.end()};
    Rule r = [c, o, value_to_compare_against, rule_result] (int64_t index) { return rule(index, c, o, value_to_compare_against, rule_result); }; 
    return r;
}

Range_Rule process_string_range_rule(std::string_view const &str) {
    auto iter = std::find_if(str.begin(), str.end(), [] (auto it) { return it == '<' || it == '>'; });
    // Final operation, return rule immediately
    if (iter == str.end()) {
        std::string workflow {str.begin(), str.end()};
        Range_Rule r {};
        if (workflow == "A") {
            r = [] (Ranges const &r) {
                Ranges rs { std::make_pair(9999, 9999),
                            std::make_pair(9999, 9999),
                            std::make_pair(9999, 9999),
                            std::make_pair(9999, 9999)};
                return std::make_tuple(r, "A", rs);
            };
            return r;
        } else if (workflow == "R") {
            r = [] (Ranges const &r) {
                Ranges rs { std::make_pair(9999, 9999),
                            std::make_pair(9999, 9999),
                            std::make_pair(9999, 9999),
                            std::make_pair(9999, 9999)};
                return std::make_tuple(r, "R", rs);
            };
            return r;
        } else {
            r = [workflow] (Ranges const &r) { 
                Ranges rs { std::make_pair(9999, 9999),
                            std::make_pair(9999, 9999),
                            std::make_pair(9999, 9999),
                            std::make_pair(9999, 9999)};
                return std::make_tuple(r, workflow, rs);
            };
            return r;
        }
    }
    Category c {char_to_cat(*(iter - 1))};
    Operation o {char_to_op(*iter)};
    int64_t value_to_compare_against {std::atoll(iter + 1)};
    auto colon_iter = std::find_if(iter, str.end(), [] (auto it) { return it == ':'; });
    std::string const rule_result {colon_iter + 1, str.end()};
    Range_Rule r = [o, c, value_to_compare_against, rule_result] (Ranges r) { return range_rule(r, c, o, value_to_compare_against, rule_result); }; 
    return r;
}


void add_part_to_storage(std::string_view str) {
    std::string_view const ratings {str.begin() + 1, str.end() - 1};
    std::vector<int64_t> ratings_as_nums;
    for (auto const rating: std::views::split(ratings, ',')) { 
        ratings_as_nums.push_back(std::atoll(rating.begin() + 2));
    }
    p.x.push_back(ratings_as_nums[0]);
    p.m.push_back(ratings_as_nums[1]);
    p.a.push_back(ratings_as_nums[2]);
    p.s.push_back(ratings_as_nums[3]);
}

int main() {
    auto const begin {std::chrono::high_resolution_clock::now()};
    std::filesystem::path file_path {"/home/t.talik/Programming/cpp/aoc2023/d19/input.txt"};
    std::fstream file_handle {file_path};
    std::string line;
    Workflow_Map w_m {};
    Range_Workflow_Map r_w_m {};
    while (std::getline(file_handle, line)) {
        if (line == "") {
            break;
        }
        Workflow w {};
        Range_Workflow r_w {};
        auto it = line.begin();
        while (*it != '{') {
            ++it;
        }
        std::string_view rules {it + 1, line.end() - 1};
        for (const auto rule: std::views::split(rules, ',')) {
            auto processed_rule = process_string_rule(std::string_view {rule.begin(), rule.end()});
            auto processed_range_rule = process_string_range_rule(std::string_view {rule.begin(), rule.end()});
            w.push_back(processed_rule);
            r_w.push_back(processed_range_rule);
        }
        w_m.insert_or_assign(std::string {line.begin(), it}, w);
        r_w_m.insert_or_assign(std::string {line.begin(), it}, r_w);
    }

    while (std::getline(file_handle, line)) {
        add_part_to_storage(line);
    }

    int64_t sum {0};
    auto const wf {w_m["in"]};
    for (int64_t part_index = 0; part_index < p.x.size(); ++part_index) {
        if (apply_rules(w_m, wf, part_index)) {
            sum += p.x[part_index];
            sum += p.m[part_index];
            sum += p.a[part_index];
            sum += p.s[part_index];
        }
    }

    std::cout << sum << std::endl;
    Ranges rs { std::make_pair(1, 4000),
                std::make_pair(1, 4000),
                std::make_pair(1, 4000),
                std::make_pair(1, 4000)};
    std::cout << apply_range_rules(rs, r_w_m, r_w_m["in"], 0) << "\n";
    auto const end {std::chrono::high_resolution_clock::now()};
    std::cout << (end - begin).count() << " ns.\n";
    return 0;
}
