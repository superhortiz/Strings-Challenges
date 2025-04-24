#include <algorithm>
#include <cstdio>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

using std::cin;
using std::istringstream;
using std::map;
using std::string;
using std::vector;

int const Letters = 5;

int letter_to_index(char letter) {
	switch (letter) {
		case 'A': return 0; break;
		case 'C': return 1; break;
		case 'G': return 2; break;
		case 'T': return 3; break;
        case '$': return 4; break;
		default: return -1; break;
	}
}

char index_to_letter(int index) {
	switch (index) {
		case 0: return 'A'; break;
		case 1: return 'C'; break;
		case 2: return 'G'; break;
		case 3: return 'T'; break;
        case 4: return '$'; break;
		default: return -1; break;
	}
}

// Preprocess the Burrows-Wheeler Transform bwt of some text
// and compute as a result:
//   * starts - for each character C in bwt, starts[C] is the first position 
//       of this character in the sorted array of 
//       all characters of the text.
//   * occ_count_before - for each character C in bwt and each position P in bwt,
//       occ_count_before[C][P] is the number of occurrences of character C in bwt
//       from position 0 to position P inclusive.
void PreprocessBWT(const string& bwt, 
                   map<char, int>& starts, 
                   map<char, vector<int>>& occ_count_before) {
    
    // Get the size
    int n = bwt.size();

    // Initialize occ_count_before vectors
    for (int i = 0; i < Letters; ++i) {
        occ_count_before[index_to_letter(i)] = vector<int>(n + 1, 0);
    }

    // Iterate over the characters of bwt
    for (int i = 0; i < n; ++i) {
        // Iterate over the possible characters
        for (int j = 0; j < Letters; ++j) {
            // Copy previous results
            char curr_char = index_to_letter(j);
            occ_count_before[curr_char][i + 1] = occ_count_before[curr_char][i];
        }

        // Update count if the character i of bwt
        occ_count_before[bwt[i]][i + 1]++;
    }

    // Get the first column of the suffixes
    string first_column = bwt;
    std::sort(first_column.begin(), first_column.end());

    // Populate starts
    for (int i = 0; i < n; ++i) {
        starts.insert(std::make_pair(first_column[i], i));
    }
}

// Compute the number of occurrences of string pattern in the text
// given only Burrows-Wheeler Transform bwt of the text and additional
// information we get from the preprocessing stage - starts and occ_counts_before.
int CountOccurrences(const string& pattern, 
                     const string& bwt, 
                     const map<char, int>& starts, 
                     const map<char, vector<int>>& occ_count_before) {
    
    // Define pointers
    int top = 0;
    int bottom = bwt.size() - 1;
    int i = pattern.size() - 1;

    // Iterate while we have possible matchings
    while (top <= bottom) {
        // Go through the pattern in backwards order
        if (i >= 0) {
            // Get the current symbol
            char symbol = pattern[i--];

            // If symbol is not in bwt then there is no a possible match
            if (starts.count(symbol) == 0) return 0;

            // Update pointers
            top = starts.at(symbol) + occ_count_before.at(symbol)[top];
            bottom = starts.at(symbol) + occ_count_before.at(symbol)[bottom + 1] - 1;

        // There are no more characters to check, return solution
        } else {
            return bottom - top + 1;
        }
    }

    // No matches were found
    return 0;
}

int main() {
    string bwt;
    cin >> bwt;
    int pattern_count;
    cin >> pattern_count;

    // First occurrence of each character in the sorted list of characters of bwt
    map<char, int> starts;

    // Occurrence counts for each character and each position in bwt
    map<char, vector<int>> occ_count_before;

    // Preprocess the BWT once to get starts and occ_count_before.
    // For each pattern, we will then use these precomputed values and
    // spend only O(|pattern|) to find all occurrences of the pattern
    // in the text instead of O(|pattern| + |text|).
    PreprocessBWT(bwt, starts, occ_count_before);

    // Get the pattern to find and return the result
    for (int pi = 0; pi < pattern_count; ++pi) {
        string pattern;
        cin >> pattern;
        int occ_count = CountOccurrences(pattern, bwt, starts, occ_count_before);
        printf("%d ", occ_count);
    }
    printf("\n");
    return 0;
}
