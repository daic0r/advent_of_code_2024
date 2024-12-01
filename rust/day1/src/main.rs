use std::iter::zip;

fn part1(list1: &Vec<i32>, list2: &Vec<i32>) -> i32 {
    let sum = zip(list1, list2)
        .map(|(a,b)| (a-b).abs())
        .sum::<i32>();

    sum
}

fn part2(list1: &Vec<i32>, list2: &Vec<i32>) -> i32 {
    list1
        .iter()
        .map(|&e| {
            let count = list2.iter().filter(|&&x| x == e).count() as i32;
            count * e
        })
        .sum::<i32>()
}


fn main() {
    let input = "3   4
4   3
2   5
1   3
3   9
3   3";

    let input = std::fs::read_to_string("input.txt").unwrap();

    let mut list1 = vec![];
    let mut list2 = vec![];

    input
        .split("\n")
        .filter(|e| e.len() > 0)
        .map(|e| {
            let mut splits = e.split_whitespace();
            (splits.next().unwrap().parse::<i32>().unwrap(), splits.next().unwrap().parse::<i32>().unwrap())
        })
        .for_each(|(i,j)| {
            list1.push(i);
            list2.push(j);
        });

    list1.sort();
    list2.sort();

    println!("Result Part 1 = {}", part1(&list1, &list2));
    println!("Result Part 2 = {}", part2(&list1, &list2));
}
