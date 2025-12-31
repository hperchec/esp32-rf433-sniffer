<template>
  <div
    ref="backdrop"
    class="fixed left-0 top-0 z-[100] flex size-full items-center justify-center bg-black/50"
    @click.self="handleBackdropClick"
    @keydown="handleKeydown"
  >
    <div
      ref="wrapper"
      class="relative flex min-h-72 max-w-2xl flex-col rounded border border-gray-100 bg-white p-8 shadow-lg"
      role="dialog"
      tabindex="-1"
      aria-modal="true"
      :aria-labelledby="titleElementId"
      :aria-describedby="contentElementId"
    >
      <!-- Top right close icon/button -->
      <slot
        v-if="closable"
        name="close-icon"
        v-bind="closeIconSlotProps"
      >
        <button
          v-bind="closeIconSlotProps.attrs"
          v-on="closeIconSlotProps.on"
        >
          <fa-icon
            icon="fas fa-times"
            class="pointer-events-none"
          />
        </button>
      </slot>

      <!-- Title -->
      <slot
        name="title"
        v-bind="titleSlotProps"
      >
        <div v-bind="titleSlotProps.attrs">
          {{ title }}
        </div>
      </slot>

      <!-- Default (content) -->
      <slot v-bind="contentSlotProps">
        <div v-bind="contentSlotProps.attrs">
          {{ content }}
        </div>
      </slot>

      <!-- Action buttons -->
      <div class="mt-8 flex items-center justify-center">
        <button
          class="btn btn-default"
          @click="handleCloseButtonClick"
        >
          Close
        </button>
      </div>
    </div>
  </div>
</template>

<script setup lang="ts">
import {
  computed,
  useAttrs,
  useTemplateRef,
  onMounted,
  type UnwrapRef
} from 'vue'

import { useModals } from '..'

const props = defineProps({
  /**
   * The modal title
   */
  title: {
    type: String,
    default: 'Title'
  },
  /**
   * The modal content
   */
  content: {
    type: String,
    default: 'Content'
  },
  /**
   * Auto reset the focus on the "opener" element when modal is closed
   */
  resetFocusOnOpener: {
    type: Boolean,
    default: true
  },
  /**
   * Define if modal is closable. If `false`, it disables the "close-icon" slot and
   * the "closeOnEscape" and "closeOnBackdrop" props will have no effect.
   */
  closable: {
    type: Boolean,
    default: true
  },
  /**
   * Close modal on "Escape" keydown event
   */
  closeOnEscape: {
    type: Boolean,
    default: true
  },
  /**
   * Close modal when clicking outside
   */
  closeOnBackdrop: {
    type: Boolean,
    default: true
  }
})

const emit = defineEmits<{
  /**
   * Emitted when modal is opened
   */
  open: []
  /**
   * Emitted when modal is closed
   */
  close: []
}>()

/* eslint-disable @typescript-eslint/no-explicit-any */
defineSlots<{
  'close-icon': (slotProps: UnwrapRef<typeof closeIconSlotProps>) => any
  'title': (slotProps: UnwrapRef<typeof titleSlotProps>) => any
  'default': (slotProps: UnwrapRef<typeof contentSlotProps>) => any
}>()
/* eslint-enable @typescript-eslint/no-explicit-any */

// Expose some data, methods, etc...
defineExpose({
  close
})

const templateRefs = {
  backdrop: useTemplateRef('backdrop'),
  wrapper: useTemplateRef('wrapper')
}

// "name" attribute is required
const { name } = useAttrs() as { name: string }
// Check if "name" attribute has been provided
if (typeof name !== 'string' || !name.length) throw new Error('The "name" attribute of BaseModal component is required')

const { close: closeModal } = useModals()

const titleElementId = computed(() => `${name}-title`)
const contentElementId = computed(() => `${name}-content`)

const closeIconSlotProps = computed(() => ({
  props,
  attrs: {
    id: titleElementId.value,
    class: [
      'absolute',
      'flex', 'items-center', 'justify-center',
      'top-2', 'right-2',
      'size-6' /* 24px */,
      'cursor-pointer',
      'opacity-60',
      'transition', 'duration-200', 'ease-in-out',
      'hover:opacity-100',
      'focus:opacity-100'
    ],
    style: {},
    title: 'Close',
    ariaLabel: 'Close'
  },
  on: {
    click: (evt: MouseEvent) => {
      close()
    }
  }
}))

const titleSlotProps = computed(() => ({
  props,
  attrs: {
    id: titleElementId.value,
    class: [ 'h2', 'text-center' ],
    style: {},
    role: 'heading',
    ariaLevel: '2'
  }
}))

const contentSlotProps = computed(() => ({
  props,
  attrs: {
    id: contentElementId.value,
    class: [ 'flex', 'flex-1', 'items-center' ]
  }
}))

// Mounted hook
onMounted(() => {
  // Auto-focus this component element
  templateRefs.wrapper.value?.focus()
  // Emit "open" event
  emit('open')
})

/**
 * Close this modal
 */
function close () {
  emit('close')
  closeModal(name, props.resetFocusOnOpener)
}

/**
 * Handle keydown event
 * @param evt - The native keydown event
 */
function handleKeydown (evt: KeyboardEvent) {
  if (props.closable && props.closeOnEscape && evt.code === 'Escape') {
    close()
  }
}

/**
 * Handle click on backdrop
 * @param evt - The native click event
 */
function handleBackdropClick (evt: MouseEvent) {
  if (props.closable && props.closeOnBackdrop) {
    close()
  }
}

/**
 * Handle click on "Close" button
 * @param evt - The native click event
 */
function handleCloseButtonClick (evt: MouseEvent) {
  close()
}
</script>
