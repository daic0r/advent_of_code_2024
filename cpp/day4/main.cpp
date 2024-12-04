#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
#include <ranges>
#include <array>
#include <map>


constexpr bool search(std::span<std::string_view> lines, int row, int col, std::pair<int, int> searchVec, std::string_view strTerm) {
   int nIdx{};

   while (nIdx < strTerm.length() and lines[row][col] == strTerm[nIdx]) {
      ++nIdx;
      row += searchVec.second; 
      col += searchVec.first;
      if (row < 0 or row > lines.size()-1)
         break;
      if (col < 0 or col > lines[row].length()-1)
         break;
   }

   return nIdx == strTerm.length();
}

constexpr int part1(std::span<std::string_view> lines) {
   const std::array<std::pair<int,int>, 8> arSearchVectors = {
      std::pair<int, int>{ 0, 1 },
      { 0, -1 },
      { 1, 0 },
      { -1, 0 },
      { 1, 1 },
      { -1, -1 },
      { 1, -1 },
      { -1, 1 }
   };

   int nCount{};
   for (auto row = 0z; row < lines.size(); ++row) {
      auto strRow = lines[row];
      for (auto col = 0z; col < strRow.length(); ++col) {
         if (strRow[col] == 'X') {
            for (auto vec : arSearchVectors) {
               if (search(lines, row, col, vec, "XMAS")) {
                  ++nCount;
               }
            }
         }
      }
   }
   
   return nCount;
}

constexpr int part2(std::span<std::string_view> lines) {
   const std::array<std::pair<int,int>, 8> arSearchVectors = {
      std::pair<int, int>{ 1, 1 },
      { -1, -1 },
      { 1, -1 },
      { -1, 1 }
   };

   int nCount{};
   std::map<std::pair<int, int>, int> mCenterPos;

   for (auto row = 0z; row < lines.size(); ++row) {
      auto strRow = lines[row];
      for (auto col = 0z; col < strRow.length(); ++col) {
         if (strRow[col] == 'M') {
            for (auto vec : arSearchVectors) {
               if (search(lines, row, col, vec, "MAS")) {
                  ++mCenterPos[std::make_pair(col+vec.first, row+vec.second)];
               }
            }
         }
      }
   }

   return std::ranges::count_if(mCenterPos, [](auto const& kvp) { return kvp.second == 2; });
}


int main() {
//    std::string_view data = 
// "MMMSXXMASM\n\
// MSAMXMSMSA\n\
// AMXSXMAAMM\n\
// MSAMASMSMX\n\
// XMASAMXAMM\n\
// XXAMMXXAMA\n\
// SMSMSASXSS\n\
// SAXAMASAAA\n\
// MAMMMXMMMM\n\
// MXMXAXMASX";

   std::ifstream f{ "input.txt" };
   const std::string data{ std::istreambuf_iterator<char>{ f }, std::istreambuf_iterator<char>{} };
   f.close();

   auto lines = data
      | std::views::split('\n')
      | std::views::transform([](auto rng) { return std::string_view{ rng.begin(), rng.end() }; });

   std::vector<std::string_view> vLines{ lines.begin(), lines.end() };

   const auto nPart1 = part1(vLines);
   const auto nPart2 = part2(vLines);

   std::cout << "Result Part 1 = " << nPart1 << "\n";
   std::cout << "Result Part 2 = " << nPart2 << std::endl;
}
