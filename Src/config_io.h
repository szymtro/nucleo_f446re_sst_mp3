#include <stdint.h>
#include <stdbool.h>

// Macros
#define BUTTON			(GPIOC->IDR & GPIO_Pin_13)
#define LED				(GPIO_Pin_5)


//initialize basic io's of cpu
void InitializeIOs();
