use itertools::Itertools;

fn part1(data: &Vec<Vec<i32>>) -> usize {
    data
      .iter()
      .map(|row| row.iter().tuple_windows().map(|(x,y)| y-x).collect::<Vec<_>>())
      .filter(|row| row.iter().all(|x| (1..=3).contains(&x.abs())))
      .map(|row| {
          row
            .iter()
            .all(|x| x.signum() == row.first().unwrap().signum())
      })
      .filter(|v| *v)
      .count()
}

fn part2(data: &Vec<Vec<i32>>) -> usize {
    data
      .iter()
      .map(|row| row.iter().tuple_windows().map(|(x,y)| y-x).collect::<Vec<_>>())
      .map(|row| {
        let res = row.iter().find_position(|x| !(1..=3).contains(&x.abs()));
        match res {
            Some((idx, &num)) => {
                let mut new_row = row.clone();
                let new_val = if idx < row.len() - 1 {
                    num + row[idx+1]
                } else {
                    num
                };
                new_row[idx] = new_val;
                if idx < row.len() - 1 {
                    new_row.remove(idx+1);
                }
                new_row
            },
            None => row
        }
      })
      .filter(|row| {
          row.iter().all(|x| (1..=3).contains(&x.abs()))
          && 
          (row.iter().filter(|&&x| x < 0).count() < 2 || row.iter().filter(|&&x| x > 0).count() < 2)
       })
      .count()
}

fn main() {
//     let input = "7 6 4 2 1
// 1 2 7 8 9
// 9 7 6 2 1
// 1 3 2 4 5
// 8 6 4 4 1
// 1 3 6 7 9";
    let input = include_str!("../input.txt");

    let data = input
      .split("\n")
      .filter(|line| !line.is_empty())
      .map(|line| line.split(" "))
      .map(|str_nums| str_nums.map(|str_num| str_num.parse::<i32>().unwrap()).collect::<Vec<_>>())
      .collect::<Vec<_>>();   

    dbg!(&data); 

    let part1 = part1(&data);
    let part2 = part2(&data);

    println!("Result Part 1 = {}", part1);
    println!("Result Part 2 = {}", part2);
}
