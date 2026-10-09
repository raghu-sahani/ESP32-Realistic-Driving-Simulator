# Version 2 verification

Completed successfully:

- JavaScript syntax checks for game.js and controller.js.
- The actual game/controller scripts executed with a simulated DOM and serial stream.
- 24-car traffic initialization, acceleration, positive/right steering, and brake priority.
- High-speed front collision, guardrail collision, and a side-by-side near miss using oriented rectangles.
- Crash damage, particle generation, incident display, and restart cleanup.
- Split CTRL packets, CRLF, valid steering/button states, malformed/out-of-range rejection.
- A held accelerator cannot start the car on initial connection or after reset/recovery/pause until both buttons have been released.
- Legacy STEER lines cannot hide a missing CTRL heartbeat once full control mode is established.
- Signal timeout stops the vehicle; manual disconnect, reconnect, unplug/read error, and legacy steering-only compatibility.
- Rendered driving and crash scenes inspected as raster images from the actual canvas renderer.
- Actual Arduino sketch compiled on the host against Arduino API stubs; simulated pin changes verified both encoder directions, bounce cancellation, steering limits, center switch debounce, both pedal debouncers, simultaneous button reporting, release, and heartbeat output.

Limits: these checks use simulated hardware and DOM APIs. They are not an ESP32 board compilation/upload or a full browser interaction test. The available browser could not open the local project files. Verify USB, button orientation, browser rendering, audio, and control feel on your laptop.

## Laptop and hardware checklist

1. Upload the new sketch. At 115200 baud, both buttons released must give CTRL:0,0,0 after pressing the encoder switch.
2. Press accelerator: CTRL:0,1,0. Press brake only: CTRL:0,0,1. Press both: CTRL:0,1,1.
3. Rotate the encoder both ways and confirm signed values. Press SW to return to zero.
4. Close Serial Monitor. Open the game with Live Server or the included launcher in Chrome/Edge.
5. Connect ESP32, release both buttons, center, then accelerate. Check both pedal indicators and steering direction.
6. While accelerating, hold brake too. The car must brake, not accelerate.
7. Drive toward a traffic car or guardrail. Check impact motion, debris/smoke, and the restart panel.
8. Keep the accelerator held while clicking Restart drive. The car must remain stationary until both buttons are released, then accelerator is pressed again.
9. Unplug USB while moving. The car must stop. Reconnect and repeat the release-before-drive check.
10. Switch to another window while moving. Return, click Resume, release buttons, then drive.
11. Check C camera switching, sensitivity, sound toggle, and keyboard W/S fallback.
12. Verify Reset Car clears damage and trip distance. Press physical SW afterward to center the encoder value.
