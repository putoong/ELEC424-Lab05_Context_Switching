#include "systick_context_switcher.h"
#include "sys_init.h"

TCB TCB_table[2];

static uint32_t * sk;

inline void * read_MSP (void){
	void * p;
	asm volatile (
		"MRS %0, msp\n\t" 
		: "=r" (p)
		);
}

inline void save_thread_context (void){
	uint32_t p;
	asm volatile (
		"MRS %0, psp\n\t"
		"STMDB %0!, {r4-r11}\n\t"
		"MSR psp, %0\n\t"
		: "=r" (p)
		);
}

inline void load_thread_context (void){
	/*uint32_t p;
	asm volatile (
		"MRS %0, psp\n\t"
		"LDMFD %0!, {r4-r11}\n\t"
		"MSR psp, %0\n\t"
		: "=r" (p)
		);*/
	asm volatile (
		"MRS r3, psp\n\t"
		"LDMFD r3!, {r4-r11}\n\t"
		"MSR psp, r3\n\t"
		);	
}

void __attribute__((naked)) SysTick_Handler(void){ // set the system tick handler?
	if (counter == 49) {
		//LEDToggle();
		asm volatile (
		"MRS r12, psp\n\t"
		"STMDB r12!, {r4-r11}\n\t"
		"MSR psp, r12\n\t"
		);
		asm volatile ("MRS %0, psp\n\t" : "=r" (TCB_table[current].sp) );
		current = 1 - current;
		asm volatile ("MSR psp, %0\n\t" : : "r" (TCB_table[current].sp) );
		asm volatile (
		"MRS r12, psp\n\t"
		"LDMFD r12!, {r4-r11}\n\t"
		"MSR psp, r12\n\t"

		);	
	}
	counter = (counter + 1) % 50;
	asm volatile ("BX lr");
}

static char mem[STACK_SIZE];

void init_tasks(void) {
	create_task( task_blink_led, mem, 0);
	create_task( task_spin_motors, mem + STACK_SIZE, 1);
}

void create_task(void (*p)(void) , void * s_addr, int idx) {
	
	hstack * hs;
	hs = (hstack *) (s_addr + STACK_SIZE - sizeof(hstack));
	hs -> psr = 0x01000000;
	hs -> pc = ((uint32_t)p);
	hs -> lr = 0;
	hs -> r12 = 0;
	hs -> r3 = 0; 
	hs -> r2 = 0;
	hs -> r1 = 0;
	hs -> r0 = 0;

	TCB_table[idx].sp = s_addr + STACK_SIZE - sizeof(hstack) - sizeof(sstack);

}

void run_task() {
	uint32_t sp = (uint32_t) TCB_table[0].sp + sizeof(sstack) + sizeof(hstack);
	hstack  * hs = (hstack *) (mem + STACK_SIZE - sizeof(hstack));
	asm volatile("MOV r12, #2\n\t");
	asm volatile("MSR control, r12\n\t");
	asm volatile("ISB\n\t");
	asm volatile("MSR psp, %0\n\t" : "=r" (sp) );
	__enable_irq();
	asm volatile("BX %0\n\t" : : "r" (hs->pc));

}