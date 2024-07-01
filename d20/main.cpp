#include <fstream>
#include <filesystem>
#include <iostream>
#include <queue>
#include <ranges>
#include <variant>
#include <vector>
#include <unordered_map>

int64_t LOW_PULSES_SENT = 0;
int64_t HIGH_PULSES_SENT = 0;
int64_t PULSES_SO_FAR = 0;

enum class FF_State: uint8_t {
    OFF = 0,
    ON
};

enum class Pulse_Type: uint8_t {
    LOW = 0,
    HIGH
};
using Processing_Queue = std::queue<std::tuple<int64_t, int64_t, Pulse_Type>>;

struct Broadcaster {
    std::vector<int64_t> modules;
    void propagate_signal(Processing_Queue &p_q, int64_t my_index, int64_t source_index, Pulse_Type p_t) {
        for (auto mod: modules) {
            LOW_PULSES_SENT++;
            p_q.push({mod, my_index, Pulse_Type::LOW});
        }
    }
};

struct Conjunction {
    std::vector<int64_t> modules;
    std::vector<int64_t> inputs;
    std::vector<Pulse_Type> remembered_pulses;
    void propagate_signal(Processing_Queue &p_q, int64_t my_index, int64_t source_index, Pulse_Type p_t) {
        int64_t input_idx = 0;
        for (int64_t idx = 0; idx < inputs.size(); ++idx) {
            if (inputs.at(idx) == source_index) {
                input_idx = idx;
                break;
            }
        }
        remembered_pulses.at(input_idx) = p_t;
        for (auto pulse: remembered_pulses) {
        }

        bool all_high = std::all_of(
                remembered_pulses.begin(),
                remembered_pulses.end(),
                [] (auto p_t) { return p_t == Pulse_Type::HIGH; });
        Pulse_Type pulse_to_send = all_high ? Pulse_Type::LOW : Pulse_Type::HIGH;
        for (auto mod: modules) {
            if (pulse_to_send == Pulse_Type::HIGH) {
                HIGH_PULSES_SENT++;
            } else {
                LOW_PULSES_SENT++;
            }
            p_q.push({mod, my_index, pulse_to_send});
        }
    }
};

struct Flip_Flop {
    std::vector<int64_t> modules;
    FF_State state = FF_State::OFF;
    void propagate_signal(Processing_Queue &p_q, int64_t my_index, int64_t source_index, Pulse_Type p_t) {
        if (p_t == Pulse_Type::HIGH) return;
        Pulse_Type pulse_to_send {};
        if (state == FF_State::OFF) {
            state = FF_State::ON;
            pulse_to_send = Pulse_Type::HIGH;
        } else {
            state = FF_State::OFF;
            pulse_to_send = Pulse_Type::LOW;
        }
        for (auto mod: modules) {
            if (pulse_to_send == Pulse_Type::HIGH) {
                HIGH_PULSES_SENT++;
            } else {
                LOW_PULSES_SENT++;
            }
            p_q.push({mod, my_index, pulse_to_send});
        }
    }
};

using Module = std::variant<Flip_Flop, Conjunction, Broadcaster>;

