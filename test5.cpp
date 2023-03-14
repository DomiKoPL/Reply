#include <bits/stdc++.h>
#include <sys/time.h>

using namespace std;

const int N = 5000;

int snakes_count;

using int8 = signed char;
using int16 = signed short;
using int32 = signed int;
using int64 = signed long long;
using uint8 = unsigned char;
using uint16 = unsigned short;
using uint32 = unsigned int;
using uint64 = unsigned long long;

class Stopwatch {
 public:
  Stopwatch(uint64 ms) { Start(ms); }

  void Start(uint64 ms) {
    time_ = Now();
    timeout_ = time_ + ms * 1000;
  }

  uint64 Now() const {
    timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1'000'000 + tv.tv_usec;
  }

  bool Timeout() const { return Now() >= timeout_; }

  uint64 ElapsedMs() const { return (Now() - time_) / 1000; }

  double ElapsedFraction() const { return (double)(Now() - time_) / (timeout_ - time_); }

 private:
  uint64 time_;
  uint64 timeout_;
};

class Random {
 public:
  Random(int seed) : seed_(seed) {}

  inline bool NextBool() { return (XRandom() & 4) == 4; }

  // return random int in range [0, range)
  inline uint32 NextInt(uint32 range) { return XRandom() % range; }

  // return random int in range [a, b]
  inline int32 NextInt(int32 a, int32 b) { return (int32)NextInt((uint32)(b - a + 1)) + a; }

  // return random float in range [0, 1]
  inline float NextFloat() {
    uint32 xr = XRandom();
    if (xr == 0U) return 0.0f;
    union {
      float f;
      uint32 i;
    } pun = {(float)xr};
    pun.i -= 0x10000000U;

    return pun.f;
  }

  // return random float in range [a, b]
  inline float NextFloat(float a, float b) { return NextFloat() * (b - a) + a; }

 private:
  inline uint32 XRandom() {
    seed_ = seed_ * kM + kA;
    return (uint32)(seed_ >> (29 - (seed_ >> 61)));
  }

  uint64 seed_;
  static constexpr uint64 kM = 0x9b60933458e17d7d;
  static constexpr uint64 kA = 0xd737232eeccdf7ed;
};

Random rnd(time(0));

struct Snake {
  int len;
  int id;
};

struct Pos {
  int row, col;

  bool operator==(const Pos& other) const { return row == other.row && col == other.col; }
};

Snake snakes[N];

constexpr int WORMHOLE = 20000;

// na razie na hama 2D nie ma co pajacowac
struct Grid {
  int val[N][N];
  bool empty[N][N];

  int width;
  int height;

  void Read() {
    cin >> width >> height >> snakes_count;

    for (int i = 0; i < snakes_count; ++i) {
      cin >> snakes[i].len;
      snakes[i].id = i;
    }

    sort(snakes, snakes + snakes_count, [](auto& a, auto& b) { return a.len > b.len; });

    for (int i = 0; i < height; ++i) {
      for (int j = 0; j < width; ++j) {
        string s;
        cin >> s;
        if (s == "*") {
          val[i][j] = WORMHOLE;
          // TODO: use wormholes in future!
          empty[i][j] = false;
        } else {
          val[i][j] = stoi(s);
          empty[i][j] = true;
        }
      }
    }
  }

  int Val(int row, int col) const {
    if (Hole(row, col)) {
      return 0;
    }
    return val[row][col];
  }

  bool Hole(int row, int col) const { return val[row][col] == WORMHOLE; }
} grid;

struct BeamState {
  int score = 0;
  int score_h = 0;
  vector<Pos> pos;

  void Add(int row, int col) {
    pos.push_back({row, col});
    score += grid.Val(row, col);
    score_h = score + rnd.NextInt(-100, 100);
  }

  bool CanMove(int row, int col) const {
    if (!grid.empty[row][col]) {
      return false;
    }
    return !count(pos.begin(), pos.end(), Pos{row, col});
  }

  bool CanUse() const {
    if (score <= 0) {
      return false;
    }
    auto [row, col] = pos.back();
    return !grid.Hole(row, col);
  }

  int Eval() const { return score; }

  int EvalH() const { return score_h; }
};

int score = 0;
vector<string> result;

