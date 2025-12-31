<template>
  <section>
    <h2>Usage</h2>

    <div class="mb-4 border-b border-gray-100">
      <p>
        First, connect your ESP32 to your computer via <b>USB</b>.<br>
        You can set a baud rate in the field below (if you did not manually change the baud rate before compiling the source code, leave it at <code>115200</code>, which is the default value).
        Then, click the <b>Connect</b> button below to start the connection.
      </p>
      <p>
        For more information, please refer to the <a href="https://github.com/hperchec/esp32-rf433-sniffer">repository's README file</a>.
      </p>
    </div>

    <h2>Connect</h2>

    <!-- If Serial API is supported -->
    <div v-if="serialAPISupport">
      <!-- Device info -->
      <div
        v-if="deviceInfo"
        class="mb-4 rounded-md border border-gray-200 bg-gray-100 p-4"
      >
        <div class="mb-2">
          <b>Connected device:</b>
        </div>
        <ul>
          <li>VID: <span class="font-mono">{{ deviceInfo.vid }}</span> <i class="text-sm">({{ deviceInfo.vendor }})</i></li>
          <li>PID: <span class="font-mono">{{ deviceInfo.pid }}</span> <i class="text-sm">({{ deviceInfo.product }})</i></li>
        </ul>
      </div>

      <label
        for="baud-rate-input"
        class="mb-2 inline-block"
      >
        <div class="mb-2">Baud rate</div>
        <input
          id="baud-rate-input"
          v-model="baudRate"
          name="baud-rate"
          list="baud-rates-datalist"
          placeholder="115200"
          class="input block"
          :class="{
            'border-red-800': !isBaudRateValid,
            'hover:border-red-800': !isBaudRateValid,
            'focus:border-red-800': !isBaudRateValid
          }"
          :disabled="!!isPortOpened"
          type="text"
        >
        <datalist id="baud-rates-datalist">
          <option value="2400" />
          <option value="9600" />
          <option value="38400" />
          <option value="57600" />
          <option value="115200" />
        </datalist>
      </label>
      <div class="mb-2">
        <div class="text-sm text-gray-600">
          Frequently used values are <code>2400</code>, <code>9600</code>, <code>38400</code>, <code>57600</code>, <code>115200</code>, etc... Default is <code>115200</code>.
        </div>
        <div
          v-if="!isBaudRateValid"
          class="text-sm text-red-800"
        >
          Please provide a valid number (integer)
        </div>
      </div>

      <button
        v-if="!isPortOpened"
        class="btn btn-primary mb-4"
        :disabled="!isBaudRateValid"
        @click="connect"
      >
        <fa-icon
          icon="fas fa-plug"
          class="-ml-1"
        /> Connect
      </button>

      <button
        v-else
        class="btn btn-error mb-4"
        @click="disconnect"
      >
        <fa-icon
          icon="fas fa-plug-circle-xmark"
          class="-ml-1"
        /> Disconnect
      </button>

      <hr class="mb-4 h-px border-none bg-gray-100">

      <!-- Console -->
      <Console
        ref="console"
        :disable-prompt="!isPortOpened"
        class="mb-4"
        @prompt="handleUserPrompt"
        @clear="handleConsoleClear"
      />
    </div>

    <!-- Else: error message (not compatible browser) -->
    <div
      v-else
      class="rounded-md border border-red-800 bg-red-100 p-4 text-red-800"
    >
      <fa-icon icon="fas fa-exclamation-triangle" /> This browser <b>does not support</b> <a href="https://developer.mozilla.org/en-US/docs/Web/API/Web_Serial_API">Serial API</a>.
      Please use another web browser like <b>Chrome</b> or <b>Edge</b>.
    </div>
  </section>
</template>

<script setup lang="ts">
import { ref, computed, useTemplateRef } from 'vue'
import {
  isSerialAPISupported,
  requestSerialPortPermission,
  identifyUsbDevice,
  openSerialPort,
  connectSerialPort,
  type UsbDeviceInfo,
  type SerialPortConnection,
  type DataReadenChunk
} from '@/lib/serial'
import Console from './Console.vue'

