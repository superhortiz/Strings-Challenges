#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

using std::cin;
using std::string;
using std::vector;

vector<int> get_prefix_function(string p) {
    // initialize data structure
    int n = p.size();
    vector<int> s(n);

    // Define starting values
    s[0] = 0;
    int border = 0;
    
    for (int i = 1; i < n; ++i) {
        // Explore valid borders until we find that the last character of the border is equal to p[i]
        while (border > 0 && p[i] != p[border]){
            border = s[border - 1];
        }

        // If we have a coincidence, we can make bigger the current border
        if (p[i] == p[border]) {
            border++;

        // If we dont have a coincidence, then we need to start from 0
        } else {
            border = 0;
        }

        // Store the current longest border in the s vector
        s[i] = border;
    }
    return s;
}

// Find all occurrences of the pattern in the text and return a
// vector with all positions in the text (starting from 0) where 
// the pattern starts in the text.
vector<int> find_pattern(const string& pattern, const string& text) {
    // Initialize data structure
    vector<int> result;
    int p = pattern.size();
    int t = text.size();

    // Get the complete string we will the the prefix function
    string complete_string = pattern + '$' + text;

    // Get the prefix function
    vector<int> s = get_prefix_function(complete_string);

    // Iterate over the text looking for matches
    for (int i = p + 1; i <= p + t; ++i) {
        // If the value of the prefix function is equal to the 
        // length of the pattern, then we have found a match
        if (s[i] == p) {
            result.push_back(i - 2 * p);
        }
    }
    return result;
}

int main() {
    string pattern, text;
    cin >> pattern;
    cin >> text;
    vector<int> result = find_pattern(pattern, text);
    for (int i = 0; i < result.size(); ++i) {
        printf("%d ", result[i]);
    }
    printf("\n");
    return 0;
}
