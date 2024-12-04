defmodule Day4 do
@part1_dirs [
  {1, 0},
  {-1, 0},
  {0, 1},
  {0, -1},
  {1, 1},
  {-1, 1},
  {1, -1},
  {-1, -1}
]

@part2_dirs [
  {1, 1},
  {-1, 1},
  {1, -1},
  {-1, -1}
]

  defp part1_dirs, do: @part1_dirs
  defp part2_dirs, do: @part2_dirs

  defp search(vec, input, str, row, col)
  defp search(_vec, _input, <<>>, _row, _col) do
    true
  end
  defp search({dir_x, dir_y}, input, <<next_char, rest::binary>>, row, col) do
    line = Map.get(input, row)
    if line != nil and col >= 0 and String.at(line, col) == <<next_char>> do
      search({dir_x, dir_y}, input, rest, row + dir_y, col + dir_x)
    else
      false
    end
  end

  def part1(input) do
    input
      |> Enum.map(fn {row, line} ->
        line_results = for i <- 0..String.length(line)-1 do
          part1_dirs()
            |> Enum.map(fn vec ->
              search(vec, input, "XMAS", row, i)
            end)
            |> Enum.count(& &1 == true)
        end
        line_results
          |> Enum.sum
      end)
      |> Enum.sum
  end  

  def part2(input) do
    positions = 
      input
        |> Enum.map(fn {row, line} ->
          for col <- 0..String.length(line)-1 do
            part2_dirs()
              |> Enum.map(fn vec ->
                {{col+elem(vec,0), row+elem(vec,1)}, search(vec, input, "MAS", row, col)}
              end)
              |> Enum.filter(& elem(&1, 1) == true)
              |> Enum.map(& elem(&1, 0))
          end
          |> List.flatten
        end)
        |> List.flatten

    positions
      |> Enum.filter(& Enum.count(positions, fn pos -> pos == &1 end) == 2)
      |> Enum.uniq
      |> Enum.count
  end
end


# input = """
# MMMSXXMASM
# MSAMXMSMSA
# AMXSXMAAMM
# MSAMASMSMX
# XMASAMXAMM
# XXAMMXXAMA
# SMSMSASXSS
# SAXAMASAAA
# MAMMMXMMMM
# MXMXAXMASX
# """

input = File.read!("input.txt")
tups = input
  |> String.split("\n")
  |> Enum.filter(&String.length(&1) > 0)
  |> Enum.with_index(fn line, row ->
    {row, line}
  end)
map = for {row, line} <- tups, into: %{}, do: {row, line}

task1 = Task.async(fn -> Day4.part1(map) end)
task2 = Task.async(fn -> Day4.part2(map) end)
part1 = Task.await(task1)
part2 = Task.await(task2)

IO.puts "Result Part 1 = #{part1}"
IO.puts "Result Part 2 = #{part2}"
