import React, { useState, useEffect, useRef } from 'react'
import { Header } from './components/Header'
import { EditorPanel } from './components/EditorPanel'
import { OutputPanel } from './components/OutputPanel'
import { GeneratedCPanel } from './components/GeneratedCPanel'
import { ExamplesDrawer } from './components/ExamplesDrawer'
import { useCompiler } from './hooks/useCompiler'
import { useWasmExecutor } from './hooks/useWasmExecutor'
import { useLocalStorage } from './hooks/useLocalStorage'
import { useUrlState } from './hooks/useUrlState'
import './App.css'

export function App() {
  const [code, setCode] = useLocalStorage('miruscript-code', 'print(42);')
  const [generatedC, setGeneratedC] = useState('')
  const [output, setOutput] = useState('')
  const [isRunning, setIsRunning] = useState(false)
  const [isDarkMode, setIsDarkMode] = useState(() => {
    if (typeof window === 'undefined') return false
    return window.matchMedia('(prefers-color-scheme: dark)').matches
  })
  const [showExamples, setShowExamples] = useState(false)
  const urlState = useUrlState(code, setCode)
  const shareToastRef = useRef<{ show: (message: string) => void }>()

  const { compile, isCompiling, error: compileError } = useCompiler()
  const { execute, error: executeError } = useWasmExecutor()

  const handleRun = async () => {
    setOutput('')
    setIsRunning(true)

    try {
      // Step 1: Compile Miru to C
      const cCode = await compile(code)
      if (!cCode) {
        setOutput('Error: Compilation failed')
        setIsRunning(false)
        return
      }

      setGeneratedC(cCode)

      // Step 2: Compile C to WASM and execute
      const result = await execute(cCode)
      setOutput(result || '(No output)')
    } catch (err) {
      setOutput(`Error: ${err instanceof Error ? err.message : String(err)}`)
    } finally {
      setIsRunning(false)
    }
  }

  // Handle Ctrl+Enter to run
  useEffect(() => {
    const handleRunCodeEvent = () => {
      handleRun()
    }

    window.addEventListener('run-code', handleRunCodeEvent)
    return () => window.removeEventListener('run-code', handleRunCodeEvent)
  }, [handleRun])

  useEffect(() => {
    document.documentElement.classList.toggle('dark', isDarkMode)
  }, [isDarkMode])

  return (
    <div className={`app ${isDarkMode ? 'dark' : 'light'}`}>
      <Header
        isDarkMode={isDarkMode}
        onToggleTheme={() => setIsDarkMode(!isDarkMode)}
        onRun={handleRun}
        isRunning={isRunning || isCompiling}
        onShowExamples={() => setShowExamples(true)}
        onShare={urlState.share}
        sharableUrl={urlState.getSharableUrl()}
      />

      <main className="main-container">
        <div className="panels">
          <EditorPanel
            code={code}
            onChange={setCode}
            error={compileError}
          />
          <OutputPanel
            output={output}
            error={executeError}
            isRunning={isRunning}
          />
          <GeneratedCPanel
            code={generatedC}
          />
        </div>
      </main>

      {showExamples && (
        <ExamplesDrawer
          onClose={() => setShowExamples(false)}
          onSelectExample={(example) => {
            setCode(example)
            setShowExamples(false)
          }}
        />
      )}
    </div>
  )
}