int32_t main() {

    std::filesystem::path file_path {"input.txt"};
    std::fstream file_handle {file_path};
    std::string line {};
    std::unordered_map<std::string, int64_t> module_name_to_index;

    int64_t free_index {0};
    std::vector<Module> modules {};
    std::vector<uint8_t> present_modules { 0 };
    while (std::getline(file_handle, line)) {
        Module new_mod {};
        int64_t current_mod_index = free_index;
        auto it {line.begin()};

        if (*it == '%' || *it == '&') {
            if (*it == '%') {
                new_mod = Flip_Flop {};
            } else {
                new_mod = Conjunction {};
            }
            std::string mod_name { it + 1, it + 3};

            if (module_name_to_index.contains(mod_name)) {
                current_mod_index = module_name_to_index.at(mod_name);
            } else {
                module_name_to_index.insert(
                        { mod_name,
                          free_index });
                current_mod_index = free_index;
                free_index++;
            }
            it += 7;
        } else {
            new_mod = Broadcaster {};
            module_name_to_index.insert(
                    { std::string {it, it + 11},
                      free_index });
            it += 15;
            current_mod_index = free_index;
            free_index++;
        }

        if (current_mod_index >= modules.size()) {
            modules.resize(free_index + 1);
            present_modules.resize(free_index + 1);
        }
        if (!present_modules.at(current_mod_index)) {
            modules.at(current_mod_index) = new_mod;
            present_modules.at(current_mod_index) = 1;
        }

        std::string_view module_list {it, line.end()};
        for (const auto mod: module_list | std::views::split(',')) {
            auto it = mod.begin();
            std::string mod_name;
            int64_t connected_mod_index = 0;

            if (*it == ' ') {
                std::string trimmed {it.base() + 1, it.base() + 3};
                mod_name = trimmed;
            } else {
                mod_name = {it.base(), it.base() + 2};
            }

            if (module_name_to_index.contains(mod_name)) {
                connected_mod_index = module_name_to_index.at(mod_name);
            } else {
                connected_mod_index = free_index;
                module_name_to_index.insert(
                        { mod_name,
                          free_index });
                free_index++;
            }
            std::visit([connected_mod_index] (auto&& arg) { arg.modules.push_back(connected_mod_index); },
                    modules.at(current_mod_index));
        }
    }

    for (int64_t idx = 0; idx < modules.size(); ++idx) {
        auto &mod = modules.at(idx);
        std::visit([&modules, idx] (auto&& arg) { 
            for (auto i: arg.modules) {
                if (std::holds_alternative<Conjunction>(modules.at(i))) {
                    std::visit([idx, i] (auto&& arg) { 
                            using T = std::decay_t<decltype(arg)>;
                            if constexpr (std::is_same_v<T, Conjunction>) {
                                arg.inputs.push_back(idx);
                            }
                        }, modules.at(i));
                }
            }
        }, mod);
    }

    int64_t broadcaster_index {};
    for (int64_t idx = 0; idx < modules.size(); ++idx) {
        auto &mod = modules.at(idx);
        if (std::holds_alternative<Conjunction>(mod)) {
            auto &m = std::get<Conjunction>(mod);
            m.remembered_pulses.resize(m.inputs.size());
            std::fill(m.remembered_pulses.begin(), m.remembered_pulses.end(), Pulse_Type::LOW);
        } else if (std::holds_alternative<Broadcaster>(mod)) {
            broadcaster_index = idx;
        }
    }

    Processing_Queue p_q {};
    int64_t lowest_required_pulses {};
    for (int pulses = 0; pulses < 10000000; ++pulses) {
        LOW_PULSES_SENT++;
        PULSES_SO_FAR++;
        p_q.push({broadcaster_index, -1, Pulse_Type::LOW});
        while (!p_q.empty()) {
            auto front = p_q.front();
            p_q.pop();
            auto &mod = modules.at(std::get<0>(front));
            if (std::get<0>(front) == module_name_to_index.at("rx") &&
                    std::get<2>(front) == Pulse_Type::LOW) {
                std::cout << "FOUND: " << pulses << "\n";
                break;
            }
            std::visit([&p_q, &front] (auto&& arg) {
                    arg.propagate_signal(p_q, std::get<0>(front), std::get<1>(front), std::get<2>(front)); },
                    mod);
        }
        if (pulses == 999) {
            std::cout << HIGH_PULSES_SENT << " " << LOW_PULSES_SENT << std::endl;
            std::cout << "Result : " << LOW_PULSES_SENT * HIGH_PULSES_SENT << std::endl;
        }
    }
    std::cout << lowest_required_pulses << std::endl;
}