const serialAPISupport = isSerialAPISupported()
let connection: SerialPortConnection | undefined
// @ts-expect-error TS6133
let dataReader: ReadableStreamDefaultReader<DataReadenChunk> | undefined // eslint-disable-line no-unused-vars
// @ts-expect-error TS6133
let dataWriter: WritableStreamDefaultWriter<string> | undefined // eslint-disable-line no-unused-vars

const templateRefs = {
  console: useTemplateRef('console')
}

const baudRate = ref('115200')
const port = ref<SerialPort>()
const deviceInfo = ref<UsbDeviceInfo>()

const isBaudRateValid = computed(() => baudRate.value.length && baudRate.value.match(/^[0-9]*$/))
const isPortOpened = computed(() => port.value?.readable || port.value?.writable)

/**
 * Reset all local port related data
 */
function resetSerialPort () {
  // Reset port
  port.value = undefined
  // Reset deviceInfo
  deviceInfo.value = undefined
  // Reset dataReader and dataWriter
  dataReader = undefined
  dataWriter = undefined
  // Reset connection object
  connection = undefined
}

/**
 * Connect to serial port
 * @param evt - The native click event
 */
async function connect (evt: Event) {
  let tmpPort: SerialPort
  try {
    tmpPort = await requestSerialPortPermission()
  } catch (error) {
    resetSerialPort()
    console.error(error)
    return
  }

  // Get USB VID & PID
  const { usbVendorId: vid, usbProductId: pid } = tmpPort.getInfo()
  // Try ot identify device vendor and product names
  if (vid && pid) {
    deviceInfo.value = identifyUsbDevice(vid, pid)
  }

  // Add listener to catch unexpected disconnection (e.g. physical unplugging)
  tmpPort.ondisconnect = (event: Event) => {
    resetSerialPort()
    console.warn('Serial connection unexpectedly closed')
  }

  // Wait for the serial port to open
  await openSerialPort(
    tmpPort,
    {
      baudRate: Number(baudRate.value)
    },
    // Pass a custom callback when port is open to always reset the ESP
    async () => {
      // Pull DTR down to trigger Reset
      await tmpPort.setSignals({ dataTerminalReady: false })
      // Wait for a short duration (usually 100ms is enough)
      await new Promise(resolve => setTimeout(resolve, 100))
      // Bring DTR back up to let the ESP32 boot
      await tmpPort.setSignals({ dataTerminalReady: true })
    }
  )

  // Everything is okay, assign port.value
  port.value = tmpPort

  // Get Console component reference
  const ConsoleRef = templateRefs.console.value!

  connection = await connectSerialPort(
    port.value,
    // On data readen
    (chunk) => {
      if (chunk.value) {
        ConsoleRef.appendToOutput(chunk.value)
      }
    },
    // On error
    (error, source) => {
      // Due to the explicit delay() inside the setup() function in the ESP32 code,
      // a FramingError can be caught here. Just ignore it
      if (source === 'readableStream') {
        if (error.name === 'FramingError') {
          console.warn('ReadableStream: a FramingError has been ignored')
        } else {
          throw error
        }
      } else { // Errors from 'readingLoop'...
        // Handle non-fatal read error. We log it and let the loop restart
        console.error('Reading error:', error)
      }
    },
    // On reader created
    (reader) => {
      // Reader has been created...
      dataReader = reader
    },
    // On writer created
    (writer) => {
      // Writer has been created...
      dataWriter = writer
    }
  )

  connection.stopped!.then((res) => {
    console.log('Reader has been stopped')
  })
}

/**
 * Disconnect the opened serial port
 * @param evt - The native click event
 */
async function disconnect (evt: Event) {
  await connection!.cancel()
  await port.value?.close()
  await port.value?.forget()
  resetSerialPort()
}

/**
 * Handle the Console "prompt" event
 * @param prompt - The user prompt as string
 */
async function handleUserPrompt (prompt: string) {
  await connection!.write(prompt)
}

/**
 * Handle the Console "clear" event
 */
function handleConsoleClear () {
  // ...
}
</script>
