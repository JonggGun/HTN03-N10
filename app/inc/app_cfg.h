#ifndef APP_CFG_H
#define APP_CFG_H
#define SENSOR_PERIOD_MS 100u
#define FILTER_SIZE 10u
#define SENSOR_PRIORITY 3u
#define ANALYSIS_PRIORITY 2u
#define DISPLAY_PRIORITY 1u
#define SENSOR_STACK_WORDS 192u
#define ANALYSIS_STACK_WORDS 384u
#define DISPLAY_STACK_WORDS 384u
#define DEFAULT_T1 650u
#define DEFAULT_T2 850u
#define DEFAULT_T3 1100u
#define HYSTERESIS_ADC 50u
#define OLED_ADDRESS 0x3Cu
/* Set 1 to exercise tasks/PC without MQ-135; do not report as sensor data. */
#define APP_DEMO_MODE 0
#endif
