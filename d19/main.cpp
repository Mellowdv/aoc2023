#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <ranges>

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

using Rule_Result = std::variant<bool, Special, std::string>;
using Rule = std::function<Rule_Result(int64_t)>;
using Workflow = std::vector<Rule>;
using Workflow_Map = std::unordered_map<std::string, Workflow>;

struct Parts {
    std::vector<int64_t> x;
    std::vector<int64_t> m;
    std::vector<int64_t> a;
    std::vector<int64_t> s;
};

static Parts p;

bool rule(int64_t rating, Category c, Operation o, int64_t value_to_compare_against) {
    return true;
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
                return false;
            }
            Workflow wf {w_m[std::get<std::string>(result)]};
            return apply_rules(w_m, wf, part_index);
        }

        return false;
    }
    return true;
}

Rule process_string_rule(const std::string_view str, Workflow_Map w_m) {
    auto iter = std::find_if(str.begin(), str.end(), [] (auto it) { return it == '<' || it == '>'; });
    // Final operation, return rule immediately
    if (iter == str.end()) {
        std::string_view workflow {str.begin(), str.end()};
        Rule r {};
        if (workflow == "A") {
            r = [] (int64_t part_index) { return Special::ACCEPT; };
        } else if (workflow == "R") {
            r = [] (int64_t part_index) { return Special::REJECT; };
        } else {
            r = [&workflow] (int64_t part_index) { return std::string {workflow}; }; 
        }
    }
    Category c {char_to_cat(*(iter - 1))};
    Operation o {char_to_op(*iter)};
    int64_t value_to_compare_against {std::atoll(iter + 1)};
    auto colon_iter = std::find_if(iter, str.end(), [] (auto it) { return it == ':'; });
    std::string rule_result {colon_iter + 1, str.end()};
    Rule r = [c, o, value_to_compare_against] (int64_t index) { return rule(index, c, o, value_to_compare_against); }; 
    return r;
}

int main() {
    std::filesystem::path file_path {"/home/t.talik/Programming/cpp/aoc2023/d19/input.txt"};
    std::fstream file_handle {file_path};
    std::string line;
    Workflow_Map w_m {};
    while (std::getline(file_handle, line)) {
        Workflow w {};
        auto it = line.begin();
        while (*it != '{') {
            ++it;
        }
        std::string_view rules {it + 1, line.end() - 1};
        for (const auto rule: std::views::split(rules, ',')) {
            auto processed_rule = process_string_rule(std::string_view {rule.begin(), rule.end()}, w_m);
            w.push_back(processed_rule);
        }
    }
    return 0;
}
