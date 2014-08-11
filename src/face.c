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
  "heure\n",
  "heures\n",
  "heures\n",
  "heures\n",
  "heures\n",
  "heures\n",
  "heures\n",
  "heures\n",
  "heures\n",
  "heures\n",
  "heures\n",
  "",
  "heure\n",
  "heures\n",
  "heures\n",
  "heures\n",
  "heures\n",
  "heures\n",
  "heures\n",
  "heures\n",
  "heures\n",
  "heures\n",
  "heures\n",
  ""
};

static char minutes[60][18] = {
  "",                  // 0
  "",                  // 1
  "",                  // 2
  "cinq",              // 3
  "cinq",              // 4
  "cinq",              // 5
  "cinq",              // 6
  "cinq",              // 7
  "dix",               // 8
  "dix",               // 9
  "dix",               // 10
  "dix",               // 11
  "dix",               // 12
  "et quart",          // 13
  "et quart",          // 14
  "et quart",          // 15
  "et quart",          // 16
  "et quart",          // 17
  "vingt",             // 18
  "vingt",             // 19
  "vingt",             // 20
  "vingt",             // 21
  "vingt",             // 22
  "vingt cinq",        // 23
  "vingt cinq",        // 24
  "vingt cinq",        // 25
  "vingt cinq",        // 26
  "vingt cinq",        // 27
  "et demie",          // 28
  "et demie",          // 29
  "et demie",          // 30
  "et demie",          // 32
  "moins\nvingt cinq", // 33
  "moins\nvingt cinq", // 34
  "moins\nvingt cinq", // 35
  "moins\nvingt cinq", // 36
  "moins\nvingt cinq", // 37
  "moins\nvingt",      // 38
  "moins\nvingt",      // 39
  "moins\nvingt",      // 40
  "moins\nvingt",      // 41
  "moins\nvingt",      // 42
  "moins\nle quart",   // 43
  "moins\nle quart",   // 44
  "moins\nle quart",   // 45
  "moins\nle quart",   // 46
  "moins\nle quart",   // 47
  "moins\ndix",        // 48
  "moins\ndix",        // 49
  "moins\ndix",        // 50
  "moins\ndix",        // 51
  "moins\ndix",        // 52
  "moins\ncinq",       // 53
  "moins\ncinq",       // 54
  "moins\ncinq",       // 55
  "moins\ncinq",       // 56
  "moins\ncinq",       // 57
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
  time_layer_hour = text_layer_create(GRect(0, 25, frame.size.w /* width */, frame.size.h - 25/* height */));
  text_layer_set_text_color(time_layer_hour, GColorWhite);
  text_layer_set_background_color(time_layer_hour, GColorClear);
  text_layer_set_text_alignment(time_layer_hour, GTextAlignmentLeft);
  text_layer_set_font(time_layer_hour, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ASAP_BOLD_30)));

  time_layer_min = text_layer_create(GRect(0, 55, frame.size.w /* width */, frame.size.h - 55/* height */));
  text_layer_set_text_color(time_layer_min, GColorWhite);
  text_layer_set_background_color(time_layer_min, GColorClear);
  text_layer_set_text_alignment(time_layer_min, GTextAlignmentLeft);
  text_layer_set_font(time_layer_min, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ASAP_REGULAR_30)));

  connection_layer_icon = text_layer_create(GRect(5, 0, /* width */ frame.size.w / 2 - 5, 25 /* height */));
  text_layer_set_text_color(connection_layer_icon, GColorWhite);
  text_layer_set_background_color(connection_layer_icon, GColorClear);
  text_layer_set_text_alignment(connection_layer_icon, GTextAlignmentLeft);
  text_layer_set_font(connection_layer_icon, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_FONTELLO_20)));
  handle_bluetooth(bluetooth_connection_service_peek());

  battery_layer_icon = text_layer_create(GRect(frame.size.w / 2, 0, /* width */ frame.size.w / 2 - 30, 25 /* height */));
  text_layer_set_text_color(battery_layer_icon, GColorWhite);
  text_layer_set_background_color(battery_layer_icon, GColorClear);
  text_layer_set_text_alignment(battery_layer_icon, GTextAlignmentRight);
  text_layer_set_font(battery_layer_icon, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_FONTELLO_20)));

  battery_layer_txt = text_layer_create(GRect(frame.size.w - 30, 3, 30, 25));
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
  bluetooth_connected = bluetooth_connection_service_peek(); 
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
