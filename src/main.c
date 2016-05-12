#include <pebble.h>

static Window *s_window;
static TextLayer *s_hour_layer, *s_time_layer, *s_text_layer;
static GFont s_date_font, s_date_font2, s_date_font3;
static char s_num_buffer[8];
static BitmapLayer *s_background_layer;
static GBitmap *s_background_map;

static uint8_t mycolor[24] = { (uint8_t)0b11001100 //ARGB8
                                      ,(uint8_t)0b11011110
                                      ,(uint8_t)0b11110000
                                      ,(uint8_t)0b11110110
                                      ,(uint8_t)0b11111100
                                      ,(uint8_t)0b11011011
                                      ,(uint8_t)0b11101101
                                      ,(uint8_t)0b11111000
                                      ,(uint8_t)0b11100011
                                      ,(uint8_t)0b11101010
                                      ,(uint8_t)0b11100111
                                      ,(uint8_t)0b11111111
                                      ,(uint8_t)0b11001100 
                                      ,(uint8_t)0b11011110
                                      ,(uint8_t)0b11110000
                                      ,(uint8_t)0b11110110
                                      ,(uint8_t)0b11111100
                                      ,(uint8_t)0b11011011
                                      ,(uint8_t)0b11101101
                                      ,(uint8_t)0b11111000
                                      ,(uint8_t)0b11100011
                                      ,(uint8_t)0b11101010
                                      ,(uint8_t)0b11100111
                                      ,(uint8_t)0b11111111
                             };



static void update_time() {
  // Structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  
  // Buffer Day
  static char s_day_buffer[6];
  strftime(s_day_buffer, sizeof(s_day_buffer), "%l", tick_time);
  static char s_time_buffer[8];
  strftime(s_time_buffer, sizeof(s_time_buffer), clock_is_24h_style() ?
                                          "%H:%M" : "%I:%M", tick_time);
  int hour = tick_time->tm_hour;
  
  GColor hour_color = ((GColor)mycolor[hour]);
  
  text_layer_set_text_color(s_time_layer, hour_color);
  text_layer_set_text_color(s_text_layer, hour_color);
  text_layer_set_text_color(s_hour_layer, hour_color);
           
  // Show the hour
  text_layer_set_text(s_hour_layer, s_day_buffer);
  // Show the time
  text_layer_set_text(s_time_layer, s_time_buffer);
  
}


static void handle_second_tick(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(window_get_root_layer(s_window));
  
  update_time();
}

static void update_timer_callback(void *data) {
  layer_set_hidden((Layer *)s_time_layer, true);
  layer_set_hidden((Layer *)s_hour_layer, false);
  layer_set_hidden((Layer *)s_text_layer, false);
}

static void tap_handler(AccelAxisType axis, int32_t direction) {
  
  layer_set_hidden((Layer *)s_time_layer, false);
  layer_set_hidden((Layer *)s_hour_layer, true);
  layer_set_hidden((Layer *)s_text_layer, true);
  
  AppTimer *updateTimer = app_timer_register(3000, (AppTimerCallback) update_timer_callback, NULL);
	}

static void window_load(Window *s_window) {
  
  Layer *window_layer = window_get_root_layer(s_window);
  GRect bounds = layer_get_bounds(window_layer);
  
  s_date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_font80));
  s_date_font2 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_font32));
  s_date_font3 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_font48));
  
  
  //Background
  s_background_map = gbitmap_create_with_resource(RESOURCE_ID_hexa);
  s_background_layer = bitmap_layer_create(bounds);
  bitmap_layer_set_bitmap(s_background_layer, s_background_map);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_background_layer));
  
  
  //Hour
  s_hour_layer = text_layer_create PBL_IF_ROUND_ELSE((GRect(63, 40, 90, 130)), (GRect(39,49,70,70)));
  text_layer_set_background_color(s_hour_layer, GColorClear);
  PBL_IF_ROUND_ELSE((text_layer_set_font(s_hour_layer, s_date_font)), (text_layer_set_font(s_hour_layer, s_date_font2)));
  text_layer_set_text_alignment(s_hour_layer, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(s_window), text_layer_get_layer(s_hour_layer));
  layer_set_hidden((Layer *)s_hour_layer, false);
  
  //Text
  s_text_layer = text_layer_create(GRect(43, 114, 100, 100));
  text_layer_set_background_color(s_text_layer, GColorClear);
  text_layer_set_font(s_text_layer, s_date_font2);
  text_layer_set_text(s_text_layer, "PA IR");
  text_layer_set_text_alignment(s_text_layer, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(s_window), text_layer_get_layer(s_text_layer));
  layer_set_hidden((Layer *)s_text_layer, false);

  //Time
  s_time_layer = text_layer_create(GRect(19, 55, 140, 70));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_font(s_time_layer, s_date_font3);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(s_window), text_layer_get_layer(s_time_layer));
  layer_set_hidden((Layer *)s_time_layer, true);
  
}


static void window_unload(Window *window) {
  
  text_layer_destroy(s_hour_layer);
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_text_layer);
  bitmap_layer_destroy(s_background_layer);
  gbitmap_destroy(s_background_map);
  fonts_unload_custom_font(s_date_font);
  fonts_unload_custom_font(s_date_font2);
  fonts_unload_custom_font(s_date_font3);
  window_destroy(s_window);
  
}

static void init() {
  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(s_window, true);

  s_num_buffer[0] = '\0';


  accel_tap_service_subscribe(tap_handler);
  tick_timer_service_subscribe(SECOND_UNIT, handle_second_tick);
  
  
}

static void deinit() {


  tick_timer_service_unsubscribe();
  accel_tap_service_unsubscribe();
}

int main() {
  init();
  app_event_loop();
  deinit();
}


  
  