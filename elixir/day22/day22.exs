defmodule Day22 do
  def generate_next(n) do
    import Bitwise

    n = rem(bxor(n * 64, n), 16777216)
    n = rem(bxor(div(n, 32), n), 16777216)
    n = rem(bxor(n * 2048, n), 16777216)
    n
  end

  def generate(n, 0), do: n
  def generate(n, iterations) do
    generate_next(n)
      |> generate(iterations - 1)
  end

  def generate_into(_n, list, 0), do: list |> Enum.reverse
  def generate_into(n, list, iterations) do
    generate_into(generate_next(n), [n | list], iterations - 1)
  end

  def part1(input) do
    input
      |> Enum.map(&generate(&1, 2000))
      |> Enum.sum
  end

  def part2(input) do
    input
      |> Enum.map(&generate_into(&1, [], 2000))
      |> Enum.map(fn list ->
        list
          |> Enum.map(&rem(&1, 10))
      end)
      |> Enum.map(fn list ->
        gradients = 
          list
            |> Enum.chunk_every(2, 1, :discard)
            |> Enum.map(fn [a, b] -> b - a end)
        Enum.zip(list, [nil | gradients])
      end)
      |> Enum.map(fn list_gradients ->
        sequences =
          list_gradients
            |> Enum.drop(1)
            |> Enum.chunk_every(4, 1, :discard)
        Enum.reduce(sequences, Map.new(), fn sequence, acc ->
          gradients =
            sequence
              |> Enum.map(fn {_price, gradient} -> gradient end)
          Map.put_new(acc, gradients,
            sequence |> List.last |> elem(0))
        end)
      end)
      |> Enum.reduce(Map.new(), fn number_sequences, acc ->
        Enum.reduce(number_sequences, acc, fn {seq, price}, acc ->
          Map.update(acc, seq, price, & &1 + price)
        end)
      end)
      |> Enum.max_by(&elem(&1, 1), fn -> 0 end)
      |> elem(1)
  end
  
end

# input = """
# 1
# 2
# 3
# 2024
# """
input = File.read!("input.txt")
  |> String.split("\n")
  |> Enum.filter(&String.length(&1) > 0)
  |> Enum.map(&String.to_integer/1)

part1 = Day22.part1(input)
IO.puts "Result Part 1 = #{part1}"

part2 = Day22.part2(input)
IO.puts "Result Part 2 = #{part2}"
