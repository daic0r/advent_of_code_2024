#include <cassert>
#include <charconv>
#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>
#include <span>
#include <string>
#include <ranges>
#include <numeric>
#include <cstdint>
#include <functional>
#include <optional>

struct sEquation {
   std::uint64_t nResult{};
   std::vector<std::uint64_t> vData{};
};

enum class Operator {
   PLUS,
   MULTIPLIES,
   CONCAT
};

using operator_t = std::pair<Operator, std::function<std::uint64_t(std::uint64_t, std::uint64_t)>>;

constexpr std::uint64_t concat_numbers(std::uint64_t left, std::uint64_t right) {
   return left * std::pow(10, static_cast<std::uint64_t>(std::log10(right)) + 1) + right;
}

constexpr std::span<const operator_t> get_operators(int nPart) {
   static const std::array<operator_t, 2> ops_part1 = { std::make_pair(Operator::PLUS, std::plus{}), std::make_pair(Operator::MULTIPLIES, std::multiplies{}) };
   static const std::array<operator_t, 3> ops_part2 = { 
      std::make_pair(Operator::PLUS, std::plus{}),
      std::make_pair(Operator::MULTIPLIES, std::multiplies{}),
      std::make_pair(Operator::CONCAT, &concat_numbers)
   };
   switch (nPart) {
      case 1:
         return ops_part1;
      case 2:
         return ops_part2;
         break;
   }
   throw std::logic_error{ "unreachable" };
}

constexpr bool solve_equation(std::uint64_t nTarget, std::span<std::uint64_t> vData, std::optional<std::uint64_t> acc, std::span<const operator_t> operators) {
   if (vData.size() == 1) {
      for (const auto& op : operators) {
         const auto result = !acc ? vData.front() : op.second(*acc, vData.front());
         if (result == nTarget) {
            return true;
         }
      }
      return false;
   }

   for (const auto& op : operators) {
      const auto result = !acc ? vData.front() : op.second(*acc, vData.front());
      if (result > nTarget) {
         continue;
      }
      auto found = solve_equation(nTarget, std::span<std::uint64_t>{ std::next(vData.begin()), vData.end() }, result, operators);
      if (found) {
         return true;
      }
   }

   return false;
}

constexpr std::uint64_t solve(std::span<sEquation> equations, int nPart) {
   auto tmp = equations
      | std::views::transform([nPart,cnt=0](sEquation& eq) mutable { 
         return std::make_pair(eq.nResult, solve_equation(eq.nResult, eq.vData, std::nullopt, get_operators(nPart)));
      });

   auto intermediate_results = std::vector<std::pair<std::uint64_t, bool>>{ tmp.begin(), tmp.end() };

   auto valid_results = intermediate_results
      | std::views::filter([](const std::pair<std::uint64_t, bool>& res) { return res.second; })
      | std::views::transform([cnt=0](const auto& p) mutable { 
         return p.first; 
      });

   // std::vector<std::uint64_t> v{ valid_results.begin(), valid_results.end() };

   return std::reduce(valid_results.begin(), valid_results.end(), 0ull);
}

int main() {
//    const std::string data = "190: 10 19\n\
// 3267: 81 40 27\n\
// 83: 17 5\n\
// 156: 15 6\n\
// 7290: 6 8 6 15\n\
// 161011: 16 10 13\n\
// 192: 17 8 14\n\
// 21037: 9 7 18 13\n\
// 292: 11 6 16 20";
//

   std::ifstream f{ "input.txt" };
   const std::string data{ std::istreambuf_iterator<char>{ f }, std::istreambuf_iterator<char>{} };
   f.close();

   auto tmp = 
      data
      | std::views::split('\n')
      | std::views::transform([](auto rng) { return std::string_view{ rng.begin(), rng.end() }; })
      | std::views::filter([](std::string_view str) { return not str.empty(); })
      | std::views::transform([](std::string_view str) {
         auto left_right = str
            | std::views::split(':')
            | std::views::transform([](auto rng) { return std::string_view{ rng.begin(), rng.end() }; });
         std::vector<std::string_view> arr{ left_right.begin(), left_right.end() };
         auto input_numbers = arr[1]
            | std::views::split(' ')
            | std::views::transform([](auto rng) { return std::string_view{ rng.begin(), rng.end() }; })
            | std::views::filter([](std::string_view str) { return not str.empty(); })
            | std::views::transform([](std::string_view str) { std::uint64_t res; std::from_chars(str.begin(), str.end(), res); return res; });

         std::uint64_t nEqResult;
         std::from_chars(arr[0].begin(), arr[0].end(), nEqResult);

         return sEquation{   
            .nResult = nEqResult,
            .vData = std::vector<std::uint64_t>{ input_numbers.begin(), input_numbers.end() }
         };
      });

   auto input = std::vector<sEquation>{ tmp.begin(), tmp.end() };

   const auto nPart1 = solve(input, 1);
   std::cout << "Result Part 1 = " << nPart1 << "\n";
   const auto nPart2 = solve(input, 2);
   std::cout << "Result Part 2 = " << nPart2 << "\n";
}
