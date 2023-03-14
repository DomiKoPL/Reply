#include <bits/stdc++.h>

using namespace std;

const int N = 5000;

int snakes_count;
int snakes[N];

constexpr int WORMHOLE = 20000;

vector<pair<int,int> > snake_position[N];

struct warmhole {
    int col, row;
    vector<int> used_exits;

    warmhole (int _col, int _row) {
        col = _col; row = _row;
        used_exits.resize(4, 0);
    }
};

vector<warmhole> warmholes;

// na razie na hama 2D nie ma co pajacowac
struct Grid {
  int val[N][N];
  bool used[N][N] = {};
  
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
          warmholes.emplace_back(i, j);
        } else {
          val[i][j] = stoi(s);
        }
      }
    }
  }
} grid;

const int MAX_TRIES = 1;

mt19937 mt(1234);

int DY [] = {0, 0, -1, 1};
int DX [] = {1, -1, 0, 0};

char d_to_letter[] = {'R', 'L', 'U','D'};


void set_used(const vector<pair<int,int> > & pos, bool val) {
    for (auto [y, x] : pos) {
        grid.used[y][x] = val;
    }
}

int calc(const vector<pair<int,int> > & pos) {
    int score = 0;
    for (auto [y, x] : pos) {
        if (grid.val[y][x] == WORMHOLE) continue;
        
        score += grid.val[y][x];
    }
    return score;
}

int add_snake(int len, int id) {
    int score = 0;
    cerr << "Adding next snake with id: " << id << endl;
    uniform_int_distribution<int> height_dist(0, grid.height - 1);
    uniform_int_distribution<int> width_dist(0, grid.width - 1);

    for (int _ = 0; _ < MAX_TRIES; ++_) {
        vector<pair<int, int> > current;
        int s_x = width_dist(mt);
        int s_y = height_dist(mt);

        while (grid.used[s_y][s_x] or grid.val[s_y][s_x] == WORMHOLE) {
            s_x = width_dist(mt);
            s_y = height_dist(mt);
        }

        grid.used[s_y][s_x] = true;
        current.push_back({s_y, s_x});

        for (int i = 1; i < len; ++i) {
            // cell_value, direction, if != WORMHOLE then wormhole is used
            vector<vector<tuple<int,int, int, int>>> free_neighbours;
            for (int d = 0; d < 4; ++d) {
                int n_y = (s_y + DY[d] + grid.height) % grid.height;
                int n_x = (s_x + DX[d] + grid.width) % grid.width;

                // if (n_x < 0 or n_x >= grid.width) continue;
                // if (n_y < 0 or n_y >= grid.height) continue;

                if (grid.used[n_y][n_x]) continue;
                
                if (grid.val[n_y][n_x] == WORMHOLE) {
                    if (i == len - 1) continue;

                    for (int out_d = 0; out_d < 4; ++out_d) {
                        for (int warmhole_idx = 0; warmhole_idx < warmholes.size(); ++warmhole_idx) {
                            int nn_y = (warmholes[warmhole_idx].row + DY[out_d] + grid.height) % grid.height;
                            int nn_x = (warmholes[warmhole_idx].col + DX[out_d] + grid.width) % grid.width;

                            if (grid.used[n_y][n_x]) continue;

                            free_neighbours.push_back({
                                {0, d, warmholes[warmhole_idx].row, warmholes[warmhole_idx].col},
                                {grid.val[nn_y][nn_x], out_d, WORMHOLE, WORMHOLE}
                            });
                        }

                    }


                    // free_neighbours.push_back({0, d});
                }
                else {
                    free_neighbours.push_back({{grid.val[n_y][n_x], d, WORMHOLE, WORMHOLE}});
                }
            }

            if (free_neighbours.empty()) {
                break;
            }

            sort(free_neighbours.begin(), free_neighbours.end(), [](const auto& a, const auto& b) {
                int sum_f = 0;
                for (auto [value, _x, _xx, ___x] : a) {
                    sum_f += value;
                }

                int sum_s = 0;
                for (auto [value, _x, _xx, ___x] : b) {
                    sum_f += value;
                }

                return sum_f < sum_s;
            });

            reverse(free_neighbours.begin(), free_neighbours.end());

            auto &move_to_make = free_neighbours[0];

            i += (move_to_make.size() - 1);

            for (auto [value, direction, wormhole_pos_y, wormhole_pos_x] : move_to_make) {
                if (wormhole_pos_x != WORMHOLE) {
                    s_y = wormhole_pos_y;
                    s_x = wormhole_pos_x;
                } else {
                    (s_y += DY[direction] + grid.height) %= grid.height;
                    (s_x += DX[direction] + grid.width) %= grid.width ;
                    grid.used[s_y][s_x] = true;
                }

                current.push_back({s_y, s_x});
            }
            // current.push_back({s_y, s_x});
            // grid.used[s_y][s_x] = true;
        }

        if (current.size() == len) {
            auto my_score = calc(current);
            if (my_score > score) {
                score = my_score;
                snake_position[id] = current;
            }
        }
        set_used(current, false);

    }
    // cerr << "ok\n";
    // for(auto [y, x] : snake_position[id]) {
    //     cerr << "(" << y << ' ' << x << ") ";
    // }
    // cerr << '\n';
    set_used(snake_position[id], true);
    return score;
}

void print_move(pair<int,int> pre, pair<int,int> cur) {
    bool printed = false;
    for (int d = 0; d < 4; ++d) {
        int nx = (pre.second + DX[d] + grid.width) % grid.width;
        int ny = (pre.first + DX[d] + grid.height) % grid.height;

        if (nx == cur.second && ny == cur.first) {
            cout << " " << d_to_letter[d];
            printed = true;
        }
    }

    if (!printed) {
        cout << cur.second << " " << cur.first;
    }

    // int d_y = cur.first - pre.first;
    // int d_x = cur.second - pre.second;

    // if (grid.val[cur.first][cur.second] == WORMHOLE) {
    //     cout << ' ' << cur.first << ' ' << cur.second;
    // }
    // if (d_y == 1) {
    //     cout << " D";
    // } else if (d_y == -1) {
    //     cout << " U";
    // } else if (d_x == 1) {
    //     cout << " R";
    // } else {
    //     cout << " L";
    // }
}
int main() {
  grid.Read();

  int res = 0;
  for (int i = 0; i < snakes_count; ++i) {
    int val = add_snake(snakes[i], i);
    res += val;
  }
  std::cerr << "score: " << res << '\n';

  for (int i = 0; i < snakes_count; ++i) {
    if (snake_position[i].empty()) {
        cout << '\n';
        continue;
    }

    cout << snake_position[i][0].second << ' ' << snake_position[i][0].first;
    for (int j = 1; j < snakes[i]; ++j) {
        print_move(snake_position[i][j - 1], snake_position[i][j]);
    }
    if (i != snakes_count)
        cout << '\n';
  }

  return 0;
}