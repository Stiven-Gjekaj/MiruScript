import { useState, useCallback } from 'react'

interface UseCompilerReturn {
  compile: (code: string) => Promise<string | null>
  isCompiling: boolean
  error: string | null
}

let wasmModule: any = null
let isInitializing = false
let initPromise: Promise<any> | null = null

/**
 * Initialize the WASM compiler module
 */
async function initializeWasm(): Promise<any> {
  if (wasmModule) {
    return wasmModule
  }

  if (initPromise) {
    return initPromise
  }

  if (isInitializing) {
    // Wait for initialization to complete
    while (isInitializing) {
      await new Promise(resolve => setTimeout(resolve, 100))
    }
    return wasmModule
  }

  isInitializing = true

  initPromise = (async () => {
    try {
      // This imports the Emscripten-generated module
      const response = await fetch('/compiler.js')
      const jsCode = await response.text()

      // Create a scope for the module
      const moduleExports = {}
      const moduleFunc = new Function('Module', jsCode)

      // Initialize Module
      let Module = {
        onRuntimeInitialized: () => {
          wasmModule = {
            compile: Module.cwrap('miru_compile', 'string', ['string']),
            freeResult: Module.cwrap('miru_free_result', null, ['string']),
            version: Module.cwrap('miru_version', 'string', []),
          }
        },
      }

      moduleFunc(Module)

      // Wait for runtime initialization
      while (!wasmModule) {
        await new Promise(resolve => setTimeout(resolve, 50))
      }

      return wasmModule
    } catch (error) {
      isInitializing = false
      initPromise = null
      throw error
    } finally {
      isInitializing = false
    }
  })()

  return initPromise
}

/**
 * Hook for compiling Miru source code to C
 */
export function useCompiler(): UseCompilerReturn {
  const [isCompiling, setIsCompiling] = useState(false)
  const [error, setError] = useState<string | null>(null)

  const compile = useCallback(async (code: string): Promise<string | null> => {
    setIsCompiling(true)
    setError(null)

    try {
      // Initialize WASM if not already done
      if (!wasmModule) {
        await initializeWasm()
      }

      if (!wasmModule || !wasmModule.compile) {
        throw new Error('Failed to initialize Miru compiler. Please reload the page.')
      }

      // Call the WASM compiler
      const result = wasmModule.compile(code)

      if (!result) {
        throw new Error('Compilation returned no result')
      }

      // Check if result is an error message
      if (result.includes('Error:')) {
        setError(result)
        return null
      }

      return result
    } catch (err) {
      const message = err instanceof Error ? err.message : String(err)
      setError(message)
      return null
    } finally {
      setIsCompiling(false)
    }
  }, [])

  return { compile, isCompiling, error }
}
