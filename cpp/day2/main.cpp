#include <charconv>
#include <fstream>
#include <vector>
#include <span>
#include <iostream>
#include <string>
#include <ranges>
#include <algorithm>

constexpr int sgn(int n) noexcept {
   if (n < 0)
      return -1;
   if (n == 0)
      return 1;
   return 0;
}

constexpr std::vector<int> calc_gradients(std::span<int> lineNums) {
   auto tmp = lineNums
      | std::views::slide(2)
      | std::views::transform([](auto rng) { auto iter = rng.begin(); return *iter++ - *iter; });

   std::vector<int> vecGradients{ tmp.begin(), tmp.end() };

   return vecGradients;
}

constexpr int part1(std::span<std::vector<int>> data) {
   auto ret = data
      | std::views::transform(&calc_gradients)
      | std::views::filter([](std::vector<int> lineGradients) {
         return std::ranges::all_of(lineGradients, [](int n) { return abs(n) >= 1 and abs(n) <= 3; });
      })
      | std::views::filter([](std::vector<int> lineGradients) {
         return std::ranges::all_of(lineGradients, [&lineGradients](int n) { return sgn(n) == sgn(lineGradients.front()); });
      });

   return std::ranges::distance(ret);
}

constexpr int part2(std::span<std::vector<int>> data) {
   auto ret = data
      | std::views::transform(&calc_gradients)
      | std::views::transform([](std::vector<int> vecGradients) {
         auto iter = std::ranges::find_if(vecGradients, [](int n) { return abs(n) < 1 or abs(n) > 3; });

         if (iter == vecGradients.end())
            return vecGradients;

         if (std::next(iter) != vecGradients.end()) {
            *iter = *iter + *std::next(iter);
            vecGradients.erase(std::next(iter));
         }

         return vecGradients;
      })
      | std::views::filter([](std::vector<int> vecGradients) {
         const auto numPos = std::ranges::distance(vecGradients | std::views::filter([](int n) { return n > 0; }));
         const auto numNeg = std::ranges::distance(vecGradients | std::views::filter([](int n) { return n < 0; }));
         return std::ranges::all_of(vecGradients, [](int n) { return abs(n) >= 1 and abs(n) <= 3; })
                  and (numPos < 2 or numNeg < 2);
      });

   return std::ranges::distance(ret);
}

int main() {
   std::ifstream f{ "input.txt" };
   std::string strBuffer{ std::istreambuf_iterator<char>{ f }, std::istreambuf_iterator<char>{} };
   f.close();

   auto tmp = strBuffer 
      | std::views::split('\n')
      | std::views::transform([](auto rng) { return std::string_view{ rng.begin(), rng.end() }; })
      | std::views::filter([](std::string_view line) { return not line.empty(); })
      | std::views::transform([](std::string_view strLine) { 
         return strLine 
            | std::views::split(' ') 
            | std::views::transform([](auto rng) {
               return std::string_view{ rng.begin(), rng.end() };
            });
      })
      | std::views::transform([](auto lineSplits) {
         std::vector<int> ret{};
         std::ranges::transform(lineSplits, std::back_inserter(ret), [](auto strNum) {
            int val{};
            std::from_chars(strNum.begin(), strNum.end(), val);
            return val;
         });
         return ret;
      });
   
   std::vector<std::vector<int>> data{ std::make_move_iterator(tmp.begin()), std::make_move_iterator(tmp.end()) };

   const auto nPart1 = part1(data);
   const auto nPart2 = part2(data);

   std::cout << "Result Part 1 = " << nPart1 << "\n";
   std::cout << "Result Part 2 = " << nPart2 << std::endl;
}
