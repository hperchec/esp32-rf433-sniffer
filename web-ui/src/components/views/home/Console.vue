<template>
  <div class="flex flex-col">
    <div class="relative flex-1 rounded-t bg-gray-900">
      <!-- Output -->
      <textarea
        id="console-output-textarea"
        ref="console-output"
        v-model="output"
        name="console-output"
        rows="12"
        readonly
        class="
          block
          min-h-44
          w-full
          resize-y
          overflow-x-hidden overflow-y-scroll
          bg-transparent
          p-2
          font-mono text-white
          focus:outline-none
        "
        :style="{
          scrollbarWidth: 'thin',
          scrollbarColor: `${theme('colors.gray.500')} ${theme('colors.gray.900')}`
        }"
      />
      <!-- Actions -->
      <div
        class="absolute bottom-0 flex w-auto items-center pr-1"
        :style="{
          right: `${outputScrollbarWidth}px`
        }"
      >
        <!-- Download button -->
        <button
          class="
            p-1
            text-xs font-extralight text-white
            opacity-60
            transition duration-200 ease-in-out
            hover:opacity-100
            focus:opacity-100
            disabled:pointer-events-none
          "
          title="Download output as plain text"
          :disabled="!(output.length > 1)"
          @click="handleClickOnSave"
        >
          <fa-icon
            icon="fas fa-download"
            size="sm"
            class="pointer-events-none"
          />
          Save
        </button>
        <!-- Clear output -->
        <button
          class="
            p-1
            text-xs font-extralight text-white
            opacity-60
            transition duration-200 ease-in-out
            hover:opacity-100
            focus:opacity-100
            disabled:pointer-events-none
          "
          title="Clear output"
          :disabled="!(output.length > 1)"
          @click="handleClickOnClear"
        >
          <fa-icon
            icon="fas fa-trash"
            size="sm"
            class="pointer-events-none"
          />
          Clear
        </button>
      </div>
    </div>
    <div class="mt-px flex items-center rounded-b bg-gray-900 font-mono text-white">
      <div class="pl-2">
        &gt;
      </div>
      <input
        id="console-prompt-input"
        v-model="prompt"
        name="console-prompt"
        spellcheck="false"
        placeholder="_"
        class="
          flex-1
          border-none
          bg-transparent
          px-2 py-1.5
          text-white
          hover:outline-none
          focus:outline-none
          disabled:cursor-not-allowed
        "
        type="text"
        :disabled="disablePrompt"
        @keydown="handleKeyDownOnPrompt"
      >
      <div class="pr-2">
        <button
          class="opacity-60 transition duration-200 ease-in-out hover:opacity-100 focus:opacity-100"
          title="Console help"
          aria-label="Console help"
          @click="handleClickOnConsoleHelp"
        >
          <fa-icon
            icon="fas fa-circle-question"
            class="pointer-events-none"
          />
        </button>
      </div>
    </div>
  </div>
</template>

<script setup lang="ts">
import { ref, useTemplateRef, watch, onMounted } from 'vue'
import { useModals } from '@/lib/modals'
import { useTailwind } from '@/lib/tailwind'
import ConsoleHelpModal from '@/components/modals/ConsoleHelpModal.vue'

defineProps({
  disablePrompt: {
    type: Boolean,
    default: false
  }
})

const emit = defineEmits<{
  /**
   * Emitted when prompt is submitted
   */
  prompt: [prompt: string]
  /**
   * Emitted when output is cleared
   */
  clear: []
}>()

// Expose some data, methods, etc...
defineExpose({
  appendToOutput,
  clearOutput,
  saveAsPlainText
})

const templateRefs = {
  'console-output': useTemplateRef('console-output')
}

const prompt = ref('')
const promptHistory = ref<string[]>([])
const currentPromptHistoryIndex = ref<number | null>(null)
const output = ref('')
const outputScrollbarWidth = ref(0)

