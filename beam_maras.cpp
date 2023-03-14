#include <bits/stdc++.h>

using namespace std;

const int N = 5000;

int snakes_count;
pair <int, int> snakes[N];

vector <pair <int, int> > snake_positions[N];

int dirs[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

constexpr int WORMHOLE = 20000;

random_device rnd;
mt19937 gen(rnd());

// na razie na chama 2D nie ma co pajacowac
struct Grid {
    int val[N][N];
    int blocked[N][N];

    int width;
    int height;

    void Read() {
        cin >> width >> height >> snakes_count;

        for (int i = 0; i < snakes_count; ++i) {
            cin >> snakes[i].first;
            snakes[i].second = i;
        }

        sort(snakes, snakes + snakes_count);
        reverse(snakes, snakes + snakes_count);

        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                    string s;
                    cin >> s;
                    if (s == "*") {
                        val[i][j] = WORMHOLE;
                    } else {
                        val[i][j] = stoi(s);
                    }

                    blocked[i][j] = 0;
                }
            }
    }

    pair <int, int> random_head() {
        vector <float> weights;

        int cnt = 0;
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                if (val[i][j] == WORMHOLE or blocked[i][j] or val[i][j] < 0) {
                    weights.push_back(0);
                }
                else {
                    weights.push_back(pow(val[i][j] + 2, 3));
                    cnt += 1;
                }
            }
        }

        if (!cnt) {
            return {-1, -1};
        }

        discrete_distribution<int> dist(weights.begin(), weights.end());
        int id = dist(gen);

        return {id / width, id % width};
    }

} grid;

struct BeamSearch {
    struct State {
        pair <int, int> cur_pos;
        int value;
        set <pair <int, int> > blocked;
        vector <pair <int, int> > history;

        vector <State> next_states() {
            vector <State> next_states;
            
            for (auto& d : dirs) {
                auto new_pos = cur_pos;
                new_pos.first += d[0];
                new_pos.second += d[1];

                new_pos.first = (new_pos.first + grid.height) % grid.height;
                new_pos.second = (new_pos.second + grid.width) % grid.width;

                if (grid.val[new_pos.first][new_pos.second] == WORMHOLE or
                    grid.blocked[new_pos.first][new_pos.second] or
                    blocked.count(new_pos)) {
                    continue;
                }

                next_states.push_back({
                    new_pos,
                    value + grid.val[new_pos.first][new_pos.second],
                    blocked,
                    history
                });

                next_states.back().blocked.insert(new_pos);
                next_states.back().history.push_back(new_pos);
            }

            return next_states;
        }
    };

    vector <State> states;
    const int BEAM_WIDTH = 20;

    State run(pair <int, int> start, int depth) {
        states.clear();

        State start_state = {start, grid.val[start.first][start.second], {start}, {start}};
        states.push_back(start_state);

        for (int i = 0; i < depth - 1; ++i) {
            vector <State> new_states;

            if(states.size() == 0) {
                break;
            }

            for (auto& state : states) {
                auto next_states = state.next_states();
                new_states.insert(new_states.end(), next_states.begin(), next_states.end());
            }

            sort(new_states.begin(), new_states.end(), 
                [](const State& s1, const State& s2) {
                return s1.value > s2.value;
            });

            new_states.resize(min(BEAM_WIDTH, (int)new_states.size()));
            states = new_states;
        }

        if (states.size() == 0) {
            return {{-1, -1}, 0, {}, {}};
        }

        return states[0];
    }
} beam_search;


void print_snake(const vector <pair <int, int> >& positions) {

    if (positions.size() == 0) {
        cout << "\n";
        return;
    }

    cout << positions[0].second << " " << positions[0].first << " ";

    for (int i = 1; i < positions.size(); ++i) {
        int dx = positions[i].first - positions[i - 1].first;
        int dy = positions[i].second - positions[i - 1].second;

        if ((dx == 1 or dx == -(grid.height - 1)) and dy == 0) {
            cout << "D ";
        }
        else if ((dx == -1 or dx == grid.height - 1) and dy == 0) {
            cout << "U ";
        }
        else if (dx == 0 and (dy == 1 or dy == -(grid.width - 1))) {
            cout << "R ";
        }
        else if (dx == 0 and (dy == -1 or dy == grid.width - 1)) {
            cout << "L ";
        }
        else {
            cerr << dx << " " << dy << endl;
            assert(false);
        }
    }
    cout << "\n";
}

int main() {
    grid.Read();

    int total_value = 0;
    int repeats = 0;

    for (int i = 0; i < snakes_count; ++i) {
        auto [snake_len, snake_id] = snakes[i];
        auto head = grid.random_head();
        
        if (head.first == -1) {
            continue;
        }
        else {
            cerr << i << "\n";
            auto best_state = beam_search.run(head, snake_len);

            if (best_state.value <= 0) {
                
                if (repeats == 3) {
                    repeats = 0;
                    continue;
                }
                
                i--;
                repeats++;
                continue;
            }

            total_value += best_state.value;

            for (auto& pos : best_state.history) {
                grid.blocked[pos.first][pos.second] = 1;
            }

            snake_positions[snake_id] = best_state.history;
            repeats = 0;

            cerr << total_value << "\n";
        }
    }

    for (int i = 0; i < snakes_count; ++i) {
        print_snake(snake_positions[i]);
    }

    cerr << total_value << "\n";

    return 0;
}