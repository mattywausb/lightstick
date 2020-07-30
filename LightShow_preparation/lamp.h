#ifndef lamp_h
#define lamp_h
#include "Arduino.h"
#include "mainSettings.h"

#define HUE_RED 0.0
#define HUE_YELLOW 60.0
#define HUE_GREEN 120.0
#define HUE_CYAN 180.0
#define HUE_BLUE 240.0
#define HUE_PINK 300.0

#define HUE_PURPLE 250.0
#define HUE_ORANGE 25.0
#define HUE_LEMON 95.0

typedef struct {
    int r;       // 0-255
    int g;       // 0-255
    int b;       // 0-255
} t_color_rgb_int;

typedef struct {
    float h;       // angle in degrees
    float s;       // a fraction between 0 and 1
    float v;       // a fraction between 0 and 1
} t_color_hsv;

class Lamp {
 public:
        Lamp(void);

        void set_hsv(float h,float s,float v);
        void add_hue_angle(float angle) ;
        void set_hue_angle(float angle) ;
        void add_saturation(float increment)  ;
        void set_saturation(float saturation)  ;
        float get_saturation(){return m_color_hsv.s;}  ;
        void multiply_value(float fact);
        void set_value(float value);
        void trace_hsv();
        void trace_rgb(t_color_rgb_int color_rgb );

        t_color_rgb_int get_color_rgb(float maximum_value);

 protected:
        t_color_hsv m_color_hsv;
        
};
        
#endif
