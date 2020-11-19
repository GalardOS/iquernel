#ifndef HAL_INTC_H
#define HAL_INTC_H

#ifdef __cplusplus
extern "C" {
#endif

typedef void(*intc_handler)(void);

typedef struct {
    unsigned int domain;
    unsigned int device_id;
} intc_id;

void intc_initialize();

void intc_add_handler(intc_id id, intc_handler handler, int aff_cpu);

void intc_add_local_handler(intc_id id, intc_handler handler);

#ifdef __cplusplus
}
#endif

#endif // HAL_INTC_H