#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <cassert>

using std::cin;
using std::cout;
using std::endl;
using std::map;
using std::string;
using std::vector;

int const Letters =    5;
int const NA      =   -1;

struct Node {
    int next[Letters]; // Array to store indices of child nodes for each letter
    int string_start, string_length; // Start index and length of the substring represented by the edge
    bool has_type, leaf, non_branching; // Flags to cache node type information

    // Constructor to initialize a Node
    Node(int i) : string_start(i), string_length(1), has_type(false), leaf(false), non_branching(false) {
        std::fill(next, next + Letters, NA);
    }

    // Function to determine and cache the node type (leaf, non-branching, or branching)
    void get_type() {
        int count = 0;
        for (int i = 0; i < Letters; ++i) {
            if (next[i] != NA) count++;
        }
        has_type = true;
        if (count == 0) {
            leaf = true;
        } else if (count == 1) {
            non_branching = true;
        }
    }

    // Function to check if the node is a leaf
    bool is_leaf() {
        if (!has_type) get_type();
        return leaf;
    }

    // Function to check if the node is non-branching
    bool is_non_branching() {
        if (!has_type) get_type();
        return non_branching;
    }
};

// Function to convert a character to its corresponding index
int letterToIndex(char letter) {
	switch (letter) {
		case 'A': return 0; break;
		case 'C': return 1; break;
		case 'G': return 2; break;
		case 'T': return 3; break;
        case '$': return 4; break;
		default: assert (false); return -1;
	}
}

vector<Node> build_trie(const string& text) {
	// Initialize data structure
	vector<Node> trie;

	// Add root node to the trie
	trie.push_back(Node(NA));

    // Reserve space to avoid reallocations
    trie.reserve(text.size() * text.size());

	// Iterate over the suffixes of the input text
	for (int start_suffix = 0; start_suffix < text.size(); ++start_suffix) {
		// Start from the root node for each suffix
		int node = 0;

		// Iterate over the characters in the current suffix
		for (int i = start_suffix; i < text.size(); ++i) {
            // Convert the current character to its corresponding index
			int index = letterToIndex(text[i]);

            // Get a reference to the next node in the trie
            int& next_node = trie[node].next[index];

            // If there is no edge from the current node to the next character, create a new node
            if (next_node == NA) {
                int new_node = trie.size(); // Get the index for the new node
                next_node = new_node; // Add an edge from the current node to the new node
                trie.push_back(Node(i)); // Add the new node to the trie, setting the string_start to the current index i
                node = new_node; // Move to the newly created node
            } else {
                // If an edge exists, move to the next node
                node = next_node;
            }
		}
	}
    
    // Return the constructed trie
	return trie;
}

void compress_node(vector<Node>& trie, Node& node, const int edge) {
    // Get a reference to the child node of the current node along the given edge
    int& index_child = node.next[edge];
    Node& child_node = trie[index_child];

    // If the child node is a leaf, there's nothing to compress, so return
    if (child_node.is_leaf()) return;

    // Iterate while the child node has only one child (is non-branching)
    while (child_node.is_non_branching()) {
        // Get the index of the grandchild node (the single child of the child node)
        int index_grandchild = -1;
        for (int i = 0; i < Letters; ++i) {
            if (child_node.next[i] != NA) {
                index_grandchild = child_node.next[i];
            }
        }

        // Get a reference to the grandchild node
        Node& grandchild_node = trie[index_grandchild];

        // Update the string_start and string_length of the grandchild node
        // to reflect the compressed path
        grandchild_node.string_start = child_node.string_start;
        grandchild_node.string_length = child_node.string_length + 1;

        // Update the index_child and child_node to point to the grandchild
        // effectively compressing the path
        index_child = index_grandchild;
        child_node = grandchild_node;
    }
}

void build_tree(const int index_node, vector<Node>& trie, vector<string>& result, const string& text) {
    // Get a reference to the current node in the trie
    Node& node = trie[index_node];

    // If the current node is a leaf, add its substring to the result and return
    if (node.is_leaf()) {
        result.push_back(text.substr(node.string_start, node.string_length));
        return;

    // If the current node is not non-branching and has a valid string_start, add its substring to the result
    } else if (!node.is_non_branching() && node.string_start != NA) {
        result.push_back(text.substr(node.string_start, node.string_length));
    }

    // Iterate through the child nodes of the current node
    for (int i = 0; i < Letters; ++i) {
        // If there is a child node along the edge represented by index i
        if (node.next[i] != NA) {
            // Compress the path from the current node to the child node
            compress_node(trie, node, i);

            // Recursively build the tree from the child node
            build_tree(node.next[i], trie, result, text);
        }
    }
}

// Build a suffix tree of the string text and return a vector
// with all of the labels of its edges (the corresponding 
// substrings of the text) in any order.
vector<string> ComputeSuffixTreeEdges(const string& text) {
    // Initialize a vector to store the edge labels (substrings)
    vector<string> result;

    // Build the suffix trie from the input text
    vector<Node> trie = build_trie(text);

    // Build the suffix tree from the trie, populating the result vector with edge labels
    build_tree(0, trie, result, text);

    // Return the vector of edge labels
    return result;
}

int main() {
    string text;
    cin >> text;
    vector<string> edges = ComputeSuffixTreeEdges(text);
    for (int i = 0; i < edges.size(); ++i) {
        cout << edges[i] << endl;
    }
    return 0;
}
