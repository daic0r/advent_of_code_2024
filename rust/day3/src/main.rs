use regex::Regex;

fn part1(data: &str) -> i32 {
    let rex = Regex::new(r"mul\((\d{1,3}),(\d{1,3})\)").unwrap();
    rex
        .captures_iter(data)
        .map(|m| m.extract())
        .map(|(_, [a, b])| (a.parse::<i32>().unwrap(), b.parse::<i32>().unwrap()))
        .map(|(a,b)| a * b)
        .sum()
}

fn part2(data: &str) -> i32 {
    let rex = Regex::new(r"mul\((\d{1,3}),(\d{1,3})\)|do\(\)|don't\(\)").unwrap();
    rex
        .captures_iter(data)
        .fold((0, 1), |(sum, factor), caps| {
            let first = caps.get(0).map(|m| m.as_str()).unwrap();
            if first == "do()" {
                (sum, 1)
            } else 
            if first == "don't()" {
                (sum, 0) 
            } else {
                let a = caps.get(1).map(|m| m.as_str()).unwrap().parse::<i32>().unwrap();
                let b = caps.get(2).map(|m| m.as_str()).unwrap().parse::<i32>().unwrap();
                (sum + (factor * a * b), factor)
            }
        })
        .0
}

fn main() {
    // let data = "xmul(2,4)%&mul[3,7]!@^do_not_mul(5,5)+mul(32,64]then(mul(11,8)mul(8,5))";
    // let data = "xmul(2,4)&mul[3,7]!^don't()_mul(5,5)+mul(32,64](mul(11,8)undo()?mul(8,5))";
    let data = include_str!("../input.txt");

    let part1 = part1(data);
    let part2 = part2(data);

    println!("Result Part 1 = {}", part1);
    println!("Result Part 2 = {}", part2);
}
