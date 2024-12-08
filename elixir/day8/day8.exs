defmodule Day8 do
  defp add_vec(vec1, vec2) do
    {elem(vec1, 0) + elem(vec2, 0), elem(vec1, 1) + elem(vec2, 1)}
  end
  defp sub_vec(vec1, vec2) do
    {elem(vec1, 0) - elem(vec2, 0), elem(vec1, 1) - elem(vec2, 1)}
  end

  defp within_bounds?(pos, width, height) when elem(pos, 0) >= 0 and elem(pos, 0) < width and elem(pos, 1) >= 0 and elem(pos, 1) < height, do: true
  defp within_bounds?(_pos, _width, _height), do: false

  defp traverse(pos, diff, width, height, op, output) do
    new_pos = case op do
      :add -> add_vec(pos, diff)
      :sub -> sub_vec(pos, diff)
    end
    if within_bounds?(new_pos, width, height) do
      traverse(new_pos, diff, width, height, op, [new_pos | output])
    else
      output
    end
  end

  defp get_antinodes([], _width, _height, _part), do: []
  defp get_antinodes([head | tail], width, height, part) do
    [tail
      |> Enum.map(fn other ->
        diff = sub_vec(other, head)
        if part == 1 do
          [sub_vec(head, diff), add_vec(other, diff)]
        else
          traverse(other, diff, width, height, :add, [other]) ++ traverse(head, diff, width, height, :sub, [head])
        end
      end)
      |> List.flatten
      |> Enum.filter(fn pos ->
        within_bounds?(pos, width, height)
      end) | get_antinodes(tail, width, height, part)]
        |> List.flatten
      
  end

  def solve(input, part) do
    input
      |> Enum.with_index(fn line, row ->
        {row, line}
      end)
      |> Enum.map(fn {row, line} ->      
        line
          |> String.graphemes
          |> Enum.with_index(fn ch, idx ->
            {{idx, row}, ch}
          end)
          |> Enum.filter(fn {_idx, ch} ->
            ch != "." 
          end)
      end)
      |> List.flatten
      |> Enum.group_by(fn {_pos, antenna} ->
        antenna
      end)
      |> Map.new(fn {antenna, positions} ->
        {antenna, positions |> Enum.map(fn {pos, _} -> pos end)}
      end)
      |> Map.new(fn {antenna, positions} ->
        {
          antenna, 
          get_antinodes(positions, length(input), String.length(hd(input)), part)
            |> Enum.sort_by(fn {_x, y} -> y end)            
        }
      end)
      |> Map.values
      |> List.flatten
      |> Enum.uniq
      |> length
  end
end

# input = """
# ............
# ........0...
# .....0......
# .......0....
# ....0.......
# ......A.....
# ............
# ............
# ........A...
# .........A..
# ............
# ............
# """
input = File.read!("input.txt")
|> String.split("\n")
|> Enum.filter(&String.length(&1) > 0)

part1 = Day8.solve(input, 1)
IO.puts "Result Part 1 = #{part1}"
part2 = Day8.solve(input, 2)
IO.puts "Result Part 2 = #{part2}"
