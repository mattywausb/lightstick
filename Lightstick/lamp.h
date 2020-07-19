#ifndef lamp_h
#define lamp_h
#include "Arduino.h"
#include "mainSettings.h"

typedef struct {
    float r;       // a fraction between 0 and 1
    float g;       // a fraction between 0 and 1
    float b;       // a fraction between 0 and 1
} t_color_rgb;

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
        void add_saturation(float saturation)  ;
        void add_value(float value);

        t_color_rgb get_color_rgb(float maximum_value);

 protected:
        t_color_hsv m_color_hsv;
        
};
        
#endif
