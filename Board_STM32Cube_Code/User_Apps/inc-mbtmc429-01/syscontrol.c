#include <rthw.h>
#include <rtthread.h>

#include <drivers/pin.h>

//int main(void)
//{
//    
//    rt_thread_t tid;
//       
//    /* ??key?? */
//    tid = rt_thread_create("key",
//                    key_thread_entry, 
//                    RT_NULL,
//                    1024, 
//                    2, 
//                    10);
//    /* ????????? */
//    if (tid != RT_NULL)
//        rt_thread_startup(tid);
//    
//    /* ??led?? */
//    tid = rt_thread_create("led",
//                    dido_hw_init, 
//                    RT_NULL,
//                    1024, 
//                    3, 
//                    10);
//    /* ????????? */
//    if (tid != RT_NULL)
//        rt_thread_startup(tid);
//    
//    /* ??irq?? */
//    tid = rt_thread_create("exirq",
//                    org_irq_thread_entry, 
//                    RT_NULL,
//                    1024, 
//                    4, 
//                    10);
//    /* ????????? */
//    if (tid != RT_NULL)
//        rt_thread_startup(tid);
//              
//    return 0;
//}

