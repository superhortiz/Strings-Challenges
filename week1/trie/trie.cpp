#include <string>
#include <iostream>
#include <vector>
#include <map>

using std::map;
using std::vector;
using std::string;

typedef map<char, int> edges;
typedef vector<edges> trie;

trie build_trie(vector<string>& patterns) {
    // Initialize data structure
    trie t;

    // Add empty mapping to the root node 0
    edges edge;
    t.push_back(edge);

    // Iterate over the patterns
    for (const auto& pattern : patterns) {
        // Start from root
        int node = 0;

        // Iterate over the characters in pattern
        for (const auto& c : pattern) {
            // If node does not have an edge to char c, add a new node and include it
            if (t[node].count(c) == 0) {
                int new_node = t.size(); // Add the new node at the end
                t[node][c] = new_node; // Add edge to the new node
                t.push_back(edge); // Add the new node
                node = new_node; // Update node

            } else {
                // Move on to the next node
                node = t[node][c];
            }
        }
    }
    return t;
}

int main() {
    size_t n;
    std::cin >> n;
    vector<string> patterns;
    for (size_t i = 0; i < n; i++) {
        string s;
        std::cin >> s;
        patterns.push_back(s);
    }

    trie t = build_trie(patterns);
    for (size_t i = 0; i < t.size(); ++i) {
        for (const auto &j : t[i]) {
            std::cout << i << "->" << j.second << ":" << j.first << "\n";
        }
    }

    return 0;
}