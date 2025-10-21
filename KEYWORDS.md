<div align="center">

# 📖 MiruScript Language Reference

### Complete Keyword & Syntax Guide

_All language constructs, operators, and grammar rules_

<p align="center">
  <img src="https://img.shields.io/badge/Keywords-12-blue?style=for-the-badge" alt="Keywords"/>
  <img src="https://img.shields.io/badge/Operators-14-green?style=for-the-badge" alt="Operators"/>
  <img src="https://img.shields.io/badge/Precedence-9_Levels-orange?style=for-the-badge" alt="Precedence"/>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/Control_Flow-3_constructs-purple?style=flat-square" alt="Control Flow"/>
  <img src="https://img.shields.io/badge/Data_Types-Integer_MVP-red?style=flat-square" alt="Types"/>
  <img src="https://img.shields.io/badge/Built--ins-print()-yellow?style=flat-square" alt="Built-ins"/>
</p>

<p align="center" style="font-weight: bold;">
  <a href="#-keywords">Keywords</a> •
  <a href="#️-operators">Operators</a> •
  <a href="#-variables">Variables</a> •
  <a href="#-functions">Functions</a> •
  <a href="#-control-flow">Control Flow</a>
</p>

</div>

---

## 🔤 Keywords

Miru has **12 reserved keywords**:

<table>
<tr>
<td width="50%">

### Control Flow

| Keyword | Description        |
| ------- | ------------------ |
| `if`    | Conditional branch |
| `else`  | Alternative branch |
| `while` | Loop               |
| `for`   | ⚠️ Not implemented |

</td>
<td width="50%">

### Functions

| Keyword  | Description          |
| -------- | -------------------- |
| `func`   | Function declaration |
| `return` | Return value         |

</td>
</tr>
<tr>
<td width="50%">

### Variables

| Keyword | Description      |
| ------- | ---------------- |
| `let`   | Mutable variable |
| `const` | Constant         |

</td>
<td width="50%">

### Literals & Built-ins

| Keyword | Description     |
| ------- | --------------- |
| `true`  | Boolean (int 1) |
| `false` | Boolean (int 0) |
| `null`  | Null (int 0)    |
| `print` | Built-in output |

</td>
</tr>
</table>

---

## ⚙️ Operators

<table>
<tr>
<td width="50%">

### Arithmetic

| Op  | Name           | Example        |
| --- | -------------- | -------------- |
| `+` | Addition       | `5 + 3` → `8`  |
| `-` | Subtraction    | `5 - 3` → `2`  |
| `*` | Multiplication | `5 * 3` → `15` |
| `/` | Division       | `10 / 2` → `5` |
| `%` | Modulo         | `10 % 3` → `1` |

</td>
<td width="50%">

### Comparison

| Op   | Name          | Example         |
| ---- | ------------- | --------------- |
| `==` | Equal         | `5 == 5` → true |
| `!=` | Not equal     | `5 != 3` → true |
| `<`  | Less than     | `3 < 5` → true  |
| `<=` | Less/equal    | `5 <= 5` → true |
| `>`  | Greater       | `5 > 3` → true  |
| `>=` | Greater/equal | `5 >= 5` → true |

</td>
</tr>
<tr>
<td width="50%">

### Logical

| Op   | Name | Description |
| ---- | ---- | ----------- | --- | ----------------- |
| `&&` | AND  | Both true   |
| `    |      | `           | OR  | At least one true |
| `!`  | NOT  | Negation    |

</td>
<td width="50%">

### Assignment

| Op  | Name   | Example  |
| --- | ------ | -------- |
| `=` | Assign | `x = 10` |

_Note: `+=`, `-=` not yet implemented_

</td>
</tr>
</table>

### Precedence (High → Low)

| Level | Operators         | Assoc | Example       |
| ----- | ----------------- | ----- | ------------- | ---- | --- | --- | --- |
| 1     | `()` literals     | -     | `42`, `x`     |
| 2     | `func()`          | Left  | `add(1, 2)`   |
| 3     | `!` `-`           | Right | `!true`, `-5` |
| 4     | `*` `/` `%`       | Left  | `3 * 4`       |
| 5     | `+` `-`           | Left  | `1 + 2`       |
| 6     | `<` `<=` `>` `>=` | Left  | `x > 0`       |
| 7     | `==` `!=`         | Left  | `x == 5`      |
| 8     | `&&`              | Left  | `a && b`      |
| 9     | `                 |       | `             | Left | `a  |     | b`  |

---

## 📦 Variables

<table>
<tr>
<td width="50%">

### Mutable (`let`)

```miru
let x = 10;
let counter = 0;
let sum = a + b;
```

Use when value changes

</td>
<td width="50%">

### Constant (`const`)

