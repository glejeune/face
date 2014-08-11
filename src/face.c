// Standard includes
#include "pebble.h"


// App-specific data
Window *window; // All apps must have at least one window
TextLayer *time_layer_hour; 
TextLayer *time_layer_min; 
TextLayer *battery_layer_icon;
TextLayer *battery_layer_txt;
TextLayer *connection_layer_icon;
bool bluetooth_connected = true;

static char hours[25][7] = {
  "minuit",
  "une",
  "deux",
  "trois",
  "quatre",
  "cinq",
  "six",
  "sept",
  "huit",
  "neuf",
  "dix",
  "onze",
  "midi",
  "une",
  "deux",
  "trois",
  "quatre",
  "cinq",
  "six",
  "sept",
  "huit",
  "neuf",
  "dix",
  "onze",
  "minuit"
};

static char hours_unit[25][8] = {
  "",
  "heure",
  "heures",
  "heures",
  "heures",
  "heures",
  "heures",
  "heures",
  "heures",
  "heures",
  "heures",
  "heures",
  "",
  "heure",
  "heures",
  "heures",
  "heures",
  "heures",
  "heures",
  "heures",
  "heures",
  "heures",
  "heures",
  "heures",
  ""
};

static char minutes[60][18] = {
  "",                  // 0
  "",                  // 1
  "",                  // 2
  "\ncinq",             // 3
  "\ncinq",             // 4
  "\ncinq",             // 5
  "\ncinq",             // 6
  "\ncinq",             // 7
  "\ndix",              // 8
  "\ndix",              // 9
  "\ndix",              // 10
  "\ndix",              // 11
  "\ndix",              // 12
  "\net quart",         // 13
  "\net quart",         // 14
  "\net quart",         // 15
  "\net quart",         // 16
  "\net quart",         // 17
  "\nvingt",            // 18
  "\nvingt",            // 19
  "\nvingt",            // 20
  "\nvingt",            // 21
  "\nvingt",            // 22
  "\nvingt cinq",       // 23
  "\nvingt cinq",       // 24
  "\nvingt cinq",       // 25
  "\nvingt cinq",       // 26
  "\nvingt cinq",       // 27
  "\net demi",          // 28
  "\net demi",          // 29
  "\net demi",          // 30
  "\net demi",          // 32
  "\nmoins\nvingt cinq", // 33
  "\nmoins\nvingt cinq", // 34
  "\nmoins\nvingt cinq", // 35
  "\nmoins\nvingt cinq", // 36
  "\nmoins\nvingt cinq", // 37
  "\nmoins\nvingt",      // 38
  "\nmoins\nvingt",      // 39
  "\nmoins\nvingt",      // 40
  "\nmoins\nvingt",      // 41
  "\nmoins\nvingt",      // 42
  "\nmoins\nle quart",   // 43
  "\nmoins\nle quart",   // 44
  "\nmoins\nle quart",   // 45
  "\nmoins\nle quart",   // 46
  "\nmoins\nle quart",   // 47
  "\nmoins\ndix",        // 48
  "\nmoins\ndix",        // 49
  "\nmoins\ndix",        // 50
  "\nmoins\ndix",        // 51
  "\nmoins\ndix",        // 52
  "\nmoins\ncinq",       // 53
  "\nmoins\ncinq",       // 54
  "\nmoins\ncinq",       // 55
  "\nmoins\ncinq",       // 56
  "\nmoins\ncinq",       // 57
  "",                  // 58
  ""                   // 59
};

static void handle_battery(BatteryChargeState charge_state) {
  static char battery_text[] = "100";
  if (charge_state.is_charging) {
    text_layer_set_text(battery_layer_icon, "\ue808");
  } else {
    if (charge_state.charge_percent < 10) {
      text_layer_set_text(battery_layer_icon, "\ue805");
    } else if (charge_state.charge_percent < 90) {
      text_layer_set_text(battery_layer_icon, "\ue806");
    } else {
      text_layer_set_text(battery_layer_icon, "\ue807");
    }
  }
  snprintf(battery_text, sizeof(battery_text), "%d", charge_state.charge_percent);
  text_layer_set_text(battery_layer_txt, battery_text);
}

// Called once per second
static void handle_second_tick(struct tm* tick_time, TimeUnits units_changed) {
  static char time_text_min[] = "heures moins vingt cinq";

  int min = tick_time->tm_min;
  int hour = tick_time->tm_hour;
  if(min > 32) {
    hour++;
  }

  text_layer_set_text(time_layer_hour, hours[hour]);

  snprintf(time_text_min, sizeof(time_text_min), "%s%s", hours_unit[hour], minutes[min]);
  text_layer_set_text(time_layer_min, time_text_min);

  handle_battery(battery_state_service_peek());
}

