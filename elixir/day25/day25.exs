defmodule Day25 do
  def get_column_heights(key_or_lock, width) do
    key_or_lock 
      |> Enum.reduce(List.duplicate(0, width), fn line, acc ->
        Enum.zip(acc, line
          |> String.graphemes
          |> Enum.map(fn ch ->
            case ch do
              "#" -> 1
              "." -> 0
            end
          end))
        |> Enum.map(fn {a,b} -> a + b end)
      end)
  end
end

keys_and_locks = File.read!("input.txt")
  |> String.split("\n\n")
  |> Enum.filter(& String.length(&1) > 0)

locks = keys_and_locks
  |> Enum.filter(fn lines ->
    lines
      |> String.split("\n")
      |> Enum.filter(&String.length(&1) > 0)
      |> hd
      |> String.graphemes
      |> Enum.all?(& &1 == "#")
  end)
  |> Enum.map(fn str ->
    str
      |> String.split("\n")
      |> Enum.filter(&String.length(&1) > 0)
      |> Enum.drop(1)
  end)

keys = keys_and_locks
  |> Enum.filter(fn lines ->
    lines
      |> String.split("\n")
      |> Enum.filter(&String.length(&1) > 0)
      |> List.last
      |> String.graphemes
      |> Enum.all?(& &1 == "#")
  end)
  |> Enum.map(fn str ->
    lines = str
      |> String.split("\n")
      |> Enum.filter(&String.length(&1) > 0)
    lines
      |> Enum.take(length(lines) - 1)
  end)

height = keys_and_locks
  |> hd
  |> String.split("\n")
  |> Enum.filter(&String.length(&1) > 0)
  |> length
  |> (&(&1 - 2)).()

width = keys
  |> hd
  |> hd
  |> String.length

key_heights = keys 
  |> Enum.map(& Day25.get_column_heights(&1, width))
lock_heights = locks
  |> Enum.map(& Day25.get_column_heights(&1, width))

cartesian_product = for key_height <- key_heights, lock_height <- lock_heights, do: {key_height, lock_height}

result = 
  cartesian_product
    |> Enum.count(fn {key, lock} ->
      Enum.zip(key, lock)
        |> Enum.all?(fn {a, b} ->
          a + b <= height
        end)
    end)

IO.puts "Result = #{result}"
