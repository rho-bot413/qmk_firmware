// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

#define L_SYMBOLS MO(_SYMBOLS)
#define L_ARROWS MO(_ARROWS)

// Modify these values to adjust the scrolling speed, smaller is faster
#define SCROLL_DIVISOR_H 8.0
#define SCROLL_DIVISOR_V 8.0

enum layers {
    _QWERTY,
	_SYMBOLS, 
	_ARROWS
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
	[_QWERTY] = LAYOUT(
    KC_ESC, KC_1, KC_2, KC_3, KC_4, KC_5,                                                   KC_6, KC_7, KC_8, KC_9, KC_0, KC_MINS,
    KC_TAB, KC_Q, KC_W, KC_E, KC_R, KC_T,                                                   KC_Y, KC_U, KC_I, KC_O, KC_P, KC_BSPC,
    KC_LSFT, KC_A, KC_S, KC_D, KC_F, KC_G,                                                  KC_H, KC_J, KC_K, KC_L, KC_SCLN, KC_QUOT,
    KC_LCTL, KC_Z, KC_X, KC_C, KC_V, KC_B,                                                  KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH, KC_RSFT,
                KC_LALT, L_ARROWS,KC_ENT,KC_SPC,MS_BTN1,                                    	KC_SPC,L_SYMBOLS, KC_RALT,
                                  KC_Q, KC_W, MS_BTN2,                             		KC_R,KC_SPC,KC_ENT),
								  
	[_ARROWS] = LAYOUT(
    KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,                                                   KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
    KC_TRNS, KC_TRNS, KC_PSCR, KC_TRNS, KC_TRNS, KC_TRNS,                                                   KC_TRNS, KC_TRNS, KC_UP, KC_TRNS, KC_TRNS, KC_TRNS,
    KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,                                                  KC_TRNS, KC_LEFT, KC_DOWN, KC_RIGHT, KC_TRNS, KC_TRNS,
    KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,                                                  KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
                KC_TRNS, KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,                                    	KC_TRNS,KC_TRNS, KC_TRNS,
                                  KC_TRNS, KC_TRNS, KC_TRNS,                             		KC_TRNS,KC_TRNS,KC_TRNS),
								  
	[_SYMBOLS] = LAYOUT(
    KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,                                                   KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
    KC_TRNS, KC_TRNS, KC_PSCR, KC_TRNS, KC_TRNS, KC_TRNS,                                                   KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
    KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,                                                  KC_MINS, KC_EQL , KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
    KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,                                                  KC_UNDS, KC_PLUS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
                KC_TRNS, KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,                                    	KC_TRNS,KC_TRNS, KC_TRNS,
                                  KC_TRNS, KC_TRNS, KC_TRNS,                             		KC_TRNS,KC_TRNS,KC_TRNS)
};

//Yeah, I'm putting globals in the middle of my file, sue me
static bool scrolling_mode = false;

layer_state_t layer_state_set_user(layer_state_t state) {
    switch (get_highest_layer(state)) {
        case _SYMBOLS:  // If we're on the _RAISE layer enable scrolling mode
            scrolling_mode = true;
            pointing_device_set_cpi(100);
            break;
        default:
            if (scrolling_mode) {  // check if we were scrolling before and set disable if so
                scrolling_mode = false;
                pointing_device_set_cpi(1600);
            }
            break;
    }
    return state;
}

// Variables to store accumulated scroll values
float scroll_accumulated_h = 0;
float scroll_accumulated_v = 0;

// Function to handle mouse reports and perform drag scrolling
report_mouse_t pointing_device_task_user(report_mouse_t mouse_report) {
    // Check if drag scrolling is active
    if (scrolling_mode) {
        // Calculate and accumulate scroll values based on mouse movement and divisors
        scroll_accumulated_h += (float)		mouse_report.x / SCROLL_DIVISOR_H;
        scroll_accumulated_v += (float)(-1*(mouse_report.y / SCROLL_DIVISOR_V));

        // Assign integer parts of accumulated scroll values to the mouse report
        mouse_report.h = (int8_t)scroll_accumulated_h;
        mouse_report.v = (int8_t)scroll_accumulated_v;

        // Update accumulated scroll values by subtracting the integer parts
        scroll_accumulated_h -= (int8_t)scroll_accumulated_h;
        scroll_accumulated_v -= (int8_t)scroll_accumulated_v;

        // Clear the X and Y values of the mouse report
        mouse_report.x = 0;
        mouse_report.y = 0;
    }
    return mouse_report;
}
