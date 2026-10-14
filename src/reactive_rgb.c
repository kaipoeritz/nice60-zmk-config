/*
 * SPDX-License-Identifier: MIT
 * nice!60 WS2812B reactive regional underglow, experimental.
 * One SPI strip owner; ZMK built-in RGB must be disabled in nice60.conf.
 */
#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/led_strip.h>
#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <drivers/ext_power.h>
#include <zmk/event_manager.h>
#include <zmk/events/position_state_changed.h>
#include "led_regions.h"

LOG_MODULE_REGISTER(nice60_reactive_rgb, LOG_LEVEL_INF);

#define STRIP_NODE DT_CHOSEN(zmk_underglow)
#define NUM_PIXELS DT_PROP(STRIP_NODE, chain_length)
#define AFTERGLOW_MS CONFIG_NICE60_REACTIVE_RGB_AFTERGLOW_MS

BUILD_ASSERT(DT_HAS_CHOSEN(zmk_underglow), "nice!60 underglow chosen node missing");
BUILD_ASSERT(NUM_PIXELS == 12, "Regional map expects 12 WS2812B LEDs");
BUILD_ASSERT(sizeof(nice60_region_for_position) == 61, "Map expects 61 key positions");
BUILD_ASSERT(!IS_ENABLED(CONFIG_ZMK_RGB_UNDERGLOW), "Disable legacy RGB animation engine");

static const struct device *const strip = DEVICE_DT_GET(STRIP_NODE);
static const struct device *const rgb_power = DEVICE_DT_GET(DT_INST(0, zmk_ext_power_generic));
static struct led_rgb pixels[NUM_PIXELS];
static int64_t expiry_ms[NUM_PIXELS];
static struct k_spinlock rgb_lock;
static struct k_work_delayable draw_work;

static void draw_handler(struct k_work *work)
{
    const int64_t now = k_uptime_get();
    int64_t next = INT64_MAX;
    k_spinlock_key_t key = k_spin_lock(&rgb_lock);
    for (unsigned int i = 0; i < NUM_PIXELS; ++i) {
        if (expiry_ms[i] > now) {
            /* Warm white illumination, one-second afterglow. */
            pixels[i] = (struct led_rgb){ .r = 255, .g = 230, .b = 196 };
            if (expiry_ms[i] < next) next = expiry_ms[i];
        } else {
            pixels[i] = (struct led_rgb){0};
        }
    }
    k_spin_unlock(&rgb_lock, key);
    if (next != INT64_MAX) {
        int power_err = ext_power_enable(rgb_power);
        if (power_err) {
            LOG_ERR("RGB power enable failed: %d", power_err);
            return;
        }
        k_msleep(2);
    }
    int err = led_strip_update_rgb(strip, pixels, NUM_PIXELS);
    if (next == INT64_MAX) {
        (void)ext_power_disable(rgb_power);
    }
    if (err) LOG_WRN("WS2812 update failed: %d", err);
    if (next != INT64_MAX) {
        int64_t remaining = next - k_uptime_get();
        (void)k_work_reschedule(&draw_work, K_MSEC(remaining > 0 ? remaining : 1));
    }
}

static int event_listener(const zmk_event_t *eh)
{
    const struct zmk_position_state_changed *event = as_zmk_position_state_changed(eh);
    if (!event) return ZMK_EV_EVENT_BUBBLE;
    if (!event->state || event->position >= sizeof(nice60_region_for_position)) {
        return ZMK_EV_EVENT_BUBBLE;
    }
    /* Every key press illuminates all 12 LEDs. */
    k_spinlock_key_t key = k_spin_lock(&rgb_lock);
    const int64_t deadline = k_uptime_get() + AFTERGLOW_MS;

    for (unsigned int i = 0; i < NUM_PIXELS; ++i) {
        expiry_ms[i] = deadline;
    }

    k_spin_unlock(&rgb_lock, key);
    (void)k_work_reschedule(&draw_work, K_NO_WAIT);
    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(nice60_regional_rgb, event_listener);
ZMK_SUBSCRIPTION(nice60_regional_rgb, zmk_position_state_changed);

static int reactive_rgb_init(void)
{
    if (!device_is_ready(strip)) {
        LOG_ERR("WS2812 SPI strip not ready");
        return -ENODEV;
    }
    k_work_init_delayable(&draw_work, draw_handler);
    memset(pixels, 0, sizeof(pixels));
    if (!device_is_ready(rgb_power)) {
        LOG_ERR("RGB power device not ready");
        return -ENODEV;
    }
    return ext_power_disable(rgb_power);
}
SYS_INIT(reactive_rgb_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
