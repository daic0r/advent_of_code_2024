#include <algorithm>
#include <cassert>
#include <iostream>
#include <map>
#include <numeric>
#include <queue>
#include <ranges>
#include <set>
#include <utility>
#include <vector>
#include <string>
#include <fstream>

struct sCoord {
   int x{}, y{};

   constexpr bool operator<(const sCoord& rhs) const {
      if (y < rhs.y)
         return true;
      if (y == rhs.y)
         return x < rhs.x;

      return false;
   } 

   constexpr bool operator==(const sCoord& rhs) const {
      return x == rhs.x and y == rhs.y;
   }

   constexpr sCoord operator+(const sCoord& rhs) const {
      return sCoord{ x + rhs.x, y + rhs.y };
   } 

   constexpr sCoord operator-(const sCoord& rhs) const {
      return sCoord{ x - rhs.x, y - rhs.y };
   } 

   constexpr sCoord operator*(int n) const {
      return sCoord{ x * n, y * n };
   }
};

std::ostream& operator<<(std::ostream& os, const sCoord& c) {
   os << "(" << c.x << ", " << c.y << ")";
   return os;
}

struct sCheat {
   sCoord target{};
   int nSavings{};

   bool operator<(const sCheat& rhs) const {
      return target < rhs.target;
   }
};

struct sTraverseInfo {
   sCoord pos{};
   sCoord prev{ -1, -1 };
   int nCost{};
   std::set<sCheat> sCheats{};

   bool operator<(const sTraverseInfo& rhs) const {
      return pos < rhs.pos;
   }
};

struct sCostCompare {
   bool operator()(const sTraverseInfo& a, const sTraverseInfo& b) const {
      return a.nCost > b.nCost;
   }
};

constexpr sCoord find_in_grid(std::span<std::string_view> vInput, char ch) {
   for (const auto [nRow, strLine] : vInput | std::views::enumerate) {
      for (const auto [nCol, c] : strLine | std::views::enumerate) {
         if (c == ch) {
            return sCoord{ static_cast<int>(nCol), static_cast<int>(nRow) };
         }
      }
   }
   return sCoord{ -1, -1 };   
}

auto find_path(std::span<std::string_view> vMap) {

   std::set<sTraverseInfo> sCache;
   std::priority_queue<sTraverseInfo, std::vector<sTraverseInfo>, sCostCompare> q;

   const auto start = find_in_grid(vMap, 'S');
   const auto end = find_in_grid(vMap, 'E');

   q.emplace(start);
   sCache.emplace(start);

   while (not q.empty()) {
      const auto cur = q.top();
      q.pop();

      if (cur.pos == end)
         break;

      for (const auto neighbor : {
            cur.pos + sCoord{ 1, 0 },
            cur.pos + sCoord{ -1, 0 },
            cur.pos + sCoord{ 0, -1 },
            cur.pos + sCoord{ 0, 1 } }) 
      {
         if (neighbor == cur.prev)
            continue;
         if (neighbor.x < 0 or neighbor.x > vMap.front().size()-1 or
               neighbor.y < 0 or neighbor.y > vMap.size()-1)
            continue;
         if (vMap[neighbor.y][neighbor.x] == '#')
            continue;

         auto iter = std::ranges::find_if(sCache, [neighbor](const auto& p) { return p.pos == neighbor; });
         const auto nCost = cur.nCost + 1;
         if (iter == sCache.end() or nCost < cur.nCost) {
            sTraverseInfo info{};
            info.pos = neighbor;
            info.prev = cur.pos;
            info.nCost = nCost;

            if (iter != sCache.end()) {
               sCache.erase(iter);
            } else {
               q.push(info);
            }
            sCache.insert(info);
         }
      }
   }

   auto iter = std::ranges::find_if(sCache, [end](const auto& p) { return p.pos == end; });
   assert(iter != sCache.end());
   
   return sCache;
}

