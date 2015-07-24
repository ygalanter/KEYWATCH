#include <pebble.h>
#include <ctype.h>
#include "effect_layer.h"
  
static Window *s_window;
static GBitmap *s_res_image_bg;
static GFont s_res_lucida_concole_regular_17;
static BitmapLayer *s_bitmaplayer_bg;
static TextLayer *s_textlayer_dow;
static TextLayer *s_textlayer_date;
static TextLayer *s_textlayer_time;
static TextLayer *s_textlayer_ampm;
static InverterLayer *s_inverterlayer_battery;

char buffer[] = "1234";
char buffer_date[] = "SEP21";
char buffer_dow[] = "WED";
char buffer_ampm[] = "AM<>";

void tick_handler(struct tm *tick_time, TimeUnits units_changed)
{
   
    if (!clock_is_24h_style()) {
    
        if( tick_time->tm_hour > 11 ) {   // YG Jun-25-2014: 0..11 - am 12..23 - pm
            strcpy(buffer_ampm, "PM<>" );
            if( tick_time->tm_hour > 12 ) tick_time->tm_hour -= 12;
        } else {
            strcpy(buffer_ampm, "AM<>" );
            if( tick_time->tm_hour == 0 ) tick_time->tm_hour = 12;
        }        
      
        text_layer_set_text(s_textlayer_ampm, buffer_ampm);
    }
 
    strftime(buffer, sizeof("0000"), "%H%M", tick_time);
    text_layer_set_text(s_textlayer_time, buffer);
  
    strftime(buffer_date, sizeof("SEP31"), "%b%d", tick_time);
    buffer_date[1] = toupper((unsigned char)buffer_date[1]);
    buffer_date[2] = toupper((unsigned char)buffer_date[2]);
    text_layer_set_text(s_textlayer_date, buffer_date);
  
    strftime(buffer_dow, sizeof("SAT"), "%a", tick_time);
    buffer_dow[1] = toupper((unsigned char)buffer_dow[1]);
    buffer_dow[2] = toupper((unsigned char)buffer_dow[2]);
    text_layer_set_text(s_textlayer_dow, buffer_dow);
  
}
  

// battery update
static void battery_handler(BatteryChargeState state) {
   layer_set_frame( inverter_layer_get_layer(s_inverterlayer_battery), GRect(49, 142, 89 * state.charge_percent / 100, 19));
}

static void handle_window_load(Window *window) {
  
  s_res_image_bg = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BG);
  s_res_lucida_concole_regular_17 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_LUCIDA_CONCOLE_REGULAR_17));
  
  // s_bitmaplayer_bg
  s_bitmaplayer_bg = bitmap_layer_create(GRect(0, 0, 144, 168));
  bitmap_layer_set_bitmap(s_bitmaplayer_bg, s_res_image_bg);
  layer_add_child(window_get_root_layer(s_window), bitmap_layer_get_layer(s_bitmaplayer_bg));
  
  // s_textlayer_dow
  s_textlayer_dow = text_layer_create(GRect(10, 117, 114, 28));
  text_layer_set_background_color(s_textlayer_dow, GColorClear);
  text_layer_set_text_color(s_textlayer_dow, GColorWhite);
  text_layer_set_text(s_textlayer_dow, "WED");
  text_layer_set_font(s_textlayer_dow, s_res_lucida_concole_regular_17);
  layer_add_child(window_get_root_layer(s_window), text_layer_get_layer(s_textlayer_dow));
  
  // s_textlayer_date
  s_textlayer_date = text_layer_create(GRect(9, 72, 137, 20));
  text_layer_set_background_color(s_textlayer_date, GColorClear);
  text_layer_set_text_color(s_textlayer_date, GColorWhite);
  text_layer_set_text(s_textlayer_date, "NOV23");
  text_layer_set_font(s_textlayer_date, s_res_lucida_concole_regular_17);
  layer_add_child(window_get_root_layer(s_window), text_layer_get_layer(s_textlayer_date));
  
  // s_textlayer_time
  s_textlayer_time = text_layer_create(GRect(33, 26, 112, 22));
  text_layer_set_background_color(s_textlayer_time, GColorClear);
  text_layer_set_text_color(s_textlayer_time, GColorWhite);
  text_layer_set_text(s_textlayer_time, "1234");
  text_layer_set_font(s_textlayer_time, s_res_lucida_concole_regular_17);
  layer_add_child(window_get_root_layer(s_window), text_layer_get_layer(s_textlayer_time));
  
  // s_textlayer_ampm
  s_textlayer_ampm = text_layer_create(GRect(54, 48, 116, 20));
  text_layer_set_background_color(s_textlayer_ampm, GColorClear);
  text_layer_set_text_color(s_textlayer_ampm, GColorWhite);
  text_layer_set_text(s_textlayer_ampm, "QWER");
  text_layer_set_font(s_textlayer_ampm, s_res_lucida_concole_regular_17);
  layer_add_child(window_get_root_layer(s_window), text_layer_get_layer(s_textlayer_ampm));
  
  // s_inverterlayer_battery
  s_inverterlayer_battery = inverter_layer_create(GRect(49, 142, 89, 19));
  layer_add_child(window_get_root_layer(s_window), inverter_layer_get_layer( s_inverterlayer_battery));
  
  battery_handler(battery_state_service_peek());
}


static void handle_window_unload(Window* window) {
  bitmap_layer_destroy(s_bitmaplayer_bg);
  text_layer_destroy(s_textlayer_dow);
  text_layer_destroy(s_textlayer_date);
  text_layer_destroy(s_textlayer_time);
  text_layer_destroy(s_textlayer_ampm);
  inverter_layer_destroy(s_inverterlayer_battery);
  gbitmap_destroy(s_res_image_bg);
  fonts_unload_custom_font(s_res_lucida_concole_regular_17);
}

void init(void) {
  s_window = window_create();
  window_set_background_color(s_window, GColorBlack);
  
  window_set_window_handlers(s_window,  (WindowHandlers) {
    .load = handle_window_load,
    .unload = handle_window_unload
  });
 
  window_stack_push(s_window, true);
  
  // initializing battery service
  battery_state_service_subscribe(battery_handler);
  
  // initializing timer
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
 
}

void deinit(void) {
  tick_timer_service_unsubscribe(); 
  battery_state_service_unsubscribe();
  window_destroy(s_window);
}


int main() {
  init();
  app_event_loop();
  deinit();
}

