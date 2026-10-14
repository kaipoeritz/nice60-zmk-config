# nice!60 reactive regional RGB (experimental)

**Do not flash until the GitHub Actions build succeeds.** Full technical notes: `docs/README_DE.md`.

Custom Zephyr/ZMK module for 12 WS2812B LEDs. Emits a dim regional glow on key press for 1000 ms, then turns it off. Deep sleep is configured for 15 minutes, but wake-up and battery life remain to be tested on hardware. LED-index mapping is explicitly provisional.

The legacy ZMK RGB control key bindings remain in `config/nice60.keymap` as comments for recovery; all non-RGB bindings are preserved.
