import React from 'react'
import { EXAMPLES } from '../lib/examples'
import './ExamplesDrawer.css'

interface ExamplesDrawerProps {
  onClose: () => void
  onSelectExample: (code: string) => void
}

export function ExamplesDrawer({ onClose, onSelectExample }: ExamplesDrawerProps) {
  return (
    <>
      <div className="drawer-overlay" onClick={onClose} />
      <div className="drawer">
        <div className="drawer-header">
          <h2>📚 Example Programs</h2>
          <button className="close-button" onClick={onClose}>✕</button>
        </div>
        <div className="drawer-content">
          {EXAMPLES.map((example) => (
            <div key={example.id} className="example-item">
              <div className="example-header">
                <h3>{example.name}</h3>
                <span className="difficulty">{example.difficulty}</span>
              </div>
              <p className="example-description">{example.description}</p>
              <button
                className="load-button"
                onClick={() => {
                  onSelectExample(example.code)
                  onClose()
                }}
              >
                Load Example
              </button>
            </div>
          ))}
        </div>
      </div>
    </>
  )
}