const { open: openModal } = useModals()
const { theme } = useTailwind()

onMounted(() => {
  const consoleOutputEl = templateRefs['console-output'].value
  if (consoleOutputEl) {
    // Get the console-output-textarea scrollbar width
    outputScrollbarWidth.value = consoleOutputEl.offsetWidth - consoleOutputEl.clientWidth
  }
  // Watch output to auto scroll to the bottom
  watch(output, (to, from) => {
    if (consoleOutputEl) {
      consoleOutputEl.scrollTop = consoleOutputEl.scrollHeight
    }
  })
})

/**
 * Handle keydown event on prompt input
 * @param evt - The native keydown event
 */
function handleKeyDownOnPrompt (evt: KeyboardEvent) {
  if (evt.key === 'ArrowUp') {
    evt.preventDefault()
    // Fill with previous prompt from history
    if (promptHistory.value.length) {
      if (currentPromptHistoryIndex.value === null) {
        currentPromptHistoryIndex.value = promptHistory.value.length - 1
      } else if (currentPromptHistoryIndex.value > 0) {
        currentPromptHistoryIndex.value--
      }
      prompt.value = promptHistory.value[currentPromptHistoryIndex.value] as string
    }
  } else if (evt.key === 'ArrowDown') {
    evt.preventDefault()
    // Fill with next prompt from history (or empty string if last)
    if (currentPromptHistoryIndex.value !== null) {
      if (currentPromptHistoryIndex.value < (promptHistory.value.length - 1)) {
        currentPromptHistoryIndex.value++
        prompt.value = promptHistory.value[currentPromptHistoryIndex.value] as string
      } else if (currentPromptHistoryIndex.value === (promptHistory.value.length - 1)) {
        currentPromptHistoryIndex.value = null
        prompt.value = ''
      }
    }
  } else if (evt.key === 'Enter') {
    // Send command
    sendCommand(prompt.value)
  }
}

/**
 * Send command and save prompt in history
 * @param value - The user prompt string
 */
function sendCommand (value: string) {
  // Prevent empty string
  if (!value.length) return
  // Prevent "blank" string
  if (value.trim().length) {
    // Save in history
    promptHistory.value.push(value.trim())
    // Update current prompt history index
    currentPromptHistoryIndex.value = null
    // Send
    emit('prompt', value.trim())
    appendToOutput(value.trim() + '\n')
  }
  // Empty prompt input
  prompt.value = ''
}

/**
 * Clear the output
 */
function clearOutput () {
  output.value = ''
  emit('clear')
}

/**
 * Append the given string to the output
 * @param value - The string to append
 */
function appendToOutput (value: string) {
  output.value += value
}

/**
 * Save (download) the console output content as plain text
 * in a `.txt` file
 */
function saveAsPlainText () {
  // create file link in browser's memory
  const href = URL.createObjectURL(new Blob([ output.value ], {
    type: 'text/plain'
  }))
  const fileName = 'esp32-rf433-sniffer-output'
  // create "a" HTML element with href to file & click
  const vElement = document.createElement('a')
  vElement.href = href
  vElement.setAttribute('download', `${fileName}.txt`)
  vElement.click()
  // Free memory
  URL.revokeObjectURL(href)
}

/**
 * Handle click on console help button
 * @param evt - The native click event
 */
function handleClickOnConsoleHelp (evt: MouseEvent) {
  openModal('ConsoleHelpModal', ConsoleHelpModal, {}, evt.currentTarget as HTMLButtonElement)
}

/**
 * Handle click on "save" button
 * @param evt - The native click event
 */
function handleClickOnSave (evt: MouseEvent) {
  // Save as plain text
  saveAsPlainText()
}

/**
 * Handle click on "clear" button
 * @param evt - The native click event
 */
function handleClickOnClear (evt: MouseEvent) {
  clearOutput()
}
</script>

<style>
#console-output-textarea::-webkit-resizer {
  @apply bg-gray-600;
}
</style>