const int DX[4]{1, -1, 0, 0};
const int DY[4]{0, 0, 1, -1};

Pos NextPos(Pos pos, int d) {
  return {(pos.row + grid.height + DX[d]) % grid.height,
          (pos.col + grid.width + DY[d]) % grid.width};
}

const int kBeamWidth = 10000;
const int kBeamWidthFirst = 1000;
const int kMaxBeamWidth = 50000;
BeamState states[2][kMaxBeamWidth + 100];
int states_cnt[2];

void SaveSnake(Snake snake, BeamState state) {
  stringstream ss;

  auto& pos = state.pos;
  score += state.score;

  ss << pos[0].col << " " << pos[0].row;
  grid.empty[pos[0].row][pos[0].col] = 0;

  for (int i = 1; i < snake.len; ++i) {
    auto [r, c] = pos[i - 1];
    auto [row, col] = pos[i];
    grid.empty[row][col] = 0;

    int dx = row - r;
    int dy = col - c;

    if ((dx == 1 or dx == -(grid.height - 1)) and dy == 0) {
      ss << " D";
    } else if ((dx == -1 or dx == grid.height - 1) and dy == 0) {
      ss << " U";
    } else if (dx == 0 and (dy == 1 or dy == -(grid.width - 1))) {
      ss << " R";
    } else if (dx == 0 and (dy == -1 or dy == grid.width - 1)) {
      ss << " L";
    } else {
      cerr << "DX, DY: " << dx << " " << dy << "\n";
      assert(false);
    }

    if (grid.Hole(row, col)) {
      ss << " " << col << " " << row;
    }
  }

  result[snake.id] = ss.str();
}

optional<BeamState> SolveSnake(Snake snake) {
  states_cnt[0] = 0;
  for (int row = 0; row < grid.height; ++row) {
    for (int col = 0; col < grid.width; ++col) {
      if (!grid.empty[row][col] || grid.Hole(row, col)) {
        continue;
      }

      auto& s = states[0][states_cnt[0]++];
      s = BeamState();
      s.Add(row, col);
    }
  }

  random_shuffle(states[0], states[0] + states_cnt[0]);
  states_cnt[0] = min(states_cnt[0], kBeamWidthFirst);
  // sort(states[0], states[0] + states_cnt[0], [](auto& a, auto& b) { return a.Eval() > b.Eval();
  // });

  int idx = 0;
  for (int l = 0; l < snake.len - 1; ++l) {
    int nidx = idx ^ 1;
    states_cnt[nidx] = 0;

    for (int i = 0; i < states_cnt[idx]; ++i) {
      auto& state = states[idx][i];
      for (int d = 0; d < 4; ++d) {
        assert(state.pos.size() > 0);

        Pos pos = NextPos(state.pos.back(), d);
        if (state.CanMove(pos.row, pos.col)) {
          auto& ns = states[nidx][states_cnt[nidx]];
          ns = states[idx][i];
          ns.Add(pos.row, pos.col);
          states_cnt[nidx] += 1;
        }
      }
    }

    sort(states[nidx], states[nidx] + states_cnt[nidx],
         [](auto& a, auto& b) { return a.Eval() > b.Eval(); });
    states_cnt[nidx] = min(states_cnt[nidx], kBeamWidth);

    idx ^= 1;
  }

  // sort(states[idx], states[idx] + states_cnt[idx],
  //      [](auto& a, auto& b) { return a.Eval() > b.Eval(); });

  for (int i = 0; i < states_cnt[idx]; ++i) {
    auto& state = states[idx][i];
    if (state.CanUse()) {
      return state;
    }
  }
  return nullopt;
}

int main() {
  Stopwatch watch(0);
  srand(watch.Now());
  grid.Read();
  result.resize(snakes_count);

  for (int s = 0; s < snakes_count; ++s) {
    if (s % 1 == 0) {
      std::cerr << s << "/" << snakes_count << " Score: " << score << "\n";
    }

    BeamState best_state;
    for (int i = 0; i < 1; ++i) {
      auto state = SolveSnake(snakes[s]);
      if (state) {
        if (state->score > best_state.score) {
          best_state = *state;
        }
      }
    }

    if (best_state.score > 0) {
      SaveSnake(snakes[s], best_state);
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