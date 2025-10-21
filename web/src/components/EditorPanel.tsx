import React from 'react'
import { MonacoEditorComponent } from './MonacoEditor'
import './EditorPanel.css'

interface EditorPanelProps {
  code: string
  onChange: (code: string) => void
  error?: string
}

export function EditorPanel({ code, onChange, error }: EditorPanelProps) {
  // Parse error to get line number if available
  const errorMarkers = error ? parseError(error) : []

  return (
    <div className="panel editor-panel">
      <div className="panel-header">
        📝 Miru Editor
        {error && <span className="error-indicator">⚠️</span>}
      </div>
      <div className="panel-content editor-content">
        <MonacoEditorComponent
          code={code}
          onChange={onChange}
          markers={errorMarkers}
        />
      </div>
      {error && (
        <div className="panel-error">
          <strong>Compilation Error:</strong>
          <pre>{error}</pre>
        </div>
      )}
    </div>
  )
}

/**
 * Parse error messages to extract line/column information
 * Format: "Error at line X, column Y: message"
 */
function parseError(error: string): Array<{
  startLineNumber: number
  startColumn: number
  endLineNumber: number
  endColumn: number
  message: string
  severity: 'error'
}> {
  const lineMatch = error.match(/line (\d+)/i)
  const colMatch = error.match(/column (\d+)/i)

  if (lineMatch) {
    const line = parseInt(lineMatch[1], 10)
    const col = parseInt(colMatch?.[1] || '1', 10)

    return [{
      startLineNumber: line,
      startColumn: col,
      endLineNumber: line,
      endColumn: Math.max(col + 1, col + 10),
      message: error,
      severity: 'error',
    }]
  }

  return []
}
