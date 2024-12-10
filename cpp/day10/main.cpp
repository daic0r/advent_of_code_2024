#include <functional>
#include <iostream>
#include <vector>
#include <string>
#include <ranges>
#include <span>
#include <algorithm>
#include <array>
#include <stack>
#include <optional>
#include <format>
#include <set>
#include <fstream>

struct sCoord {
   int x{}, y{};

   constexpr sCoord operator+(sCoord rhs) const {
      return sCoord{ x + rhs.x, y + rhs.y };
   }

   constexpr auto operator<=>(const sCoord&) const = default;
};

std::ostream& operator<<(std::ostream& os, const sCoord& coord) {
   os << std::format("({}, {})", coord.x, coord.y);
   return os;
}

constexpr std::array<sCoord, 4> arDirections = {
   sCoord{ 1, 0 },
   sCoord{ 0, 1 },
   sCoord{ -1, 0 },
   sCoord{ 0, -1 }
};

std::optional<std::pair<sCoord, char>> get_adjacent_tile(std::span<const std::string_view> vInput, sCoord coord, sCoord dir) {
   coord = coord + dir;
   if (coord.x >= 0 and coord.x < vInput.front().length() and coord.y >= 0 and coord.y < vInput.size())
      return std::make_pair(coord, vInput[coord.y][coord.x]);
   return std::nullopt;
}

template<template<typename> typename Container>
Container<sCoord> find_trailhead_ends(std::span<const std::string_view> vInput, sCoord start) {
   Container<sCoord> vEnds{};

   std::stack<sCoord> stack{};
   stack.push(start);

   while (not stack.empty()) {
      const auto cur = stack.top();
      stack.pop();

      if (vInput[cur.y][cur.x] == '9') {
         if constexpr(std::is_same_v<Container<sCoord>, std::set<sCoord>>)
            vEnds.insert(cur);
         else
            vEnds.push_back(cur);
         continue;
      }

      for (auto dir : arDirections) {
         auto tile = get_adjacent_tile(vInput, cur, dir);
         if (not tile)
            continue;

         if (tile->second == vInput[cur.y][cur.x] + 1)
            stack.push(tile->first);
      }
   }

   return vEnds;
}

template<int N>
long part(std::span<const std::string_view> vInput) {
   // Find trailheads
   std::vector<sCoord> vTrailheads;
   for (auto [row, strLine] : vInput | std::views::enumerate) {
      auto iter = strLine.begin();
      while (iter != strLine.end()) {
         iter = std::find(iter, strLine.end(), '0');
         if (iter != strLine.end()) {
            vTrailheads.emplace_back(static_cast<int>(iter - strLine.begin()), static_cast<int>(row));
            ++iter; 
         }
      }
   }

   long nSum{};
   for (const auto start : vTrailheads) {
      const auto vEnds = std::invoke([&start, vInput] {
         if constexpr(N == 1)
            return find_trailhead_ends<std::set>(vInput, start);
         else
            return find_trailhead_ends<std::vector>(vInput, start);
      });
      nSum += vEnds.size();
   }  

   return nSum;
}

int main() {
//    const std::string strData = "89010123\n\
// 78121874\n\
// 87430965\n\
// 96549874\n\
// 45678903\n\
// 32019012\n\
// 01329801\n\
// 10456732";

//    const std::string strData = "..90..9\n\
// ...1.98\n\
// ...2..7\n\
// 6543456\n\
// 765.987\n\
// 876....\n\
// 987....";
//
   std::ifstream f{ "input.txt" };
   const std::string strData{ std::istreambuf_iterator<char>{ f }, std::istreambuf_iterator<char>{} };
   f.close();

   auto tmpLines = strData 
      | std::views::split('\n')
      | std::views::transform([](auto rng) { return std::string_view{ rng.begin(), rng.end() }; });

   const std::vector<std::string_view> vInput{ tmpLines.begin(), tmpLines.end() };

   long nPart1 = part<1>(vInput);
   std::cout << "Result Part 1 = " << nPart1 << "\n";
   long nPart2 = part<2>(vInput);
   std::cout << "Result Part 2 = " << nPart2 << "\n";
}
