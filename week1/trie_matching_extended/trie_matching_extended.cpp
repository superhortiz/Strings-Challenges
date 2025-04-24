#include <algorithm>
#include <cassert>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

int const Letters =    4;
int const NA      =   -1;

struct Node {
	int next[Letters];
	bool patternEnd;

	Node() {
		fill(next, next + Letters, NA);
		patternEnd = false;
	}
};

int letterToIndex(char letter) {
	switch (letter) {
		case 'A': return 0; break;
		case 'C': return 1; break;
		case 'G': return 2; break;
		case 'T': return 3; break;
		default: assert (false); return -1;
	}
}

vector<Node> build_trie(const vector<string>& patterns) {
	// Initialize data structure
	vector<Node> trie;

	// Add root node to the trie
	trie.push_back(Node());

	// Iterate over the patterns
	for (const auto& pattern : patterns) {
		// Start from root
		int node = 0;

		// Iterate over the characters in pattern
		for (int i = 0; i < pattern.size(); ++i) {
			int index = letterToIndex(pattern[i]);

			// If node does not have an edge to char c, add a new node and include it
			if (trie[node].next[index] == NA) {
				int new_node = trie.size(); // Add the new node at the end
				trie[node].next[index] = new_node; // Add edge to the new node
				trie.push_back(Node()); // Add the new node
				node = new_node; // Update node

			} else {
				// Move on to the next node
				node = trie[node].next[index];
			}

			// If the pattern is complete, mark the node
			if (i == pattern.size() - 1) {
				trie[node].patternEnd = true;
			}
		}
	}
	return trie;
}

vector<int> solve(string text, int n, vector <string> patterns) {
	// Initialize data structures
	vector<int> result;
	vector<Node> trie = build_trie(patterns);

	// Iterate over the text
	for (int i = 0; i < text.size(); ++i) {
		// Start from root
		int node = 0;

		// Traverse the trie
		for (int j = i; j < text.size(); ++j) {
			char c = text[j];
			// If there is a coincidence continue comparing
			if (trie[node].next[letterToIndex(c)] != NA) {
				node = trie[node].next[letterToIndex(text[j])];

			// If there is no coincidence, stop comparing
			} else {
				break;
			}

			// If the last updated node is the end of a pattern, then add to the solution
			if (trie[node].patternEnd) {
				result.push_back(i);
				break;
			}
		}
	}
	return result;
}

int main(void) {
	string t;
	cin >> t;

	int n;
	cin >> n;

	vector<string> patterns (n);
	for (int i = 0; i < n; i++)	{
		cin >> patterns[i];
	}

	vector<int> ans;
	ans = solve(t, n, patterns);

	for (int i = 0; i < ans.size(); i++)	{
		cout << ans[i];
		if (i + 1 < ans.size()) {
			cout << " ";
		} else {
			cout << endl;
		}
	}
	return 0;
}
