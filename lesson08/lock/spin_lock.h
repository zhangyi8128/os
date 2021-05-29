typedef unsigned int u32_t;
typedef u32_t cpuflg_t;
typedef struct
{
	volatile u32_t lock;
}spinlock_t;

static inline void x86_spin_lock_init(spinlock_t * lock)
{
	lock->lock = 0;
}

static inline void x86_spin_lock(spinlock_t * lock)
{
	__asm__ __volatile__(
	"1: \n"
	"lock; xchg %0, %1 \n"
	"cmpl $0, %0 \n"
	"jnz 2f \n"
	"jmp 3f \n"
	"2:	\n"
	"cmpl $0, %1 \n"
	"jne 2b \n"
	"jmp 1b \n"
	"3: \n"
	:
	: "r"(1), "m"(*lock));
}

static inline void x86_spin_unlock(spinlock_t * lock)
{
	__asm__ __volatile__(
	"movl $0, %0\n"
	:
	: "m"(*lock));
}

static inline void x86_spin_lock_disable_irq(spinlock_t* lock, cpuflg_t* flags)
{
	__asm__ __volatile__(
	"pushfq	\n\t"
	"cli 	\n\t"
	"popq %0\n\t"
	"1: 	\n\t"
	"lock; xchg %1, %2 \n\t"
	"cmpl $0,%1 	   \n\t"
	"jnz 2f		   \n\t"
	"jmp 3f		   \n"
	"2:		   \n\t"
	"cmpl $0,%2	   \n\t"
	"jne 2b		   \n\t"
	"jmp 1b		   \n\t"
	"3:		   \n"
	: "=m"(*flags)
	: "r"(1), "m"(*lock));
}

static inline void x86_spin_unlock_enabled_irq(spinlock_t* lock, cpuflag_t* flags)
{
	__asm__ __volatile__(
	"movl $0,%0	\n\t"
	"pushq %1 	\n\t"
	"popfq 		\n\t"
	:
	: "m"(*lock), "m"(*flags));
}
