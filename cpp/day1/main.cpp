#include <algorithm>
#include <iostream>
#include <numeric>
#include <span>
#include <vector>
#include <fstream>
#include <ranges>
#include <sstream>

int part1(std::span<int> list1, std::span<int> list2) {
   int nResult{};
   for (const auto [x, y] : std::views::zip(list1, list2)) {
      nResult += abs(x - y);
   }
   return nResult;
}

int part2(std::span<int> list1, std::span<int> list2) {
   return std::accumulate(list1.begin(), list1.end(), 0, [&list2](int acc, int n) {
      const auto nTimes = std::ranges::count_if(list2, [n](auto e) { return e == n; });
      return acc + n * nTimes;
   });
}

int main() {
   std::vector<char> vFileBuf{};

   std::ifstream f{ "input.txt", std::ios::ate };
   const auto nSize = f.tellg();
   vFileBuf.resize(nSize);
   f.seekg(std::ios::beg);
   f.read(vFileBuf.data(), nSize);
   f.close();
   
   auto list1 = std::vector<int>{};
   auto list2 = std::vector<int>{};

   for (const auto [x,y] : vFileBuf 
         | std::views::split('\n') 
         | std::views::transform([](auto subrange) { return std::string{ subrange.begin(), subrange.end() }; })
         | std::views::filter([](auto const& str) { return str.length() > 0; })
         | std::views::transform([](auto const& line) -> std::pair<int, int> { 
            std::istringstream ss{ line };
            int n1, n2;
            ss >> n1 >> n2;
            return { n1, n2 };
         }))
   {
      list1.push_back(x);
      list2.push_back(y);

      //std::cout << x << " " << y << "\n";
   }
   std::ranges::sort(list1);
   std::ranges::sort(list2);

   const auto nPart1 = part1(list1, list2);
   const auto nPart2 = part2(list1, list2);

   std::cout << "Result Part 1 = " << nPart1 << std::endl;
   std::cout << "Result Part 2 = " << nPart2 << std::endl;
}
