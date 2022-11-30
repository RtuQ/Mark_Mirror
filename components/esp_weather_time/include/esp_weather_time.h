#ifndef __ESP_WEATHER_TIME_H
#define __ESP_WEATHER_TIME_H

#include "esp_event_base.h"

ESP_EVENT_DECLARE_BASE(NTP_TIME_EVENT);


void esp_internet_time_init(void);


void esp_start_sync_weather(void);


#endif