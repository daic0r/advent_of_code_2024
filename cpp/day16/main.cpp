#include <iostream>
#include <fstream>
#include <memory>
#include <queue>
#include <set>
#include <utility>
#include <vector>
#include <span>
#include <ranges>
#include <string>

using coord2d = std::pair<int, int>;

std::ostream& operator<<(std::ostream& os, std::span<std::string_view> v) {
   for (const auto& str : v) {
      std::cout << str << "\n";
   }
   return os;
}

std::ostream& operator<<(std::ostream& os, coord2d p) {
   std::cout << "(" << p.first << ", " << p.second << ")";
   return os;
}
constexpr auto operator-(coord2d p1, coord2d p2) {
   return std::make_pair(p1.first - p2.first, p1.second - p2.second);
}
constexpr auto operator<(coord2d p1, coord2d p2) {
   if (p1.second < p2.second)
      return true;
   if (p1.second == p2.second)
      return p1.first < p2.first;
   return false;
}

constexpr coord2d find_in_grid(std::span<std::string_view> vInput, char ch) {
   for (const auto [nRow, strLine] : vInput | std::views::enumerate) {
      for (const auto [nCol, c] : strLine | std::views::enumerate) {
         if (c == ch) {
            return std::make_pair(static_cast<int>(nCol), static_cast<int>(nRow));
         }
      }
   }
   return std::make_pair(-1, -1);
}

constexpr std::vector<coord2d> get_visitable_tiles(std::span<std::string_view> vInput, coord2d pos) {
   std::vector<coord2d> vRet{};

   const auto left = std::make_pair(pos.first - 1, pos.second);
   const auto right = std::make_pair(pos.first + 1, pos.second);
   const auto top = std::make_pair(pos.first, pos.second - 1);
   const auto bottom = std::make_pair(pos.first, pos.second + 1);

   for (auto p : { left, right, top, bottom }) {
      const auto tile = vInput[p.second][p.first];
      if (tile == '.' or tile == 'E')
         vRet.push_back(p);
   }

   return vRet;
}

constexpr auto EAST = std::make_pair(1, 0);
constexpr auto SOUTH = std::make_pair(0, 1);
constexpr auto WEST = std::make_pair(-1, 0);
constexpr auto NORTH = std::make_pair(0, -1);

struct sTraverseInfo {
   coord2d pos;
   std::set<coord2d> from;
   coord2d dir{ EAST };
   bool bIsTarget{};
   long nCost{};

   bool operator<(const sTraverseInfo& rhs) const {
      return pos < rhs.pos;
   }
};

void backtrack(const std::set<sTraverseInfo>& sCache, coord2d pos, long& nTiles) {
   ++nTiles;
   const auto iter = std::ranges::find_if(sCache, [pos](const sTraverseInfo& info) { return info.pos == pos; });
   for (const auto& prev : iter->from) {
      backtrack(sCache, prev, nTiles);
   }
}

template<int N>
long solve(std::span<std::string_view> vInput) {
   const auto startPos = find_in_grid(vInput, 'S');
   const auto endPos = find_in_grid(vInput, 'E');

   std::cout << startPos << "\n";
   std::cout << endPos << "\n";


   std::set<sTraverseInfo> sCache;
   std::queue<sTraverseInfo> q;
   q.emplace(startPos);

   while (not q.empty()) {
      const auto cur = q.front();
      q.pop();

      const auto visitable = get_visitable_tiles(vInput, cur.pos);
      for (const auto neighbor : visitable) {
         const auto travelDir = neighbor - cur.pos; 
         const auto bIs90DegreeTurn = abs(travelDir.first) != abs(cur.dir.first);
         const auto nCost = cur.nCost + 1 + (bIs90DegreeTurn ? 1000 : 0);

         const auto iter = std::ranges::find_if(sCache, [neighbor](const sTraverseInfo& info) { return info.pos == neighbor; });
         if (iter == sCache.end() or iter->nCost >= nCost) {
            std::set<coord2d> sFrom;
            if (iter != sCache.end()) {
               sFrom = iter->from;
               sCache.erase(iter);
            }
            sFrom.insert(cur.pos);
            sTraverseInfo info{};
            info.from = sFrom;
            info.pos = neighbor;
            info.nCost = nCost;
            info.dir = travelDir;
            info.bIsTarget = vInput[neighbor.second][neighbor.first] == 'E';
            sCache.insert(info);

            q.push(info);
         }
      }
   }

   if constexpr(N == 1) {
      const auto iter = std::ranges::find_if(sCache, [](const sTraverseInfo& info) { return info.bIsTarget; });
      if (iter != sCache.end()) {
         std::cout << "Found target\n";
         return iter->nCost;
      }
   }

   return -1;
}

int main() {
//    const std::string strData = "###############\n\
// #.......#....E#\n\
// #.#.###.#.###.#\n\
// #.....#.#...#.#\n\
// #.###.#####.#.#\n\
// #.#.#.......#.#\n\
// #.#.#####.###.#\n\
// #...........#.#\n\
// ###.#.#####.#.#\n\
// #...#.....#.#.#\n\
// #.#.#.###.#.#.#\n\
// #.....#...#.#.#\n\
// #.###.#.#.#.#.#\n\
// #S..#.....#...#\n\
// ###############";

//    const std::string strData = "#################\n\
// #...#...#...#..E#\n\
// #.#.#.#.#.#.#.#.#\n\
// #.#.#.#...#...#.#\n\
// #.#.#.#.###.#.#.#\n\
// #...#.#.#.....#.#\n\
// #.#.#.#.#.#####.#\n\
// #.#...#.#.#.....#\n\
// #.#.#####.#.###.#\n\
// #.#.#.......#...#\n\
// #.#.###.#####.###\n\
// #.#.#...#.....#.#\n\
// #.#.#.#####.###.#\n\
// #.#.#.........#.#\n\
// #.#.#.#########.#\n\
// #S#.............#\n\
// #################";

   std::ifstream f{ "input.txt" };
   const std::string strData{ std::istreambuf_iterator<char>{ f }, std::istreambuf_iterator<char>{} };
   f.close();

   auto tmp = strData
      | std::views::split('\n')
      | std::views::transform([](auto rng) { return std::string_view{ rng.begin(), rng.end() }; })
      | std::views::filter([](std::string_view strLine) { return not strLine.empty(); });
   
   std::vector<std::string_view> vInput{ tmp.begin(), tmp.end() };

   const auto nPart1 = solve<1>(vInput);

   std::cout << "Result Part 1 = " << nPart1 << "\n";
}
