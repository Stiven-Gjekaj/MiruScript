import { useState, useEffect, useCallback } from 'react'

/**
 * Hook to persist state to localStorage
 */
export function useLocalStorage<T>(key: string, initialValue: T): [T, (value: T) => void] {
  const [storedValue, setStoredValue] = useState<T>(() => {
    try {
      if (typeof window === 'undefined') {
        return initialValue
      }

      const item = window.localStorage.getItem(key)
      if (item) {
        try {
          return JSON.parse(item) as T
        } catch {
          // If JSON parse fails, return as string
          return item as unknown as T
        }
      }
    } catch (error) {
      console.error(`Error reading localStorage key "${key}":`, error)
    }

    return initialValue
  })

  const setValue = useCallback((value: T) => {
    try {
      setStoredValue(value)
      if (typeof window !== 'undefined') {
        window.localStorage.setItem(key, JSON.stringify(value))
      }
    } catch (error) {
      console.error(`Error setting localStorage key "${key}":`, error)
    }
  }, [key])

  return [storedValue, setValue]
}
