import defaultTheme from 'tailwindcss/defaultTheme'

import type { Config } from 'tailwindcss'

/**
 * See also https://v3.tailwindcss.com/docs/configuration
 */
export default {
  content: [
    './index.html',
    './src/**/*.{vue,ts,js,jsx}'
  ],
  theme: {
    extend: {
      fontFamily: ({ theme }) => ({
        sans: [ 'Inter', ...defaultTheme.fontFamily.sans ]
      }),
      textColor: ({ theme }) => ({
        muted: theme('colors.gray.400')
      })
    }
  }
} satisfies Config
