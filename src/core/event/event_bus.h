// event_bus.h
#ifndef EVENT_BUS_H
#define EVENT_BUS_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// Enum for event types
typedef enum EventType {
    EVENT_TYPE_SIGNAL,
    EVENT_TYPE_UPDATE,
    EVENT_TYPE_CUSTOM,
} EventType;

// Define the structure of an event
typedef struct Event {
    EventType   type;  // Type of the event
    void       *data;  // Pointer to the event data
} Event;

// An event listener is a function that takes an Event pointer as an argument
typedef void (*EventListener)(const Event *e);

// A Channel holds a dynamic array of listeners
typedef struct Channel {
    char           *name;
    EventListener  *listeners;      // heap buffer of subscribers
    size_t          listener_count; // number in use
    size_t          listener_cap;   // allocated capacity
} Channel;

// The EventBus is a dynamic array of Channels
typedef struct EventBus {
    Channel *channels;      // heap buffer of channels
    size_t   channel_count; // number in use
    size_t   channel_cap;   // allocated capacity
} EventBus;

/* ─── Channel helpers ─────────────────────────────────────────────────────── */

// Initialize a Channel
void channel_init(Channel *c, const char *name);

// Free all resources held by a Channel
void channel_destroy(Channel *c);

// Subscribe a listener function to a Channel
void channel_subscribe(Channel *c, EventListener fn);

// Unsubscribe a listener function from a Channel
void channel_unsubscribe(Channel *c, EventListener fn);

/* ─── EventBus helpers ──────────────────────────────────────────────────── */

// Initialize an EventBus
void bus_init(EventBus *bus);

// Destroy an EventBus and all its Channels
void bus_destroy(EventBus *bus);

// Get existing channel or create a new one
Channel *bus_get_or_create(EventBus *bus, const char *name);

// Subscribe a listener to a named channel
void bus_subscribe(EventBus *bus,
                   const char *channel_name,
                   EventListener fn);

// Emit an event on a named channel
void bus_emit(EventBus *bus,
              const char *channel_name,
              const Event *e);

#ifdef __cplusplus
}
#endif

#endif // EVENT_BUS_H