import { createApp } from 'vue'
import { library as fontAwesomeLibrary } from '@fortawesome/fontawesome-svg-core'
import { FontAwesomeIcon } from '@fortawesome/vue-fontawesome'
import {
  faCircleQuestion as farCircleQuestion
} from '@fortawesome/free-regular-svg-icons'
import {
  faArrowDown as fasArrowDown,
  faArrowRotateLeft as fasArrowRotateLeft,
  faArrowUp as fasArrowUp,
  faCircleQuestion as fasCircleQuestion,
  faDownload as fasDownload,
  faExclamationTriangle as fasExclamationTriangle,
  faPlug as fasPlug,
  faPlugCircleXmark as fasPlugCircleXmark,
  faTimes as fasTimes,
  faTrash as fasTrash
} from '@fortawesome/free-solid-svg-icons'
import {
  faGithub as fabGithub
} from '@fortawesome/free-brands-svg-icons'

// Main style
import './style/main.css'

// App component
import App from './components/App.vue'

// Custom libs
import { initModals } from './lib/modals'
import { initTailwind } from './lib/tailwind'

/* Add icons to the font awesome Library */
fontAwesomeLibrary.add(
  // regular
  farCircleQuestion,
  // solid
  fasArrowDown,
  fasArrowRotateLeft,
  fasArrowUp,
  fasCircleQuestion,
  fasDownload,
  fasExclamationTriangle,
  fasPlug,
  fasPlugCircleXmark,
  fasTimes,
  fasTrash,
  // brands
  fabGithub
)

// Create the Vue app
const vueApp = createApp(App)
// Register global components (types in ./vue-augment.d.ts)
vueApp.component('FaIcon', FontAwesomeIcon)
// Modal management
initModals(vueApp)
// Tailwind
initTailwind(vueApp)

vueApp.mount('#app')
