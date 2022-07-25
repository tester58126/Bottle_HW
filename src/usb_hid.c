#include "usb_hid.h"


enum {
  ITF_KEYBOARD = 0,
  ITF_MOUSE = 1
};

static uint32_t blink_interval_ms = BLINK_NOT_MOUNTED;

void usb_hid_init()
{
    board_init();
    tusb_init();
}

void usb_hid_task()
{
    tud_task();
    led_blinking_task();

    // Poll every 1ms
    const uint32_t interval_ms = 1;
    static uint32_t start_ms = 0;

    if ( board_millis() - start_ms < interval_ms) 
        return; // not enough time
    start_ms += interval_ms;

    // Remote wakeup
    if ( tud_suspended())
    {
        // Wake up host if we are in suspend mode
        // and REMOTE_WAKEUP feature is enabled by host
        tud_remote_wakeup();
    }

    /*------------- Keyboard -------------*/
    if ( tud_hid_n_ready(ITF_KEYBOARD) )
    {
         // use to avoid send multiple consecutive zero report for keyboard
        static bool has_key = false;

        uint8_t keycode[6] = { 0 };
        keycode[0] = HID_KEY_A;
        //tud_hid_n_keyboard_report(ITF_KEYBOARD, 0, 0, keycode);
        if (has_key)
            tud_hid_n_keyboard_report(ITF_KEYBOARD, 0, 0, NULL);
        has_key = false;
    }

    /*------------- Mouse -------------*/
    if ( tud_hid_n_ready(ITF_MOUSE) )
    {
        int8_t const delta_x = 0;
        int8_t const delta_y = 0;

        // no button, right + down, no scroll pan
        tud_hid_n_mouse_report(ITF_MOUSE, 0, 0x00, delta_x, delta_y, 0, 0);
    }
}

// Invoked when device is mounted
void tud_mount_cb(void)
{
  blink_interval_ms = BLINK_MOUNTED;
}

// Invoked when device is unmounted
void tud_umount_cb(void)
{
  blink_interval_ms = BLINK_NOT_MOUNTED;
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en)
{
  (void) remote_wakeup_en;
  blink_interval_ms = BLINK_SUSPENDED;
}

// Invoked when usb bus is resumed
void tud_resume_cb(void)
{
  blink_interval_ms = BLINK_MOUNTED;
}


// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
    // TODO not Implemented
    (void) itf;
    (void) report_id;
    (void) report_type;
    (void) buffer;
    (void) reqlen;

    return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
    // TODO set LED based on CAPLOCK, NUMLOCK etc...
    (void) itf;
    (void) report_id;
    (void) report_type;
    (void) buffer;
    (void) bufsize;
}


void led_blinking_task(void)
{
  static uint32_t start_ms = 0;
  static bool led_state = false;

  // Blink every interval ms
  if ( board_millis() - start_ms < blink_interval_ms) return; // not enough time
  start_ms += blink_interval_ms;

  board_led_write(led_state);
  led_state = 1 - led_state; // toggle
}
