import React, { useState } from 'react'
import './Header.css'

interface HeaderProps {
  isDarkMode: boolean
  onToggleTheme: () => void
  onRun: () => void
  isRunning: boolean
  onShowExamples: () => void
  onShare?: () => Promise<boolean>
  sharableUrl?: string
}

export function Header({
  isDarkMode,
  onToggleTheme,
  onRun,
  isRunning,
  onShowExamples,
  onShare,
  sharableUrl,
}: HeaderProps) {
  const [showShareToast, setShowShareToast] = useState(false)

  const handleShare = async () => {
    if (onShare) {
      const success = await onShare()
      if (success) {
        setShowShareToast(true)
        setTimeout(() => setShowShareToast(false), 2000)
      }
    } else if (sharableUrl) {
      // Fallback: copy to clipboard
      try {
        await navigator.clipboard.writeText(sharableUrl)
        setShowShareToast(true)
        setTimeout(() => setShowShareToast(false), 2000)
      } catch (err) {
        console.error('Failed to copy:', err)
      }
    }
  }
  return (
    <header className="header">
      <div className="header-left">
        <h1 className="logo">🚀 MiruScript</h1>
        <span className="subtitle">Online IDE</span>
      </div>

      <div className="header-center">
        <button
          className="run-button"
          onClick={onRun}
          disabled={isRunning}
          title="Run code (Ctrl+Enter)"
        >
          {isRunning ? (
            <>
              <span className="spinner"></span> Running...
            </>
          ) : (
            <>▶ Run</>
          )}
        </button>
      </div>

      <div className="header-right">
        <button
          className="icon-button"
          onClick={onShowExamples}
          title="Load example programs"
        >
          📚 Examples
        </button>

        {sharableUrl && (
          <button
            className="icon-button"
            onClick={handleShare}
            title="Share your code"
          >
            🔗 Share
          </button>
        )}

        <button
          className="icon-button"
          onClick={onToggleTheme}
          title="Toggle dark/light theme"
        >
          {isDarkMode ? '☀️' : '🌙'}
        </button>

        <a
          href="https://github.com/yourusername/miruscript"
          target="_blank"
          rel="noopener noreferrer"
          className="icon-button"
          title="View on GitHub"
        >
          🐙 GitHub
        </a>
      </div>

      {showShareToast && (
        <div className="share-toast">
          ✓ Copied to clipboard!
        </div>
      )}
    </header>
  )
}
