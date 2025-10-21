import { useState, useCallback } from 'react'

interface UseWasmExecutorReturn {
  execute: (cCode: string) => Promise<string>
  error: string | null
  isRunning?: boolean
}

/**
 * Execute C code by:
 * 1. Compiling C to WASM using Emscripten (via backend or service worker)
 * 2. Running the WASM module
 * 3. Capturing print output
 */
export function useWasmExecutor(): UseWasmExecutorReturn {
  const [error, setError] = useState<string | null>(null)

  const execute = useCallback(async (cCode: string): Promise<string> => {
    setError(null)

    try {
      const output = await compileAndRunC(cCode)
      return output
    } catch (err) {
      const message = err instanceof Error ? err.message : String(err)
      setError(message)
      return ''
    }
  }, [])

  return { execute, error }
}

/**
 * Compile C code to WASM and execute it
 *
 * For MVP, uses a simulated execution that parses the generated C code.
 * In production, this would:
 * 1. Send C code to a backend service running Emscripten
 * 2. Backend compiles C to WASM binary
 * 3. Returns WASM binary + memory limits
 * 4. Browser executes using Wasmer.js or Wasm runtime
 *
 * Alternative approach:
 * - Use Emscripten in a Service Worker for client-side compilation
 * - Cache compiled modules for faster re-runs
 */
async function compileAndRunC(cCode: string): Promise<string> {
  return new Promise((resolve, reject) => {
    try {
      // MVP: Simulate execution by parsing the generated C code
      // Extract all printf calls and their arguments
      const outputs: string[] = []

      // Pattern 1: printf("%d\n", value);
      const intPrintMatches = cCode.matchAll(/printf\("%d\\n",\s*([^)]+)\)/g)
      for (const match of intPrintMatches) {
        const expr = match[1].trim()
        // Try to evaluate simple expressions
        const result = evaluateSimpleExpression(expr, cCode)
        if (result !== null) {
          outputs.push(String(result))
        }
      }

      // Pattern 2: printf("%f\n", value);
      const floatPrintMatches = cCode.matchAll(/printf\("%f\\n",\s*([^)]+)\)/g)
      for (const match of floatPrintMatches) {
        const expr = match[1].trim()
        const result = evaluateSimpleExpression(expr, cCode)
        if (result !== null) {
          outputs.push(String(result))
        }
      }

      // Pattern 3: printf("%s\n", value);
      const stringPrintMatches = cCode.matchAll(/printf\("%s\\n",\s*([^)]+)\)/g)
      for (const match of stringPrintMatches) {
        const expr = match[1].trim()
        if (expr.startsWith('"')) {
          outputs.push(expr.slice(1, -1))
        }
      }

      if (outputs.length === 0) {
        // Check if there's a main function at all
        if (cCode.includes('int main')) {
          resolve('(Program executed but produced no output)')
        } else {
          resolve('(No main function generated)')
        }
      } else {
        resolve(outputs.join('\n'))
      }
    } catch (err) {
      reject(err)
    }
  })
}

/**
 * Simple expression evaluator for MVP
 * Handles constants and basic arithmetic
 */
function evaluateSimpleExpression(expr: string, cCode: string): number | null {
  expr = expr.trim()

  // If it's a number literal
  if (/^\d+$/.test(expr)) {
    return parseInt(expr, 10)
  }

  // If it's a floating point literal
  if (/^\d+\.\d+$/.test(expr)) {
    return parseFloat(expr)
  }

  // If it's a negated number
  if (/^-\d+$/.test(expr)) {
    return parseInt(expr, 10)
  }

  // If it's a simple binary operation like "2 + 3"
  const binaryMatch = expr.match(/^(\d+)\s*([+\-*/%])\s*(\d+)$/)
  if (binaryMatch) {
    const left = parseInt(binaryMatch[1], 10)
    const op = binaryMatch[2]
    const right = parseInt(binaryMatch[3], 10)

    switch (op) {
      case '+':
        return left + right
      case '-':
        return left - right
      case '*':
        return left * right
      case '/':
        return Math.floor(left / right)
      case '%':
        return left % right
    }
  }

  // For variables, try to find their value in the code
  // Look for simple assignments like "int x = 42;"
  const varMatch = cCode.match(new RegExp(`int\\s+${expr}\\s*=\\s*([\\d+\\-*/% ]+);`))
  if (varMatch) {
    return evaluateSimpleExpression(varMatch[1], cCode)
  }

  return null
}
