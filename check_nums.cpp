#include <bits/stdc++.h>

using namespace std;

const int N = 5000;

int snakes_count;
int snakes[N];

constexpr int WORMHOLE = 20000;

// na razie na hama 2D nie ma co pajacowac
struct Grid {
  int val[N][N];

  int width;
  int height;

  void Read() {
    cin >> width >> height >> snakes_count;

    for (int i = 0; i < snakes_count; ++i) {
      cin >> snakes[i];
    }

    for (int i = 0; i < height; ++i) {
      for (int j = 0; j < width; ++j) {
        string s;
        cin >> s;
        if (s == "*") {
          val[i][j] = WORMHOLE;
        } else {
          val[i][j] = stoi(s);
        }
      }
    }

    for (int i = 0; i < height; ++i) {
      for (int j = 0; j < width; ++j) {
        if (val[i][j] > 0) {
          cout << "+";
        } else {
          cout << " ";
        }
      }
      cout << "\n";
    }
  }
} grid;

int main() {
  grid.Read();

  return 0;
}