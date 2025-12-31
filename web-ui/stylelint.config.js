/** @type {import('stylelint').Config} */
export default {
  extends: [
    'stylelint-config-standard',
    'stylelint-config-html',
    'stylelint-config-recommended-vue',
    '@stylistic/stylelint-config',
    'stylelint-config-tailwindcss'
  ],
  ignoreFiles: [
    'dist/**'
  ]
}
