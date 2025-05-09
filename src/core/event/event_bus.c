/*
This event bus system is designed to facilitate communication between different
components of a game engine. It allows for the decoupling of event producers and
consumers, enabling a more modular architecture. The event bus can handle
different types of events,
*/
#include "event_bus.h"
#include <string.h>

// ––– helpers for Channel –––
void channel_init(Channel *c, const char *name) {
    c->name = strdup(name);
    c->listeners = NULL;
    c->listener_count = 0;
    c->listener_cap = 0;
}

void channel_destroy(Channel *c) {
    free(c->name);
    free(c->listeners);
    // zero fields just in case
    c->name = NULL;
    c->listeners = NULL;
    c->listener_count = c->listener_cap = 0;
}

void channel_subscribe(Channel *c, EventListener fn) {
    if (c->listener_count == c->listener_cap) {
        size_t newcap = c->listener_cap ? c->listener_cap * 2 : 1;
        void *tmp = realloc(c->listeners, newcap * sizeof *c->listeners);
        if (!tmp) { /* handle OOM, e.g. abort() or return error */
            fprintf(stderr, "Error with Realloc in channel_subscribe\n");
            return;
        }
        c->listeners = tmp;
        c->listener_cap = newcap;
        c->listener_cap = newcap;
    }
    c->listeners[c->listener_count++] = fn;
}

void channel_unsubscribe(Channel *c, EventListener fn) {
    for (size_t i = 0; i < c->listener_count; ++i) {
        if (c->listeners[i] == fn) {
            // slide down the tail
            memmove(&c->listeners[i], &c->listeners[i + 1],
                    (c->listener_count - i - 1) * sizeof *c->listeners);
            --c->listener_count;
            // optional shrink: if cap is a lot bigger than count,
            // you could realloc down here.
            break;
        }
    }
}

// ––– helpers for EventBus –––
void bus_init(EventBus *bus) {
    bus->channels = NULL;
    bus->channel_count = 0;
    bus->channel_cap = 0;
}

void bus_destroy(EventBus *bus) {
    for (size_t i = 0; i < bus->channel_count; ++i)
        channel_destroy(&bus->channels[i]);
    free(bus->channels);
    bus->channels = NULL;
    bus->channel_count = bus->channel_cap = 0;
}

static Channel *bus_find(EventBus *bus, const char *name) {
    for (size_t i = 0; i < bus->channel_count; ++i)
        if (strcmp(bus->channels[i].name, name) == 0)
            return &bus->channels[i];
    return NULL;
}

Channel *bus_get_or_create(EventBus *bus, const char *name) {
    // if name matches an existing channel, return it
    Channel *find_result = bus_find(bus, name);
    if (find_result)
        return find_result;

    // need a new channel
    if (bus->channel_count == bus->channel_cap) {
        size_t newcap = bus->channel_cap ? bus->channel_cap * 2 : 1;
        bus->channels = realloc(bus->channels, newcap * sizeof *bus->channels);
        bus->channel_cap = newcap;
    }
    Channel *c = &bus->channels[bus->channel_count++];
    channel_init(c, name);
    return c;
}

void bus_subscribe(EventBus *bus, const char *channel_name, EventListener fn) {
    Channel *c = bus_get_or_create(bus, channel_name);
    channel_subscribe(c, fn);
}

void bus_emit(EventBus *bus, const char *channel_name, const Event *e) {
    Channel *c = bus_find(bus, channel_name);
    if (c) {
        for (size_t i = 0; i < c->listener_count; ++i)
            c->listeners[i](e);
    }
}
