#include <cassert>
#include <cmath>
#include <format>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include <fstream>
#include <ranges>

template<template<typename> typename Container>
std::ostream& operator<<(std::ostream& os, const Container<long>& v) {
   for (const auto [idx,val] : v | std::views::enumerate) {
      assert(val < 8);
      os << val;
      if (idx < v.size() - 1)
         os << ",";
   }
   return os;
}

enum class Instruction {
   adv,
   bxl,
   bst,
   jnz,
   bxc,
   out,
   bdv,
   cdv
};

struct sProgram {
   long A_, B_, C_;
   std::vector<long> vInstructions_;
   std::size_t IP_{};

   long get_combo_operand(long n) const {
      switch (n) {
         case 0:
         case 1:
         case 2:
         case 3:
            return n;
         case 4:
            return A_;
         case 5:
            return B_;
         case 6:
            return C_;
         case 7:
            throw std::logic_error("reserved");
      }
      std::unreachable();
   }

   void reset() {
      IP_ = 0;
      B_ = 0;
      C_ = 0;
   }

   std::vector<long> run() {
      std::vector<long> vOutput;

      while (IP_ < vInstructions_.size()) {
         const auto instr = static_cast<Instruction>(vInstructions_[IP_]);
         bool bIsJump{};
         switch (instr) {
            case Instruction::adv:
               A_ = A_ / std::pow(2, get_combo_operand(vInstructions_[IP_ + 1]));
               break;
            case Instruction::bxl:
               B_ = B_ ^ vInstructions_[IP_ + 1];
               break;
            case Instruction::bst:
               B_ = get_combo_operand(vInstructions_[IP_ + 1]) & 7L;
               break;
            case Instruction::jnz:
               if (A_ != 0) {
                  bIsJump = true;
                  IP_ = vInstructions_[IP_ + 1];
               }
               break;
            case Instruction::bxc:
               B_ = B_ ^ C_;
               break;
            case Instruction::out:
            {
               const auto res = get_combo_operand(vInstructions_[IP_ + 1]) & 7L;
               assert(res < 8);
               vOutput.push_back(res);
            }
               break;
            case Instruction::bdv:
               B_ = A_ / std::pow(2, get_combo_operand(vInstructions_[IP_ + 1]));
               break;
            case Instruction::cdv:
               C_ = A_ / std::pow(2, get_combo_operand(vInstructions_[IP_ + 1]));
               break;
         }
         if (not bIsJump)
            IP_ += 2;
      }

      return vOutput;
   }
};

std::string part1(sProgram& prog) {
   const auto res = prog.run();

   std::stringstream ss;
   ss << res;

   return ss.str();
}

long part2(sProgram& prog, long start_val) {
   for (long i = 0; i < 8; ++i) {
      const auto A = (start_val << 3) + i;
      prog.reset();
      prog.A_ = A;
      const auto vNums = prog.run();
      if (std::equal(std::prev(prog.vInstructions_.end(), vNums.size()), prog.vInstructions_.end(), vNums.begin(), vNums.end())) {
         if (vNums.size() == prog.vInstructions_.size()) {
            return A;
         }
         if (const auto res = part2(prog, A); res < std::numeric_limits<long>::max())
            return res;
      }
   }
   return std::numeric_limits<long>::max();
}

int main() {
//    const std::string strData = "Register A: 729\n\
// Register B: 0\n\
// Register C: 0\n\
// \n\
// Program: 0,1,5,4,3,0";

   std::ifstream f{ "input.txt" };
   const std::string strData{ std::istreambuf_iterator<char>{ f }, std::istreambuf_iterator<char>{} };
   f.close();

   auto tmp = strData
      | std::views::split('\n')
      | std::views::transform([](auto rng) {
         return std::string_view(rng.begin(), rng.end());
      });

   sProgram prog{};

   auto iter = tmp.begin();
   std::string_view line = *iter++;
   line.remove_prefix(12);
   prog.A_ = std::stol(line.data());

   line = *iter++;
   line.remove_prefix(12);
   prog.B_ = std::stol(line.data());

   line = *iter++;
   line.remove_prefix(12);
   prog.C_ = std::stol(line.data());

   ++iter;

   line = *iter;
   line.remove_prefix(9);
   
   auto tmp2 = line
      | std::views::split(',')
      | std::views::transform([](auto rng) { return std::string_view{ rng.begin(), rng.begin() }; })
      | std::views::transform([](std::string_view s) { return std::stol(s.data()); });
   prog.vInstructions_ = std::vector<long>{ tmp2.begin(), tmp2.end() };

   const auto strPart1 = part1(prog);
   std::cout << "Result Part 1 = " << strPart1 << std::endl;
   const auto strPart2 = part2(prog, 0);
   std::cout << "Result Part 2 = " << strPart2 << std::endl;

}
