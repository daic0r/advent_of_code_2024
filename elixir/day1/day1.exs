defmodule Day1 do
  def part1(list1, list2) do
    Enum.zip(Enum.sort(list1), Enum.sort(list2))
      |> Enum.map(& elem(&1,0)-elem(&1,1))
      |> Enum.map(&abs/1)
      |> Enum.sum
  end

  def part2(list1, list2) do
    list1
      |> Enum.map(fn x ->
        x * Enum.count(list2, & &1==x)
      end)
      |> Enum.sum
  end

  def process(input) do
    {list1, list2} = input
      |> String.split("\n")
      |> Enum.filter(fn s -> String.length(s) > 0 end)
      |> Enum.map(fn s ->
        splits = String.split(s)
        #List.to_tuple(splits)
        splits
      end)
      |> Enum.reduce({[], []}, fn [a, b],{list1, list2} ->
        {[String.to_integer(a) | list1], [String.to_integer(b) | list2]}
      end)

    {part1(list1, list2), part2(list1, list2)}  
  end
end

{result1, result2} = Day1.process(File.read!("input.txt"))
IO.puts "Result Part 1 = #{result1}"
IO.puts "Result Part 2 = #{result2}"
