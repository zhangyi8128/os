typedef unsigned int u32_t;
typedef u32_t cpuflg_t;
static inline void hal_save_flags_cli(cpuflg_t* flags)
{
	__asm__ __volatile__(
		"pushfl \t\n"
		"cli \t\n"
		"popl %0 \t\n"
		: "=m"(*flags)
		:
		: "memory"
		);
}

static inline void hal_restore_flags_sti(cpuflg_t* flags)
{
	__asm__ __volatile__(
		"pushl %0 \t\n"
		"popfl \t\n"
		:
		: "m"(*flags)
		: "memory"
		);
}
