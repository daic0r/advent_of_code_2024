defmodule Day2 do
  defp is_pair_valid(dir, {a, b}) when dir == :asc and a-b in -3..-1, do: true
  defp is_pair_valid(dir, {a, b}) when dir == :desc and a-b in 1..3, do: true
  defp is_pair_valid(_dir, {_a, _b}), do: false

  defp is_row_valid(row) do
    [first, second | _] = row
    dir = if first < second, do: :asc, else: :desc
    {result, _} = 
      row
        |> Enum.drop(1)
        |> Enum.reduce_while({true, first}, fn e, {_result, prev} ->
          # IO.inspect "#{prev}-#{e} = #{prev - e}"
          if is_pair_valid(dir, {prev, e}) do
            {:cont, {true, e}}
          else
            {:halt, {false, e}}
          end
        end)

    result
  end

  def part1(data) do
    data
      |> Enum.map(& is_row_valid(&1))
      |> Enum.count(& &1 == true)
  end

  def part2(data) do
    data
      |> Enum.map(fn row ->
        row
          |> Enum.chunk_every(2, 1, :discard)
          |> Enum.map(fn [a, b] ->
            b - a
          end)
      end)
      |> Enum.map(fn grads ->
# Too big?
        invalid_idx = Enum.find_index(grads, & abs(&1) not in 1..3)
        if !invalid_idx do
          grads
        else
          {a, b} = Enum.split(grads, invalid_idx)
# Sum the first two elements of b and replace them by the sum to simulate removal of the rogue element
          case b do
            [x, y | tail] -> a ++ [x + y | tail]
            [x, y] -> a ++ [x + y]
            [x] -> a ++ [x]
          end
        end
      end)
      |> Enum.filter(fn grads ->
# Keep the ones that have only values within the valid range after removal, but
# allow up to one gradient with the wrong sign (because above we only remove elements
# that are outside the range 1..3, therefore we could still remove one of these
# elements with a wrong sign now)
        Enum.all?(grads, & abs(&1) in 1..3)
        and
        (Enum.count(grads, & &1 < 0) < 2 or Enum.count(grads, & &1 > 0) < 2)
      end)
      |> IO.inspect
      |> Enum.count
  end

  def process(input) do
    data = input
      |> String.split("\n")
      |> Enum.filter(&String.length(&1) > 0)
      |> Enum.map(&String.split(&1, " "))
      |> Enum.map(fn row -> row |> Enum.map(&String.to_integer/1) end)
    
    {part1(data), part2(data)} 
  end   
end

input = File.read!("input.txt")
# input = "7 6 4 2 1
# 1 2 7 8 9
# 9 7 6 2 1
# 1 3 2 4 5
# 8 6 4 4 1
# 1 3 6 7 9"

{result1, result2} = Day2.process(input)
IO.puts "Result Part 1 = #{result1}"
IO.puts "Result Part 2 = #{result2}"
