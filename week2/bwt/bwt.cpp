#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::vector;

string BWT(const string& text) {
    int n = text.size();
    vector<int> indices(n);
    string result(n, ' ');

    for (int i = 0; i < n; ++i) indices[i] = i;

    std::sort(indices.begin(), indices.end(),
            [&] (int i, int j) {
                return text.substr(i, n) + text.substr(0, i) < text.substr(j, n) + text.substr(0, j);
            });

    for (int i = 0; i < n; ++i) {
        result[i] = text[(indices[i] + n - 1) % n];
    }

    return result;
}

int main() {
    string text;
    cin >> text;
    cout << BWT(text) << endl;
    return 0;
}