/**
 * USB device info type
 */
export interface UsbDeviceInfo {
  vid?: string
  vendor?: string
  pid?: string
  product?: string
}

/**
 * Static references of ESP32 USB vendor IDs and products IDs
 */
export const ESP_USB_IDS_MAP = {
  // Silicon Labs VID
  0x10C4: {
    name: 'Silicon Labs',
    products: {
      0xEA60: 'CP2102/CP2104'
    }
  },
  // Espressif VID
  0x303A: {
    name: 'Espressif',
    products: {
      0x0002: 'ESP32-S2',
      0x1001: 'ESP32-S3/C3/C5/C6/C61/H2/P4'
    }
  }
}

/**
 * Converts hexadecimal string to integer
 * @param hexStr - The hexadecimal value
 */
export function usbIdHexToInt (hexStr: string) {
  return Number(hexStr.startsWith('0x') ? hexStr.substring(2) : hexStr)
}

/**
 * Converts integer to hexadecimal string
 * @param int - The integer
 */
export function usbIdIntToHex (int: number) {
  return '0x' + int.toString(16).toUpperCase()
}

/**
 * Try to find vendor and product name based on VID and PID
 * @param vid - The usb VID
 * @param pid - The usb PID
 */
export function identifyUsbDevice (vid: number, pid: number): UsbDeviceInfo {
  const vendor = ESP_USB_IDS_MAP[vid as keyof typeof ESP_USB_IDS_MAP]
  return {
    vid: usbIdIntToHex(vid),
    vendor: vendor?.name || undefined,
    pid: usbIdIntToHex(pid),
    // @ts-expect-error TS7053
    product: vendor?.products[pid] as string | undefined
  }
}

/**
 * Check if the Web Serial API is supported
 */
export function isSerialAPISupported () {
  if ('serial' in navigator) {
    return true
  } else {
    return false
  }
}

/**
 * Prompt user to select any serial port
 * @param options - Same options as Serial.requestPort() method
 */
export function requestSerialPortPermission (options?: SerialPortRequestOptions) {
  return navigator.serial.requestPort(options)
}

/**
 * Type for openSerialPort function callback
 */
export type PortOpenCallback = () => void | Promise<void>

/**
 * Open the given serial port
 * @param port - The serial port
 * @param onPortOpen - The callback to execute when port is open
 */
export async function openSerialPort (
  port: SerialPort,
  options: SerialOptions = { baudRate: 9600 },
  onPortOpen?: PortOpenCallback
) {
  // Check if port is already open
  if (port.readable || port.writable) {
    throw new Error('The target port is already open')
  } else {
    await port.open(options)
    if (onPortOpen) {
      await onPortOpen()
    }
    return port
  }
}

/**
 * Interface for data readen chunk
 */
export interface DataReadenChunk {
  value: string | undefined
  raw: Uint8Array | undefined
}

/**
 * Type for connectSerialPort "onDataReaden" callback
 */
export type DataReadenCallback = (
  chunk: DataReadenChunk
) => void | Promise<void>

/**
 * Type for connectSerialPort "onError" callback
 */
export type ReaderErrorCallback = (
  error: Error,
  source: 'readableStream' | 'readingLoop'
) => void

/**
 * Type for connectSerialPort "onReaderCreated" callback
 */
export type ReaderCreatedCallback = (
  reader: ReadableStreamDefaultReader<DataReadenChunk>
) => void | Promise<void>

/**
 * Type for connectSerialPort "onWriterCreated" callback
 */
export type WriterCreatedCallback = (
  writer: WritableStreamDefaultWriter<string> | undefined
) => void | Promise<void>

/**
 * Create data reader for an open serial port
 * @param port - The open serial port
 * @param onDataReaden - The callback to execute when data is readen
 */
