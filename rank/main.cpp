#include <chrono>
#include <cstdint>
#include <iostream>
#include <random>

#include "sorted_set.hpp"

struct rank_item {
  int score{0};
  int64_t time{0};

  bool operator<(const rank_item& r) {
    if (score == r.score) {
      return time > r.time;
    }
    return score < r.score;
  }

  bool operator==(const rank_item& r) {
    return score == r.score && time == r.time;
  }
};

// [left, right]
int rand_num(int left, int right) {
  static std::default_random_engine gen(std::random_device{}());
  std::uniform_int_distribution<decltype(right)> dis(left, right);
  return dis(gen);
}

int64_t get_time_milli() {
  auto now = std::chrono::system_clock::now().time_since_epoch();
  return std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
}

int main(int argc, char* argv[]) {
  sorted_set<int, rank_item> rank_info;
  for (int i = 1; i <= 10; ++i) {
    rank_info.insert(i, rank_item{rand_num(50, 100), get_time_milli()});
  }
  int rank = 1;
  for (auto& n : rank_info) {
    std::cout << "rank: " << rank << ", userid: " << n->first
              << ", score: " << n->second.score << std::endl;
    ++rank;
  }
  return 0;
}
