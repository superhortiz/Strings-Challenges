#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::vector;

string InverseBWT(const string& bwt) {
    int n = bwt.size();
    vector<int> next(n);
    int index = -1;

    for (int i = 0; i < n; ++i) {
        next[i] = i;
        if (bwt[i] == '$')
            index = i;
    }

    std::stable_sort(next.begin(), next.end(), [&] (int i, int j) { return bwt[i] < bwt[j]; });

    string text(n, ' ');
    for (int i = 0; i < n; ++i) {
        text[i] = bwt[next[index]];
        index = next[index];
    }

    return text;
}

int main() {
    string bwt;
    cin >> bwt;
    cout << InverseBWT(bwt) << endl;
    return 0;
}
