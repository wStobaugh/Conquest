#ifndef CONQUEST_SERVICE_MANAGER_H
#define CONQUEST_SERVICE_MANAGER_H
#include <stddef.h>

/* Add new kinds of services here as your engine grows */
typedef enum {
    INPUT_SERVICE,
    STATE_MANAGER_SERVICE,
    AUDIO_SERVICE,
    SETTINGS_MANAGER_SERVICE,
    EVENT_BUS_SERVICE,
    RESOURCE_MANAGER_SERVICE,
    CLOCK_SERVICE,
    RENDER_SERVICE
} ServiceType;

/* Opaque handle */
typedef struct ServiceManager ServiceManager;

/* life-cycle ------------------------------------------------------------- */
ServiceManager *svc_create(void);               /* heap-allocate              */
void            svc_destroy(ServiceManager *sm);/* frees ALL resources        */

/* registration / lookup -------------------------------------------------- */
int   svc_register  (ServiceManager *sm, ServiceType t, void *instance);
/* returns  0 on success
 *         -1 if |t| is already registered (existing mapping unchanged)     */

void *svc_get       (const ServiceManager *sm, ServiceType t);
/* returns the instance pointer or NULL if not found                        */

void  svc_unregister(ServiceManager *sm, ServiceType t);

#endif /* CONQUEST_SERVICE_MANAGER_H */
