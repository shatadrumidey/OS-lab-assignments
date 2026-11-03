#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "spinlock.h"

void kinit1(void *vstart, void *vend);
void kinit2(void *vstart, void *vend);
void freerange(void *vstart, void *vend);

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  int use_lock;
  struct run *freelist;
} kmem;

static int total_allocated = 0;
static int total_freed = 0;
static int total_pages = 0;
static int free_pages = 0;

extern char end[];

void
kinit1(void *vstart, void *vend)
{
  initlock(&kmem.lock, "kmem");
  kmem.use_lock = 0;
  freerange(vstart, vend);
}

void
kinit2(void *vstart, void *vend)
{
  freerange(vstart, vend);
  kmem.use_lock = 1;
}

void
freerange(void *vstart, void *vend)
{
  char *p;
  p = (char*)PGROUNDUP((uint)vstart);
  for(; p + PGSIZE <= (char*)vend; p += PGSIZE){
    total_pages++;
    kfree(p);
  }
}

void
kfree(char *v)
{
  struct run *r;
  if((uint)v % PGSIZE || v < end || V2P(v) >= PHYSTOP)
    panic("kfree");
  memset(v, 1, PGSIZE);
  if(kmem.use_lock)
    acquire(&kmem.lock);
  r = (struct run*)v;
  r->next = kmem.freelist;
  kmem.freelist = r;
  free_pages++;
  total_freed++;
  if(kmem.use_lock)
    release(&kmem.lock);
}

char*
kalloc(void)
{
  struct run *r;
  if(kmem.use_lock)
    acquire(&kmem.lock);
  r = kmem.freelist;
  if(r){
    kmem.freelist = r->next;
    free_pages--;
    total_allocated++;
  }
  if(kmem.use_lock)
    release(&kmem.lock);
  if(r)
    memset((char*)r, 1, PGSIZE);
  return (char*)r;
}

int get_total_allocated(void) { return total_allocated; }
int get_total_freed(void) { return total_freed; }
int ntotal_pages(void) { return total_pages; }
int nfree_pages(void) { return free_pages; }
