// implement fork from user space

#include <inc/string.h>
#include <inc/lib.h>

// PTE_COW marks copy-on-write page table entries.
// It is one of the bits explicitly allocated to user processes (PTE_AVAIL).
#define PTE_COW		0x800

//
// Custom page fault handler - if faulting page is copy-on-write,
// map in our own private writable copy.
//
static void
pgfault(struct UTrapframe *utf)
{
	void *addr = (void *) utf->utf_fault_va;
	uint32_t err = utf->utf_err;
	int r;

	// Check that the faulting access was (1) a write, and (2) to a
	// copy-on-write page.  If not, panic.
	// Hint:
	//   Use the read-only page table mappings at uvpt
	//   (see <inc/memlayout.h>).

	// LAB 4: Your code here.
	addr = ROUNDDOWN(addr, PGSIZE);
	pte_t pte = uvpt[(uint32_t) addr / PGSIZE];

	if (!(err & FEC_WR) || !(pte & PTE_COW)) {
		panic("pgfault: faulting access was not a write or page was not COW");
	}

	// Allocate a new page, map it at a temporary location (PFTEMP),
	// copy the data from the old page to the new page, then move the new
	// page to the old page's address.
	// Hint:
	//   You should make three system calls.

	// LAB 4: Your code here.
	if (sys_page_alloc(0, PFTEMP, PTE_W | PTE_U | PTE_P) < 0) {
		panic("pgfault: sys_page_alloc failed");
	}
	memcpy((void *) PFTEMP, addr, PGSIZE);

	if (sys_page_map(0, (void *) PFTEMP, 0, addr, PTE_W | PTE_U | PTE_P) < 0) {
		panic("pgfault: sys_page_map failed");
	}

	if (sys_page_unmap(0, PFTEMP) < 0) {
		panic("pgfault: sys_page_map failed");
	}
}

//
// Map our virtual page pn (address pn*PGSIZE) into the target envid
// at the same virtual address.  If the page is writable or copy-on-write,
// the new mapping must be created copy-on-write, and then our mapping must be
// marked copy-on-write as well.  (Exercise: Why do we need to mark ours
// copy-on-write again if it was already copy-on-write at the beginning of
// this function?)
//
// Returns: 0 on success, < 0 on error.
// It is also OK to panic on error.
//
static int
duppage(envid_t envid, unsigned pn)
{
	int r = 0;

	// LAB 4: Your code here.
	pte_t pte = uvpt[pn];
	void *addr = (void *) (pn * PGSIZE);

	if (pte & PTE_SHARE) {
		r = sys_page_map(0, addr, envid, addr, PTE_SYSCALL);
	} else if (pte & PTE_W || pte & PTE_COW) {
		r = sys_page_map(0, addr, envid, addr, PTE_COW | PTE_U | PTE_P);
		if (r == 0 && !(pte & PTE_COW)) {
			r = sys_page_map(0, addr, 0, addr, PTE_COW | PTE_U | PTE_P);
		}
	} else {
		r = sys_page_map(0, addr, envid, addr, PTE_U | PTE_P);
	}

	return r;
}

//
// User-level fork with copy-on-write.
// Set up our page fault handler appropriately.
// Create a child.
// Copy our address space and page fault handler setup to the child.
// Then mark the child as runnable and return.
//
// Returns: child's envid to the parent, 0 to the child, < 0 on error.
// It is also OK to panic on error.
//
// Hint:
//   Use uvpd, uvpt, and duppage.
//   Remember to fix "thisenv" in the child process.
//   Neither user exception stack should ever be marked copy-on-write,
//   so you must allocate a new page for the child's user exception stack.
//
envid_t
fork(void)
{
	// LAB 4: Your code here.
	set_pgfault_handler(pgfault);

	envid_t pid = sys_exofork();

	if (pid < 0) {
		panic("fork: sys_exofork failed");
	} else if (pid == 0) { // child
		thisenv = &envs[ENVX(sys_getenvid())];
		return 0;
	}

	// parent

	for (unsigned pn = 0; pn < USTACKTOP / PGSIZE; pn++) {
		if ((uvpd[PDX(pn * PGSIZE)] & PTE_P) && (uvpt[pn] & PTE_P) && (uvpt[pn] & PTE_U)) {
			duppage(pid, pn);
		}
	}

	if (sys_page_alloc(pid, (void *) (UXSTACKTOP - PGSIZE), PTE_W | PTE_U | PTE_P) < 0) {
		panic("fork: sys_page_alloc failed");
	}

	if (sys_env_set_pgfault_upcall(pid, thisenv->env_pgfault_upcall) < 0) {
		panic("fork: sys_env_set_pgfault_upcall failed");
	}

	if (sys_env_set_status(pid, ENV_RUNNABLE) < 0) {
		panic("fork: sys_env_set_status failed");
	}

	return pid;
}

// Challenge!
int
sfork(void)
{
	panic("sfork not implemented");
	return -E_INVAL;
}
