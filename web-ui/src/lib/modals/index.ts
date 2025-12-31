import { type Component } from 'vue'

export * from './vue'

export interface DisplayedModal {
  /**
   * The modal unique name
   */
  name: string
  /**
   * The modal component factory
   */
  component: () => Component
  /**
   * The props (or attributes) to pass to the modal component instance
   */
  props: Record<string, any> // eslint-disable-line @typescript-eslint/no-explicit-any
  /**
   * The "opener" HTML element. For example, it can be the element to which
   * the user has clicked to open the modal
   */
  openerElement?: HTMLElement
}
