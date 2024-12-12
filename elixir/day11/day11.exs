defmodule Day11 do
  def get_digit_count(num) do
    trunc(:math.log10(num)) + 1
  end

  def split_number(num) do
    divisor = trunc(:math.pow(10, div(trunc(:math.log10(num)), 2) + 1))
    { div(num, divisor), rem(num, divisor) }
  end

  def blink(m, 0), do: m
  def blink(m, iterations) do
    m
      |> Enum.reduce(%{}, fn {num, count}, acc -> 
        cond do
          num == 0 -> 
            acc
              |> Map.update(1, count, &(&1 + count))
          rem(get_digit_count(num), 2) == 0 ->
            {left, right} = split_number(num)
            acc
              |> Map.update(left, count, &(&1 + count))
              |> Map.update(right, count, &(&1 + count))
          true -> 
            Map.update(acc, num * 2024, count, &(&1 + count))
        end
      end)
      |> blink(iterations - 1)
  end

  def solve(input, num_blinks) do
    numbers = input
      |> String.split(" ")
      |> Enum.map(&String.to_integer/1)
    
    numbers 
      |> Enum.map(fn num -> {num, Enum.count(numbers, & &1 == num)} end)
      |> Enum.uniq
      |> Enum.group_by(& elem(&1, 0))
      |> Map.new(fn {num, [{_, count}]} -> {num, count} end)
      |> blink(num_blinks)
      |> Enum.reduce(0, fn {_k, v}, acc -> acc + v end)
  end
end

input = "28 4 3179 96938 0 6617406 490 816207"
part1 = Day11.solve(input, 25)
IO.puts "Result Part 1 = #{part1}"
part2 = Day11.solve(input, 75)
IO.puts "Result Part 2 = #{part2}"
