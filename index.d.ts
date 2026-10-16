import constants = require('./lib/constants')

export { constants }

/** A window, as its `HWND`. */
export type Window = bigint

export interface Point {
  x: number
  y: number
}

export interface MouseInput {
  type: typeof constants.INPUT_MOUSE
  mi: {
    dx?: number
    dy?: number
    mouseData?: number
    /** A combination of `MOUSEEVENTF_*` constants. */
    flags?: number
    time?: number
  }
}

export interface KeyboardInput {
  type: typeof constants.INPUT_KEYBOARD
  ki: {
    vk?: number
    scan?: number
    /** A combination of `KEYEVENTF_*` constants. */
    flags?: number
    time?: number
  }
}

export type Input = MouseInput | KeyboardInput

/** `ClientToScreen()`: the point `x`, `y` in the client area of `window`, in screen coordinates, or `null` if it fails. */
export function clientToScreen(window: Window, x: number, y: number): Point | null

/** `ScreenToClient()`: the screen point `x`, `y`, in the client area of `window`, or `null` if it fails. */
export function screenToClient(window: Window, x: number, y: number): Point | null

/** `GetCursorPos()`: where the cursor is, in screen coordinates, or `null` if it fails. */
export function getCursorPos(): Point | null

/** `GetDesktopWindow()`. */
export function getDesktopWindow(): Window

/** `GetSystemMetrics()`, given an `SM_*` constant. */
export function getSystemMetrics(index: number): number

/** `SendInput()`: how many of `inputs` were inserted into the input stream. */
export function sendInput(inputs: Input[]): number
