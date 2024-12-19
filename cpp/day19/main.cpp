#include <cassert>
#include <functional>
#include <iostream>
#include <numeric>
#include <vector>
#include <ranges>
#include <fstream>
#include <string>

using namespace std::literals;

constexpr long get_possible_patterns(std::string_view strPattern, std::span<const std::string_view> vAvail, std::unordered_map<std::string_view, long>& memo) {
   if (const auto iter = memo.find(strPattern); iter != memo.end()) {
      return iter->second;
   }

   long nCnt{};
   for (auto strAvail : vAvail) {
      if (strAvail.length() > strPattern.length())
         continue;

      if (strPattern.starts_with(strAvail)) {
         auto strNext = strPattern;
         strNext.remove_prefix(strAvail.length());

         const auto nRet = get_possible_patterns(strNext, vAvail, memo);
         nCnt += nRet;
      }
   }

   memo[strPattern] = nCnt;

   return nCnt;
}

template<int N>
constexpr long solve(std::span<const std::string_view> vAvail, std::span<const std::string_view> vDesired) {
   return std::accumulate(vDesired.begin(), vDesired.end(), 0L,
         [vAvail](long acc, std::string_view strPattern) {
            std::unordered_map<std::string_view, long> memo;
            memo[""sv] = 1;
            if constexpr(N == 1)
               return acc + (get_possible_patterns(strPattern, vAvail, memo) > 0 ? 1 : 0);
            else
               return acc + get_possible_patterns(strPattern, vAvail, memo);
         });
}

int main() {
//    const std::string strData = "r, wr, b, g, bwu, rb, gb, br\n\
// \n\
// brwrr\n\
// bggr\n\
// gbbr\n\
// rrbgbr\n\
// ubwu\n\
// bwurrg\n\
// brgr\n\
// bbrgwb";

   std::ifstream f{ "input.txt" };
   const std::string strData{ std::istreambuf_iterator<char>{ f }, std::istreambuf_iterator<char>{} };
   f.close();

   std::string_view strAvailPatterns;
   std::string_view strDesiredPatterns;
   {
      auto tmp = strData
         | std::views::split("\n\n"sv)
         | std::views::transform([](auto rng) { return std::string_view{ rng.begin(), rng.end() }; });

      auto iter = tmp.begin();
      strAvailPatterns = *iter++;
      strDesiredPatterns = *iter;
   }

   const std::vector<std::string_view> vAvail = std::invoke([strAvailPatterns]() {
      auto tmp = strAvailPatterns
         | std::views::split(", "sv)
         | std::views::transform([](auto rng) { return std::string_view{ rng.begin(), rng.end() }; })
         | std::views::filter([](std::string_view str) { return not str.empty(); });

      return std::vector<std::string_view>{ tmp.begin(), tmp.end() };
   });

   const std::vector<std::string_view> vDesired = std::invoke([strDesiredPatterns]() {
      auto tmp = strDesiredPatterns
         | std::views::split("\n"sv)
         | std::views::transform([](auto rng) { return std::string_view{ rng.begin(), rng.end() }; })
         | std::views::filter([](std::string_view strLine) { return not strLine.empty(); });

      return std::vector<std::string_view>{ tmp.begin(), tmp.end() };
   });

   const auto nPart1 = solve<1>(vAvail, vDesired);
   std::cout << "Result Part 1 = " << nPart1 << "\n";
   const auto nPart2 = solve<2>(vAvail, vDesired);
   std::cout << "Result Part 2 = " << nPart2 << "\n";
}
