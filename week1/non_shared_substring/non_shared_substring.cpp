#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <cassert>
#include <queue>

using namespace std;

int const Letters =    6;
int const NA      =   -1;

struct Node {
    int next[Letters]; // Array to store indices of child nodes for each letter
    int string_start, string_length, start_suffix; // Start index and length of the substring represented by the edge
    bool has_type, leaf, non_branching; // Flags to cache node type information
    bool yellow;

    // Constructor to initialize a Node
    Node(int start, int len) : string_start(start), string_length(len), start_suffix(NA), has_type(false), leaf(false), non_branching(false), yellow(false) {
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
		case '#': return 5; break;
		default: assert (false); return -1;
	}
}

vector<Node> build_trie(const string& text) {
	// Initialize data structure
	vector<Node> trie;

	// Add root node to the trie
	trie.push_back(Node(NA, 0));

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
                trie.push_back(Node(i, 1)); // Add the new node to the trie, setting the string_start to the current index i
                node = new_node; // Move to the newly created node
            } else {
                // If an edge exists, move to the next node
                node = next_node;
            }

			// Store in the last node the starting position of the suffix
			if (i == text.size() - 1) {
				trie[node].start_suffix = start_suffix;
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

void build_tree(const int index_node, vector<Node>& trie, const string& text, const int n) {
    // Get a reference to the current node in the trie
    Node& node = trie[index_node];

    // If the current node is a leaf, check if the suffix is just in text2 (yellow node) and return
    if (node.is_leaf()) {
        if (node.string_start > n) node.yellow = true;
        return;
    }

    // Flag to check if any child of node is just in text2
    bool just_in_text2 = false;

    // Iterate through the child nodes of the current node
    for (int i = 0; i < Letters; ++i) {
        // If there is a child node along the edge represented by index i
        if (node.next[i] != NA) {
            // Compress the path from the current node to the child node
            compress_node(trie, node, i);

            // Recursively build the tree from the child node
            build_tree(node.next[i], trie, text, n);

            // Get if any child of node is just in text2
            if (trie[node.next[i]].yellow) just_in_text2 = true;
        }
    }

    // Mark the node if it is just in text2
    if (just_in_text2) node.yellow = true;
}

// Function to check tree construction
void check_tree(const int parent, const int index_node, const vector<Node>& tree, const string& text, const int len) {
	Node node = tree[index_node];
	if (node.string_start != NA) {
		cout << parent << "->" << index_node << ": t = " << text.substr(node.string_start, node.string_length);
		if (node.yellow) {
            cout << ", yellow";
		}
		cout << endl;
	}

	if (node.is_leaf()) return;

	for (int i = 0; i < Letters; ++i) {
        if (node.next[i] != NA) {
            check_tree(index_node, node.next[i], tree, text, len);
        }
    }
}

void get_shortest_non_shared(const int index_node, const vector<Node>& tree, const string& text, const int n, string& result, int len) {
    // Get the current node in the suffix tree
    Node node = tree[index_node];

    // Red nodes: Suffixes starting at or after the '#' delimiter.
    // These represent substrings exclusively within the second input string (text2), so we skip them.
    if (node.string_start == n) {
        return;

    // Yellow nodes: Nodes with a path leading directly to characters from the second input string (text2).
    // While not the solution themselves, they may lie on a path to a valid solution node.
    } else if (node.yellow) {
        len += node.string_length;
    
    // Green nodes: Nodes representing substrings found in the first input string (text1) but not exclusively in text2.
    // The path from the root to the first character of this substring is a candidate solution.
    } else {
        // Update the shortest non-shared substring if a shorter one is found.
        if (result.size() > ++len) {
            int start = node.string_start - len + 1;
            result = text.substr(start, len);
        }
        // Stop further exploration from this node, as we've found a valid solution.
        return;
    }

    // Recursively explore the children nodes of the current node
    for (int i = 0; i < Letters; ++i) {
        if (node.next[i] != NA) {
            get_shortest_non_shared(node.next[i], tree, text, n, result, len);
        }
    }
}

string solve(string p, string q) {
    // Get the complete text we will use to create the suffix tree
    int n = p.size();
	string text = p + '#' + q + '$';

	// Build the suffix tree from the input text
	vector<Node> trie = build_trie(text);
	build_tree(0, trie, text, n);

    // check_tree(-1, 0, trie, text, n);

    // Get the shortest non shared substring
    string result = p;
    get_shortest_non_shared(0, trie, text, n, result, 0);

	return result;
}

int main(void) {
	string p;
	cin >> p;
	string q;
	cin >> q;
	string ans = solve (p, q);
	cout << ans << endl;
	return 0;
}