static void handle_bluetooth(bool connected) {
  if (bluetooth_connected != connected) {
    vibes_long_pulse();
    bluetooth_connected = connected;
  }
  text_layer_set_text(connection_layer_icon, connected ? "\ue800" : "\ue801");
}

// Handle the start-up of the app
static void do_init(void) {

  // Create our app's base window
  window = window_create();
  window_stack_push(window, true);
  window_set_background_color(window, GColorBlack);

  Layer *root_layer = window_get_root_layer(window);
  GRect frame = layer_get_frame(root_layer);

  // Init the text layer used to show the time
  time_layer_hour = text_layer_create(GRect(0, 20, frame.size.w /* width */, frame.size.h - 20/* height */));
  text_layer_set_text_color(time_layer_hour, GColorWhite);
  text_layer_set_background_color(time_layer_hour, GColorClear);
  text_layer_set_text_alignment(time_layer_hour, GTextAlignmentLeft);
  text_layer_set_font(time_layer_hour, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ASAP_BOLD_30)));

  time_layer_min = text_layer_create(GRect(0, 50, frame.size.w /* width */, frame.size.h - 50/* height */));
  text_layer_set_text_color(time_layer_min, GColorWhite);
  text_layer_set_background_color(time_layer_min, GColorClear);
  text_layer_set_text_alignment(time_layer_min, GTextAlignmentLeft);
  text_layer_set_font(time_layer_min, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ASAP_REGULAR_30)));

  connection_layer_icon = text_layer_create(GRect(5, 0, /* width */ frame.size.w / 2 - 5, 20 /* height */));
  text_layer_set_text_color(connection_layer_icon, GColorWhite);
  text_layer_set_background_color(connection_layer_icon, GColorClear);
  text_layer_set_text_alignment(connection_layer_icon, GTextAlignmentLeft);
  text_layer_set_font(connection_layer_icon, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_FONTELLO_20)));
  handle_bluetooth(bluetooth_connection_service_peek());

  battery_layer_icon = text_layer_create(GRect(frame.size.w / 2, 0, /* width */ frame.size.w / 2 - 30, 20 /* height */));
  text_layer_set_text_color(battery_layer_icon, GColorWhite);
  text_layer_set_background_color(battery_layer_icon, GColorClear);
  text_layer_set_text_alignment(battery_layer_icon, GTextAlignmentRight);
  text_layer_set_font(battery_layer_icon, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_FONTELLO_20)));

  battery_layer_txt = text_layer_create(GRect(frame.size.w - 30, 3, 30, 20));
  text_layer_set_text_color(battery_layer_txt, GColorWhite);
  text_layer_set_background_color(battery_layer_txt, GColorClear);
  text_layer_set_text_alignment(battery_layer_txt, GTextAlignmentCenter);
  text_layer_set_font(battery_layer_txt, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ASAP_REGULAR_14)));

  // Ensures time is displayed immediately (will break if NULL tick event accessed).
  // (This is why it's a good idea to have a separate routine to do the update itself.)
  time_t now = time(NULL);
  struct tm *current_time = localtime(&now);
  handle_second_tick(current_time, SECOND_UNIT);

  tick_timer_service_subscribe(SECOND_UNIT, &handle_second_tick);
  battery_state_service_subscribe(&handle_battery);
  bluetooth_connection_service_subscribe(&handle_bluetooth);

  layer_add_child(root_layer, text_layer_get_layer(time_layer_hour));
  layer_add_child(root_layer, text_layer_get_layer(time_layer_min));
  layer_add_child(root_layer, text_layer_get_layer(connection_layer_icon));
  layer_add_child(root_layer, text_layer_get_layer(battery_layer_icon));
  layer_add_child(root_layer, text_layer_get_layer(battery_layer_txt));
}

static void do_deinit(void) {
  tick_timer_service_unsubscribe();
  battery_state_service_unsubscribe();
  bluetooth_connection_service_unsubscribe();
  text_layer_destroy(time_layer_hour);
  text_layer_destroy(time_layer_min);
  text_layer_destroy(connection_layer_icon);
  text_layer_destroy(battery_layer_icon);
  text_layer_destroy(battery_layer_txt);
  window_destroy(window);
}

// The main event/run loop for our app
int main(void) {
  do_init();
  app_event_loop();
  do_deinit();
}
