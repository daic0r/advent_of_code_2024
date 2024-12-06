#include <future>
#include <iostream>
#include <map>
#include <ranges>
#include <unordered_map>
#include <vector>
#include <string>
#include <algorithm>
#include <span>
#include <fstream>
#include <cassert>

constexpr char rotate(char dir) {
   switch (dir) {
      case '^':
         return '>';
      case '>':
         return 'v';
      case 'v':
         return '<';
      case '<':
         return '^';
   }
   throw std::logic_error("unreachable");
}

const std::unordered_map<char, std::pair<int, int>> mMoveDirs = {
   { '^', { 0, -1 } },
   { '>', { 1, 0 } },
   { 'v', { 0, 1 } },
   { '<', { -1, 0 } }
};

int part1(std::vector<std::string> map) {
   auto iterY = std::ranges::find_if(map, [](std::string_view str) { return str.contains('^') or str.contains('>') or str.contains('v') or str.contains('<'); });
   assert(iterY != map.end());
   int posY = std::distance(map.begin(), iterY);
   int posX = iterY->find_first_of("^>v<");
   
   int nVisited = 1;
   char curDir = map[posY][posX];

   map[posY][posX] = 'X';

   const auto isValidCoord = [&map](int x, int y) {
      return y >= 0 and y < map.size() and x >= 0 and x < map[y].length();
   };

   do {
      const auto moveVec = mMoveDirs.at(curDir);
      const auto [nNextX, nNextY] = std::make_pair(posX + moveVec.first, posY + moveVec.second);
      if (!isValidCoord(nNextX, nNextY))
         break;
      const auto nextTile = map[nNextY][nNextX];
      switch (nextTile) {
         case 'X':
            posX = nNextX; 
            posY = nNextY;
            break;
         case '.':
            posX = nNextX;
            posY = nNextY;
            ++nVisited;
            map[posY][posX] = 'X';
            break;
         case '#':
            curDir = rotate(curDir);
            break;
      }
   } while (isValidCoord(posX, posY));

   return nVisited;
}

bool has_cycle_with_obstacle(std::span<const std::string> map, std::pair<int, int> startPos, char startDir, std::pair<int, int> obstacle) {
   std::map<std::pair<int, int>, std::string> mDirAtPos{};
   char curDir = startDir;
   
   int posX = startPos.first;
   int posY = startPos.second;

   mDirAtPos.emplace(std::make_pair(posX, posY), std::string{ startDir });

   const auto isValidCoord = [&map](int x, int y) {
      return y >= 0 and y < map.size() and x >= 0 and x < map[y].length();
   };

   do {
      const auto moveVec = mMoveDirs.at(curDir);
      const auto [nNextX, nNextY] = std::make_pair(posX + moveVec.first, posY + moveVec.second);
      if (!isValidCoord(nNextX, nNextY))
         break;
      const auto nextTile = (nNextX == obstacle.first and nNextY == obstacle.second) ? 'O' : map[nNextY][nNextX];
      switch (nextTile) {
         case 'O':
         case '#':
            curDir = rotate(curDir);
            break;
         default:
         {
            posX = nNextX;
            posY = nNextY;
            auto& history = mDirAtPos[std::make_pair(nNextX, nNextY)];
            if (history.contains(curDir))
               return true;
            history.push_back(curDir);
            break;
         }
      }
   } while (isValidCoord(posX, posY));

   return false;
}

int part2(std::span<const std::string> map) {
   auto iterY = std::ranges::find_if(map, [](std::string_view str) { return str.contains('^') or str.contains('>') or str.contains('v') or str.contains('<'); });
   assert(iterY != map.end());
   const int posY = std::distance(map.begin(), iterY);
   const int posX = iterY->find_first_of("^>v<");

   const char startDir = map[posY][posX]; 
   
   char curDir = map[posY][posX];
   int nNumObstacles{}; 
   for (auto y = 0z; y < map.size(); ++y) {
      const auto& line = map[y];
      for (auto x = 0z; x < line.length(); ++x) { 
         if (line[x] == '#')
            continue;
         if (has_cycle_with_obstacle(map, std::make_pair(posX, posY), startDir, std::make_pair(x, y))) {
            ++nNumObstacles;
         }
      }
   }
   return nNumObstacles;
}
int main() {
//    const std::string data = "....#.....\n\
// .........#\n\
// ..........\n\
// ..#.......\n\
// .......#..\n\
// ..........\n\
// .#..^.....\n\
// ........#.\n\
// #.........\n\
// ......#...";

   std::ifstream f{ "input.txt" };
   const std::string data{ std::istreambuf_iterator<char>{ f }, std::istreambuf_iterator<char>{} };
   f.close();

   auto lines = 
      data 
      | std::views::split('\n') 
      | std::views::transform([](auto rng) { return std::string{ rng.begin(), rng.end() }; })
      | std::views::filter([](auto str) { return !str.empty(); });

   std::vector<std::string> vInput{ lines.begin(), lines.end() };

   auto nFutPart1 = std::async(std::launch::async, [](const std::vector<std::string>& v) {
      return part1(v);
   }, vInput);   
   auto nFutPart2 = std::async(std::launch::async, [](const std::vector<std::string>& v) {
      return part2(v);
   }, vInput);   
   const auto nPart1 = nFutPart1.get();
   std::cout << "Result Part 1 = " << nPart1 << "\n";
   const auto nPart2 = nFutPart2.get();
   std::cout << "Result Part 2 = " << nPart2 << "\n";
}