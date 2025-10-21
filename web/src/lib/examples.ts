export interface Example {
  id: string
  name: string
  description: string
  difficulty: string
  code: string
}

export const EXAMPLES: Example[] = [
  {
    id: 'hello',
    name: 'Hello World',
    description: 'The simplest program - print a number',
    difficulty: '⭐',
    code: 'print(42);',
  },
  {
    id: 'factorial',
    name: 'Factorial',
    description: 'Calculate factorial using recursion',
    difficulty: '⭐⭐',
    code: `func factorial(n) {
    if (n <= 1) {
        return 1;
    }
    return n * factorial(n - 1);
}

print(factorial(5));`,
  },
  {
    id: 'fibonacci',
    name: 'Fibonacci Sequence',
    description: 'Calculate Fibonacci number using recursion',
    difficulty: '⭐⭐',
    code: `func fib(n) {
    if (n <= 1) {
        return n;
    }
    return fib(n - 1) + fib(n - 2);
}

print(fib(10));`,
  },
  {
    id: 'sum',
    name: 'Sum to N',
    description: 'Calculate sum of numbers from 1 to N using a loop',
    difficulty: '⭐⭐',
    code: `func sum_to_n(n) {
    let sum = 0;
    let i = 1;
    while (i <= n) {
        sum = sum + i;
        i = i + 1;
    }
    return sum;
}

print(sum_to_n(100));`,
  },
  {
    id: 'max',
    name: 'Maximum Value',
    description: 'Find the maximum of two numbers',
    difficulty: '⭐⭐',
    code: `func max(a, b) {
    if (a > b) {
        return a;
    }
    return b;
}

print(max(10, 20));`,
  },
  {
    id: 'gcd',
    name: 'Greatest Common Divisor',
    description: 'Calculate GCD using Euclidean algorithm',
    difficulty: '⭐⭐⭐',
    code: `func gcd(a, b) {
    if (b == 0) {
        return a;
    }
    return gcd(b, a % b);
}

print(gcd(48, 18));`,
  },
  {
    id: 'prime',
    name: 'Prime Checker',
    description: 'Check if a number is prime',
    difficulty: '⭐⭐⭐',
    code: `func is_prime(n) {
    if (n < 2) {
        return 0;
    }
    let i = 2;
    while (i * i <= n) {
        if (n % i == 0) {
            return 0;
        }
        i = i + 1;
    }
    return 1;
}

print(is_prime(17));`,
  },
  {
    id: 'power',
    name: 'Power Function',
    description: 'Calculate x raised to the power of n',
    difficulty: '⭐⭐',
    code: `func power(x, n) {
    if (n == 0) {
        return 1;
    }
    return x * power(x, n - 1);
}

print(power(2, 10));`,
  },
  {
    id: 'collatz',
    name: 'Collatz Conjecture',
    description: 'Count steps in Collatz sequence',
    difficulty: '⭐⭐⭐',
    code: `func collatz_steps(n) {
    let steps = 0;
    while (n != 1) {
        if (n % 2 == 0) {
            n = n / 2;
        } else {
            n = 3 * n + 1;
        }
        steps = steps + 1;
    }
    return steps;
}

print(collatz_steps(27));`,
  },
  {
    id: 'ackermann',
    name: 'Ackermann Function',
    description: 'Deep recursion test with Ackermann function',
    difficulty: '⭐⭐⭐⭐',
    code: `func ackermann(m, n) {
    if (m == 0) {
        return n + 1;
    }
    if (n == 0) {
        return ackermann(m - 1, 1);
    }
    return ackermann(m - 1, ackermann(m, n - 1));
}

print(ackermann(3, 3));`,
  },
]
