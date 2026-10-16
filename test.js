const test = require('brittle')
const user32 = require('.')

const {
  SM_CXSCREEN,
  SM_CYSCREEN,
  INPUT_MOUSE,
  INPUT_KEYBOARD,
  MOUSEEVENTF_MOVE,
  MOUSEEVENTF_ABSOLUTE,
  KEYEVENTF_KEYUP
} = user32.constants

const VK_SHIFT = 0x10

test('measures the screen', (t) => {
  t.ok(user32.getSystemMetrics(SM_CXSCREEN) > 0, 'width')
  t.ok(user32.getSystemMetrics(SM_CYSCREEN) > 0, 'height')
})

test('maps points through the desktop window', (t) => {
  const desktop = user32.getDesktopWindow()

  t.is(typeof desktop, 'bigint')
  t.alike(user32.clientToScreen(desktop, 12, 34), { x: 12, y: 34 }, 'to the screen')
  t.alike(user32.screenToClient(desktop, 12, 34), { x: 12, y: 34 }, 'to the client area')
})

test('cannot map points through a window that does not exist', (t) => {
  t.is(user32.clientToScreen(0n, 0, 0), null)
})

test('takes only a window handle', (t) => {
  t.exception.all(() => user32.clientToScreen(0, 0, 0), /Expected a window handle/)
})

test('moves the cursor', (t) => {
  const width = user32.getSystemMetrics(SM_CXSCREEN)
  const height = user32.getSystemMetrics(SM_CYSCREEN)

  const x = Math.floor(width / 2)
  const y = Math.floor(height / 2)

  // Absolute coordinates divide the primary screen into 65536 steps.
  const sent = user32.sendInput([
    {
      type: INPUT_MOUSE,
      mi: {
        dx: Math.round((x * 65536) / width),
        dy: Math.round((y * 65536) / height),
        flags: MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE
      }
    }
  ])

  t.is(sent, 1, 'sent')
  t.alike(user32.getCursorPos(), { x, y }, 'moved')
})

test('presses and releases a key', (t) => {
  const sent = user32.sendInput([
    { type: INPUT_KEYBOARD, ki: { vk: VK_SHIFT } },
    { type: INPUT_KEYBOARD, ki: { vk: VK_SHIFT, flags: KEYEVENTF_KEYUP } }
  ])

  t.is(sent, 2)
})

test('sends nothing', (t) => {
  t.is(user32.sendInput([]), 0)
})

test('rejects an unknown input type', (t) => {
  t.exception.all(() => user32.sendInput([{ type: 42 }]), /Unknown input type '42'/)
})
