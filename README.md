# Encoder Drive II — Coastal Run

A local browser driving game controlled by an ESP32, a rotary encoder, and **two additional push buttons**. No npm install, CDN, account, or internet connection is required to play.

## What changed in version 2

- Coastal highway with gentle bends, sea, mountains, roadside trees, signs, and guardrails.
- More detailed cars with sloped windows, wheels, mirrors, lights, and visible impact deformation.
- 24 moving traffic vehicles in four lanes; vehicles follow bends and slow behind other traffic.
- Chase camera plus a switchable hood view (C).
- Acceleration, braking, steering smoothing, speed-dependent steering, body roll/pitch, and off-road resistance.
- Traffic and guardrail collisions, sliding/spinning, camera shake, debris, sparks, smoke, and skid marks. Severe impacts can show a brief fire effect; not every crash catches fire.
- Engine and impact audio. Click Sound off to turn audio on.
- Live speedometer, steering input, accelerator/brake indicators, distance, and overtakes.
- Steering sensitivity slider, pause/resume, and restart after a crash.
- Physical accelerator and brake buttons, debounced in firmware.
- Release-before-drive interlock after reset, crash, pause, connection, or signal recovery.

This is a lightweight, stylized 3D arcade simulation, not photorealistic graphics or a calibrated vehicle/crash physics model. Body damage, fire, and collision speed are game effects, not real-world safety predictions.

## Hardware

- 1 classic ESP32 DevKit / ESP32-WROOM development board
- 1 mechanical rotary encoder with CLK, DT, SW, +, GND (such as KY-040)
- 2 normally-open momentary push buttons
- Breadboard and jumper wires
- USB **data** cable

The encoder already includes the steering-center switch. The two new buttons are one accelerator and one brake. This pin map targets the classic ESP32; it is not an ESP32-C3 or ESP32-S3 pin map.

## Wiring

Disconnect power before wiring. Keep all grounds common.

| Component terminal | Connect to |
| --- | --- |
| Encoder GND | ESP32 GND |
| Encoder + | ESP32 3.3V |
| Encoder SW | GPIO 25 |
| Encoder DT | GPIO 33 |
| Encoder CLK | GPIO 32 |
| Accelerator button, first contact | GPIO 26 |
| Accelerator button, other contact | GND |
| Brake button, first contact | GPIO 27 |
| Brake button, other contact | GND |

**The new buttons connect between GPIO and GND. They do not need a connection to 3.3V or 5V.** The sketch enables the ESP32 internal pull-up resistors. Released reads HIGH; pressed reads LOW.

On a four-leg tactile push button, two legs in each internally connected contact group are already joined. Use one terminal from each group so they connect only when the button is pressed. If unsure, use a multimeter continuity test. Connecting two already-common legs will make the input appear permanently pressed.

Power the ESP32 through USB. Supply the encoder at 3.3V. No motor or external motor driver is required.

## Upload the NEW ESP32 sketch

1. Open Arduino IDE and install **esp32 by Espressif Systems** in Boards Manager if needed.
2. Open `firmware/EncoderSteering/EncoderSteering.ino` from this updated folder.
3. Select **ESP32 Dev Module** for your classic DevKit, select its USB COM port, and upload.
4. Open Serial Monitor at **115200 baud** to verify the values below.
5. **Close Serial Monitor and Serial Plotter** before connecting the game. Also close old simulator tabs using the ESP32.

No additional Arduino libraries are required.

At rest:

```text
STEER:0
CTRL:0,0,0
```

Accelerator pressed:

```text
STEER:0
CTRL:0,1,0
```

Steering right at 40, brake pressed:

```text
STEER:40
CTRL:40,0,1
```

The format is `CTRL:steering,accelerator,brake`. Buttons are 0=released, 1=pressed. A simultaneous press produces `CTRL:0,1,1`; the game gives braking priority.

If the direction is reversed, change `REVERSE_ENCODER` to `true`, then upload again. Default steering changes by 5 per click, requiring 20 clicks from center to full lock. Adjust `STEERING_STEP` if needed. If your encoder needs two clicks for one change, try `TRANSITIONS_PER_CLICK = 2` instead of 4.

## Run the game

Extract the ZIP fully. Use **desktop Chrome or Edge**.

### Windows shortcut (Python required)

Double-click `START_WINDOWS.bat`. This starts the included local server and opens the game. Keep the terminal window open. If your default browser is not Chrome or Edge, copy the displayed URL into Chrome or Edge.

### VS Code Live Server

Open the extracted `encoder-drive` folder in VS Code, right-click `index.html`, and select **Open with Live Server**. Open the localhost / 127.0.0.1 address in Chrome or Edge.

### Any system with Python 3

From inside the folder:

```sh
python run_game.py
```

Use `python3 run_game.py` on macOS/Linux if needed. Or use a standard server:

```sh
python -m http.server 8000 --bind 127.0.0.1
```

Then open **http://127.0.0.1:8000**. Do not launch the game from inside the ZIP. Keep the HTML, CSS and JavaScript files together. Use a normal browser tab rather than an embedded editor preview.

## Connect and drive

