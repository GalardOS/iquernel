#include "hal/intc.h"
#include "drivers/gic400/gic400.h"

#define GIC400_BCM2711_BASEADDR 0x4C0040000

int interrupt_controller_initialized = false;

void intc_initialize() {
    if(!interrupt_controller_initialized) {
        gic400_initialize(GIC400_BCM2711_BASEADDR); 
        gic400_set_interrupt_mode(GIC400_INT_MODEL_1N);
    }

    gic400_enable_interrupts();
}

void intc_add_handler(intc_id id, intc_handler handler, uint8 aff_cpu) {
    gic400_enable_interrupt(id);
    gic400_set_target(id, GIC400_CPUALL);            // Set all CPU as targets
}

void intc_add_local_handler(intc_id id, intc_handler handler) {
    byte cpuid = gic400_get_cpuid();
    gic400_enable_interrupt(id);
    gic400_set_target(id, cpuid);            // Set this cpu as target
}