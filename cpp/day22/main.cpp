#include <algorithm>
#include <charconv>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <ranges>
#include <string>
#include <fstream>

constexpr long generate_next(long n) {
   n = ((n * 64) ^ n) % 16777216;
   n = ((n / 32) ^ n) % 16777216;
   n = ((n * 2048) ^ n) % 16777216;

   return n;
}

constexpr long part1(std::span<long> vInput) {

   long nRet{};
   for (auto n : vInput) {
      for (auto i = 0; i < 2000; ++i) {
         n = generate_next(n);
      }
      nRet += n;
   }

   return nRet;
}

struct sSequence {
   std::array<long, 4> arGradients{};

   bool operator==(const sSequence& rhs) const {
      return std::ranges::equal(arGradients, rhs.arGradients);
   }
};
std::ostream& operator<<(std::ostream& os, const sSequence& seq) {
   os << "(";
   for (auto [nIdx,n] : seq.arGradients | std::views::enumerate) {
      os << n;
      if (nIdx < 3)
         os << ", ";
   }
   os << ")";
   return os;
}
template<>
struct std::hash<sSequence> {
   std::size_t operator()(const sSequence& s) const noexcept {
      return ((s.arGradients[0] & 15) << 12) | ((s.arGradients[1] & 15) << 8) | ((s.arGradients[2] & 15) << 4) | (s.arGradients[3] & 15);
   }
};

constexpr long part2(std::span<long> vInput) {
   std::unordered_map<sSequence, long> mSeqSums;

   std::vector<std::pair<long, std::optional<long>>> vPrices;
   vPrices.reserve(2000);

   for (auto n : vInput) {
      vPrices.clear();

      std::optional<long> nPrev{};
      std::size_t nIdxSeqBegin{1};
      std::unordered_map<sSequence, long> mSeqSumsLocal;
      for (auto i = 0; i < 2000; ++i) {
         long nDigit = n % 10;
         std::optional<long> nChange{};
         if (nPrev) {
            nChange = nDigit - *nPrev;
         }
         vPrices.emplace_back(nDigit, nChange); 
         nPrev = nDigit;

         if (i > 3) {
            sSequence seq{};

            auto tmp = std::ranges::subrange{ vPrices.begin() + nIdxSeqBegin, vPrices.begin() + nIdxSeqBegin + 4 }
               | std::views::transform([](std::pair<long, std::optional<long>> p) { return *p.second; });
            std::copy(tmp.begin(), tmp.end(), seq.arGradients.begin());

            if (const auto iter = mSeqSumsLocal.find(seq); iter == mSeqSumsLocal.end()) {
               mSeqSumsLocal[seq] = nDigit;
            }

            ++nIdxSeqBegin;
         }
         n = generate_next(n);
      }

      for (const auto& [seq, nDigit] : mSeqSumsLocal) {
         mSeqSums[seq] += nDigit;
      }
   }

   auto iter = std::ranges::max_element(mSeqSums, std::less{}, [](const auto& kvp) { return kvp.second; });

   return iter->second;
}

int main() {
//    const std::string strData = "1\n\
// 2\n\
// 3\n\
// 2024";

   std::ifstream f{ "input.txt" };
   const std::string strData{ std::istreambuf_iterator<char>{ f }, std::istreambuf_iterator<char>{} };
   f.close();

   auto tmp = strData
      | std::views::split('\n')
      | std::views::transform([](auto rng) { return std::string_view{ rng.begin(), rng.end() }; })
      | std::views::filter([](std::string_view str) { return not str.empty(); })
      | std::views::transform([](std::string_view str) {
         long nOut{};
         std::from_chars(str.data(), str.data() + str.length(), nOut);
         return nOut;
      });

   std::vector<long> vInput{ tmp.begin(), tmp.end() };

   const auto nPart1 = part1(vInput);
   std::cout << "Result Part 1 = " << nPart1 << "\n";
   const auto nPart2 = part2(vInput);
   std::cout << "Result Part 2 = " << nPart2 << "\n";
}
