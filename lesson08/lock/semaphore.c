#include "atomic.h"
#include "spin_lock.h"
#include "interrupt.h"

#define SEM_FLG_MUTEX 0
#define SEM_FLG_MULTI 1
#define SEM_MUTEX_ONE_LOCK 1
#define SEM_MULTI_LOCK 0

typedef struct s_KWLST
{
	spinlock_t wl_lock;
	uint_t wl_tdnr;
	list_h_t wl_list;
}kwlst_t;

typedef struct s_SEM
{
	spinlock_t sem_lock;
	uint_t sem_flg;
	sint_t sem_count;
	kwlst_t sem_waitlst;
}sem_t; 

void krlsem_down(sem_t* sem)
{
	cpuflg_t cpufg;
start_step:
	krlspinlock_cli(&sem->sem_lock, &cpufg);
	if(sem->sem_count<1)
	{
		krlwlst_wait(&sem->sem_waitlst);
		krlspinunlock_sti(&sem->sem_lock,&cpufg);
		krlschedul();
		goto start_step;
	}
	sem->sem_count--;
	krlspinunlock_sti(&sem->sem_lock,&cpufg);
	return;
}

void krlsem_up(sem_t* sem)
{
	cpuflg_t cpufg;
	krlspinlock_cli(&sem->sem_lock, &cpufg);
	sem->sem_count++;
	if(sem->sem_count<1)
	{
		krlspinunlock_sti(&sem->sem_lock,&cpufg);
		hal_sysdie("sem up err");
	}
	krlwlst_allup(&sem->sem_waitlst);
	krlspinunlock_sti(&sem->sem_lock,&cpufg);
	krlsched_set_schedflgs();
	return;
}
