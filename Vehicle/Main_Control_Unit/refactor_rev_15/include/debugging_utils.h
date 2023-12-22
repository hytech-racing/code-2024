/**
 * @file debugging_utils.h
 * @author Eric Galluzzi
 * @brief reduce overhead of serial when not needed
 * @version 0.1
 * @date 2023-12-21
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef DEBUG
  #define Debug_begin(x)
  #define Debug_print(x)
  #define Debug_println(x)
  #define Debug_printf(x)
#else
  #define Debug_begin(x) Serial.begin(x)
  #define Debug_print(x)  Serial.print(x)
  #define Debug_println(x) Serial.println(x)
  #define Debug_printf(x,y) Serial.printf(x, y)
#endif