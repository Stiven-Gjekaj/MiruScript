<div align="center">

# 📚 Miru Example Programs

### Learn by Example

_Complete, runnable programs demonstrating Miru language features_

<p align="center">
  <img src="https://img.shields.io/badge/Examples-10-blue?style=for-the-badge" alt="Examples"/>
  <img src="https://img.shields.io/badge/Difficulty-Beginner_to_Advanced-green?style=for-the-badge" alt="Difficulty"/>
  <img src="https://img.shields.io/badge/Topics-Algorithms-orange?style=for-the-badge" alt="Topics"/>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/Recursion-7_examples-purple?style=flat-square" alt="Recursion"/>
  <img src="https://img.shields.io/badge/Loops-5_examples-red?style=flat-square" alt="Loops"/>
  <img src="https://img.shields.io/badge/Math-8_examples-yellow?style=flat-square" alt="Math"/>
</p>

<p align="center" style="font-weight: bold;">
  <a href="#-quick-reference">Quick Reference</a> •
  <a href="#-beginner-examples">Beginner</a> •
  <a href="#-intermediate">Intermediate</a> •
  <a href="#-advanced">Advanced</a>
</p>

</div>

---

## 🎯 Quick Reference

| Program | Difficulty | Concepts | Output | Complexity |
|---------|-----------|----------|--------|------------|
| [hello.mi](hello.mi) | ⭐ | Print | `42` | O(1) |
| [fib.mi](fib.mi) | ⭐⭐ | Recursion | `55` | O(2^n) |
| [fact.mi](fact.mi) | ⭐⭐ | Recursion | `120` | O(n) |
| [sum.mi](sum.mi) | ⭐⭐ | Loops | `5050` | O(n) |
| [max.mi](max.mi) | ⭐⭐ | Functions | `20, 12` | O(1) |
| [power.mi](power.mi) | ⭐⭐ | Exponentiation | `1024` | O(exp) |
| [gcd.mi](gcd.mi) | ⭐⭐⭐ | Euclidean | `6, 5` | O(log n) |
| [prime.mi](prime.mi) | ⭐⭐⭐ | Optimization | `1, 0, 1` | O(√n) |
| [collatz.mi](collatz.mi) | ⭐⭐⭐ | Sequences | `111` | O(?) |
| [ackermann.mi](ackermann.mi) | ⭐⭐⭐⭐ | Deep recursion | `125` | O(HUGE) |

---

## 🌟 Beginner Examples

<table>
<tr>
<td width="50%">

### hello.mi - Hello World

**Difficulty**: ⭐ Beginner

```miru
print(42);
```

**Output**: `42`

**Learn**: Basic syntax, print()

</td>
<td width="50%">

### max.mi - Maximum Value

**Difficulty**: ⭐⭐ Easy

```miru
func max(a, b) {
    if (a > b) {
        return a;
    }
    return b;
}
print(max(10, 20));
```

**Output**: `20`

**Learn**: Conditionals, functions

</td>
</tr>
</table>

---

## 🔥 Intermediate Examples

<table>
<tr>
<td width="50%">

### fib.mi - Fibonacci

**Difficulty**: ⭐⭐ Easy
**Complexity**: O(2^n)

```miru
func fib(n) {
    if (n <= 1) {
        return n;
    }
    return fib(n - 1) + fib(n - 2);
}
print(fib(10));
```

**Output**: `55`

</td>
<td width="50%">

### fact.mi - Factorial

**Difficulty**: ⭐⭐ Easy
**Complexity**: O(n)

```miru
func factorial(n) {
    if (n <= 1) {
        return 1;
    }
    return n * factorial(n - 1);
}
print(factorial(5));
```

**Output**: `120`

</td>
</tr>
<tr>
<td width="50%">

### sum.mi - Sum to N

**Difficulty**: ⭐⭐ Easy
**Complexity**: O(n)

```miru
func sum_to_n(n) {
    let sum = 0;
    let i = 1;
    while (i <= n) {
        sum = sum + i;
        i = i + 1;
    }
    return sum;
}
print(sum_to_n(100));
```

