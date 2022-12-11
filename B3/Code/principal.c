#include "principal.h"
#include "cmsis_os2.h"

typedef enum{reposo, manual, memoria, prog_hora} estados_t;

static osThreadId_t id_Th_principal;

static void Th_principal(void *argument);

int Init_Th_principal(void){
  id_Th_principal = osThreadNew(Th_principal, NULL, NULL);
  if(id_Th_principal == NULL)
    return(-1);
  return(Init_Th_clock() | 
					Init_Th_com() | 
					Init_Th_joystick() | 
					Init_Th_lcd() | 
					Init_Th_pwm() | 
					Init_Th_rda() | 
					Init_Th_temp() | 
					Init_Th_vol());
}

static void Th_principal(void *argument){
	static estados_t estado = reposo;
	set_clock(0, 0, 0);
	
	
	while(1){
		osThreadYield();
	}
}
