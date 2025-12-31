// Augmenting vue types
declare module 'vue' {
  /**
   * Augment global properties
   */
  export interface ComponentCustomProperties { // eslint-disable-line @typescript-eslint/no-empty-object-type
    // ...
  }

  /**
   * Define global components
   */
  export interface GlobalComponents {
    FaIcon: typeof import('@fortawesome/vue-fontawesome').FontAwesomeIcon
  }
}

export {}
