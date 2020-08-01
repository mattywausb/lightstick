#ifndef lamp_h
#define lamp_h
#include "Arduino.h"
#include "mainSettings.h"

#define HUE_RED 0.0
#define SAT_ROSE 0.7

#define HUE_ORANGE 15.0
#define HUE_LORANGE 25.0

#define HUE_YELLOW 60.0
#define SAT_LYELLOW 0.8

#define HUE_LEMON 95.0

#define HUE_GREEN 120.0
#define SAT_LGREEN 0.8

#define HUE_CYAN 180.0
#define SAT_LCYAN 0.7

#define HUE_SKYBLUE 220.0

#define HUE_BLUE 240.0
#define HUE_LBLUE 230.0
#define SAT_LBLUE 0.8

#define HUE_MAGENTA 300.0

#define HUE_PINK 340.0
#define SAT_LPINK 0.85

#define HUE_PURPLE 250.0
#define SAT_LPURPLE 0.8

/* pattern list
1:PULSE Steps_until_next_color,followup_tick_count
2:WHIPE: over black(true/false)
3:DOUBLE_ORBIT: Steps_until_next_color
4:COLOR_ORBIT: Steps_until_next_color,Palette_increment
5:RAINBOW: neighbour_distance, Increment
 */


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
