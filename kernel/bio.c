// Buffer cache.
//
// The buffer cache is a linked list of buf structures holding
// cached copies of disk block contents.  Caching disk blocks
// in memory reduces the number of disk reads and also provides
// a synchronization point for disk blocks used by multiple processes.
//
// Interface:
// * To get a buffer for a particular disk block, call bread.
// * After changing buffer data, call bwrite to write it to disk.
// * When done with the buffer, call brelse.
// * Do not use the buffer after calling brelse.
// * Only one process at a time can use a buffer,
//     so do not keep them longer than necessary.

#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "riscv.h"
#include "defs.h"
#include "fs.h"
#include "buf.h"

#define BHashSize 13
struct
{
  struct spinlock BucketLock[BHashSize];
  struct buf buf[NBUF];

  // Linked list of all buffers, through prev/next.
  // Sorted by how recently the buffer was used.
  // head.next is most recent, head.prev is least.
  // struct buf head;
  struct buf bufBucket[BHashSize];
} bcache;

void binit(void)
{
  struct buf *b;

  // initlock(&bcache.lock, "bcache");
  for (int i = 0; i < BHashSize; i++)
  {
    initlock(&bcache.BucketLock[i], "bcache");
  }

  // Create linked list of buffers
  for (int i = 0; i < BHashSize; i++)
  {
    bcache.bufBucket[i].prev = &bcache.bufBucket[i];
    bcache.bufBucket[i].next = &bcache.bufBucket[i];
    for (b = bcache.buf + i * NBUF; b < bcache.buf + (i + 1) * NBUF; b++)
    {
      b->next = bcache.bufBucket[i].next;
      b->prev = &bcache.bufBucket[i];
      initsleeplock(&b->lock, "buffer");
      bcache.bufBucket[i].next->prev = b;
      bcache.bufBucket[i].next = b;
    }
  }
}

// Look through buffer cache for block on device dev.
// If not found, allocate a buffer.
// In either case, return locked buffer.
static struct buf *
bget(uint dev, uint blockno)
{
  struct buf *b;
  
  int idx = (dev+blockno)%BHashSize;
  //acquire(&bcache.lock);
  acquire(&bcache.bufBucket[idx].lock);

  // Is the block already cached?
  for (b = bcache.bufBucket[idx].next; b != &bcache.bufBucket[idx]; b = b->next)
  {
    if (b->dev == dev && b->blockno == blockno)
    {
      b->refcnt++;
      release(&bcache.bufBucket[idx].lock);
      acquiresleep(&b->lock);
      return b;
    }
  }

  // Not cached.
  // Recycle the least recently used (LRU) unused buffer.
  for (b = bcache.bufBucket[idx].prev; b != &bcache.bufBucket[idx]; b = b->prev)
  {
    if (b->refcnt == 0)
    {
      b->dev = dev;
      b->blockno = blockno;
      b->valid = 0;
      b->refcnt = 1;
      release(&bcache.BucketLock[idx]);
      acquiresleep(&b->lock);
      return b;
    }
  }
  panic("bget: no buffers");
}

// Return a locked buf with the contents of the indicated block.
struct buf *
bread(uint dev, uint blockno)
{
  struct buf *b;

  b = bget(dev, blockno);
  if (!b->valid)
  {
    virtio_disk_rw(b, 0);
    b->valid = 1;
  }
  return b;
}

// Write b's contents to disk.  Must be locked.
void bwrite(struct buf *b)
{
  if (!holdingsleep(&b->lock))
    panic("bwrite");
  virtio_disk_rw(b, 1);
}

// Release a locked buffer.
// Move to the head of the most-recently-used list.
void brelse(struct buf *b)
{
  if (!holdingsleep(&b->lock))
    panic("brelse");

  releasesleep(&b->lock);

  int idx = (b->dev+b->blockno)%BHashSize;
  acquire(&bcache.BucketLock[idx]);
  b->refcnt--;
  if (b->refcnt == 0)
  {
    // no one is waiting for it.
    b->next->prev = b->prev;
    b->prev->next = b->next;
    b->next = bcache.bufBucket[idx].next;
    b->prev = &bcache.bufBucket[idx];
    bcache.bufBucket[idx].next->prev = b;
    bcache.bufBucket[idx].next = b;
  }

  release(&bcache.BucketLock[idx]);
}

void bpin(struct buf *b)
{
  int idx = (b->dev+b->blockno)%BHashSize;
  acquire(&bcache.BucketLock[idx]);
  b->refcnt++;
  release(&bcache.BucketLock[idx]);
}

void bunpin(struct buf *b)
{
  int idx = (b->dev+b->blockno)%BHashSize;
  acquire(&bcache.BucketLock[idx]);
  b->refcnt--;
  release(&bcache.BucketLock[idx]);
}
