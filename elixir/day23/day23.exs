defmodule Day23 do
  def get_connected(conn_map, input, output \\ [])
  def get_connected(_conn_map, [_comp], output), do: output
  def get_connected(conn_map, [comp | maybe_connected], output) do
    connected_to_comp = (conn_map[comp] 
      |> Enum.filter(& &1 in maybe_connected))
      |> Enum.map(& [comp, &1])
    get_connected(conn_map, maybe_connected, connected_to_comp ++ output)
  end

  def part1(input) do
    input
      |> Enum.flat_map(fn {comp, _list_connected} ->
        get_connected(input, input[comp])
          |> Enum.map(& [comp | &1] |> Enum.sort) 
      end)
      |> Enum.uniq
      |> Enum.filter(fn list ->
        list
          |> Enum.any?(&String.starts_with?(&1, "t"))
      end)
      |> Enum.count
  end 

  def part2(input) do
    input
      |> Enum.map(fn {comp, _list_connected} ->
        get_connected(input, input[comp])
          |> Enum.flat_map(& [comp | &1] |> Enum.sort) 
      end)
      |> Enum.group_by(&hd/1)
      |> Enum.filter(fn {_first, lists} ->
        Enum.all?(lists, &length(&1) == length(hd(lists))) 
      end)
      |> Enum.max_by(fn {_comp, lists} ->
        length(hd(lists))
      end)
      |> elem(1)
      |> hd
      |> Enum.uniq
      |> Enum.sort
      |> Enum.join(",")
  end
end

input = File.read!("input.txt")
  |> String.split("\n")
  |> Enum.filter(&String.length(&1) > 0)
  |> Enum.map(fn line ->
    String.split(line, "-")
  end)
  |> Enum.reduce(%{}, fn [a, b], acc ->
    acc = Map.update(acc, a, [b], fn list -> [b | list] end)
    Map.update(acc, b, [a], fn list -> [a | list] end)
  end)

part1 = Day23.part1(input)
IO.puts "Result Part 1 = #{part1}"
part2 = Day23.part2(input)
IO.puts "Result Part 2 = #{part2}"
