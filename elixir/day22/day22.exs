defmodule Day22 do
  import Bitwise

  def generate(n, 0), do: n
  def generate(n, iterations) do
    n = rem(bxor(n * 64, n), 16777216)
    n = rem(bxor(div(n, 32), n), 16777216)
    n = rem(bxor(n * 2048, n), 16777216)
    generate(n, iterations - 1)
  end

  def part1(input) do
    input
      |> Enum.map(&generate(&1, 2000))
      |> Enum.sum
  end
  
end

# input = """
# 1
# 10
# 100
# 2024
# """
input = File.read!("input.txt")
  |> String.split("\n")
  |> Enum.filter(&String.length(&1) > 0)
  |> Enum.map(&String.to_integer/1)

part1 = Day22.part1(input)
IO.puts "Result Part 1 = #{part1}"

