import {
  type App,
  type InjectionKey
} from 'vue'

import { tailwindTheme, theme } from '..'

/**
 * Vue injection keys
 * See also https://vuejs.org/guide/typescript/composition-api.html#typing-provide-inject
 */
export const injectionKeys = {
  tailwindTheme: Symbol('tailwindTheme') as InjectionKey<typeof tailwindTheme>
}

/**
 * Composable to get tailwind utils
 */
export function useTailwind () {
  return {
    theme
  }
}

/**
 * Init the root Vue app to provide resolved Tailwind theme
 * @param app - The root Vue app
 */
export function initTailwind (app: App) {
  app.provide(injectionKeys.tailwindTheme, tailwindTheme)
}
