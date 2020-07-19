#include "lamp.h"
#include "Arduino.h"

Lamp::Lamp()
{
  m_color_hsv.h=0.0;
  m_color_hsv.s=0.0;
  m_color_hsv.v=0.0;
}

void Lamp::add_hue_angle(float angle) 
{
  m_color_hsv.h+=angle; 
  while(m_color_hsv.h>=360.) {m_color_hsv.h-=360.0;}; 
  while(m_color_hsv.h<0.0) {m_color_hsv.h+=360.0;};
 }

void Lamp::add_saturation(float saturation) 
{
  m_color_hsv.s+=saturation; 
  if(m_color_hsv.s>=1.0) m_color_hsv.s=1.0; 
  if(m_color_hsv.s<0.0) m_color_hsv.s=0.0;
}

void Lamp::add_value(float value) 
{
  m_color_hsv.v+=value; 
  if(m_color_hsv.v>=1.0) m_color_hsv.v=1.0; 
  if(m_color_hsv.v<0.0) m_color_hsv.v=0.0;
}

void Lamp::set_hsv(float h,float s,float v)
{
  m_color_hsv.h=h; 
  while(m_color_hsv.h>=360.0) {m_color_hsv.h-=360.0;}; 
  while(m_color_hsv.h<0.0) {m_color_hsv.h+=360.0;};

  m_color_hsv.s=s; 
  if(m_color_hsv.s>=1.0) m_color_hsv.s=1.0; 
  if(m_color_hsv.s<0.0) m_color_hsv.s=0.0;

  m_color_hsv.v=v; 
  if(m_color_hsv.v>=1.0) m_color_hsv.v=1.0; 
  if(m_color_hsv.v<0.0) m_color_hsv.v=0.0;
}


t_color_rgb Lamp::get_color_rgb(float maximum_value)
{
    float      hue_segment, p, q, t, hue_segment_remainder;
    long        hue_segment_int;
    t_color_rgb         out;

    if(m_color_hsv.s <= 0.0) {       // zero or negative saturation will result in white
        out.r = m_color_hsv.v;
        out.g = m_color_hsv.v;
        out.b = m_color_hsv.v;
        return out;
    }

    // Determine position 60 Angle segment of hue
    hue_segment = m_color_hsv.h;
    hue_segment /= 60.0;
    hue_segment_int = (long)hue_segment;
    hue_segment_remainder = hue_segment - hue_segment_int;
    
    p = m_color_hsv.v * (1.0 - m_color_hsv.s); // whitening factor
    
    #define hsv_get_q   m_color_hsv.v * (1.0 - (m_color_hsv.s * hue_segment_remainder))
    #define hsv_get_t   m_color_hsv.v * (1.0 - (m_color_hsv.s * (1.0 - hue_segment_remainder)))

    // Calculate rgb  participation depending on hure segment
    switch(hue_segment_int) {
    case 0:   // pure red to pure yellow 0-60
        out.r = m_color_hsv.v;
        out.g = hsv_get_t;
        out.b = p;
        break;
    case 1:   // yellow to pure green   60-120
        out.r = hsv_get_q;
        out.g = m_color_hsv.v;
        out.b = p;
        break;
    case 2:   // green to cyan   120-180
        out.r = p;
        out.g = m_color_hsv.v;
        out.b = hsv_get_t;
        break;

    case 3:  // cyan to blue  180-240
        out.r = p;
        out.g = hsv_get_q;
        out.b = m_color_hsv.v;
        break;
    case 4: // blue to magenta  240-300
        out.r = hsv_get_t;
        out.g = p;
        out.b = m_color_hsv.v;
        break;
    case 5: // magenta to red  300-360
    default:
        out.r = m_color_hsv.v;
        out.g = p;
        out.b = hsv_get_q;
        break;
    }
    return out;     
}


