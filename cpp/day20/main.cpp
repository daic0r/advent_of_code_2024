#include <cassert>
#include <format>
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
};

struct sTraverseInfo {
   sCoord pos{};
   sCoord prev{ -1, -1 };
   int nCost{};
   std::vector<sCheat> vCheats{};

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

long part1(std::span<std::string_view> vMap) {
   const auto start = find_in_grid(vMap, 'S');
   const auto end = find_in_grid(vMap, 'E');

   std::map<int, std::size_t> mCheatOverview;

   auto sCache = find_path(vMap);
   auto iter = std::ranges::find_if(sCache, [end](const auto& p) { return p.pos == end; });
   auto prev_iter = sCache.end();
   while (iter != sCache.end()) {
      const auto pos = iter->pos;

      auto node = sCache.extract(iter);
      for (auto diff : {
            sCoord{ 0, 1 },
            sCoord{ -1, 0 },
            sCoord{ 0, -1 },
            sCoord{ 1, 0 }
      })
      {
         const auto neighbor = pos + diff;
         if (neighbor == prev_iter->pos or neighbor == node.value().prev)
            continue;

         assert(vMap[neighbor.y][neighbor.x] == '#');

         for (int i = 2; i < 3; ++i) {
            const auto target = pos + (diff * i);
            auto target_iter = std::ranges::find_if(sCache, [target](const auto& p) { return p.pos == target; });
            if (target_iter == sCache.end()) {
               continue;
            }
            const auto tile = vMap[target.y][target.x];

            sCheat cheat{};
            cheat.nSavings = node.value().nCost - target_iter->nCost - i;
            cheat.target = neighbor;
            
            node.value().vCheats.push_back(cheat);
         }
      }

      //std::cout << "Cheats at " << node.value().pos << "\n";
      for (const auto& cheat : node.value().vCheats) {
         // if (cheat.nSavings == 4)
         //    std::cout << cheat.target << " for a savings of " << cheat.nSavings << "\n";
         ++mCheatOverview[cheat.nSavings];
      }

      iter = sCache.insert(iter, std::move(node));
      prev_iter = iter;
      iter = std::ranges::find_if(sCache, [iter](const auto& p) { return p.pos == iter->prev; });
   }

   for (const auto [nSavings, nCount] : mCheatOverview) {
      std::cout << nCount << " cheats save " << nSavings << " picoseconds.\n";
   }

   auto from_iter = mCheatOverview.lower_bound(100);
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

   const auto nPart1 = part1(vInput);
   std::cout << "Result Part 1 = " << nPart1 << std::endl;
}
