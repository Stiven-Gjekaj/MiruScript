import React, { useEffect, useRef } from 'react'
import * as monaco from 'monaco-editor'
import './MonacoEditor.css'

interface MonacoEditorProps {
  code: string
  onChange: (code: string) => void
  markers?: Array<{
    startLineNumber: number
    startColumn: number
    endLineNumber: number
    endColumn: number
    message: string
    severity: 'error' | 'warning' | 'info'
  }>
}

// Register Miru language
function registerMiruLanguage() {
  monaco.languages.register({ id: 'miru' })

  monaco.languages.setMonarchTokensProvider('miru', {
    keywords: [
      'func', 'if', 'else', 'while', 'for', 'return', 'let', 'const',
      'true', 'false', 'null', 'break', 'continue',
    ],
    builtins: ['print'],
    operators: [
      '=', '+', '-', '*', '/', '%', '==', '!=', '<', '>', '<=', '>=',
      '&&', '||', '!', '(', ')', '{', '}', '[', ']', ',', ';', '.', ':',
    ],
    tokenizer: {
      root: [
        // Comments
        [/\/\/.*$/, 'comment'],
        [/\/\*/, 'comment', '@comment'],

        // Strings
        [/"(?:\\.|[^"\\])*"/, 'string'],
        [/'(?:\\.|[^'\\])*'/, 'string'],

        // Numbers
        [/\b\d+\b/, 'number'],
        [/\b\d+\.\d+\b/, 'number'],

        // Built-in functions
        [/\b(print)\b/, 'builtin'],

        // Keywords
        [
          /\b(func|if|else|while|for|return|let|const|true|false|null|break|continue)\b/,
          'keyword',
        ],

        // Identifiers
        [/[a-zA-Z_][a-zA-Z0-9_]*/, 'identifier'],

        // Whitespace
        [/\s+/, 'white'],

        // Operators
        [/[+\-*/%=!<>&|]/, 'operator'],
        [/[{}()\[\],;:.]/, 'delimiter'],
      ],
      comment: [
        [/[^*/]+/, 'comment'],
        [/\*\//, 'comment', '@pop'],
        [/[*/]/, 'comment'],
      ],
    },
  })

  // Define theme colors
  monaco.editor.defineTheme('miru-light', {
    base: 'vs',
    inherit: true,
    rules: [
      { token: 'keyword', foreground: 'd946ef', fontStyle: 'bold' },
      { token: 'builtin', foreground: '0066cc', fontStyle: 'bold' },
      { token: 'string', foreground: '22863a' },
      { token: 'comment', foreground: '6a737d', fontStyle: 'italic' },
      { token: 'number', foreground: '005cc5' },
      { token: 'operator', foreground: '24292e' },
      { token: 'delimiter', foreground: '24292e' },
    ],
    colors: {
      'editor.background': '#ffffff',
      'editor.foreground': '#1a1a1a',
      'editor.lineNumbersBackground': '#f5f5f5',
      'editor.lineNumbersForeground': '#999999',
      'editorCursor.foreground': '#0066cc',
      'editor.selectionBackground': '#b3d9ff',
      'editorError.squiggles': '#ff0000',
      'editorWarning.squiggles': '#ff9800',
    },
  })

  monaco.editor.defineTheme('miru-dark', {
    base: 'vs-dark',
    inherit: true,
    rules: [
      { token: 'keyword', foreground: 'd946ef', fontStyle: 'bold' },
      { token: 'builtin', foreground: '4a9eff', fontStyle: 'bold' },
      { token: 'string', foreground: '85e89d' },
      { token: 'comment', foreground: '6a737d', fontStyle: 'italic' },
      { token: 'number', foreground: '79b8ff' },
      { token: 'operator', foreground: 'e0e0e0' },
      { token: 'delimiter', foreground: 'e0e0e0' },
    ],
    colors: {
      'editor.background': '#1e1e1e',
      'editor.foreground': '#e0e0e0',
      'editor.lineNumbersBackground': '#2d2d2d',
      'editor.lineNumbersForeground': '#666666',
      'editorCursor.foreground': '#4a9eff',
      'editor.selectionBackground': '#264f78',
      'editorError.squiggles': '#f44336',
      'editorWarning.squiggles': '#ff9800',
    },
  })
}

export function MonacoEditorComponent({
  code,
  onChange,
  markers = [],
}: MonacoEditorProps) {
  const editorRef = useRef<monaco.editor.IStandaloneCodeEditor | null>(null)
  const containerRef = useRef<HTMLDivElement>(null)
  const registeredRef = useRef(false)

  useEffect(() => {
    if (!registeredRef.current) {
      registerMiruLanguage()
      registeredRef.current = true
    }
  }, [])

  useEffect(() => {
    if (!containerRef.current) return

    const isDark = document.documentElement.classList.contains('dark')
    const theme = isDark ? 'miru-dark' : 'miru-light'

    const editor = monaco.editor.create(containerRef.current, {
      value: code,
      language: 'miru',
      theme,
      automaticLayout: true,
      minimap: { enabled: false },
      scrollBeyondLastLine: false,
      wordWrap: 'on',
      wrappingIndent: 'indent',
      formatOnPaste: false,
      fontFamily: "'Monaco', 'Menlo', 'Ubuntu Mono', 'Consolas', monospace",
      fontSize: 14,
      lineHeight: 1.6,
      tabSize: 2,
      insertSpaces: true,
      renderWhitespace: 'none',
      autoClosingBrackets: 'always',
      autoClosingQuotes: 'always',
      autoIndent: 'keep',
    })

    editorRef.current = editor

    const handleChange = () => {
      const value = editor.getValue()
      onChange(value)
    }

    const listener = editor.onDidChangeModelContent(handleChange)

    // Handle keyboard shortcuts
    editor.addCommand(monaco.KeyMod.CtrlCmd | monaco.KeyCode.Enter, () => {
      window.dispatchEvent(new CustomEvent('run-code'))
    })

    return () => {
      listener.dispose()
      editor.dispose()
    }
  }, [])

  // Update markers (errors/warnings)
  useEffect(() => {
    if (!editorRef.current) return

    const model = editorRef.current.getModel()
    if (!model) return

    const formattedMarkers = markers.map(m => ({
      severity: m.severity === 'error' ? monaco.MarkerSeverity.Error :
                m.severity === 'warning' ? monaco.MarkerSeverity.Warning :
                monaco.MarkerSeverity.Info,
      startLineNumber: m.startLineNumber,
      startColumn: m.startColumn,
      endLineNumber: m.endLineNumber,
      endColumn: m.endColumn,
      message: m.message,
    }))

    monaco.editor.setModelMarkers(model, 'miru', formattedMarkers)
  }, [markers])

  // Handle theme changes
  useEffect(() => {
    const handleThemeChange = () => {
      if (!editorRef.current) return
      const isDark = document.documentElement.classList.contains('dark')
      const theme = isDark ? 'miru-dark' : 'miru-light'
      monaco.editor.setTheme(theme)
    }

    window.addEventListener('themechange', handleThemeChange)
    return () => window.removeEventListener('themechange', handleThemeChange)
  }, [])

  return <div ref={containerRef} className="monaco-editor-container" />
}
