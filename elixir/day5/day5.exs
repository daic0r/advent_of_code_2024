defmodule Day5 do
  defp update_valid?([], _rules), do: true
  defp update_valid?([page | rest], rules) do
    result = rest 
      |> Enum.reduce_while(true, fn other, _acc ->
        case rules |> Map.get(other, []) |> Enum.find(nil, fn p -> p == page end) do
          nil -> {:cont, true}
          _ -> {:halt, false}
        end
      end)
    if result do
      update_valid?(rest, rules)
    else
      false
    end
  end

  defp determine_validity(rules, updates) do
    updates
      |> Enum.map(fn update ->      
        {update_valid?(update, rules), update}
      end)
  end

  def part1(rules, updates) do
    determine_validity(rules, updates)
      |> Enum.filter(& elem(&1, 0) == true)
      |> Enum.map(fn {true, update} ->
        Enum.at(update, div(length(update), 2))
      end)
      |> Enum.sum
  end 

  defp fix_order(rules, update) do
    rules 
      |> Map.filter(& elem(&1, 0) in update)
      |> Enum.map(fn {k, v} -> {k, v |> Enum.filter(& &1 in update)} end)
      |> Enum.sort_by(fn {_, v} -> length(v) end, :desc)
      |> Enum.map(fn {k, _} -> k end)
  end

  def part2(rules, updates) do
    determine_validity(rules, updates)
      |> Enum.filter(& elem(&1, 0) == false)
      |> Enum.map(fn {_,update} -> fix_order(rules, update) end)
      |> Enum.map(& Enum.at(&1, div(length(&1), 2)))
      |> Enum.sum
  end
end

# input = """
# 47|53
# 97|13
# 97|61
# 97|47
# 75|29
# 61|13
# 75|53
# 29|13
# 97|29
# 53|29
# 61|53
# 97|53
# 61|29
# 47|13
# 75|47
# 97|75
# 47|61
# 75|61
# 47|29
# 75|13
# 53|13
#
# 75,47,61,53,29
# 97,61,53,29,13
# 75,29,13
# 75,97,47,61,53
# 61,13,29
# 97,13,75,29,47
# """

input = File.read!("input.txt")

[rules, updates] = input |> String.split("\n\n")

rules = rules
  |> String.split("\n")
  |> Enum.map(fn rule ->
      rule
        |> String.split("|")
        |> Enum.map(&String.to_integer/1)
        |> List.to_tuple
  end)
  |> Enum.reduce(%{}, fn {page1, page2}, acc ->
    Map.update(acc, page1, [page2], fn list -> [ page2 | list ] end)
  end)

updates = updates
  |> String.split("\n")
  |> Enum.filter(& String.length(&1) > 0)
  |> Enum.map(fn update ->
    update
      |> String.split(",")
      |> Enum.map(&String.to_integer/1)
  end)
  
result1 = Day5.part1(rules, updates)
result2 = Day5.part2(rules, updates)

IO.puts "Result Part 1 = #{result1}"
IO.puts "Result Part 2 = #{result2}"
