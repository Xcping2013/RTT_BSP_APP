#include <rthw.h>
#include <rtthread.h>

#include <drivers/pin.h>

#define HOME_RTT_DEBUG
#ifdef 	HOME_RTT_DEBUG
#define HOME_TRACE         rt_kprintf
#else
#define HOME_TRACE(...)    
#endif

uint8_t org[3]={3,4,5};

void org1_hdr_callback(void *args)
{
    //????
    char *a = args;
    //rt_kprintf("key0 down! %s\n",a);
}

void org2_hdr_callback(void *args)
{
    char *a = args;
}

void org3_hdr_callback(void *args)
{
    char *a = args;

}

void org_irq_thread_entry(void* parameter)
{    
	rt_pin_attach_irq(org[0], PIN_IRQ_MODE_FALLING,org1_hdr_callback,(void*)"callback args");
  rt_pin_irq_enable(org[0], PIN_IRQ_ENABLE);  
  
	rt_pin_attach_irq(org[1], PIN_IRQ_MODE_FALLING,org2_hdr_callback,(void*)"callback args");
  rt_pin_irq_enable(org[1], PIN_IRQ_ENABLE); 
  
	rt_pin_attach_irq(org[2], PIN_IRQ_MODE_FALLING,org3_hdr_callback,(void*)"callback args");
  rt_pin_irq_enable(org[2], PIN_IRQ_ENABLE);           
}
