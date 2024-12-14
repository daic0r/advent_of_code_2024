defmodule Day14 do
  def simulate({{pos_x, pos_y}, {vel_x, vel_y}}, iterations, width, height) do
    new_pos_x = rem(pos_x + (vel_x * iterations), width)
    new_pos_y = rem(pos_y + (vel_y * iterations), height)

    new_pos_x = 
      if new_pos_x < 0 do
        width + new_pos_x     
      else
        new_pos_x
      end
    new_pos_y = 
      if new_pos_y < 0 do
        height + new_pos_y     
      else
        new_pos_y
      end

    {new_pos_x, new_pos_y}
  end

  def part1(input, width, height) do
    robots = input 
      |> Enum.map(&simulate(&1, 100, width, height))

    quad1 = robots
      |> Enum.count(fn {pos_x, pos_y} ->
        pos_x < div(width, 2) and pos_y < div(height, 2)
      end)
    quad2 = robots
      |> Enum.count(fn {pos_x, pos_y} ->
        pos_x < div(width, 2) and pos_y > div(height, 2)
      end)
    quad3 = robots
      |> Enum.count(fn {pos_x, pos_y} ->
        pos_x > div(width, 2) and pos_y < div(height, 2)
      end)
    quad4 = robots
      |> Enum.count(fn {pos_x, pos_y} ->
        pos_x > div(width, 2) and pos_y > div(height, 2)
      end)

    quad1 * quad2 * quad3 * quad4
  end

  def part2(input, width, height) do
    robots = input
      |> Enum.map(&simulate(&1, 7847, width, height))

    positions =
      robots
        |> Enum.reduce(MapSet.new(), fn {pos_x, pos_y}, acc ->
          MapSet.put(acc, {pos_x, pos_y}) 
        end)

    content = Enum.reduce(1..height, "P1\n#{width} #{height}\n", fn row, acc ->
      line = Enum.reduce(1..width, "", fn col, acc ->
        if MapSet.member?(positions, {col-1, row-1}) do
          acc <> "1 "
        else
          acc <> "0 "
        end
      end)
      acc <> "#{line}\n"
    end)

    File.write!("7847.pbm", content)
  end
end

input = File.read!("input.txt")
  |> String.split("\n")
  |> Enum.filter(&String.length(&1) > 0)
  |> Enum.map(fn line ->
    [pos_x, pos_y, vel_x, vel_y] = 
      Regex.run(~r/p=(-?\d+),(-?\d+) v=(-?\d+),(-?\d+)/, line, capture: :all_but_first)
        |> Enum.map(&String.to_integer/1)
    {{pos_x, pos_y}, {vel_x, vel_y}}
  end)

part1 = Day14.part1(input, 101, 103)
IO.puts "Result Part 1 = #{part1}"

Day14.part2(input, 101, 103)
