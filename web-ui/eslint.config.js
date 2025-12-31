import globals from 'globals'
import { defineConfig } from 'eslint/config'
import eslint from '@eslint/js'
import tseslint from 'typescript-eslint'
import stylistic from '@stylistic/eslint-plugin'
import tsdoc from 'eslint-plugin-tsdoc'
import jsdoc from 'eslint-plugin-jsdoc'
import vue from 'eslint-plugin-vue'
import tailwind from 'eslint-plugin-tailwindcss'

/** @type {import('eslint').Linter.Config[]} */
export default [
  {
    /**
     * If only `ignores` is set on flat config object,
     * it will be applied as global.
     * See: https://eslint.org/docs/latest/use/configure/ignore#ignoring-files
     */
    ignores: [
      '**/dist'
    ]
  },
  ...defineConfig({
    extends: [
      eslint.configs.recommended,
      ...tseslint.configs.recommended,
      stylistic.configs['recommended'],
      vue.configs['flat/essential'],
      vue.configs['flat/recommended'],
      vue.configs['flat/strongly-recommended'],
      tailwind.configs['flat/recommended']
    ],
    files: [
      '**/*.{js,ts,vue}'
    ],
    languageOptions: {
      ecmaVersion: 'latest',
      sourceType: 'module',
      parserOptions: {
        parser: tseslint.parser,
        projectService: true,
        extraFileExtensions: [
          '.vue'
        ]
      }
    },
    rules: {
      // Natively handled by typescript: see https://eslint.org/docs/latest/rules/no-redeclare#handled_by_typescript
      'no-redeclare': 'off',
      'no-unused-vars': [ 'error', {
        args: 'none'
      } ],
      // @typescript-eslint
      '@typescript-eslint/no-unused-vars': 'off',
      // @stylistic/eslint-plugin
      '@stylistic/array-bracket-spacing': [ 'error', 'always' ],
      '@stylistic/brace-style': [ 'error', '1tbs' ],
      '@stylistic/comma-dangle': [ 'error', 'never' ],
      '@stylistic/space-before-function-paren': [ 'error', 'always' ],
      // eslint-plugin-vue
      'vue/multi-word-component-names': 'off',
      // eslint-plugin-tailwindcss
      'tailwindcss/no-custom-classname': [ 'error', {
        whitelist: [
          // ...
        ]
      } ]
    }
  }),
  ...defineConfig({
    plugins: {
      tsdoc
    },
    files: [
      '**/*.ts',
      'src/**/*.vue'
    ],
    rules: {
      // tsdoc
      'tsdoc/syntax': 'warn'
    }
  }),
  ...defineConfig({
    plugins: {
      jsdoc
    },
    extends: [
      jsdoc.configs['flat/recommended'],
      jsdoc.configs['flat/stylistic-typescript']
    ],
    files: [
      '**/*.js'
    ],
    rules: {
      // jsdoc
      'jsdoc/require-jsdoc': [ 'warn', {
        publicOnly: true
      } ],
      'jsdoc/check-tag-names': [ 'warn', {
        definedTags: []
      } ],
      'jsdoc/require-hyphen-before-param-description': [ 'warn', 'always', {
        tags: {
          param: 'always',
          returns: 'never'
        }
      } ]
    }
  }),
  {
    files: [
      'src/**/*'
    ],
    languageOptions: {
      globals: {
        ...globals.browser,
        __TAILWIND_THEME__: 'readonly'
      }
    }
  }
]