**Output**: `5050`

</td>
<td width="50%">

### power.mi - Exponentiation

**Difficulty**: ⭐⭐ Easy
**Complexity**: O(exp)

```miru
func power(base, exp) {
    if (exp == 0) {
        return 1;
    }
    return base * power(base, exp - 1);
}
print(power(2, 10));
```

**Output**: `1024`

</td>
</tr>
<tr>
<td width="50%">

### gcd.mi - Greatest Common Divisor

**Difficulty**: ⭐⭐⭐ Medium
**Complexity**: O(log min(a,b))

```miru
func gcd(a, b) {
    while (b != 0) {
        let temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}
print(gcd(48, 18));
```

**Output**: `6`

</td>
<td width="50%">

### prime.mi - Prime Checker

**Difficulty**: ⭐⭐⭐ Medium
**Complexity**: O(√n)

```miru
func is_prime(n) {
    if (n <= 1) return 0;
    if (n == 2) return 1;

    let i = 2;
    while (i * i <= n) {
        if (n % i == 0) return 0;
        i = i + 1;
    }
    return 1;
}
print(is_prime(17));
```

**Output**: `1` (true)

</td>
</tr>
<tr>
<td width="50%">

### collatz.mi - Collatz Conjecture

**Difficulty**: ⭐⭐⭐ Medium
**Complexity**: O(?)

```miru
func collatz_length(n) {
    let count = 0;
    while (n != 1) {
        if (n % 2 == 0) {
            n = n / 2;
        } else {
            n = 3 * n + 1;
        }
        count = count + 1;
    }
    return count;
}
print(collatz_length(27));
```

**Output**: `111`

**Note**: Unsolved problem!

</td>
<td width="50%">

</td>
</tr>
</table>

---

## 🚀 Advanced Examples

<table>
<tr>
<td>

### ackermann.mi - Ackermann Function

**Difficulty**: ⭐⭐⭐⭐ Advanced
**Complexity**: O(HUGE!)

```miru
func ackermann(m, n) {
    if (m == 0) {
        return n + 1;
    }
    if (n == 0) {
        return ackermann(m - 1, 1);
    }
    return ackermann(m - 1, ackermann(m, n - 1));
}
print(ackermann(3, 4));
```

**Output**: `125`

⚠️ **WARNING**: DO NOT try `ackermann(4, 2)` - it will take forever!

**Growth Rate**: Faster than exponential, faster than tetration!

</td>
</tr>
</table>

---

## 🚀 Running Examples

### Quick Run (One Command)

```bash
./miru examples/fib.mi && gcc -o out/fib out/gen.c runtime/print.c && ./out/fib
```

### Step by Step

```bash
# 1. Compile Miru → C
./miru examples/fib.mi

# 2. Compile C → Binary
gcc -o out/fib out/gen.c runtime/print.c

# 3. Run
./out/fib
```

---

## 💡 Learning Path

<div align="center">

| Level | Examples | Skills |
|-------|----------|--------|
| **Beginner** | hello, max | Syntax, conditionals |
| **Easy** | sum, fib, fact, power | Loops, recursion |
| **Medium** | gcd, prime, collatz | Algorithms, optimization |
| **Advanced** | ackermann | Deep recursion, limits |

</div>

---

## 🎯 Challenges

**Beginner**:
- Modify `fib.mi` to use iteration instead
- Add input validation to `factorial`
- Extend `max` to find max of 4 numbers

**Advanced**:
- Implement fast exponentiation in `power.mi` (O(log n))
- Add LCM function to `gcd.mi`
- Create Sieve of Eratosthenes in `prime.mi`

---

<div align="center">

## 📚 More Documentation

<table>
<tr>
<td align="center" width="50%">
<b>Language Reference</b><br/>
Keywords & syntax guide<br/>
<a href="../KEYWORDS.md">KEYWORDS.md</a>
</td>
<td align="center" width="50%">
<b>Main Docs</b><br/>
Quick start & overview<br/>
<a href="../README.md">README.md</a>
</td>
</tr>
</table>

---

**Happy Coding!** 🚀

</div>
