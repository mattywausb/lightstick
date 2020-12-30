#ifndef __MAINSETTINGS_H__
#define __MAINSETTINGS_H__

#define TRACE_ON 
#ifdef TRACE_ON
#define TR_WARNING
#endif

//#define FEAT_ALLOW_SERIAL_INPUT

#define MAX_NUMBER_OF_PROGRAM_SLOTS 10

#define MAX_NUMBER_OF_PROGRAM_SEQUENCE_STEPS 30

/* Macro to declare Strings with reserved buffer */
#define DECLARE_PREALLOCATED_STRING(varname, alloc_size) String varname((const char*)nullptr); varname.reserve(alloc_size)

constexpr unsigned SMALL_STR = 64-1;
constexpr unsigned MED_STR = 256-1;
constexpr unsigned LARGE_STR = 512-1;
constexpr unsigned XLARGE_STR = 1024-1;

#endif __MAINSETTINGS_H__
