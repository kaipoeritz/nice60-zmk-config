
# nice!60 reactive regional RGB lighting
# Build the custom LED controller only when enabled.

zephyr_library()

zephyr_library_sources_ifdef(
    CONFIG_NICE60_REACTIVE_RGB
    ../src/reactive_rgb.c
)
