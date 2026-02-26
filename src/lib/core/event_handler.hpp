/**
 * @file event_handler.hpp
 * @author Sean McGinty (newfolderlocation@gmail.com)
 * @brief Event handler class to keep track of events and their handlers.
 * @version 1.1
 * @date 2021-12-16
 *
 * Tracks keys and callback functions for events.
 *
 * @code{cpp}
 * addListener(KEY_LEFT, myFunction);        add a listener to the LEFT key that calls
 *                                           myFunctionName when pressed
 *
 * addListener2(KEY_LEFT, myFunction);       there are two different key enums, Keys1
 *                                           and Keys2, so you use the same function
 *                                           name with a 2 on the end for Keys2
 *
 * addListener2(KEY_LEFT, myFunction, true); create a listener that is holdable (you
 *                                           don't need to unpress the key for it to call
 *                                           the linked function in the next frame)
 *                                           this is useful for controls like Left/Right
 *
 * bool game_running = true;                 starts a game loop
 * while (game_running) {
 *     checkEvents();                        check for events, including key presses
 * }
 * @endcode
 */

#pragma once
#include "../../calc.hpp"

#ifndef PC
#include <sdk/os/input.h>
#include <string.h> // for memset
#endif

uint32_t ev_key1, ev_key2;
uint32_t ev_old1 = 0;
uint32_t ev_old2 = 0;

// Keys1 - 16 keys in enum
Keys1 listeners[16] = {KEY_CLEAR, KEY_CLEAR, KEY_CLEAR, KEY_CLEAR, KEY_CLEAR, KEY_CLEAR, KEY_CLEAR, KEY_CLEAR, KEY_CLEAR, KEY_CLEAR, KEY_CLEAR, KEY_CLEAR, KEY_CLEAR, KEY_CLEAR, KEY_CLEAR, KEY_CLEAR};
void (*func_ptr[16])() = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
bool hold_keys[16] = {false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false};
// Track if the listener's key is currently pressed
bool listener_pressed[16] = {false};
uint8_t listener_count = 0;

void addListener(Keys1 key, void (*func)(), bool hold = false) {
   listeners[listener_count] = key;
   func_ptr[listener_count] = func;
   hold_keys[listener_count] = hold;
   listener_pressed[listener_count] = false;
   listener_count++;
}

void removeListener(Keys1 key) {
   for (uint8_t i = 0; i < listener_count; i++) {
      if (listeners[i] == key) {
         // Shift all elements down
         listener_count--;
         for (uint8_t j = i; j < listener_count; j++) {
            listeners[j] = listeners[j + 1];
            func_ptr[j] = func_ptr[j + 1];
            listener_pressed[j] = listener_pressed[j + 1];
         }
      }
   }
}

// Keys2 - 18 keys in enum
// Note: On Calc v3, Keys2 and Keys1 are same type (uint32_t keycode), so this separation is legacy but kept for API compatibility.
Keys2 listeners2[18] = {KEY_KEYBOARD, KEY_KEYBOARD, KEY_KEYBOARD, KEY_KEYBOARD, KEY_KEYBOARD, KEY_KEYBOARD, KEY_KEYBOARD, KEY_KEYBOARD, KEY_KEYBOARD, KEY_KEYBOARD, KEY_KEYBOARD, KEY_KEYBOARD, KEY_KEYBOARD, KEY_KEYBOARD, KEY_KEYBOARD, KEY_KEYBOARD, KEY_KEYBOARD, KEY_KEYBOARD};
void (*func_ptr2[18])() = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
bool hold_keys2[18] = {false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false};
bool listener_pressed2[18] = {false};
uint8_t listener_count2 = 0;

void addListener2(Keys2 key, void (*func)(), bool hold = false) {
   listeners2[listener_count2] = key;
   func_ptr2[listener_count2] = func;
   hold_keys2[listener_count2] = hold;
   listener_pressed2[listener_count2] = false;
   listener_count2++;
}

void removeListener2(Keys2 key) {
   for (uint8_t i = 0; i < listener_count2; i++) {
      if (listeners2[i] == key) {
         // Shift all elements down
         listener_count2--;
         for (uint8_t j = i; j < listener_count2; j++) {
            listeners2[j] = listeners2[j + 1];
            func_ptr2[j] = func_ptr2[j + 1];
            listener_pressed2[j] = listener_pressed2[j + 1];
         }
      }
   }
}

void removeAllListeners() {
   listener_count = 0;
   listener_count2 = 0;
}

#ifndef PC
// We track per-listener state instead of global key state to handle sparse keycodes
static bool old_listener_pressed[16];
static bool old_listener_pressed2[18];
#endif

void checkEvents() {
#ifdef PC
   //Read the keyboard
   ev_old1 = ev_key1;
   ev_old2 = ev_key2;
   getKey(&ev_key1, &ev_key2);


   bool key1_pressed = false;
   for (uint8_t i = 0; i < listener_count; i++) {
      if (testKey(ev_key1,ev_key2,listeners[i])) {
         // check if the key is holdable or is different from previous if not holdable
         if (ev_old1 != ev_key1 || hold_keys[i]) {
            // call the function
            (*func_ptr[i])();
            key1_pressed = true;
            break; // no need to check the rest of the keys
         }
      }
      if (!key1_pressed) { // check if key already found
         for (uint8_t i = 0; i < listener_count2; i++) {
            // Check if the key is pressed
            if (testKey(ev_key1,ev_key2,listeners2[i])) {
               // check if the key is holdable or is different from previous if not holdable
               if (ev_old2 != ev_key2 || hold_keys2[i]) {
                  // call the function
                  (*func_ptr2[i])();
                  break; // no need to check the rest of the keys
               }
            }
         }
      }
   }
#else
   // Calculator V3 implementation
   // Backup old states
   memcpy(old_listener_pressed, listener_pressed, sizeof(listener_pressed));
   memcpy(old_listener_pressed2, listener_pressed2, sizeof(listener_pressed2));

   struct Input_Event event;
   // Use 0 for non-blocking polling
   while(1) {
       GetInput(&event, 0, 0x10);
       if (event.type == EVENT_NONE) break;

       if (event.type == EVENT_KEY) {
           uint32_t k = event.data.key.keyCode;
           bool is_pressed = (event.data.key.direction == KEY_PRESSED || event.data.key.direction == KEY_HELD);
           bool is_released = (event.data.key.direction == KEY_RELEASED);

           // Update listener states directly
           if (is_pressed || is_released) {
               for (uint8_t i = 0; i < listener_count; i++) {
                   if (listeners[i] == k) {
                       listener_pressed[i] = is_pressed;
                   }
               }
               for (uint8_t i = 0; i < listener_count2; i++) {
                   if (listeners2[i] == k) {
                       listener_pressed2[i] = is_pressed;
                   }
               }
           }
       }
   }

   bool key1_pressed = false;
   for (uint8_t i = 0; i < listener_count; i++) {
      if (listener_pressed[i]) {
          // Trigger if it wasn't pressed before (edge) or if hold is enabled
          if (!old_listener_pressed[i] || hold_keys[i]) {
             (*func_ptr[i])();
             key1_pressed = true;
             break;
          }
      }
   }

   if (!key1_pressed) {
      for (uint8_t i = 0; i < listener_count2; i++) {
         if (listener_pressed2[i]) {
             if (!old_listener_pressed2[i] || hold_keys2[i]) {
                 (*func_ptr2[i])();
                 break;
             }
         }
      }
   }
#endif
}
