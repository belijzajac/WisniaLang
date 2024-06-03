import argparse

cpp_prelude = '''void calculate_N() {
  int i = 0;
  int a = 0;
  int b = 0;
  while (b < N) {
    a = a + b + i;
    b = a - b - i;
    int c = a + b;
    int d = a + b + c;
    int e = a + b + c + d;
    int f = a + b + c + d + e;
    i = f - e - d - c + 1;
  }
}'''

rust_prelude = '''fn calculate_N() {
  let mut i = 0;
  let mut a = 0;
  let mut b = 0;
  while b < N {
    a = a + b + i;
    b = a - b - i;
    let c = a + b;
    let d = a + b + c;
    let e = a + b + c + d;
    let f = a + b + c + d + e;
    i = f - e - d - c + 1;
  }
}'''


def generate_wisnia(repeat_times):
    with open('calculate.wsn', 'w') as f:
        for i in range(1, repeat_times):
            print(cpp_prelude.replace('void', 'fn').replace('N', str(i)), file=f)
        print("fn main() {", file=f)
        for i in range(1, repeat_times):
            print("  calculate_N();".replace('N', str(i)), file=f)
        print("}", file=f)


def generate_cpp(repeat_times):
    with open('calculate.cpp', 'w') as f:
        for i in range(1, repeat_times):
            print(cpp_prelude.replace('N', str(i)), file=f)
        print("int main() {", file=f)
        for i in range(1, repeat_times):
            print("  calculate_N();".replace('N', str(i)), file=f)
        print("}", file=f)


def generate_rust(repeat_times):
    with open('calculate.rs', 'w') as f:
        for i in range(1, repeat_times):
            print(rust_prelude.replace('N', str(i)), file=f)
        print("fn main() {", file=f)
        for i in range(1, repeat_times):
            print("  calculate_N();".replace('N', str(i)), file=f)
        print("}", file=f)


def main():
    parser = argparse.ArgumentParser(description="Generate programs in different programming languages for use in benchmarks.")
    parser.add_argument('--wisnia', action='store_true', help='Option for WisniaLang')
    parser.add_argument('--cpp', action='store_true', help='Option for C++')
    parser.add_argument('--rust', action='store_true', help='Option for Rust')
    parser.add_argument('repeat', type=int, help='Number of times to repeat functions')

    args = parser.parse_args()

    if args.wisnia:
        generate_wisnia(args.repeat)
    if args.cpp:
        generate_cpp(args.repeat)
    if args.rust:
        generate_rust(args.repeat)


if __name__ == '__main__':
    main()
