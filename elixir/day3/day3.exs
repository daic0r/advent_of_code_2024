defmodule Day3 do
  defp part1(input) do
    Regex.scan(~r/mul\((\d{1,3}),(\d{1,3})\)/, input)
    |> Enum.map(&tl/1)
    |> Enum.map(fn [a,b] -> {String.to_integer(a), String.to_integer(b)} end)
    |> Enum.map(fn {a,b} -> a*b end)
    |> Enum.sum
  end
  
  defp part2(input) do
    Regex.scan(~r/mul\((\d{1,3}),(\d{1,3})\)|do\(\)|don't\(\)/, input)
    |> Enum.map(fn x ->
      case length(x) do
        1 -> x 
        _ -> tl(x)
      end
    end)
    |> Enum.map(fn x ->
      case x do
        [a,b] -> {String.to_integer(a), String.to_integer(b)}
        ["do()"] -> 1
        ["don't()"] -> 0
      end
    end)
    |> Enum.reduce({0, 1}, fn elem, {sum, factor} ->
      case elem do
        {a,b} -> {sum + (factor * a * b), factor}
        1 -> {sum, 1}
        0 -> {sum, 0}
      end 
    end)
    |> elem(0)
  end

  def process(input) do
    {part1(input), part2(input)}    
  end  
end

#input = "xmul(2,4)%&mul[3,7]!@^do_not_mul(5,5)+mul(32,64]then(mul(11,8)mul(8,5))"
input = File.read!("input.txt")

{result1, result2} = Day3.process(input)
IO.puts "Result Part 1 = #{result1}"
IO.puts "Result Part 2 = #{result2}"
