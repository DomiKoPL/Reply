#include <bits/stdc++.h>

using namespace std;

const int N = 5000;

int snakes_count;

struct Snake {
  int len;
  int id;
};

Snake snakes[N];

constexpr int WORMHOLE = 20000;

// na razie na hama 2D nie ma co pajacowac
struct Grid {
  int val[N][N];

  int width;
  int height;

  void Read() {
    cin >> width >> height >> snakes_count;

    for (int i = 0; i < snakes_count; ++i) {
      cin >> snakes[i].len;
      snakes[i].id = i;
    }

    sort(snakes, snakes + snakes_count, [](auto& a, auto& b) { return a.len < b.len; });

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
  }
} grid;

int main() {
  grid.Read();

  vector<int> cur_row(grid.width, 0);
  vector<string> result(snakes_count);

  int score = 0;
  for (int s = 0; s < snakes_count; ++s) {
    auto& snake = snakes[s];

    int best_col = -1;
    int best_score = 0;

    for (int col = 0; col < grid.width; ++col) {
      int& row = cur_row[col];
      while (row < grid.height && grid.val[row][col] == WORMHOLE ) {
        row += 1;
      }

      int score = 0;
      if (row + snake.len <= grid.height && grid.val[row + snake.len - 1][col] != WORMHOLE) {
        for (int i = 0; i < snake.len; ++i) {
          if (grid.val[row + i][col] != WORMHOLE) {
            score += grid.val[row + i][col];
          }
        }

        if (score > best_score) {
          best_score = score;
          best_col = col;
        }
      }
    }

    if (best_col != -1) {
      score += best_score;
      stringstream ss;
      int& row = cur_row[best_col];
      ss << best_col << " " << row;
      for (int i = 0; i < snake.len - 1; ++i) {
        ss << " D";
        row += 1;
        if (grid.val[row][best_col] == WORMHOLE) {
          ss << " " << best_col << " " << row;
        }
      }
      row += 1;
      result[snake.id] = ss.str();
    }
  }

  std::cerr << "Score: " << score << "\n";

  for (int s = 0; s < snakes_count; ++s) {
    cout << result[s];
    if (s + 1 < snakes_count) {
      cout << "\n";
    }
  }

  return 0;
}