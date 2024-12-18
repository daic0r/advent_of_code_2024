#include <cassert>
#include <format>
#include <iostream>
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
};

std::ostream& operator<<(std::ostream& os, const sCoord& c) {
   os << "(" << c.x << ", " << c.y << ")";
   return os;
}

struct sTraverseInfo {
   sCoord pos{};
   sCoord prev{ -1, -1 };
   int nCost{};

   bool operator<(const sTraverseInfo& rhs) const {
      return pos < rhs.pos;
   }
};

struct sCostCompare {
   bool operator()(const sTraverseInfo& a, const sTraverseInfo& b) const {
      return a.nCost > b.nCost;
   }
};

int part1(std::span<std::vector<char>> vMap) {

   std::set<sTraverseInfo> sCache;
   std::priority_queue<sTraverseInfo, std::vector<sTraverseInfo>, sCostCompare> q;

   const sCoord target{ static_cast<int>(vMap.front().size()) - 1, static_cast<int>(vMap.size()) - 1 };

   q.emplace();

   while (not q.empty()) {
      const auto cur = q.top();
      q.pop();

      if (cur.pos == target)
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

   auto iter = std::ranges::find_if(sCache, [target](const auto& p) { return p.pos == target; });
   if (iter != sCache.end())
      return iter->nCost;
   return -1;
}

std::string part2(std::vector<std::vector<char>> vMap, std::span<const sCoord> vObstacles) {
   for (auto i = 1024z; i < vObstacles.size(); ++i) {
      const auto obst = vObstacles[i];
      vMap[obst.y][obst.x] = '#';
      const auto nCost = part1(vMap);
      if (nCost == -1)
         return std::format("{},{}", obst.x, obst.y);
   }
   std::unreachable();
}

int main() {
//    const std::string strData = "5,4\n\
// 4,2\n\
// 4,5\n\
// 3,0\n\
// 2,1\n\
// 6,3\n\
// 2,4\n\
// 1,5\n\
// 0,6\n\
// 3,3\n\
// 2,6\n\
// 5,1\n\
// 1,2\n\
// 5,5\n\
// 2,5\n\
// 6,5\n\
// 1,4\n\
// 0,4\n\
// 6,4\n\
// 1,1\n\
// 6,1\n\
// 1,0\n\
// 0,5\n\
// 1,6\n\
// 2,0";

   std::ifstream f{ "input.txt" };
   const std::string strData{ std::istreambuf_iterator<char>{ f }, std::istreambuf_iterator<char>{} };
   f.close();

   auto tmp = strData
      | std::views::split('\n')
      | std::views::transform([](auto rng) { return std::string_view{ rng.begin(), rng.end() }; })
      | std::views::filter([](std::string_view strLine) { return not strLine.empty(); })
      | std::views::transform([](std::string_view strLine) {
         auto pos = strLine.find(',');
         return sCoord{ std::stoi(std::string_view{ strLine.substr(0, pos) }.data()), std::stoi(std::string_view{ strLine.substr(pos + 1) }.data()) };
      });
   
   std::vector<std::vector<char>> vMap(71, std::vector<char>(71, '.'));
   const std::vector<sCoord> vObstacles { tmp.begin(), tmp.end() };

   auto iter = vObstacles.begin();
   for (auto i = 0z; i < 1024; ++i) {
      auto coord = *iter++;
      vMap[coord.y][coord.x] = '#';
   }

   for (const auto& row : vMap) {
      for (auto ch : row)
         std::cout << ch;
      std::cout << "\n";
   }

   const auto nPart1 = part1(vMap); 
   std::cout << "Result Part 1 = " << nPart1 << "\n";

   const auto nPart2 = part2(vMap, vObstacles); 
   std::cout << "Result Part 2 = " << nPart2 << "\n";
}
