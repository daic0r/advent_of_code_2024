defmodule Day7 do
  defp concat_numbers(a, b) do
    a * (Integer.pow(10, trunc(:math.log10(b)) + 1)) + b 
  end

  defp get_operators(1), do: [ {:add, &+/2}, {:mul, &*/2} ]
  defp get_operators(2), do: [ 
    {:add, &+/2},
    {:mul, &*/2},
    {:concat, fn left, right ->
      case {left, right}do
        {nil, _} -> right
        _ -> concat_numbers(left, right)
      end
    end}
  ]

  defp calculate([head], acc, part) do
    get_operators(part)
      |> Enum.map(fn {_, op} ->
        op.(acc, head)
      end)
  end
  defp calculate([head | tail], acc, part) do
    get_operators(part)    
      |> Enum.map(fn {op_name, op} ->
        acc = case acc do
          nil -> case op_name do 
            :add -> 0
            :mul -> 1
            :concat -> nil
          end
          _ -> acc
        end
        calculate(tail, op.(acc, head), part)
      end)
  end

  def solve(input, part) do
    input
      |> Enum.map(fn [result, inputs] ->
        results = calculate(inputs, nil, part)
          |> List.flatten
        {result, result in results}
      end)
      |> Enum.filter(fn {_, valid} -> valid end)
      |> Enum.map(fn {result, _} -> result end)
      |> Enum.sum
  end 
end

input = File.read!("input.txt")
  |> String.split("\n")
  |> Enum.filter(& String.length(&1) > 0)
  |> Enum.map(&String.split(&1, ": "))
  |> Enum.map(fn [result, inputs] ->
    [ String.to_integer(result), String.split(inputs, " ") |> Enum.map(&String.to_integer/1)]
  end)

result1 = Day7.solve(input, 1)
IO.puts "Result Part 1 = #{result1}"
result2 = Day7.solve(input, 2)
IO.puts "Result Part 2 = #{result2}"
