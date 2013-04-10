#ifndef _GNSS_Splitter_H_BAB
#define _GNSS_Splitter_H_BAB

extern struct rt_semaphore GNSS_R_sem;

void GNSS_Splitter(void);

void GNSS_Splitter_thread_entry(void* parameter);

#endif
