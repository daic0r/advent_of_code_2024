#include <format>
#include <iostream>
#include <ranges>
#include <string>
#include <utility>
#include <vector>
#include <span>
#include <fstream>

std::ostream& operator<<(std::ostream& os, const std::vector<std::string>& v) {
   for (const auto& str : v) {
      std::cout << str << "\n";
   }
   return os;
}

std::ostream& operator<<(std::ostream& os, std::pair<int, int> p) {
   std::cout << "(" << p.first << ", " << p.second << ")";
   return os;
}

std::pair<int, int> get_moved_pos(std::pair<int, int> pos, char chMove) {
   switch (chMove) {
      case '<':
         return std::make_pair(pos.first - 1, pos.second);
      case '^':
         return std::make_pair(pos.first, pos.second - 1);
      case '>':
         return std::make_pair(pos.first + 1, pos.second);
      case 'v':
         return std::make_pair(pos.first, pos.second + 1);
   }
   std::unreachable();
}

std::pair<int, int> move(std::vector<std::string>& vMap, std::pair<int, int> pos, char chMove) {
   auto& orig_tile = vMap[pos.second][pos.first];
   const auto new_pos = get_moved_pos(pos, chMove);
   std::cout << "Moving " << pos << " to " << new_pos << "\n";
   auto& tile = vMap[new_pos.second][new_pos.first];
   std::cout << "New position holds a " << tile << "\n";
   switch (tile) {
      case '#':
         return pos;
      case '.':
         tile = orig_tile;
         orig_tile = '.';
         return new_pos;
      case 'O':
         std::cout << "gotta move the box\n";
         move(vMap, new_pos, chMove);
         if (tile == '.') {
            tile = orig_tile;
            orig_tile = '.';
            return new_pos;
         }
         return pos;
   }

   std::unreachable();
}

long calc_gps_sum(std::span<std::string> vMap) {
   long nSum{};
   for (const auto& [nRow, strLine] : vMap | std::views::enumerate) {
      for (auto [nCol, ch] : strLine | std::views::enumerate) {
         if (ch == 'O')
            nSum += static_cast<long>(nRow) * 100L + static_cast<long>(nCol);
      }
   }
   return nSum;
}

long part1(std::vector<std::string> vMap, std::string_view strMovements) {
   std::pair<int, int> robotPos;
   for (const auto& [nRow, strLine] : vMap | std::views::enumerate) {
      auto nPos = strLine.find('@');
      if (nPos != std::string::npos) {
         robotPos = std::make_pair<int, int>(nPos, nRow);
         break;
      }
   }
   for (auto chMove : strMovements) {
      if (chMove == '\n')
         continue;
      std::cout << "Moving " << chMove << "\n";
      robotPos = move(vMap, robotPos, chMove);
      std::cout << vMap << "\n================================\n";
   }

   return calc_gps_sum(vMap);
}

int main() {
//    const std::string strData = "########\n\
// #..O.O.#\n\
// ##@.O..#\n\
// #...O..#\n\
// #.#.O..#\n\
// #...O..#\n\
// #......#\n\
// ########\n\
// \n\
// <^^>>>vv<v>>v<<";

//    const std::string strData = "##########\n\
// #..O..O.O#\n\
// #......O.#\n\
// #.OO..O.O#\n\
//
// #..O@..O.#\n\
// #O#..O...#\n\
// #O..O..O.#\n\
// #.OO.O.OO#\n\
// #....O...#\n\
// ##########\n\
// \n\
// <vv>^<v^>v>^vv^v>v<>v^v<v<^vv<<<^><<><>>v<vvv<>^v^>^<<<><<v<<<v^vv^v>^\n\
// vvv<<^>^v^^><<>>><>^<<><^vv^^<>vvv<>><^^v>^>vv<>v<<<<v<^v>^<^^>>>^<v<v\n\
// ><>vv>v^v^<>><>>>><^^>vv>v<^^^>>v^v^<^^>v^^>v^<^v>v<>>v^v^<v>v^^<^^vv<\n\
// <<v<^>>^^^^>>>v^<>vvv^><v<<<>^^^vv^<vvv>^>v<^^^^v<>^>vvvv><>>v^<<^^^^^\n\
// ^><^><>>><>^^<<^^v>>><^<v>^<vv>>v>>>^v><>^v><<<<v>>v<v<v>vvv>^<><<>^><\n\
// ^>><>^v<><^vvv<^^<><v<<<<<><^v<<<><<<^^<v<^^^><^>>^<v^><<<^>>^v<v^v<v^\n\
// >^>>^v>vv>^<<^v<>><<><<v<<v><>v<^vv<<<>^^v^>^^>>><<^v>>v^v><^^>>^<>vv^\n\
// <><^^>^^^<><vvvvv^v<v<<>^v<v>v<<^><<><<><<<^^<<<^<<>><<><^^^>^^<>^>v<>\n\
// ^^>vv<^v^v<vv>^<><v<^v>^^^>>>^^vvv^>vvv<>>>^<^>>>>>^<<^v>^vvv<>^<><<v>\n\
// v^^>>><<^^<>>^v^<v^vv<>v^<<>^<^v^v><^<<<><<^<v><v<>vv>>v><v^<vv<>v^<<^";


   std::ifstream f{ "input.txt" };
   const std::string strData{ std::istreambuf_iterator<char>{ f }, std::istreambuf_iterator<char>{} };
   f.close();

   using namespace std::literals;

   std::string_view strMap, strMovements;

   {
      auto tmp = strData
         | std::views::split("\n\n"sv)
         | std::views::transform([](auto rng) { return std::string_view(rng.begin(), rng.end()); });

      strMap = *tmp.begin();
      strMovements = *std::next(tmp.begin());
   }

   std::vector<std::string> vMap;
   {
      auto tmp = strMap
         | std::views::split('\n')
         | std::views::transform([](auto rng) { return std::string(rng.begin(), rng.end()); })
         | std::views::filter([](std::string_view strLine) { return not strLine.empty(); });

      vMap = std::vector<std::string>{ tmp.begin(), tmp.end() };
   }

   const auto nPart1 = part1(vMap, strMovements);
   std::cout << "Result Part 1 = " << nPart1 << "\n";
}
