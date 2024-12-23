defmodule Day23 do
  def get_connected(conn_map, input, output \\ [])
  def get_connected(conn_map, [_comp], output), do: output
  def get_connected(conn_map, [comp | maybe_connected], output) do
    connected_to_comp = (conn_map[comp] 
      |> Enum.filter(& &1 in maybe_connected))
    output = if length(connected_to_comp) > 0 do
      [[comp | connected_to_comp] | output]
    else
      output
    end
    get_connected(conn_map, maybe_connected, output)
  end

  def part1(input) do
    input
      |> Enum.flat_map(fn {comp, list_connected} ->
        get_connected(input, input[comp])
          |> Enum.map(& [comp | &1]) 
      end)
      |> Enum.filter(&length(&1) == 3)
      |> IO.inspect
    42
  end 
end

input = """
kh-tc
qp-kh
de-cg
ka-co
yn-aq
qp-ub
cg-tb
vc-aq
tb-ka
wh-tc
yn-cg
kh-ub
ta-co
de-co
tc-td
tb-wq
wh-td
ta-ka
td-qp
aq-cg
wq-ub
ub-vc
de-ta
wq-aq
wq-vc
wh-yn
ka-de
kh-ta
co-tc
wh-qp
tb-vc
td-yn
"""
  |> String.split("\n")
  |> Enum.filter(&String.length(&1) > 0)
  |> Enum.map(fn line ->
    String.split(line, "-")
  end)
  |> Enum.reduce(%{}, fn [a, b], acc ->
    acc = Map.update(acc, a, [b], fn list -> [b | list] end)
    Map.update(acc, b, [a], fn list -> [a | list] end)
  end)
  |> IO.inspect

part1 = Day23.part1(input)
IO.puts "Result Part 1 = #{part1}"
