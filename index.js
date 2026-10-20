const binding = require('./binding')

exports.constants = require('./lib/constants')

const { INPUT_MOUSE, INPUT_KEYBOARD } = exports.constants

// Matches `BARE_USER32_INPUT_FIELDS` in `binding.c`.
const FIELDS = 6

exports.clientToScreen = function clientToScreen(window, x, y) {
  return binding.clientToScreen(window, x, y)
}

exports.screenToClient = function screenToClient(window, x, y) {
  return binding.screenToClient(window, x, y)
}

exports.getCursorPos = function getCursorPos() {
  return binding.getCursorPos()
}

exports.getDesktopWindow = function getDesktopWindow() {
  return binding.getDesktopWindow()
}

exports.windowFromPoint = function windowFromPoint(x, y) {
  return binding.windowFromPoint(x, y)
}

exports.getAncestor = function getAncestor(window, flags) {
  return binding.getAncestor(window, flags)
}

exports.getForegroundWindow = function getForegroundWindow() {
  return binding.getForegroundWindow()
}

exports.setForegroundWindow = function setForegroundWindow(window) {
  return binding.setForegroundWindow(window)
}

exports.getCapture = function getCapture() {
  return binding.getCapture()
}

exports.releaseCapture = function releaseCapture() {
  return binding.releaseCapture()
}

exports.getSystemMetrics = function getSystemMetrics(index) {
  return binding.getSystemMetrics(index)
}

exports.sendInput = function sendInput(inputs) {
  const fields = new Int32Array(inputs.length * FIELDS)

  inputs.forEach((input, i) => {
    fields.set(flatten(input), i * FIELDS)
  })

  return binding.sendInput(fields)
}

function flatten(input) {
  switch (input.type) {
    case INPUT_MOUSE: {
      const { dx = 0, dy = 0, mouseData = 0, flags = 0, time = 0 } = input.mi
      return [INPUT_MOUSE, dx, dy, mouseData, flags, time]
    }
    case INPUT_KEYBOARD: {
      const { vk = 0, scan = 0, flags = 0, time = 0 } = input.ki
      return [INPUT_KEYBOARD, vk, scan, flags, time, 0]
    }
    default:
      throw new TypeError(`Unknown input type '${input.type}'`)
  }
}
