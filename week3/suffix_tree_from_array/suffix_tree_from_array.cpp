#include <algorithm>
#include <cstdio>
#include <map>
#include <string>
#include <utility>
#include <vector>
#include <iostream>

using std::make_pair;
using std::map;
using std::pair;
using std::string;
using std::vector;

// Data structure to store edges of a suffix tree.
struct Edge {
    // The ending node of this edge.
    int node;
    // Starting position of the substring of the text 
    // corresponding to the label of this edge.
    int start;
    // Position right after the end of the substring of the text 
    // corresponding to the label of this edge.
    int end;

    Edge(int node_, int start_, int end_) : node(node_), start(start_), end(end_) {}
    Edge(const Edge& e) : node(e.node), start(e.start), end(e.end) {}
};

void create_new_leaf(map<int, vector<Edge>>& tree, const string& text, int& node, int& depth, int& node_counting, int suffix) {
    // Get the size of the text
    int n = text.size();

    // Get the ID of the leaf node and update the node counting
    int leaf_node = ++node_counting;

    // Get the properties of the edge and create the corresponding edge
    int start = suffix + depth;
    int end = n;
    Edge edge = Edge(leaf_node, start, end);

    // Add the edge to the tree
    if (tree.count(node) == 0) {
        vector<Edge> new_edges;
        tree[node] = new_edges;
    }
    tree[node].push_back(edge);

    // Update current node and depth
    node = leaf_node;
    depth = n - suffix;
}

void create_mid_node(map<int, vector<Edge>>& tree, const string& text, int& node, int& depth, int& node_counting, int edge_start, int offset) {
    // Get the ID of the mid node and update the node counting
    int mid_node = ++node_counting;

    // Get the properties of the edge and create the corresponding edge
    int end = edge_start + offset;
    Edge new_edge = Edge(mid_node, edge_start, end);

    // Get the existing edge to break and remove from node's children
    Edge existing_edge = tree[node].back();
    tree[node].pop_back();

    // Add the new edge as a node's child
    tree[node].push_back(new_edge);

    // Update the existing edge and add to mid node's children
    existing_edge.start += offset;
    vector<Edge> edges;
    edges.push_back(existing_edge);
    tree[mid_node] = edges;

    // Update current node and depth
    node = mid_node;
    depth += offset;
}

// Build suffix tree of the string text given its suffix array suffix_array
// and LCP array lcp_array. Return the tree as a mapping from a node ID
// to the vector of all outgoing edges of the corresponding node. The edges in the
// vector must be sorted in the ascending order by the first character of the edge label.
// Root must have node ID = 0, and all other node IDs must be different
// nonnegative integers.
//
// For example, if text = "ACACAA$", an edge with label "$" from root to a node with ID 1
// must be represented by Edge(1, 6, 7). This edge must be present in the vector tree[0]
// (corresponding to the root node), and it should be the first edge in the vector 
// (because it has the smallest first character of all edges outgoing from the root).
map<int, vector<Edge>> SuffixTreeFromSuffixArray(const vector<int>& suffix_array, const vector<int>& lcp_array, const string& text) {
    // Define a mapping to store the tree
    map<int, vector<Edge>> tree;

    // Define a vector to use as a stack to traverse the tree
    vector<pair<int, int>> stack(1, make_pair(0, 0));

    // Define variables
    int n = suffix_array.size();  // Size of the text
    int lcp_prev = 0;  // Longest common prefix between current suffix and previous suffix
    int curr_node = 0;  // Current node that is being processed
    int depth = 0;  // Depth of the current node in the tree
    int node_counting = 0;  // Counter to add new nodes. Node 0 is considered to be the root

    // Iterate over all the suffixes
    for (int i = 0; i < n; ++i) {
        // Get the current suffix in lexicographical order
        int suffix = suffix_array[i];

        // Go up the tree until getting depth <= lcpPrev
        while (depth > lcp_prev) {
            stack.pop_back();
            pair<int, int> back = stack.back();
            curr_node = back.first;
            depth = back.second;
        }

        if (depth == lcp_prev) {
            // Create leaf node
            create_new_leaf(tree, text, curr_node, depth, node_counting, suffix);

            // Add the created node to the stack
            stack.push_back(make_pair(curr_node, depth));

        } else {
            // Get the edge start and offset to break the existing edge
            int edge_start = suffix_array[i - 1] + depth;
            int offset = lcp_prev - depth;

            // Create mid node and add to the stack
            create_mid_node(tree, text, curr_node, depth, node_counting, edge_start, offset);
            stack.push_back(make_pair(curr_node, depth));

            // Create leaf node and add to the stack
            create_new_leaf(tree, text, curr_node, depth, node_counting, suffix);
            stack.push_back(make_pair(curr_node, depth));
        }

        // Update the lcp value
        if (i < n - 1) {
            lcp_prev = lcp_array[i];
        }
    }
    return tree;
}

int main() {
    char buffer[200001];
    scanf("%s", buffer);
    string text = buffer;
    vector<int> suffix_array(text.length());
    for (int i = 0; i < text.length(); ++i) {
        scanf("%d", &suffix_array[i]);
    }
    vector<int> lcp_array(text.length() - 1);
    for (int i = 0; i + 1 < text.length(); ++i) {
        scanf("%d", &lcp_array[i]);
    }
    // Build the suffix tree and get a mapping from 
    // suffix tree node ID to the list of outgoing Edges.
    map<int, vector<Edge>> tree = SuffixTreeFromSuffixArray(suffix_array, lcp_array, text);
    printf("%s\n", buffer);

    // Output the edges of the suffix tree in the required order.
    // Note that we use here the contract that the root of the tree
    // will have node ID = 0 and that each vector of outgoing edges
    // will be sorted by the first character of the corresponding edge label.
    //
    // The following code avoids recursion to avoid stack overflow issues.
    // It uses a stack to convert recursive function to a while loop.
    // The stack stores pairs (node, edge_index). 
    // This code is an equivalent of 
    //
    //    OutputEdges(tree, 0);
    //
    // for the following _recursive_ function OutputEdges:
    //
    // void OutputEdges(map<int, vector<Edge> > tree, int node_id) {
    //   const vector<Edge>& edges = tree[node_id];
    //   for (int edge_index = 0; edge_index < edges.size(); ++edge_index) {
    //     printf("%d %d\n", edges[edge_index].start, edges[edge_index].end);
    //     OutputEdges(tree, edges[edge_index].node);
    //   }
    // }
    //
    vector<pair<int, int>> stack(1, make_pair(0, 0));

    while (!stack.empty()) {
        pair<int, int> p = stack.back();
        stack.pop_back();
        int node = p.first;
        int edge_index = p.second;
        if (!tree.count(node)) {
            continue;
        }
        const vector<Edge>& edges = tree[node];
        if (edge_index + 1 < edges.size()) {
            stack.push_back(make_pair(node, edge_index + 1));
        }
        printf("%d %d\n", edges[edge_index].start, edges[edge_index].end);
        stack.push_back(make_pair(edges[edge_index].node, 0));
    }

    return 0;
}
