#include <cassert>
#include <format>
#include <iostream>
#include <numeric>
#include <ranges>
#include <set>
#include <vector>
#include <string>
#include <fstream>
#include <regex>

struct vec2d {
   long x{}, y{};

   constexpr vec2d operator+(const vec2d& rhs) const noexcept {
      return vec2d{ x + rhs.x, y + rhs.y };
   }

   constexpr vec2d operator*(long n) const noexcept {
      return vec2d{ n * x, n * y };
   }

   constexpr bool operator<(const vec2d& rhs) const noexcept {
      if (y < rhs.y)
         return true;
      if (y == rhs.y)
         return x < rhs.x;
      return false;
   }
};

struct robot {
   vec2d pos{};
   vec2d vel{};

   robot simulate(int nSeconds, int nWidth, int nHeight) const {
      robot r{};

      r.vel = vel;
      r.pos = pos + vel * nSeconds;
      r.pos.x = r.pos.x % nWidth;
      r.pos.y = r.pos.y % nHeight;

      if (r.pos.x < 0)
         r.pos.x = nWidth + r.pos.x;
      if (r.pos.y < 0)
         r.pos.y = nHeight + r.pos.y;

      return r;
   }
};

std::ostream& operator<<(std::ostream& os, const robot& r) {
   std::cout << std::format("p={},{} v={},{}", r.pos.x, r.pos.y, r.vel.x, r.vel.y);
   return os;
}

constexpr long part1(std::span<robot> vInput, int nWidth, int nHeight) {
   std::array<long, 4> arQuadrants{};

   for (const auto& robot : vInput) {
      auto res = robot.simulate(100, nWidth, nHeight);
         
      if (res.pos.x < nWidth/2 and res.pos.y < nHeight/2)
         ++arQuadrants[0];
      else
      if (res.pos.x < nWidth/2 and res.pos.y > nHeight/2)
         ++arQuadrants[1];
      else
      if (res.pos.x > nWidth/2 and res.pos.y < nHeight/2)
         ++arQuadrants[2];
      else
      if (res.pos.x > nWidth/2 and res.pos.y > nHeight/2)
         ++arQuadrants[3];
   }

   return std::accumulate(arQuadrants.begin(), arQuadrants.end(), 1L, std::multiplies{});
}

void output_image(std::span<robot> vInput, int nWidth, int nHeight, int nFileIndex) {
   std::ofstream f{ std::format("img/{}.pbm", nFileIndex) };

   f << "P1\n";
   f << nWidth << " " << nHeight << "\n";

   std::set<vec2d> sPositions;
   for (const auto& r : vInput) {
      sPositions.insert(r.pos);
   }

   for (auto y = 0z; y < nHeight; ++y) {
      for (auto x = 0z; x < nWidth; ++x) {
         if (sPositions.contains(vec2d{ x, y }))
            f << 1;
         else
            f << 0;
         if (x < nWidth-1)
            f << " ";
      }
      f << "\n";
   }

   f.close();
}

void part2(std::span<robot> vInput, int nWidth, int nHeight) {
   std::vector<robot> vRobots;
   vRobots.reserve(vInput.size());
   // for (auto i = 5000; i < 10000; ++i) {
   //    vRobots.clear();

   for (const auto& robot : vInput) {
      const auto res = robot.simulate(7847, nWidth, nHeight);
      vRobots.push_back(res);
   }

   output_image(vRobots, nWidth, nHeight, 7847);
   //}
}

int main() {
//    const std::string strData = "p=0,4 v=3,-3\n\
// p=6,3 v=-1,-3\n\
// p=10,3 v=-1,2\n\
// p=2,0 v=2,-1\n\
// p=0,0 v=1,3\n\
// p=3,0 v=-2,-2\n\
// p=7,6 v=-1,-3\n\
// p=3,0 v=-1,-2\n\
// p=9,3 v=2,3\n\
// p=7,3 v=-1,2\n\
// p=2,4 v=2,-3\n\
// p=9,5 v=-3,-3";

   std::ifstream f{ "input.txt" };
   const std::string strData{ std::istreambuf_iterator<char>{ f }, std::istreambuf_iterator<char>{} };
   f.close();

   auto tmp = strData
      | std::views::split('\n')
      | std::views::transform([](auto rng) { return std::string_view{ rng.begin(), rng.end() }; })
      | std::views::filter([](std::string_view strLine) { return not strLine.empty(); })
      | std::views::transform([](std::string_view strLine) { 
         const std::regex rex{ R"(p=(-?\d+),(-?\d+) v=(-?\d+),(-?\d+))" };
         std::cmatch match;

         assert(std::regex_match(strLine.begin(), strLine.end(), match, rex));

         const auto posX = std::stoi(match[1].str());
         const auto posY = std::stoi(match[2].str());
         const auto velX = std::stoi(match[3].str());
         const auto velY = std::stoi(match[4].str());

         return robot{ posX, posY, velX, velY };
      });

   std::vector<robot> vInput{ tmp.begin(), tmp.end() };


   const auto nPart1 = part1(vInput, 101, 103);
   std::cout << "Result Part 1 = " << nPart1 << std::endl;

   part2(vInput, 101, 103);
}

