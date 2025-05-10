#include "service_manager.h"
#include "../../utils/log.h"
#include <stdlib.h>
#include <string.h>

typedef struct {
    ServiceType type;
    void       *instance;
} Entry;

struct ServiceManager {
    Entry  *buf;
    size_t  count, cap;
};

/* ─── helpers ──────────────────────────────────────────────────────────── */
static ptrdiff_t find_index(const ServiceManager *sm, ServiceType t) {
    for (size_t i = 0; i < sm->count; ++i)
        if (sm->buf[i].type == t) return (ptrdiff_t)i;
    return -1;
}
static void reserve(ServiceManager *sm, size_t newcap) {
    if (newcap <= sm->cap) return;
    Entry *tmp = realloc(sm->buf, newcap * sizeof *sm->buf);
    if (!tmp) { LOG_FATAL("ServiceManager OOM while reallocating"); }
    sm->buf  = tmp;
    sm->cap  = newcap;
}

/* ─── public API ───────────────────────────────────────────────────────── */
ServiceManager *svc_create(void) { return calloc(1, sizeof(ServiceManager)); }

void svc_destroy(ServiceManager *sm) {
    if (!sm) return;
    free(sm->buf);
    free(sm);
}

int svc_register(ServiceManager *sm, ServiceType t, void *instance) {
    if (!sm || !instance) return -1;
    if (find_index(sm, t) >= 0) {
        LOG_WARN("Service %d already registered – ignoring", (int)t);
        return -1;
    }
    reserve(sm, sm->count + 1);
    sm->buf[sm->count++] = (Entry){ .type = t, .instance = instance };
    LOG_DEBUG("Registered service %d", (int)t);
    return 0;
}

void *svc_get(const ServiceManager *sm, ServiceType t) {
    ptrdiff_t idx = find_index(sm, t);
    return (idx >= 0) ? sm->buf[idx].instance : NULL;
}

void svc_unregister(ServiceManager *sm, ServiceType t) {
    ptrdiff_t idx = find_index(sm, t);
    if (idx < 0) return;
    memmove(&sm->buf[idx], &sm->buf[idx + 1],
            (sm->count - idx - 1) * sizeof *sm->buf);
    --sm->count;
    LOG_DEBUG("Unregistered service %d", (int)t);
}
