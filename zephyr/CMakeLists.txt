# Build the custom source only when reactive RGB is active.
zephyr_library()
zephyr_library_sources_ifdef(CONFIG_NICE60_REACTIVE_RGB ../src/reactive_rgb.c)
