#ifndef VECTOR_H
#define VECTOR_H

#define NVIC_IRQ_COUNT (1)

/** Type of an interrupt function. Only used to avoid hard-to-read function
 * pointers in the efm32_vector_table_t struct. */
typedef void (*vector_table_entry_t)(void);

typedef struct {
	unsigned int *initial_sp_value; /**< Initial stack pointer value. */
	vector_table_entry_t reset;
	vector_table_entry_t nmi;
	vector_table_entry_t hard_fault;
	vector_table_entry_t memory_manage_fault; /* not in CM0 */
	vector_table_entry_t bus_fault;           /* not in CM0 */
	vector_table_entry_t usage_fault;         /* not in CM0 */
	vector_table_entry_t reserved_x001c[4];
	vector_table_entry_t sv_call;
	vector_table_entry_t debug_monitor;       /* not in CM0 */
	vector_table_entry_t reserved_x0034;
	vector_table_entry_t pend_sv;
	vector_table_entry_t systick;
  vector_table_entry_t irq[NVIC_IRQ_COUNT];
} vector_table_t;

/* Common symbols exported by the linker script(s): */
extern unsigned _data_loadaddr, _data, _edata, _ebss, _stack;
extern vector_table_t vector_table;

#endif
