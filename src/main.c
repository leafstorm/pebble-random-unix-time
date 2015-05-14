#include <pebble.h>
    
#define MINUTES

#define TIME_FORMAT_24 "%H:%M"
#define TIME_FORMAT_12 "%I:%M"
#define TIME_SAMPLE "00:00"
#define DATE_FORMAT "%a, %b %d, %Y"
#define DATE_SAMPLE "Wed, Jun 23, 1993"

static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_date_layer;

static void main_window_load (Window *window) {
    s_time_layer = text_layer_create(GRect(0, 40, 144, 50));
    text_layer_set_background_color(s_time_layer, GColorClear);
    text_layer_set_text_color(s_time_layer, GColorBlack);
    text_layer_set_text(s_time_layer, "00:00");
    text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
    text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

    s_date_layer = text_layer_create(GRect(0, 90, 144, 30));
    text_layer_set_background_color(s_date_layer, GColorClear);
    text_layer_set_text_color(s_date_layer, GColorBlack);
    text_layer_set_text(s_date_layer, "Jun 23, 1993");
    text_layer_set_font(s_date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
    text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);

    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));
}

static void main_window_unload (Window *window) {
    text_layer_destroy(s_time_layer);
    text_layer_destroy(s_date_layer);
}

static void update_time (time_t unix_time) {
    #ifdef MINUTES
        /* only changes with the minute */
        int seconds = unix_time % 60;
        unix_time = unix_time - seconds;
    #endif

    srand(unix_time);

    time_t fake_unix_time = rand();
    struct tm *fake_time = localtime(&fake_unix_time);

    static char time_buffer[] = "00:00";

    if (clock_is_24h_style() == true) {
        strftime(time_buffer, sizeof(TIME_SAMPLE), TIME_FORMAT_24, fake_time);
    } else {
        strftime(time_buffer, sizeof(TIME_SAMPLE), TIME_FORMAT_12, fake_time);
    }

    static char date_buffer[] = DATE_SAMPLE;
    strftime(date_buffer, sizeof(DATE_SAMPLE), DATE_FORMAT, fake_time);

    text_layer_set_text(s_time_layer, time_buffer);
    text_layer_set_text(s_date_layer, date_buffer);
}


static void tick_handler (struct tm *tick_time, TimeUnits units_changed) {
    time_t unix_time = time(NULL);
    update_time(unix_time);
}


static void init() {
    s_main_window = window_create();

    window_set_window_handlers(s_main_window, (WindowHandlers) {
        .load = main_window_load,
        .unload = main_window_unload
    });

    window_stack_push(s_main_window, true);
    #ifdef MINUTES
        tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
    #else
        tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
    #endif

    time_t start_unix_time = time(NULL);
    update_time(start_unix_time);
}

static void deinit() {
    window_destroy(s_main_window);
}

int main(void) {
    init();
    app_event_loop();
    deinit();
}
