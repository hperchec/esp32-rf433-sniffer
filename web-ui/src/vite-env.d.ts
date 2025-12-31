/* eslint-disable no-unused-vars */

interface ViteTypeOptions { // eslint-disable-line @typescript-eslint/no-empty-object-type
  // By adding this line, you can make the type of ImportMetaEnv strict
  // to disallow unknown keys.
  // strictImportMetaEnv: unknown
}

interface ImportMetaEnv { // eslint-disable-line @typescript-eslint/no-empty-object-type
  // readonly VITE_APP_TITLE: string
  // more env variables...
}

interface ImportMeta {
  readonly env: ImportMetaEnv
}

/**
 * See vite.config define option
 */

/**
 * The resolved Tailwind theme
 */
declare const __TAILWIND_THEME__: import('tailwindcss/types/config').ThemeConfig
