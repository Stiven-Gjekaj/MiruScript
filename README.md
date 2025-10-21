<div align="center">

<img src="Miru.png" alt="Miru Logo" width="300"/>

### A Compact, Readable C-Based Programming Language

_Powered by a minimal toolchain: Lexer → Parser → AST → CodeGen → C_

<p align="center">
  <img src="https://img.shields.io/badge/C-11-00599C?style=for-the-badge&logo=c&logoColor=white" alt="C11"/>
  <img src="https://img.shields.io/badge/GCC-Compiler-A42E2B?style=for-the-badge&logo=gnu&logoColor=white" alt="GCC"/>
  <img src="https://img.shields.io/badge/Make-Build-427819?style=for-the-badge" alt="Make"/>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/Status-Alpha-success?style=flat-square" alt="Status"/>
  <img src="https://img.shields.io/badge/Version-0.1.0-blue?style=flat-square" alt="Version"/>
  <img src="https://img.shields.io/badge/License-MIT-green?style=flat-square" alt="License"/>
</p>

<p align="center" style="font-weight: bold;">
  <a href="https://miruscript.vercel.app">🌐 Try Online Demo</a> •
  <a href="#-quick-start">Quick Start</a> •
  <a href="#-features">Features</a> •
  <a href="#-examples">Examples</a> •
  <a href="#-documentation">Documentation</a>
</p>

</div>

---

## 📖 Overview

**Miru** is a minimalist programming language that compiles to C. Features clean syntax, recursive functions, and a complete compiler pipeline built from scratch in C11. Write functions, loops, conditionals, and recursive algorithms in a clean, C-like syntax that transpiles to readable C code.

---

## ✨ Features

<table>
<tr>
<td width="50%">

### 🔧 Compiler Pipeline

- ✅ Lexer (50 token types)
- ✅ Parser (recursive-descent)
- ✅ AST (15+ node types)
- ✅ CodeGen (emits C code)
- ✅ Runtime library

</td>
<td width="50%">

### 📝 Language

- ✅ Variables & functions
- ✅ Control flow (if/else, while)
- ✅ Operators (arithmetic, logical)
- ✅ Recursion
- ✅ Built-in print()

</td>
</tr>
</table>

---

## 🚀 Quick Start

**See [QuickStart.md](QuickStart.md) for a complete beginner-friendly guide with examples and troubleshooting!**

---

## 🌐 Online IDE

**Try MiruScript in your browser without installation!**

<table>
<tr>
<td align="center" width="50%">

### 💻 Browser-Based IDE

Write and run Miru programs directly in your browser with a professional VS Code-like editor, real-time compilation, and live execution.

**✨ MVP Features:**
- 📝 VS Code-like editor (Monaco) with Miru syntax highlighting
- ⚡ Instant compilation to C
- 🔧 Live generated C code viewer with syntax highlighting
- 🎯 Error highlighting with line/column markers
- 📚 10 example programs (one-click load)
- 🔗 Share code via compressed URL
- 🌙 Dark/Light theme toggle
- ⌨️ Keyboard shortcuts (Ctrl+Enter to run)
- 📱 Responsive design (desktop/tablet/mobile)
- 💾 Auto-save to localStorage

</td>
<td align="center" width="50%">

### 🚀 Quick Access

<a href="https://miruscript.vercel.app"><img src="https://img.shields.io/badge/Launch_IDE-Ready-success?style=for-the-badge" alt="Launch IDE"/></a>

No installation needed. Just start coding!

**Powered by:**
- WebAssembly (WASM)
- React + Vite
- Emscripten compiler

</td>
</tr>
</table>

---

## 📝 Examples

**See [examples/Examples.md](examples/Examples.md) for detailed explanations, complexity analysis, and learning path!**

---

## 📊 Project Statistics

<table>
<tr>
<td align="center" width="25%">
<img src="https://img.shields.io/badge/Lines_of_Code-2,871-blue?style=for-the-badge" alt="LOC"/><br/>
<b>Total C Code</b><br/>
2,288 src + 583 other
</td>
<td align="center" width="25%">
<img src="https://img.shields.io/badge/Files-32-green?style=for-the-badge" alt="Files"/><br/>
<b>Project Files</b><br/>
Source, headers, examples, docs
</td>
<td align="center" width="25%">
<img src="https://img.shields.io/badge/Examples-10-orange?style=for-the-badge" alt="Examples"/><br/>
<b>Example Programs</b><br/>
116 lines of Miru code
</td>
</tr>
</table>

### Component Breakdown

| Component    | Files  | Lines     | Description                            |
| ------------ | ------ | --------- | -------------------------------------- |
| **Lexer**    | 2      | 364       | Tokenization, 50 token types           |
| **Parser**   | 2      | 971       | Recursive-descent, 9 precedence levels |
| **AST**      | 2      | 420       | 15+ node types, printer                |
| **CodeGen**  | 2      | 477       | C code emission, type dispatch         |
| **Runtime**  | 2      | 56        | Print functions for all types          |
| **Main**     | 1      | 50        | Compiler entry point                   |
| **Tests**    | 3      | ~200      | Unit tests, runner scripts             |
| **Examples** | 10     | 116       | Complete Miru programs                 |
| **Total**    | **32** | **2,871** | Production-ready compiler              |

---

## 📚 Documentation

<table>
<tr>
<td align="center" width="25%">
<h3>🚀 Quick Start</h3>
<p>Get up and running<br/>in 5 minutes</p>
<a href="./QuickStart.md"><b>→ QuickStart.md</b></a>
</td>
<td align="center" width="25%">
<h3>📖 Language Reference</h3>
<p>Keywords, operators,<br/>syntax, grammar</p>
<a href="./KEYWORDS.md"><b>→ KEYWORDS.md</b></a>
</td>
<td align="center" width="25%">
<h3>📝 Examples</h3>
<p>10 complete programs<br/>with explanations</p>
<a href="./examples/Examples.md"><b>→ Examples.md</b></a>
</td>
<td align="center" width="25%">
<h3>🔧 Technical Docs</h3>
<p>IDE implementation &<br/>CI/CD setup</p>
<a href="./WEB_IMPLEMENTATION_SUMMARY.md"><b>→ Web Docs</b></a>
</td>
</tr>
</table>

---

## 📄 License

MIT License - see [LICENSE](LICENSE) for details.

---

<div align="center">

**Happy Compiling!** 🚀

</div>
