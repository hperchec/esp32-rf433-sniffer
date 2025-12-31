import get from 'dlv'

export * from './vue'

/**
 * The resolved Tailwind theme config
 */
export const tailwindTheme = __TAILWIND_THEME__

/**
 * Local implementation of Tailwind plugin API "theme" util function.
 * If defaultValue is not provided and the full key in path is missing, it will throw an error.
 * @param path - The path as string (e.g. "colors.gray.100")
 * @param defaultValue - The default value to return if the full key in path is missing
 */
export function theme (path: string, defaultValue?: any) { // eslint-disable-line @typescript-eslint/no-explicit-any
  const value = get(tailwindTheme, path, defaultValue)
  if (value === undefined && arguments.length === 1) {
    throw new Error(`The key "${path}" is missing. Please provide a default value to prevent this error to be thrown.`)
  } else {
    return value
  }
}
