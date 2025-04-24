#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <utility>

using std::cin;
using std::cout;
using std::endl;
using std::make_pair;
using std::pair;
using std::string;
using std::vector;

// Text contains symbols A, C, G, T only and the ending symbol “$” 
int const Letters = 5;

int letter_to_index(char letter) {
	switch (letter) {
        case '$': return 0; break;
		case 'A': return 1; break;
		case 'C': return 2; break;
		case 'G': return 3; break;
		case 'T': return 4; break;
		default: return -1; break;
	}
}

vector<int> SortCharacters(const string& text) {
    // Define auxiliar vectors
    int n = text.size();
    vector<int> order(n);
    vector<int> count(Letters, 0);

    // Count occurrences of each character in text
    for (int i = 0; i < n; ++i) {
        count[letter_to_index(text[i])]++;
    }

    // Get the cumulative counting
    for (int j = 1; j < Letters; ++j) {
        count[j] += count[j - 1];
    }

    // Backward for loop, it gets the ordered position for
    // each character in text
    for (int i = n - 1; i >= 0; --i) {
        order[--count[letter_to_index(text[i])]] = i;
    }

    return order;
}

vector<int> ComputeCharClasses(const string& text, vector<int>& order) {
    // Define vector to store the classes
    int n = text.size();
    vector<int> classes(n);

    // Initialize the lowest character with value 0
    classes[order[0]] = 0;

    // Iterate over the characters in text
    for (int i = 1; i < n; ++i) {
        // If we have a new class, increment by one
        if (text[order[i]] != text[order[i - 1]]) {
            classes[order[i]] = classes[order[i - 1]] + 1;

        // If we have the same class, copy from the previous
        } else {
            classes[order[i]] = classes[order[i - 1]];
        }
    }
    return classes;
}

vector<int> SortDoubled(const string& text, int l, vector<int>& order, vector<int>& classes) {
    // Define auxiliar vectors
    int n = text.size();
    vector<int> new_order(n);
    vector<int> count(n, 0);

    // Count occurrences of each class in text
    for (int i = 0; i < n; ++i) {
        count[classes[i]]++;
    }

    // Get the cumulative counting
    for (int j = 1; j < n; ++j) {
        count[j] += count[j - 1];
    }

    // Backward for loop, it gets the ordered position for each class
    for (int i = n - 1; i >= 0; --i) {
        // Move the start of the suffix L positions to the left
        int start = (order[i] - l + n) % n;

        // Get the class of the suffix C(i-L)
        int cl = classes[start];

        // Get the new order, based in sorting of the first L characters of the suffixes.
        // Counting sort is stable, then the secong L characters will stay sorted.
        new_order[--count[cl]] = start;
    }
    return new_order;
}

vector<int> UpdateClasses(vector<int>& newOrder, vector<int>& classes, int l) {
    // Define vector to store the new classes
    int n = newOrder.size();
    vector<int> newClass(n);

    // Initialize the lowest class with value 0
    newClass[newOrder[0]] = 0;

    for (int i = 1; i < n; ++i) {
        // Get the indices for the current suffix
        // The current suffix has the class formed by the pair (classes[cur], classes[mid])
        int cur = newOrder[i];
        int mid = (cur + l) % n;

        // Get the indices for the previous suffix
        // The previous suffix has the class formed by the pair (classes[prev], classes[midPrev])
        int prev = newOrder[i - 1];
        int midPrev = (prev + l) % n;

        // Compare classes[cur] vs classes[prev] (first half)
        // and classes[mid] vs classes[midPrev] (second half)
        if (classes[cur] != classes[prev] || classes[mid] != classes[midPrev]) {
            newClass[cur] = newClass[prev] + 1;
        } else {
            newClass[cur] = newClass[prev];
        }
    }
    return newClass;
}

// Build suffix array of the string text and
// return a vector result of the same length as the text
// such that the value result[i] is the index (0-based)
// in text where the i-th lexicographically smallest
// suffix of text starts.
vector<int> BuildSuffixArray(const string& text) {
    // Get the order considering the first character of each suffix
    vector<int> order = SortCharacters(text);

    // Get the classes considering the first character of each suffix
    vector<int> classes = ComputeCharClasses(text, order);

    // Start with size L = 1
    int l = 1;

    // While loop, it stops when L is equal or greater than the size of text
    while (l < text.size()) {
        // Update order by doubling L
        order = SortDoubled(text, l, order, classes);

        // Update classes by doubling L
        classes = UpdateClasses(order, classes, l);

        // Double L
        l *= 2;
    }
    return order;
}

int main() {
    string text;
    cin >> text;
    vector<int> suffix_array = BuildSuffixArray(text);
    for (int i = 0; i < suffix_array.size(); ++i) {
        cout << suffix_array[i] << ' ';
    }
    cout << endl;
    return 0;
}
