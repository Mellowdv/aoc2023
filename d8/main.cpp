#include <fstream>
#include <filesystem>
#include <iostream>
#include <numeric>
#include <unordered_map>
#include <vector>

struct Node {
    std::string left;
    std::string right;
};
using Network = std::unordered_map<std::string, Node>;

std::pair<Network, std::vector<std::string>> build_network(std::fstream &file_handle) {
    Network network {};
    std::vector<std::string> starting_nodes;
    std::string line {};
    while (std::getline(file_handle, line)) {

        std::stringstream sstream {line};
        
        std::string tmp {};
        std::string node_name {};
        std::string left {};
        std::string right {};

        sstream >> node_name;
        if (*(node_name.end() - 1) == 'A') {
            starting_nodes.push_back(std::string(node_name));
        }
        sstream >> tmp;
        sstream >> tmp;
        left = std::string(tmp.begin() + 1, tmp.end() - 1);
        sstream >> tmp;
        right = std::string(tmp.begin(), tmp.end() - 1);
        network.emplace(node_name, Node {left, right}); 
    }
    return std::make_pair(network, starting_nodes);
}

int32_t traverse_network(Network const &n, std::string_view const &directions) {
    std::string current_node {"AAA"};
    std::string goal {"ZZZ"};

    auto d_iter {directions.begin()};
    int32_t num_of_steps {};
    while (current_node != "ZZZ") {
        if (d_iter == directions.end()) {
            d_iter = directions.begin();
        }
        if (*d_iter == 'L') {
            current_node = n.at(current_node).left;
        } else {
            current_node = n.at(current_node).right;
        }
        ++num_of_steps;
        ++d_iter;
    }
    return num_of_steps;
}

int32_t ghost_the_network(std::string_view const &starting_node, Network const &n, std::string const &directions) {
    std::string current_node {starting_node};

    auto d_iter {directions.begin()};
    int32_t num_of_steps {};
    while (current_node.at(2) != 'Z') {
        if (d_iter == directions.end()) {
            d_iter = directions.begin();
        }
        if (*d_iter == 'L') {
            current_node = n.at(current_node).left;
        } else {
            current_node = n.at(current_node).right;
        }
        ++num_of_steps;
        ++d_iter;
    }
    return num_of_steps;
}

int main() {

    std::filesystem::path file_path {"/home/t.talik/Programming/aoc2023/d8/input.txt"};
    std::fstream file_handle {file_path};
    
    std::string line {};

    std::getline(file_handle, line);
    std::string directions {line};
    std::getline(file_handle, line);
    auto network_and_starting_nodes = build_network(file_handle);

    Network &network = network_and_starting_nodes.first;
    int32_t result = traverse_network(network, directions);

    int32_t result2 {};
    auto starting_nodes = network_and_starting_nodes.second;
    std::vector<int32_t> results2 {};
    for (auto const &node: starting_nodes) {
        result2 = ghost_the_network(node, network, directions);
        results2.push_back(result2); 
    }
    int64_t lcm = std::lcm(results2.at(0), results2.at(1)); 
   
    for (int32_t i = 2; i < results2.size(); ++i) {
        lcm = std::lcm(lcm, results2.at(i));
    }
    std::cout << result << std::endl;
    std::cout << lcm << std::endl;

    return 0;
}
