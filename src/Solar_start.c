#include <pebble.h>
#include <time.h>
  
// Dictionary between JS & C Language
#define KEY_CURPOWER 0
#define KEY_EDAY  1
#define KEY_SOLARVALUE 2
  
//Create a pointer to a variable of Window type:
static Window *s_main_window;
//Create a pointer to a variable of text for a text layer
static TextLayer *text_layer;

// date buffer for the selected date
static char date_buffer[16];
int day_var = 0;

 static void WhatDate(int day_setoff) {
  // Define seconds for one day
  int ONE_DAY = 24 * 60 * 60;
  // get the epoch time with option to adjustment
  time_t temp = time(NULL) + (day_setoff * ONE_DAY); 
  struct tm *tick_time = localtime(&temp);
  strftime(date_buffer, sizeof(date_buffer), "%Y-%m-%d", tick_time);
 }

static void CreateTextLayer(){
     // define the root layer as window_layer to add childs
  Layer *window_layer = window_get_root_layer(s_main_window);
  GRect bounds = layer_get_bounds(window_layer);

  // Create TextLayer
  text_layer = text_layer_create((GRect) { .origin = { 0, 0 }, .size = { bounds.size.w, 20 } });
  text_layer_set_text(text_layer, "GoodWe");
  text_layer_set_text_alignment(text_layer, GTextAlignmentLeft);
   // Add to Window, which is the root_layer
  layer_add_child(window_layer, text_layer_get_layer(text_layer));
}

 static void ShowDate() {
  // Show the date
  text_layer_set_text(text_layer, date_buffer);
 }


static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "Select");
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  day_var--;
  WhatDate(day_var);
  //  text_layer_set_text(text_layer, "Up");
  // Update textfield with new date
  text_layer_set_text(text_layer, date_buffer);
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  day_var++;
  WhatDate(day_var);
  text_layer_set_text(text_layer, "Down");
  // Update textfield with new date
  text_layer_set_text(text_layer, date_buffer);
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

// Declare handler for the load events to manage child Layers:
static void window_load(Window *s_main_window) {
 CreateTextLayer();
 ShowDate();
}

// Declare handler for the unload events to manage child Layers:
static void window_unload(Window *s_main_window) {
  text_layer_destroy(text_layer);
}


static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  // Store incoming information
  static char buffer_curpower[8];
  static char buffer_eday[8];
  static char buffer_solarvalue[512];
  static char weather_layer_buffer[32];
  
  // Read first item
  Tuple *t = dict_read_first(iterator);

  // For all items
  while(t != NULL) {
    // Which key was received?
    switch(t->key) {
    case KEY_CURPOWER:
      snprintf(buffer_curpower, sizeof(buffer_curpower), "%s", t->value->cstring);
      break;
    case KEY_EDAY :
      snprintf(buffer_eday, sizeof(buffer_eday), "%s", t->value->cstring);
      break;
    case KEY_SOLARVALUE :
      snprintf(buffer_solarvalue, sizeof(buffer_solarvalue), "%s", t->value->cstring);
      APP_LOG(APP_LOG_LEVEL_INFO , "Received buffer_solarvalue: %s", buffer_solarvalue);
      break;  
    default:
      APP_LOG(APP_LOG_LEVEL_ERROR, "Key %s not recognized!", t->value->cstring);
      break;
    }

    // Look for next item
    t = dict_read_next(iterator);
  }
  
  // Assemble full string and display
  snprintf(weather_layer_buffer, sizeof(weather_layer_buffer), "%s, %s", buffer_curpower, buffer_eday);
  text_layer_set_text(text_layer, weather_layer_buffer);
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}




static void init(void) {
 //  Create the window itself in your app initialization:
  s_main_window = window_create();

// When Pebble color, use the RGB color, else Black
#ifdef PBL_COLOR 
  window_set_background_color(s_main_window, GColorFromRGB(255, 0, 0) );
#else
  window_set_background_color(s_main_window, GColorBlack);
#endif
  
  
  window_set_click_config_provider(s_main_window, click_config_provider);
  window_set_window_handlers(s_main_window, (WindowHandlers) {
//	Register these handlers with the window iself:
    .load = window_load,
    .unload = window_unload,
  });

  // Push the window into view on top of the stack. At this point the handlers assigned to load and unload are called:
  const bool animated = true;
  window_stack_push(s_main_window, animated);

  // create the textlayer at top of screen position
  CreateTextLayer();

  
   // Register callbacks for AppMessage
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  
  // Open AppMessage
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());

}

// Make sure to destroy the window in your app's deinitialization:
static void deinit(void) {
  window_destroy(s_main_window);
}


int main(void) {
  // do set up: subscribe to event services and implement event handlers
  init();
  // Enter the main event loop. This will block until the app is ready to exit.
  app_event_loop();
  // do clean up here 
  deinit();
}