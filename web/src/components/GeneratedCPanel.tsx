import React from 'react'
import './GeneratedCPanel.css'

interface GeneratedCPanelProps {
  code: string
}

export function GeneratedCPanel({ code }: GeneratedCPanelProps) {
  const handleCopy = () => {
    navigator.clipboard.writeText(code)
  }

  return (
    <div className="panel generated-c-panel">
      <div className="panel-header">
        <div className="header-title">⚙️ Generated C Code</div>
        {code && (
          <button
            className="copy-button"
            onClick={handleCopy}
            title="Copy to clipboard"
          >
            📋 Copy
          </button>
        )}
      </div>
      <div className="panel-content">
        {code ? (
          <pre className="generated-c-code">
            <code>{code}</code>
          </pre>
        ) : (
          <div className="placeholder">
            👉 Miru code will be transpiled to C here
          </div>
        )}
      </div>
    </div>
  )
}