1. Click **Connect ESP32**, select the board's USB port, and allow access.
2. Wait for **ESP32 · steering + buttons**. The board can reset when its port opens.
3. Release both physical buttons. If paused, click **Resume** first, then release both buttons.
4. Press the encoder switch to center steering.
5. Hold the accelerator button and turn the encoder.
6. Hold the brake button to slow down. If both buttons are pressed, braking wins.
7. After a collision, watch the impact sequence, release both buttons, and click **Restart drive**. Press the encoder switch to center again.

## Controls

| Control | Action |
| --- | --- |
| Encoder left/right | Change steering; holds its value |
| Encoder switch | Set steering to 0 |
| GPIO 26 button | Accelerate while held |
| GPIO 27 button | Brake while held |
| W / Up Arrow | Keyboard acceleration, including while ESP32 is connected |
| S / Down Arrow | Keyboard braking, including while ESP32 is connected |
| A / D or Left / Right | Keyboard steering when ESP32 is disconnected |
| C / Camera | Switch chase / hood camera |
| P / Pause | Pause/resume; release physical buttons before driving again |
| Reset Car | Stop, clear damage, and reset road position and trip |
| Sound | Enable/disable engine and impact sounds |
| Steering slider | Adjust steering response from 0.30× to 1.40× |

Keyboard steering returns to center when released. The **encoder does not self-center**: turn it back or press its switch. Reset Car resets the game; it cannot reset the value stored in the ESP32 with this one-way protocol.

Switching tabs or leaving the browser window pauses and stops the vehicle. Click Resume or press P after returning. The brake takes precedence over both physical and keyboard acceleration. There is no reverse gear.

## Serial behavior and failsafes

- 115200 baud, default 8N1.
- `STEER:<-100..100>` is retained for v1 compatibility.
- New `CTRL:<-100..100>,<0|1>,<0|1>` packets provide one complete snapshot of steering and both buttons.
- New firmware transmits both lines on changes at up to 50 Hz, with a heartbeat every 100 ms.
- Once the browser sees CTRL, it requires fresh CTRL packets. Legacy STEER messages cannot keep stale pedal inputs alive.
- No valid control packet for 1.5 seconds stops and blocks the vehicle until data returns.
- Connecting, resetting, crashing, pausing, or recovering from lost data disarms hardware acceleration. Both physical buttons must be released before it rearms. A button held through a restart cannot launch the car.
- USB disconnect stops the vehicle, releases the port, and makes keyboard controls available.
- Input parsing handles split lines and CRLF; malformed lines and out-of-range values are ignored.
- Old steering-only sketches still allow encoder steering with keyboard W/S, but **you must upload the new sketch for the physical buttons**.

## Troubleshooting

| Symptom | Check |
| --- | --- |
| Buttons do nothing | Upload this version's sketch; look for CTRL packets and the steering + buttons status. |
| Status says release buttons | Let go of both buttons after clicking Resume/Restart. Confirm both raw button values are 0. |
| A button is permanently pressed | Check that its two wires use different contact groups on a four-leg tactile switch. |
| Always brakes | Brake input may be shorted to GND; verify `CTRL:0,0,0` with both buttons released. |
| Car stopped after switching apps | Click Resume or press P; release both buttons before accelerating. |
| No serial port | Try a known data cable, another USB port, and the board's appropriate CP210x/CH340 driver. |
| Cannot open serial port | Close Serial Monitor, Serial Plotter, and other tabs/programs using the COM port. |
| Signal lost | Confirm the new sketch sends newline-terminated CTRL packets continuously at 115200 baud. |
| Steering backwards | Set REVERSE_ENCODER=true in the sketch and upload. |
| Steering too sensitive | Reduce the in-game slider or STEERING_STEP in the sketch. |
| Wheels center but car keeps its direction | Normal: centered wheels stop turning the car; they do not rotate its heading back to the road. |
| Still steering after Reset Car | Press the physical encoder switch. |
| Jitter or missed encoder clicks | Shorten wires, verify GND, and check transitions per click. Fast/noisy encoders may need hardware filtering. |
| No sound | Click Sound off to enable it; check browser/tab/system volume. |

The simplified driving model uses a bicycle steering model with speed-dependent wheel angle, a lateral acceleration limit, smoothing, rolling/aerodynamic drag, braking, and body movement. Oriented rectangle collisions are checked at a fixed 120 Hz simulation step. Impact spin, deformation, smoke, and debris are authored effects rather than a full rigid/soft-body solver.

## Files

- `index.html`, `style.css`: interface and dashboard.
- `game.js`: road, rendering, physics, traffic, collision effects, audio, keyboard controls.
- `controller.js`: USB Serial parsing, connection lifecycle, pedal interlocks.
- `firmware/EncoderSteering/EncoderSteering.ino`: ESP32 code.
- `run_game.py`, `START_WINDOWS.bat`: optional local launchers.
- `TESTING.md`: completed checks and hardware checklist.

## References

- [Espressif Arduino-ESP32 GPIO and internal pull-ups](https://docs.espressif.com/projects/arduino-esp32/en/latest/api/gpio.html)
- [Chrome Developers: Web Serial API](https://developer.chrome.com/docs/capabilities/serial)
