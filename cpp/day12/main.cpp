#include <iostream>
#include <numeric>
#include <vector>
#include <ranges>
#include <unordered_map>
#include <set>
#include <string>
#include <queue>
#include <algorithm>
#include <fstream>

struct sCoord {
   int x{}, y{};
   int nAdjacent{};

   bool operator<(const sCoord& rhs) const noexcept {
      if (y < rhs.y) {
         return true;
      } else 
      if (y == rhs.y) {
         return x < rhs.x; 
      }
      return false;
   }
   bool operator==(const sCoord& rhs) const noexcept {
      return x == rhs.x and y == rhs.y;
   }

   sCoord operator+(const sCoord& rhs) const noexcept {
      return { x + rhs.x, y + rhs.y };
   }
};

std::ostream& operator<<(std::ostream& os, const sCoord& rhs) {
   os << "[(" << rhs.x << "," << rhs.y << ");" << rhs.nAdjacent << "]";
   return os;
}

std::ostream& operator<<(std::ostream& os, std::set<sCoord> const& rhs) {
   std::cout << "{ ";
   for (const auto coord : rhs) {
      std::cout << coord << " ";
   }
   std::cout << "}";
   return os;
}

std::set<sCoord> find_clusters(std::set<sCoord> const& plots) {
   std::queue<sCoord> q{};
   std::set<sCoord> ret{};
   std::set<sCoord> hasInQueue{};

   q.push(*plots.begin());

   while (not q.empty()) {
      auto cur = q.front();
      q.pop();

      // std::cout << "Current: " << cur << "\n";

      int nAdjCount{};
      for (const auto diff : { sCoord{ 1, 0 }, sCoord{ 0, 1 }, sCoord{ -1, 0 }, sCoord{ 0, -1 } }) {
         const auto next = cur + diff;
         // std::cout << "\tChecking " << next << ".\n";
         if (plots.contains(next)) {
            // std::cout << "\t" << next << " is neighbor.\n";
            ++nAdjCount;

            if (not hasInQueue.contains(next)) {
               q.push(next);
               hasInQueue.insert(next);
            }
         }
      }

      cur.nAdjacent = nAdjCount;
      ret.insert(cur);
   }

   return ret;
}

long get_perimeter(std::set<sCoord> const& area) {
   return std::accumulate(area.begin(), area.end(), 0L, [](long acc, const auto& coord) {
      return acc + (4 - coord.nAdjacent);
   });
}
 
long part1(std::unordered_map<char, std::set<sCoord>> const& plots) {
   std::unordered_map<char, std::vector<std::set<sCoord>>> mClusters;

   long nPrice{};

   for (auto [plant, positions] : plots) {
      std::cout << plant << ": \n";
      while (not positions.empty()) {
         auto cluster = find_clusters(positions);
         //std::cout << cluster << "\n";
         const auto nPerim = get_perimeter(cluster);

         nPrice += (nPerim * cluster.size());

         std::set<sCoord> sRemaining{};
         std::set_difference(positions.begin(), positions.end(),
               cluster.begin(), cluster.end(),
               std::inserter(sRemaining, sRemaining.end()));

         positions = std::move(sRemaining);
         mClusters[plant].push_back(std::move(cluster));
      }
   }

   return nPrice;
}

int main() {
//    const std::string strData = "AAAA\n\
// BBCD\n\
// BBCC\n\
// EEEC";

//    const std::string strData = "OOOOO\n\
// OXOXO\n\
// OOOOO\n\
// OXOXO\n\
// OOOOO";

//    const std::string strData = "RRRRIICCFF\n\
// RRRRIICCCF\n\
// VVRRRCCFFF\n\
// VVRCCCJFFF\n\
// VVVVCJJCFE\n\
// VVIVCCJJEE\n\
// VVIIICJJEE\n\
// MIIIIIJJEE\n\
// MIIISIJEEE\n\
// MMMISSJEEE";
//
   std::ifstream f{ "input.txt" };
   const std::string strData{ std::istreambuf_iterator<char>{ f }, std::istreambuf_iterator<char>{} };
   f.close();

   auto tmp = strData
      | std::views::split('\n')
      | std::views::transform([](auto rng) { return std::string_view{ rng.begin(), rng.end() }; })
      | std::views::filter([](std::string_view str) { return not str.empty(); });
   const std::vector<std::string_view> vLines{ tmp.begin(), tmp.end() };

   std::unordered_map<char, std::set<sCoord>> mPlots;

   for (const auto [row, line] : vLines | std::views::enumerate) {
      for (const auto [col, ch] : line | std::views::enumerate) {
         mPlots[ch].emplace(col, row);
      }
   }

   const auto nPart1 = part1(mPlots);
   std::cout << "Result Part 1 = " << nPart1 << "\n";
}
