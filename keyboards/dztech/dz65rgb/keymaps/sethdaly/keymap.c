#include QMK_KEYBOARD_H

typedef struct {
  bool is_press_action;
  int state;
} tap;

enum {
  SINGLE_TAP = 1,
  SINGLE_HOLD = 2,
  DOUBLE_TAP = 3,
  DOUBLE_HOLD = 4,
  DOUBLE_SINGLE_TAP = 5, //send two single taps
  TRIPLE_TAP = 6,
  TRIPLE_HOLD = 7
};

//Tap dance enums
enum {
  ESC_PWR,
  ENT_PWR
};

int cur_dance (qk_tap_dance_state_t *state);

//for the esc tap dance. Put it here so it can be used in any keymap
void esc_finished (qk_tap_dance_state_t *state, void *user_data);
void esc_reset (qk_tap_dance_state_t *state, void *user_data);

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT_65_ansi(
        TD(ESC_PWR),        KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC, KC_DEL,
        KC_TAB,         KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS, LCA(KC_F11),
        CTL_T(KC_CAPS), KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,          KC_ENT,  KC_HOME,
        KC_LSFT,                 KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT, KC_UP,   KC_END,
        KC_LCTL,        KC_LGUI, KC_LALT,                            KC_SPC,                    KC_RALT, MO(1),   KC_RCTL, KC_LEFT, KC_DOWN, KC_RGHT
    ),
    [1] = LAYOUT_65_ansi(
        KC_GESC,        KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  KC_DEL,  KC_HOME,
        _______,        RGB_TOG, RGB_MOD, RGB_HUI, RGB_HUD, RGB_SAI, RGB_SAD, RGB_VAI, RGB_VAD, _______, KC_PSCR, KC_SLCK, KC_PAUS, RESET,   KC_PGUP,
        CTL_T(KC_CAPS), RGB_SPI, RGB_SPD, _______, _______, _______, _______, _______, _______, _______, _______, _______,          EEP_RST, KC_MPLY,
        KC_LSFT,                 _______, _______, _______, _______, _______, NK_TOGG, _______, _______, _______, _______, _______, KC_VOLU, KC_MUTE,
        KC_SLEP,        _______, _______,                            _______,                   _______, _______, _______, KC_MPRV, KC_VOLD, KC_MNXT
    )
};

int cur_dance (qk_tap_dance_state_t *state) {
  if (state->count == 1) {
    if (state->interrupted || !state->pressed)  return SINGLE_TAP;
      else return SINGLE_HOLD;
  }
  else if (state->count == 2) {
    if (state->interrupted) return DOUBLE_SINGLE_TAP;
    else if (state->pressed) return DOUBLE_HOLD;
    else return DOUBLE_TAP;
  }
  if (state->count == 3) {
    if (state->interrupted || !state->pressed)  return TRIPLE_TAP;
    else return TRIPLE_HOLD;
  }
  else return 8; //magic number. At some point this method will expand to work for more presses
}


static tap esctap_state = {
  .is_press_action = true,
  .state = 0
};

void esc_finished (qk_tap_dance_state_t *state, void *user_data) {
  esctap_state.state = cur_dance(state);
  switch (esctap_state.state) {
    case SINGLE_TAP: register_code(KC_ESC); break;
    case SINGLE_HOLD: register_code(KC_SLEP); break;
    case DOUBLE_TAP: register_code16(LGUI(KC_L)); break;
    case DOUBLE_HOLD: register_code(KC_PWR); break;
    case DOUBLE_SINGLE_TAP: register_code(KC_ESC); unregister_code(KC_ESC); register_code(KC_ESC);
  }
}
void esc_reset (qk_tap_dance_state_t *state, void *user_data) {
  switch (esctap_state.state) {
    case SINGLE_TAP: unregister_code(KC_ESC); break;
    case SINGLE_HOLD: unregister_code(KC_SLEP); break;
    case DOUBLE_TAP: unregister_code16(LGUI(KC_L)); break;
    case DOUBLE_HOLD: unregister_code(KC_PWR);
    case DOUBLE_SINGLE_TAP: unregister_code(KC_ESC);
  }
  esctap_state.state = 0;
}

qk_tap_dance_action_t tap_dance_actions[] = {
    [ESC_PWR] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, esc_finished, esc_reset)
};