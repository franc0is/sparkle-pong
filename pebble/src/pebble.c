#include <pebble.h>

/* Types */
typedef enum {
  PlayerOne = 1,
  PlayerTwo = 2
} PlayerId;

typedef enum {
  ActionUp,
  ActionDown,
  ActionReset,
  ActionScore
} Action;

/* Player Selection Window */
static Window *start_window;
static TextLayer *start_p1_text_layer;
static TextLayer *start_p2_text_layer;
static TextLayer *start_title_text_layer;

/* Game Window */
static Window *game_window;
static ActionBarLayer *action_bar;
static PlayerId current_player;
static GBitmap *reset_bitmap;
static GBitmap *arrow_up_bitmap;
static GBitmap *arrow_down_bitmap;
static TextLayer *score_text_layer;
static char score_text[6] = "0-0";
static AppTimer *score_timer;

static void send_action(Action action) {
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  Tuplet action_tuple = TupletInteger(action, current_player);
  dict_write_tuplet(iter, &action_tuple);
  app_message_outbox_send();
}

static void score_timer_callback(void *context) {
  send_action(ActionScore);
  score_timer = app_timer_register(1000, score_timer_callback, NULL);
}

static void game_up_click_handler(ClickRecognizerRef recognizer, void *context) {
  send_action(ActionUp);
}

static void game_down_click_handler(ClickRecognizerRef recognizer, void *context) {
  send_action(ActionDown);
}

static void game_select_click_handler(ClickRecognizerRef recognizer, void *context) {
  send_action(ActionReset);
}

static void game_click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_UP, game_up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, game_down_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, game_select_click_handler);
}

static void in_received_handler(DictionaryIterator *iter, void *context) {
  Tuple *score_tuple = dict_find(iter, ActionScore);

  if (score_tuple) {
    strncpy(score_text, score_tuple->value->cstring, 6);
    text_layer_set_text(score_text_layer, score_text);
  }
}

static void game_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  app_message_register_inbox_received(in_received_handler);

  score_text_layer = text_layer_create(
    (GRect) { .origin = { 42, 72 }, .size = { bounds.size.w, 20 } }
  );
  text_layer_set_text(score_text_layer, "0-0");
  layer_add_child(window_layer, text_layer_get_layer(score_text_layer));
  text_layer_set_font(score_text_layer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));

  action_bar = action_bar_layer_create();
  action_bar_layer_add_to_window(action_bar, window);
  action_bar_layer_set_click_config_provider(action_bar, game_click_config_provider);

  arrow_up_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_ARROW_UP);
  arrow_down_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_ARROW_DOWN);
  reset_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_RESET);
  action_bar_layer_set_icon(action_bar, BUTTON_ID_UP, arrow_up_bitmap);
  action_bar_layer_set_icon(action_bar, BUTTON_ID_DOWN, arrow_down_bitmap);
  action_bar_layer_set_icon(action_bar, BUTTON_ID_SELECT, reset_bitmap);

  score_timer = app_timer_register(1000, score_timer_callback, NULL);
}

static void game_window_unload(Window *window) {
  if (score_timer) {
    app_timer_cancel(score_timer);
  }
  action_bar_layer_destroy(action_bar);
  gbitmap_destroy(arrow_up_bitmap);
  gbitmap_destroy(arrow_down_bitmap);
  gbitmap_destroy(reset_bitmap);
}

static void start_game(PlayerId player_id) {
  current_player = player_id;
  game_window = window_create();
  window_set_click_config_provider(game_window, game_click_config_provider);
  window_set_window_handlers(game_window, (WindowHandlers) {
    .load = game_window_load,
    .unload = game_window_unload,
  });
  const bool animated = true;
  window_stack_push(game_window, animated);
}

static void start_up_click_handler(ClickRecognizerRef recognizer, void *context) {
  start_game(PlayerOne);
}

static void start_down_click_handler(ClickRecognizerRef recognizer, void *context) {
  start_game(PlayerTwo);
}

static void start_click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_UP, start_up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, start_down_click_handler);
}

static void start_window_load(Window *window) {
  Layer *start_window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(start_window_layer);

  start_title_text_layer = text_layer_create(
    (GRect) { .origin = { 0, 72 }, .size = { bounds.size.w, 20 } }
  );
  start_p1_text_layer = text_layer_create(
    (GRect) { .origin = { 0, 24 }, .size = { bounds.size.w, 20 } }
  );
  start_p2_text_layer = text_layer_create(
    (GRect) { .origin = { 0, 120 }, .size = { bounds.size.w, 20 } }
  );

  text_layer_set_text(start_title_text_layer, "Sparkle Pong");
  text_layer_set_text(start_p1_text_layer, "Player 1");
  text_layer_set_text(start_p2_text_layer, "Player 2");

  text_layer_set_text_alignment(start_title_text_layer, GTextAlignmentCenter);
  text_layer_set_text_alignment(start_p1_text_layer, GTextAlignmentRight);
  text_layer_set_text_alignment(start_p2_text_layer, GTextAlignmentRight);

  layer_add_child(start_window_layer, text_layer_get_layer(start_title_text_layer));
  layer_add_child(start_window_layer, text_layer_get_layer(start_p1_text_layer));
  layer_add_child(start_window_layer, text_layer_get_layer(start_p2_text_layer));
}

static void start_window_unload(Window *window) {
  text_layer_destroy(start_p1_text_layer);
  text_layer_destroy(start_p2_text_layer);
}

static void init(void) {
  app_message_open(32, 32);
  start_window = window_create();
  window_set_click_config_provider(start_window, start_click_config_provider);
  window_set_window_handlers(start_window, (WindowHandlers) {
    .load = start_window_load,
    .unload = start_window_unload,
  });
  const bool animated = true;
  window_stack_push(start_window, animated);
}

static void deinit(void) {
  window_destroy(start_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