template<int N, int LowerLimit>
long solve(std::span<std::string_view> vMap) {
   const auto start = find_in_grid(vMap, 'S');
   const auto end = find_in_grid(vMap, 'E');

   std::map<int, std::size_t> mCheatOverview;

   auto sCache = find_path(vMap);

   auto iter = std::ranges::find_if(sCache, [end](const auto& p) { return p.pos == end; });
   auto prev_iter = sCache.end();
   auto idx_point = 0;
   const auto nSize = sCache.size();
   while (iter != sCache.end()) {
      const auto pos = iter->pos;

      //std::cout << "On " << idx_point++ << "/" << nSize << "\n";

      auto node = sCache.extract(iter);

      for (int i = 2; i < N + 1; ++i) {
         for (int y = 0; y < i + 1; ++y) {
            for (int x = -(i - y); x < (i - y) + 1; ++x) {
               if (abs(x)+abs(y) < i)
                  continue;
               const auto pt1 = pos + sCoord{ x, y };
               const auto pt2 = pos + sCoord{ x, -y };

               std::array<sCoord, 2> arTargets{ pt1, pt2 };
               std::array<decltype(sCache)::iterator, 2> arTargetIters{
                  std::ranges::find_if(sCache, [&arTargets](const auto& p) { return p.pos == arTargets[0]; }),
                  std::ranges::find_if(sCache, [&arTargets](const auto& p) { return p.pos == arTargets[1]; }),
               };

               for (auto target_iter : arTargetIters
                     | std::views::filter([&sCache](auto iter) { return iter != sCache.end(); })
                     | std::views::filter([&node, i](auto iter) { return (node.value().nCost - iter->nCost - i) > 0; }))
               {
                  sCheat cheat{};
                  cheat.nSavings = node.value().nCost - target_iter->nCost - i;
                  cheat.target = target_iter->pos;

                  node.value().sCheats.insert(cheat);
               }
            }
         }
      }

      //std::cout << "Cheats at " << node.value().pos << "\n";
      for (const auto& cheat : node.value().sCheats) {
         // if (cheat.nSavings == 4)
         //    std::cout << cheat.target << " for a savings of " << cheat.nSavings << "\n";
         ++mCheatOverview[cheat.nSavings];
      }

      iter = sCache.insert(sCache.end(), std::move(node));

      prev_iter = iter;
      iter = std::ranges::find_if(sCache, [iter](const auto& p) { return p.pos == iter->prev; });
   }

   for (const auto [nSavings, nCount] : mCheatOverview | std::views::filter([](auto kvp) { return kvp.first >= LowerLimit; })) {
      std::cout << nCount << " cheats save " << nSavings << " picoseconds.\n";
   }

   auto from_iter = mCheatOverview.lower_bound(LowerLimit);
   return std::accumulate(from_iter, mCheatOverview.end(), 0L,
         [](long acc, const auto& kvp) {
            return acc + static_cast<long>(kvp.second);
         });
}

int main() {
//    const std::string strData = "###############\n\
// #...#...#.....#\n\
// #.#.#.#.#.###.#\n\
// #S#...#.#.#...#\n\
// #######.#.#.###\n\
// #######.#.#...#\n\
// #######.#.###.#\n\
// ###..E#...#...#\n\
// ###.#######.###\n\
// #...###...#...#\n\
// #.#####.#.###.#\n\
// #.#...#.#.#...#\n\
// #.#.#.#.#.#.###\n\
// #...#...#...###\n\
// ###############";

   std::ifstream f{ "input.txt" };
   const std::string strData{ std::istreambuf_iterator<char>{ f }, std::istreambuf_iterator<char>{} };
   f.close();

   auto tmp = strData
      | std::views::split('\n')
      | std::views::transform([](auto rng) { return std::string_view{ rng.begin(), rng.end() }; })
      | std::views::filter([](std::string_view strLine) { return not strLine.empty(); });
   
   std::vector<std::string_view> vInput{ tmp.begin(), tmp.end() };

   const auto nPart1 = solve<2, 100>(vInput);
   std::cout << "Result Part 1 = " << nPart1 << std::endl;
   const auto nPart2 = solve<20, 100>(vInput);
   std::cout << "Result Part 2 = " << nPart2 << std::endl;
}
