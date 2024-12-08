#include <iostream>
#include <fstream>
#include <vector>
#include <ranges>
#include <map>
#include <set>

struct vec2 {
   int x{}, y{};

   constexpr vec2 operator-(const vec2& other) const noexcept {
      return vec2{ x - other.x, y - other.y };
   }
   constexpr vec2 operator+(const vec2& other) const noexcept {
      return vec2{ x + other.x, y + other.y };
   }

   auto operator<=>(const vec2&) const = default;
};

constexpr bool is_in_bounds(const vec2& p, int nWidth, int nHeight) {
   return p.x >= 0 and p.x < nWidth and p.y >= 0 and p.y < nHeight;
}

std::map<char, std::set<vec2>> mAntennaLocations;
int nWidth{}, nHeight{};

int solve(int nPart) {
   std::set<vec2> sAntinodes{};

   for (const auto& [antenna, locations] : mAntennaLocations) {
      for (auto iter_from = locations.begin(); iter_from != locations.end(); ++iter_from) {
         for (auto iter = std::next(iter_from); iter != locations.end(); ++iter) {
            const auto dist = *iter - *iter_from;
            if (nPart == 1) {
               const auto loc1 = *iter + dist;
               const auto loc2 = *iter_from - dist;
               if (loc1.x >= 0 and loc1.x < nWidth and loc1.y >= 0 and loc1.y < nHeight)
                  sAntinodes.insert(loc1);
               if (loc2.x >= 0 and loc2.x < nWidth and loc2.y >= 0 and loc2.y < nHeight)
                  sAntinodes.insert(loc2);
            } else 
            if (nPart == 2) {
               auto loc = *iter_from;
               while (is_in_bounds(loc, nWidth, nHeight)) {
                  sAntinodes.insert(loc);
                  loc = loc + dist;
               }
               loc = *iter_from - dist;
               while (is_in_bounds(loc, nWidth, nHeight)) {
                  sAntinodes.insert(loc);
                  loc = loc - dist;
               }
            }
         }
      }
   }
   return sAntinodes.size();
}

int main() {
   std::ifstream f{ "input.txt" };
   const std::string data{ std::istreambuf_iterator<char>{ f }, std::istreambuf_iterator<char>{} };
   f.close();

   auto tmp = 
      data
      | std::views::split('\n')
      | std::views::transform([](auto rng) { return std::string_view{ rng.begin(), rng.end() }; })
      | std::views::filter([](auto str) { return not str.empty(); });

   std::vector<std::string_view> vLines{ tmp.begin(), tmp.end() };

   for (auto y = 0z; y < vLines.size(); ++y) {
      const auto line = vLines[y];
      for (auto x = 0z; x < line.length(); ++x) {
         const auto ch = line[x];
         if ((ch >= 'A' and ch <= 'Z') or (ch >= 'a' and ch <= 'z') or (ch >= '0' and ch <= '9')) {
            mAntennaLocations[ch].insert(vec2{ static_cast<int>(x), static_cast<int>(y) });
         }
      }
   }

   nWidth = vLines.front().length();
   nHeight = vLines.size();

   const auto nPart1 = solve(1);
   std::cout << "Result Part 1 = " << nPart1 << "\n";
   const auto nPart2 = solve(2);
   std::cout << "Result Part 2 = " << nPart2 << "\n";
}