```miru
const MAX = 100;
const PI = 3;
const LIMIT = n * 2;
```

Use when value is fixed

</td>
</tr>
</table>

### Type System

⚠️ **MVP**: Only integers fully supported

| Type        | Status     | Example        | Notes           |
| ----------- | ---------- | -------------- | --------------- |
| **Integer** | ✅ Full    | `42`, `-17`    | All values      |
| **Float**   | 🔜 Planned | `3.14`         | Tokenizes only  |
| **String**  | 🔜 Planned | `"hello"`      | Tokenizes only  |
| **Boolean** | ⚠️ Partial | `true`/`false` | As integers 1/0 |

---

## 🔧 Functions

### Declaration

```miru
func function_name(param1, param2) {
    // body
    return value;
}
```

<table>
<tr>
<td width="50%">

**Simple**

```miru
func add(a, b) {
    return a + b;
}
print(add(5, 3));
```

</td>
<td width="50%">

**Recursive**

```miru
func factorial(n) {
    if (n <= 1) return 1;
    return n * factorial(n - 1);
}
print(factorial(5));
```

</td>
</tr>
</table>

---

## 🔀 Control Flow

<table>
<tr>
<td width="50%">

### If-Else

```miru
if (x > 0) {
    print(1);
} else {
    print(0);
}
```

</td>
<td width="50%">

### While Loop

```miru
let n = 5;
while (n > 0) {
    print(n);
    n = n - 1;
}
```

</td>
</tr>
</table>

---

## 🖨️ Built-in: print()

Output with automatic newline

```miru
print(42);           // 42
print(1 + 2);        // 3
print(fib(10));      // 55
```

---

## 💬 Comments

```miru
// Single-line comment
let x = 10;  // Inline comment
```

---

## 📋 Grammar (BNF)

```
program     → statement* EOF
statement   → varDecl | funcDecl | ifStmt | whileStmt | returnStmt | exprStmt

varDecl     → ("let" | "const") IDENTIFIER "=" expression ";"
funcDecl    → "func" IDENTIFIER "(" parameters? ")" "{" statement* "}"
ifStmt      → "if" "(" expression ")" "{" statement* "}" ("else" "{" statement* "}")?
whileStmt   → "while" "(" expression ")" "{" statement* "}"
returnStmt  → "return" expression? ";"
exprStmt    → expression ";"

expression  → assignment
assignment  → IDENTIFIER "=" assignment | logical_or
logical_or  → logical_and ("||" logical_and)*
logical_and → equality ("&&" equality)*
equality    → comparison (("==" | "!=") comparison)*
comparison  → term (("<" | "<=" | ">" | ">=") term)*
term        → factor (("+" | "-") factor)*
factor      → unary (("*" | "/" | "%") unary)*
unary       → ("!" | "-") unary | call
call        → primary ("(" arguments? ")")*
primary     → NUMBER | STRING | "true" | "false" | "null" | IDENTIFIER | "(" expression ")"
```

---

## 📚 Examples

<table>
<tr>
<td width="50%">

**Variables**

```miru
let x = 10;
let y = 20;
print(x + y);  // 30
```

</td>
<td width="50%">

**Functions**

```miru
func max(a, b) {
    if (a > b) return a;
    return b;
}
print(max(10, 20));
```

</td>
</tr>
</table>

See **[examples/Examples.md](examples/Examples.md)** for 10 complete programs!

---

## ⚠️ Limitations

| Feature           | Status       | Workaround         |
| ----------------- | ------------ | ------------------ |
| Integer-only      | MVP          | Use integers       |
| No arrays         | ❌           | Multiple variables |
| No strings        | ⚠️ Tokenizes | Not available      |
| No for-loops      | ❌           | Use `while`        |
| No break/continue | ❌           | Use conditionals   |

---

## 🎯 Quick Reference

| Construct | Syntax                             |
| --------- | ---------------------------------- |
| Variable  | `let x = 10;`                      |
| Constant  | `const PI = 3;`                    |
| Function  | `func add(a, b) { return a + b; }` |
| If-Else   | `if (x > 0) { ... } else { ... }`  |
| While     | `while (n > 0) { ... }`            |
| Return    | `return 42;`                       |
| Print     | `print(value);`                    |
| Comment   | `// comment`                       |

---

<div align="center">

## 📚 More Documentation

<table>
<tr>
<td align="center" width="50%">
<b>Examples</b><br/>
10 complete programs<br/>
<a href="examples/Examples.md">Examples.md</a>
</td>
<td align="center" width="50%">
<b>Main Docs</b><br/>
Quick start & overview<br/>
<a href="README.md">README.md</a>
</td>
</tr>
</table>

---

**Happy Coding!** 🚀

[← Back to README](README.md)

</div>
