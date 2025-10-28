// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem;


static int refcounts[(PHYSTOP - KERNBASE) / PGSIZE];
static uint64 nframes = (PHYSTOP - KERNBASE) / PGSIZE;

void
kinit()
{
  initlock(&kmem.lock, "kmem");
  // zero the refcounts array
  memset(refcounts, 0, sizeof(refcounts));
  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by pa,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  // memset(pa, 1, PGSIZE);

  uint64 idx = ((uint64)pa - KERNBASE) / PGSIZE;

  r = (struct run*)pa;

  acquire(&kmem.lock);
  if(idx < nframes && refcounts[idx] > 0){
    refcounts[idx]--;
    if(refcounts[idx] == 0){
      memset(pa, 1, PGSIZE);
      r = (struct run*)pa;
      r->next = kmem.freelist;
      kmem.freelist = r;
    }
  } else if (refcounts[idx] == 0){
    // ref count is 0
    memset(pa, 1, PGSIZE);
    r = (struct run*)pa;
    r->next = kmem.freelist;
    kmem.freelist = r;
  }
  else{
    printf("big porblemeno");
  }
  release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  if(r)
    kmem.freelist = r->next;
  release(&kmem.lock);

  if(r) {
    memset((char*)r, 5, PGSIZE); // fill with junk
    // set base ref to 1
    uint64 idx = ((uint64)r - KERNBASE) / PGSIZE;
      refcounts[idx] = 1;
  }
  return (void*)r;
}

void
ref_increment(void *pa)
{
  acquire(&kmem.lock);
  uint64 idx = ((uint64)pa - KERNBASE) / PGSIZE;
  refcounts[idx]++;
  release(&kmem.lock);
}

int
get_ref_count(void *pa)
{
  acquire(&kmem.lock);
  uint64 idx = ((uint64)pa - KERNBASE) / PGSIZE;
  int v = refcounts[idx];
  release(&kmem.lock);
  return v;
}
