import path from 'path'
import { defineConfig, loadEnv } from 'vite'
import checker from 'vite-plugin-checker'
import vue from '@vitejs/plugin-vue'
import autoprefixer from 'autoprefixer'
import tailwind from 'tailwindcss'
import resolveTwConfig from 'tailwindcss/resolveConfig'

import tailwindConfig from './tailwind.config'

const resolvedTwConfig = resolveTwConfig(tailwindConfig)

// Project root path
const rootPath = path.resolve(__dirname, '.')
// Resolve from root path
const resolve = (...args: string[]) => {
  return path.join(rootPath, ...args)
}

export default defineConfig(({ mode }) => {
  const viteEnvVars = loadEnv(mode, process.cwd(), 'VITE_')

  return {
    resolve: {
      alias: [
        {
          find: '~',
          replacement: resolve('.')
        },
        {
          find: '@',
          replacement: resolve('src')
        }
      ]
    },
    define: {
      __TAILWIND_THEME__: JSON.stringify(resolvedTwConfig.theme)
    },
    base: viteEnvVars.VITE_BASE_URL || '/',
    plugins: [
      checker({
        typescript: {
          tsconfigPath: './tsconfig.app.json'
        },
        vueTsc: true
      }),
      vue()
    ],
    css: {
      postcss: {
        plugins: [
          autoprefixer,
          tailwind(tailwindConfig)
        ]
      }
    }
  }
})
