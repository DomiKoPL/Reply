#include <bits/stdc++.h>

using namespace std;

const int N = 5000;

int snakes_count;
pair<int,int> snakes[N];

constexpr int WORMHOLE = 20000;
random_device rd;
mt19937 mt(rd());
vector<pair<int,int> > snake_position[N];

// na razie na hama 2D nie ma co pajacowac
struct Grid {
  int val[N][N];
  bool used[N][N] = {};
  
  int width;
  int height;

  void Read() {
    cin >> width >> height >> snakes_count;

    for (int i = 0; i < snakes_count; ++i) {
      cin >> snakes[i].first;
      snakes[i].second = i;
    }

    shuffle(snakes, snakes + snakes_count, mt);
    for (int i = 0; i < snakes_count; ++i) {
        cerr << snakes[i].first << '\n';
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
  }
} grid;

const int MAX_TRIES = 1000000;


int DY [] = {0, 0, -1, 1};
int DX [] = {1, -1, 0, 0};

uniform_int_distribution<int> rand_move(0, 99);


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
    uniform_int_distribution<int> height_dist(0, grid.height - 1);
    uniform_int_distribution<int> width_dist(0, grid.width - 1);

    for (int _ = 0; _ < MAX_TRIES; ++_) {
        vector<pair<int, int> > current;
        int s_x = width_dist(mt);
        int s_y = height_dist(mt);

        while (grid.used[s_y][s_x] or grid.val[s_y][s_x] == WORMHOLE or grid.val[s_y][s_x] < 0) {
            s_x = width_dist(mt);
            s_y = height_dist(mt);
        }

        grid.used[s_y][s_x] = true;
        current.push_back({s_y, s_x});

        for (int i = 1; i < len; ++i) {
            vector<pair<int,int> > free_neighbours;
            for (int d = 0; d < 4; ++d) {
                int n_y = s_y + DY[d];
                int n_x = s_x + DX[d];

                if (n_x < 0 or n_x >= grid.width) continue;
                if (n_y < 0 or n_y >= grid.height) continue;

                if (grid.used[n_y][n_x]) continue;
                
                if (grid.val[n_y][n_x] == WORMHOLE) {
                    continue;
                }
                else {
                    free_neighbours.push_back({grid.val[n_y][n_x], d});
                }
            }
            if (free_neighbours.empty()) {
                break;
            }

            sort(free_neighbours.begin(), free_neighbours.end());
            reverse(free_neighbours.begin(), free_neighbours.end());

            int idx = 0;
            if (rand_move(mt) < 10) {
                idx = uniform_int_distribution<int>(0, free_neighbours.size() - 1)(mt);
            }
            
            s_y += DY[free_neighbours[idx].second];
            s_x += DX[free_neighbours[idx].second];
            current.push_back({s_y, s_x});
            grid.used[s_y][s_x] = true;
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
    int d_y = cur.first - pre.first;
    int d_x = cur.second - pre.second;
    if (d_y == 1) {
        cout << " D";
    } else if (d_y == -1) {
        cout << " U";
    } else if (d_x == 1) {
        cout << " R";
    } else {
        cout << " L";
    }
}

int calc_nei(int y, int x) {
    int val = 0;
    for (int d = 0; d < 4; ++d) {
        int nx = x + DX[d];
        int ny = y + DY[d];

        if (nx < 0 or ny < 0 or nx >= grid.width or ny >= grid.height) continue;
        if (grid.used[ny][nx] or grid.val[ny][nx] == WORMHOLE or grid.val[ny][nx] < 0) val += 1;
    }
    return val;
}
int main() {
  grid.Read();

  vector< vector<pair<int,int> >> order;

  for (int y = 0; y < grid.height; ++y) {
    for (int x = 0; x < grid.width; ++x) {
        if (grid.val[y][x] == WORMHOLE or grid.val[y][x] < 0 or grid.used[y][x]) {
            continue;
        }

        vector<pair<int,int> > kol;
        kol.push_back({y, x});
        grid.used[y][x] = true;

        while(true) {
            vector<pair<int,int> > nei;
            for (int d = 0; d < 4; ++d) {
                int nx = x + DX[d];
                int ny = y + DY[d];

                if (nx < 0 or ny < 0 or nx >= grid.width or ny >= grid.height) continue;
                if (grid.used[ny][nx] or grid.val[ny][nx] == WORMHOLE or grid.val[ny][nx] < 0) continue;

                nei.push_back({calc_nei(ny, nx), d});
            }

            if (nei.empty()) {
                break;
            }

            sort(nei.begin(), nei.end());
            y += DY[nei[0].second];
            x += DX[nei[0].second];
            kol.push_back({y, x});
            grid.used[y][x] = true;
        }

        order.push_back(kol);
    }
  }
    sort(order.begin(), order.end(), [](auto & a, auto & b) {
        return a.size() > b.size();
    });
    int max_score = 0;
    while(true) {
        auto org_ord = order;
        shuffle(snakes, snakes + snakes_count, mt);
        int score = 0;

        for (auto vec : order) {
            for (int i = 0; i < snakes_count; ++i) {
                auto [len, id] = snakes[i];
                if (not snake_position[id].empty()) continue;
                if (len > vec.size()) {
                    continue;
                }

                for (int i = 0; i < len; ++i) {
                    snake_position[id].push_back(vec.back());
                    score += grid.val[vec.back().first][vec.back().second];
                    vec.pop_back();
                }
            }
        }
        if (score > max_score) {
            cerr << score << '\n';
            max_score = score;
        }
    }
    
    
    for (int i = 0; i < snakes_count; ++i) {
    if (snake_position[i].empty()) {
        cout << '\n';
        continue;
    }

    cout << snake_position[i][0].second << ' ' << snake_position[i][0].first;
    for (int j = 1; j < snake_position[i].size(); ++j) {
        print_move(snake_position[i][j - 1], snake_position[i][j]);
    }
    if (i != snakes_count)
        cout << '\n';
  }

  return 0;
}