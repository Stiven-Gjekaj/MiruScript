import { useEffect, useCallback } from 'react'
import LZ from 'lz-string'

/**
 * Hook to persist and share state via URL hash
 * Uses LZ-string for compression to keep URLs shorter
 */
export function useUrlState(code: string, onCodeChange: (code: string) => void) {
  // Load code from URL on mount
  useEffect(() => {
    try {
      const hash = window.location.hash.substring(1)
      if (hash && hash.startsWith('code=')) {
        const encoded = hash.substring(5)
        const decoded = LZ.decompressFromEncodedURIComponent(encoded)
        if (decoded) {
          onCodeChange(decoded)
        }
      }
    } catch (error) {
      console.warn('Failed to decode URL code:', error)
    }
  }, [])

  // Get sharable URL
  const getSharableUrl = useCallback(() => {
    try {
      const encoded = LZ.compressToEncodedURIComponent(code)
      const url = `${window.location.origin}${window.location.pathname}#code=${encoded}`
      return url
    } catch (error) {
      console.error('Failed to create sharable URL:', error)
      return null
    }
  }, [code])

  // Copy to clipboard
  const copyToClipboard = useCallback(async () => {
    const url = getSharableUrl()
    if (!url) return false

    try {
      await navigator.clipboard.writeText(url)
      return true
    } catch (error) {
      console.error('Failed to copy to clipboard:', error)
      return false
    }
  }, [getSharableUrl])

  // Share via Web Share API if available
  const share = useCallback(async () => {
    const url = getSharableUrl()
    if (!url) return false

    if (navigator.share) {
      try {
        await navigator.share({
          title: 'MiruScript Program',
          text: 'Check out this MiruScript program!',
          url,
        })
        return true
      } catch (error) {
        if ((error as Error).name !== 'AbortError') {
          console.error('Failed to share:', error)
        }
        return false
      }
    }

    return false
  }, [getSharableUrl])

  return {
    getSharableUrl,
    copyToClipboard,
    share,
    isShareAvailable: typeof navigator !== 'undefined' && !!navigator.share,
  }
}
