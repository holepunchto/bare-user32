# bare-user32

Windows USER bindings for Bare. It currently covers what a test needs to drive a window from the outside: mapping between client and screen coordinates, the cursor, screen metrics and synthesized input.

```
npm i bare-user32
```

## Usage

```js
const user32 = require('bare-user32')

const { MOUSEEVENTF_LEFTDOWN, MOUSEEVENTF_LEFTUP, INPUT_MOUSE } = user32.constants

user32.sendInput([
  { type: INPUT_MOUSE, mi: { flags: MOUSEEVENTF_LEFTDOWN } },
  { type: INPUT_MOUSE, mi: { flags: MOUSEEVENTF_LEFTUP } }
])
```

## License

Apache-2.0