export async function connectSerialPort (
  port: SerialPort,
  onDataReaden: DataReadenCallback,
  onError?: ReaderErrorCallback,
  onReaderCreated?: ReaderCreatedCallback,
  onWriterCreated?: WriterCreatedCallback
) {
  const textDecoder = new TextDecoder()

  const connection = new SerialPortConnection(port)

  if (onWriterCreated) {
    await onWriterCreated(connection.writer)
  }

  connection.start(async () => {
    while (port.readable && connection.keepReading) {
      // IMPORTANT: Create a NEW instance of TransformStream for each iteration.
      // Streams cannot be reused once they have been closed or faulted by an error
      const transformer = new TransformStream<Uint8Array, DataReadenChunk>({
        start () {},
        transform (chunk: Uint8Array, controller) {
          // Decode the raw chunk into a string and enqueue the custom object
          const transformed = {
            value: textDecoder.decode(chunk),
            raw: chunk
          }
          controller.enqueue(transformed)
        },
        flush () {}
      })

      // pipeTo() locks the port.readable stream to the transformer.
      // getReader() then locks the output of the transform stream.
      connection.readableStreamClosed = port.readable
        .pipeTo(transformer.writable, { signal: connection.abortController.signal })
        .catch((error: Error) => {
          if (onError) {
            onError(error, 'readableStream')
          } else {
            throw error
          }
        })
      const reader = connection.reader = transformer.readable.getReader()

      if (onReaderCreated) {
        await onReaderCreated(reader)
      }

      try {
        while (true) {
          const { value, done } = await reader.read()
          if (done) {
            // |reader| has been canceled.
            break
          }
          // Do something with |value|…
          await onDataReaden(value)
        }
      } catch (error) {
        if (onError) {
          onError(error as Error, 'readingLoop')
        } else {
          // Handle non-fatal read error. We log it and let the loop restart
          console.error('Reading error:', error)
        }
      } finally {
        // CRITICAL: releaseLock() must be called to unlock port.readable.
        // This allows the next iteration of the 'while' loop to call
        // .pipeTo() again without the "parameter 1's 'writable' is locked" error.
        reader.releaseLock()
      }
    }
  })

  return connection
}

/**
 * Represents a serial port connection
 */
export class SerialPortConnection {
  constructor (port: SerialPort) {
    this.#port = port
    // Create abort controller
    this.abortController = new AbortController()
    // Set writer
    const textEncoder = new TextEncoderStream()
    this.writableStreamClosed = textEncoder.readable.pipeTo(port.writable!, { signal: this.abortController.signal })
    this.writer = textEncoder.writable.getWriter()
  }

  /**
   * The target serial port
   */
  #port: SerialPort

  get port () {
    return this.#port
  }

  /**
   * Reading state
   */
  keepReading: boolean = true

  /**
   * The AbortController instance
   */
  abortController: AbortController

  /**
   * The reader object
   */
  reader: ReadableStreamDefaultReader<DataReadenChunk> | undefined

  /**
   * The promise resolved when readable stream is closed
   */
  readableStreamClosed: Promise<void> | undefined

  /**
   * The writer object
   */
  writer: WritableStreamDefaultWriter<string>

  /**
   * The promise resolved when writable stream is closed
   */
  writableStreamClosed: Promise<void>

  /**
   * The promise resolved when reader is stopped.
   */
  stopped: Promise<void> | undefined

  /**
   * Start the connection
   * @param readingLoopFactory - The reading loop factory
   */
  async start (readingLoopFactory: () => Promise<void>) {
    this.stopped = readingLoopFactory()
  }

  /**
   * Stop the connection via the AbortController instance
   * @param reason - Same as AbortController#abort method
   */
  async abort (reason: any) { // eslint-disable-line @typescript-eslint/no-explicit-any
    this.keepReading = false
    this.abortController.abort(reason)
    await this.readableStreamClosed?.catch(() => {})
    await this.writableStreamClosed.catch(() => {})
  }

  /**
   * Stop the connection by calling reader.cancel() and writer.close() methods
   */
  async cancel () {
    this.keepReading = false
    this.reader?.cancel()
    await this.readableStreamClosed?.catch(() => {})
    this.writer.close()
    await this.writableStreamClosed.catch(() => {})
  }

  /**
   * Write to the serial port
   * @param value - The string to write
   */
  async write (value: string) {
    await this.writer.write(value)
  }
}
