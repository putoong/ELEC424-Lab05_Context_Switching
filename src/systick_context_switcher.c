#include "systick_context_switcher.h"
#include "sys_init.h"

TCB TCB_table[2];

static uint32_t * sk;

static inline void * read_MSP (void){
	void * p;
	asm volatile (
		"MRS %0, msp\n\t" 
		: "=r" (p)
		);
}

static inline void save_thread_context (void){
	uint32_t p;
	asm volatile (
		"MRS %0, psp\n\t"
		"STMDB %0!, {r4-r11}\n\t"
		"MSR psp, %0\n\t"
		: "=r" (p)
		);
}

static inline void load_thread_context (void){
	uint32_t p;
	asm volatile (
		"MRS %0, psp\n\t"
		"LDMFD %0!, {r4-r11}\n\t"
		"MSR psp, %0\n\t"
		: "=r" (p)
		);
}

void SysTick_Handler(void){ // set the system tick handler?
	if (counter == 49) {
		//LEDToggle();
		save_thread_context();
		sk = read_PSP();
		switch_thread_context();
		load_thread_context();
	}
	counter = (counter + 1) % 50;
}

static inline void * read_PSP (){
	void * p;
	asm volatile (
		"MRS %0, psp\n\t"
		: "=r" (p)
		);
	return p;
}

static inline void write_PSP (void * p){
	asm volatile ("MSR psp, %0\n\t" : : "r" (p) );
}

static char mem[STACK_SIZE];

void switch_thread_context (void){
	TCB_table[current].sp = read_PSP ();
	current = 1 - current;
	*((uint32_t*)sk) = 0xFFFFFFFD;
	write_PSP ((TCB_table[current]).sp);
}

void init_tasks(void) {
	create_task( task_blink_led, mem, 0);
	create_task( task_spin_motors, mem + STACK_SIZE, 1);
}

void create_task(void (*p)(void) , void * s_addr, int idx) {
	
	hstack * hs;
	hs = (hstack *) (s_addr + STACK_SIZE - sizeof(hstack));
	hs -> psr = 0x21000000;
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
	asm volatile("MSR psp, %0\n\t" : "=r" (sp) );
	asm volatile("BX %0\n\t" : : "r" (hs->pc));

}