fn fibonacci(n: u32) -> u32 {
  if n <= 1 {
    return n;
  }
  let (mut prev, mut current) = (0, 1);
  for _ in 2..=n {
    let next = prev + current;
    prev = current;
    current = next;
  }
  current
}

fn main() {
  println!("{}", fibonacci(46));
}
