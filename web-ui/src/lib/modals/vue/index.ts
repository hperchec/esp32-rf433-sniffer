import {
  inject,
  ref,
  type App,
  type Component,
  type InjectionKey,
  type Ref
} from 'vue'

import { type DisplayedModal } from '..'

export { BaseModal as BaseModalComponent } from './components'

/**
 * Vue injection keys
 * See also https://vuejs.org/guide/typescript/composition-api.html#typing-provide-inject
 */
export const injectionKeys = {
  modals: Symbol('modals') as InjectionKey<Ref<DisplayedModal[]>>
}

/**
 * Composable to manage modals
 */
export function useModals () {
  const modals = inject(injectionKeys.modals) as Ref<DisplayedModal[]>

  /**
   * Open a modal
   * @param name - The modal unique name
   * @param component - The modal component
   * @param props - The props (or attributes) to pass to the modal component instance
   * @param openerElement - The "opener" HTML element. For example, it can be the element to which
   * the user has clicked to open the modal
   */
  const open = (
    name: DisplayedModal['name'],
    component: Component,
    props: DisplayedModal['props'] = {},
    openerElement?: HTMLElement
  ) => {
    // Prevent opening modal with non unique name
    if (modals.value.find(modal => modal.name === name)) throw new Error(`A modal named "${name}" already exists. Please provide unique modal name.`)

    modals.value.push({
      name,
      component: () => component,
      props,
      openerElement
    })
  }

  /**
   * Close a modal by name
   * @param name - The modal unique name
   * @param resetFocusOnOpener - Reset the focus on the "opener" element
   */
  const close = (name: string, resetFocusOnOpener = true) => {
    const { modal, index } = getOpenedModal(name)
    if (index > -1) {
      // Remove the modal from modals array
      modals.value.splice(index, 1)
      // IMPORTANT: use requestAnimationFrame here to wait for next animation frame
      requestAnimationFrame(() => {
        // Reset focus on "opener" element
        if (resetFocusOnOpener && modal?.openerElement) {
          modal.openerElement.focus()
        }
      })
    }
  }

  /**
   * Get the DisplayedModal object by name and its index in the modals array
   * @param name - The modal name
   */
  const getOpenedModal = (name: string) => {
    let modal: DisplayedModal | undefined
    let index: number = -1
    modals.value.every((m, idx) => {
      if (m.name === name) {
        modal = m
        index = idx
        // If found, return false to break the loop
        return false
      }
    })
    return { modal, index }
  }

  return {
    modals,
    open,
    close,
    getOpenedModal
  }
}

/**
 * Init the root Vue app to provide modals array
 * @param app - The root Vue app
 */
export function initModals (app: App) {
  app.provide(injectionKeys.modals, ref<DisplayedModal[]>([]))
}
