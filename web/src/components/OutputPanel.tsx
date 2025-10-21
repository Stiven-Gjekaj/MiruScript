import React, { useEffect, useRef } from 'react'
import './OutputPanel.css'

interface OutputPanelProps {
  output: string
  error?: string
  isRunning: boolean
}

export function OutputPanel({ output, error, isRunning }: OutputPanelProps) {
  const containerRef = useRef<HTMLDivElement>(null)

  // Auto-scroll to bottom when output changes
  useEffect(() => {
    if (containerRef.current) {
      containerRef.current.scrollTop = containerRef.current.scrollHeight
    }
  }, [output])

  return (
    <div className="panel output-panel">
      <div className="panel-header">
        💻 Output
        {isRunning && <span className="spinner"></span>}
      </div>
      <div className="panel-content" ref={containerRef}>
        {error && (
          <div className="output-error">
            <strong>Execution Error:</strong>
            <pre>{error}</pre>
          </div>
        )}
        {!error && output && (
          <div className="output-text">
            <pre>{output}</pre>
          </div>
        )}
        {!error && !output && !isRunning && (
          <div className="output-placeholder">
            👈 Write code and click "Run" to see output
          </div>
        )}
        {isRunning && (
          <div className="output-placeholder">
            ⏳ Running...
          </div>
        )}
      </div>
    </div>
  )
}
