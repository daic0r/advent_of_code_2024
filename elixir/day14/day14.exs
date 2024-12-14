defmodule Day14 do
  def simulate({{pos_x, pos_y}, {vel_x, vel_y}}, iterations, width, height) do
    new_pos_x = Integer.mod(pos_x + (vel_x * iterations), width)
    new_pos_y = Integer.mod(pos_y + (vel_y * iterations), height)

    {new_pos_x, new_pos_y}
  end

  def part1(input, width, height) do
    robots = input 
      |> Enum.map(&simulate(&1, 100, width, height))

    for op1 <- [&</2, &>/2], op2 <- [&</2, &>/2] do
      Enum.count(robots, fn {pos_x, pos_y} -> 
        op1.(pos_x, div(width, 2)) and op2.(pos_y, div(height, 2))
      end)
    end
      |> Enum.product
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
