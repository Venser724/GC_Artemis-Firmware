# CLAUDE.md — Artemis Watch Firmware

Fork of [CircuitMess/GC_Artemis-Firmware](https://github.com/CircuitMess/GC_Artemis-Firmware) (NASA Artemis Watch 2.0), itself derived from `Clockstar-Firmware`. ESP32-S3, ESP-IDF (native, not Arduino), LVGL UI, FreeRTOS. Fully open source, MIT-licensed upstream.

## Branch structure

- **`master`** — stable, general-purpose firmware. Only platform-agnostic fixes belong here (real bugs, correctness issues). Kept close to upstream `circuitmess/master`, with confirmed fixes cherry-picked in.
- **`a-dev`** — Android-focused branch (Venser724's own watch). Built for use with GadgetBridge instead of the official CircuitMess Connect app. Custom watch faces/features for personal use go here.
- **`i-dev`** — iPhone-focused branch (for a specific iPhone user). Built around the standard ANCS/CTS path Apple exposes natively. Custom watch faces/features for that use case go here.

Both `a-dev` and `i-dev` branch from `master` and should periodically rebase/merge from it to pick up general fixes.

## Git remotes

- `origin` — Venser724's own fork, push target for everything (`git@github.com:Venser724/GC_Artemis-Firmware.git`).
- `circuitmess` — official upstream (`CircuitMess/GC_Artemis-Firmware`). Read-only reference; never push here without explicit instruction. Has an `AndroidProto` branch with unreleased Android-specific work (media info handling) worth checking before reimplementing anything similar on `a-dev`.
- `upstream` — `CircuitMess/Clockstar-Firmware`, the base project this firmware line was originally forked from. Reference only.

## Build environment

- ESP-IDF pinned to commit `3a45d4e949a174e8829a2e4c86c421b030ceac5a` (per upstream README — labeled "v5.1" by CircuitMess, reports as `v5.2-dev-*` via `git describe` since it's a pre-release snapshot). Cloned at `~/esp/esp-idf` (shared, not inside this repo).
- **Python**: the system default (3.14 as of writing) is too new — ESP-IDF's pinned `requirements.core.txt` doesn't have wheels for it. Use `python@3.12` from Homebrew. A symlink at `~/.local/bin-esp-idf/python3 -> python@3.12/bin/python3.12` exists so `install.sh`/`export.sh` pick the right interpreter; prepend it to `PATH` before sourcing ESP-IDF's `export.sh`.
- **`pkg_resources` / setuptools**: setuptools ≥82 (Feb 2026) removed `pkg_resources`, which this ESP-IDF snapshot's tooling still imports. If `check_python_dependencies.py` fails with `ModuleNotFoundError: pkg_resources`, run `pip install "setuptools<82"` inside the ESP-IDF venv (`~/.espressif/python_env/idf5.2_py3.12_env`).
- **Patches required after any fresh ESP-IDF/submodule checkout** (see upstream README for the `.patch` files in repo root):
  - `ESP-IDF.patch` → apply inside `~/esp/esp-idf` (FPU register context-switch fix).
  - `JPEGDEC.patch` → apply inside `components/JPEGDEC`.
  - `LovyanGFX.patch` → apply inside `components/LovyanGFX`.

### Typical build/flash session

```bash
export PATH="$HOME/.local/bin-esp-idf:$PATH"
source ~/esp/esp-idf/export.sh
cd /Users/venser/git/github.com/Venser724/artemis-watch
idf.py build
idf.py -p /dev/cu.usbmodem101 flash   # port varies, check `ls /dev/cu.*`
```

`idf.py monitor` needs a real TTY and won't run backgrounded — for headless log capture, read the serial port directly with `pyserial` instead (open at 115200 baud).

## Known gotcha: log level vs. `Time` task stack

`sdkconfig` ships with `CONFIG_LOG_DEFAULT_LEVEL_WARN` — intentional, not an oversight. The `Time` FreeRTOS task (`main/src/Services/Time.cpp`) only has a 2 KB stack (`SleepyThreaded(UpdateInterval, "Time", 2 * 1024, ...)`). At `WARN`, its `ESP_LOGI` calls never reach the actual `vsnprintf`-style formatting (ESP-IDF checks the tag's level before formatting), so the tight stack is never an issue. Bumping the default level to `INFO` (e.g. for debugging) makes that formatting actually run every 5 seconds inside `Time::sleepyLoop()` and reliably overflows the stack, crashing/rebooting the watch. If you need INFO+ logs for debugging, do it temporarily and revert before considering the build "done" — or raise `Time`'s stack size properly instead of leaving default logging at INFO.

## Confirmed fixes already on `master`

- **`main/src/Notifs/Bangle.cpp` — `findArg()` out-of-bounds iterator.** Built the returned substring from `line.cend() + fnEnd` instead of `line.cbegin() + fnEnd`, reading past the buffer. Since `CONFIG_COMPILER_CXX_EXCEPTIONS` is disabled, the garbage passed to `std::stoll()` caused `abort()` rather than a catchable error — the watch crashed/rebooted every time a phone tried to send `setTime`/`setTimeZone` over the Bangle protocol. Present in upstream since the original `14a4db87` commit (2023), never fixed there. Verified live: with the fix, GadgetBridge's `setTime`/`setTimeZone` are parsed and applied cleanly (see `Bangle: Got UNIX time: ...` / `Time: Updating time by time_t` in serial log).

## `a-dev`-only: BLE device name

`main/src/BLE/GAP.cpp` advertises `"Bangle.js Artemis"` instead of the upstream `"Artemis Watch"`. GadgetBridge's `BangleJSCoordinator` gates pairing on the advertised name matching `Bangle\.js.*|Pixl\.js.*|Puck\.js.*|MDBT42Q.*|Espruino.*` *before* it ever probes the GATT services — the underlying UART service/protocol already matches Bangle.js byte-for-byte, only the name blocked it. This rename is Android/GadgetBridge-specific by intent; don't merge it into `master` or `i-dev`.

## CircuitMess Connect app version gate

The official Android/iOS companion app ("CircuitMess Connect") refuses to open the Bangle GATT/notification path at all if it considers the firmware "outdated" — confirmed this blocks things even with the exact official `v2.1.1` release binary flashed (downloaded from GitHub Releases), so the gate compares against something newer than what's public. The web flasher at `code.circuitmess.com` only offers "Restore" to that same public release — no self-service way to get whatever newer version the app expects. **GadgetBridge (`com.espruino.gadgetbridge.banglejs` on Play Store) sidesteps this entirely** since it talks the BLE protocol directly and doesn't check CircuitMess's version gate — this is the recommended companion app for `a-dev`, not CircuitMess Connect.

## Releases

- **[a-dev-v1.0](https://github.com/Venser724/GC_Artemis-Firmware/releases/tag/a-dev-v1.0)** — first clean baseline for `a-dev` (commit `d3b8f49`). Bangle.cpp fix + `Bangle.js Artemis` device name, with Lunar Lander, Theremin, themes 2-9, and Perse-Ctrl/rover control (plus the Perse-Common submodule) stripped out. Attached asset `Artemis-a-dev-v1.0.bin` is a complete 4MB flash image (bootloader + app + partition table + storage) — flash directly with `esptool --chip esp32s3 write_flash 0 Artemis-a-dev-v1.0.bin`, no toolchain needed. Made this so the firmware could be handed to someone else without them setting up ESP-IDF.

To cut a new release from any branch: tag the commit (`git tag -a <branch>-vX.Y -m "..." <sha>`, push with `git push origin <tag>`), build the merged single-file image with `esptool --chip esp32s3 merge_bin --fill-flash-size 4MB -o <name>.bin --flash_mode dio --flash_freq 80m 0x0 build/bootloader/bootloader.bin 0x8000 build/partition_table/partition-table.bin 0x10000 build/Artemis-Firmware.bin 0x2f6000 build/storage.bin`, then `gh release create <tag> --title "..." --notes-file <notes> <name>.bin`. Show release notes for approval before publishing — it's a public action.

## Current project goals

Build out `a-dev` and `i-dev` into genuinely different firmware per use case — different watch faces, different feature sets — sharing only the general-purpose fixes that land on `master`. Nothing platform-specific has been designed yet beyond the device-name change on `a-dev`.

## `a-dev` protocol decision: stay on Bangle/GadgetBridge, don't adopt `AndroidProto`

Upstream has an abandoned branch, `circuitmess/AndroidProto` (diverged from `master` at `f8c8c22`, last commit `03db596`), that replaces `Bangle.cpp`/`Bangle.h` entirely with a new `Android.cpp`/`Android.h` + `MediaSource`/`MediaInfo` — a custom `hello;`/`time;`/`notifAdd;`/`mediaState;`/`mediaInfo;` semicolon-delimited protocol with a version handshake (`Android::ProtocolVersion = "1"`, `Android::FirmwareVersion = "v2.1"`). It's backend-only — no `Screens`/`UIElements` changes, so a media/call UI would still need to be built from scratch regardless of which protocol backs it.

This most likely explains the CircuitMess Connect app's "Outdated firmware" gate (see above): the app probably sends `hello;<protocolVersion>` and expects a `version;...` reply, which current firmware (public `master`, and the official `v2.1.1` release — neither ever got this branch merged) simply doesn't understand, so the handshake never completes. This is inferred from matching symptoms, not confirmed by sniffing actual CircuitMess Connect BLE traffic.

**Decision: `a-dev` stays on the Bangle.js/GadgetBridge-compatible protocol (`Bangle.cpp`), not `AndroidProto`.** Reasoning:

- GadgetBridge is verified working end-to-end right now (notifications, calls, time sync, find-phone) — switching to `Android.cpp` would drop GadgetBridge compatibility outright, since GadgetBridge only speaks the Bangle.js wire format.
- Whether the current CircuitMess Connect app actually speaks `AndroidProto`'s protocol is unconfirmed — porting/finishing it is a bet against a closed, unverified, moving target (CircuitMess can change their app's expectations at any time, without notice, and we'd have to reverse-engineer again).
- The whole point of this fork is staying in control of the watch — GadgetBridge is open source and doesn't gate functionality behind someone else's backend/version check; CircuitMess Connect does (that's the entire "Outdated firmware" saga).

**`AndroidProto` is kept as a reference for ideas/fixes to port piecemeal, not a base to build on.** Concretely already identified: GadgetBridge sends `musicinfo` over the current Bangle protocol (confirmed live in serial log — `Unhandled command from phone: musicinfo`), which `Bangle.cpp` doesn't handle. Adding a handler for it (informed by how `Android::handleMediaInfo`/`MediaSource` model the data) is a cheap way to get media info without adopting the new protocol wholesale.
