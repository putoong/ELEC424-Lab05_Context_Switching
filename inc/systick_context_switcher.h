#ifndef SYSTICK_CONTEXT_SWITCHER_H_
#define SYSTICK_CONTEXT_SWITCHER_H_

#define STACK_SIZE 1024

typedef struct {
	void * sp;
} TCB ;

typedef struct  {
	uint32_t r0;
	uint32_t r1;
	uint32_t r2;
	uint32_t r3;
	uint32_t r12;
	uint32_t lr;
	uint32_t pc;
	uint32_t psr;
} hstack;

typedef struct  {
	uint32_t r4;
	uint32_t r5;
	uint32_t r6;
	uint32_t r7;
	uint32_t r8;
	uint32_t r9;
	uint32_t r10;
	uint32_t r11;
} sstack ;

void SysTick_Handler(void);
void create_task(void (*p) (void ), void *, int);
void run_task(void);

#endif