// #include <stdlib.h>
// #include <stdio.h>
// #include <string.h>
// #include <stdint.h>
#ifndef SORT_CMP
#define SORT_CMP(x, y)  ((x) < (y) ? -1 : ((y) < (x) ? 1 : 0))
#endif
#ifndef SMALL_SORT
#define SMALL_SORT BITONIC_SORT

#endif
#define SIZE_MAX 18446744073709551615UL
#define SORT_CONCAT(x, y) x ## _ ## y
#define SORT_MAKE_STR1(x, y) SORT_CONCAT(x,y)
#define SORT_MAKE_STR(x) SORT_MAKE_STR1(SORT_NAME,x)
#define SORT_SWAP(x,y) {SORT_TYPE _sort_swap_temp = (x); (x) = (y); (y) = _sort_swap_temp;}
#define QUICK_SORT_PARTITION           SORT_MAKE_STR(quick_sort_partition)
#define BINARY_INSERTION_FIND          SORT_MAKE_STR(binary_insertion_find)
#define QUICK_SORT                     SORT_MAKE_STR(quick_sort)
#define MEDIAN                         SORT_MAKE_STR(median)
#define QUICK_SORT_RECURSIVE           SORT_MAKE_STR(quick_sort_recursive)
#define HEAP_SORT                      SORT_MAKE_STR(heap_sort)
#define BITONIC_SORT                   SORT_MAKE_STR(bitonic_sort)

#define SMALL_SORT_BND 16
#define CLZ __builtin_clzll
#define SORT_TYPE int
#ifndef SORT_CSWAP
#define SORT_CSWAP(x, y) { if(SORT_CMP((x),(y)) > 0) {SORT_SWAP((x),(y));}}
#endif
void BITONIC_SORT(SORT_TYPE *dst, const int size);

typedef struct {
  int start;
  int length;
} TIM_SORT_RUN_T;




typedef struct {
  SORT_TYPE alloc;
  SORT_TYPE *storage;
} TEMP_STORAGE_T;
static __inline int MEDIAN(const SORT_TYPE *dst, const int a, const int b,
                              const int c) {
  const int AB = SORT_CMP(dst[a], dst[b]) < 0;

  if (AB) {
    /* a < b */
    const int BC = SORT_CMP(dst[b], dst[c]) < 0;

    if (BC) {
      /* a < b < c */
      return b;
    } else {
      /* a < b, c < b */
      const int AC = SORT_CMP(dst[a], dst[c]) < 0;

      if (AC) {
        /* a < c < b */
        return c;
      } else {
        /* c < a < b */
        return a;
      }
    }
  } else {
    /* b < a */
    const int AC = SORT_CMP(dst[a], dst[b]) < 0;

    if (AC) {
      /* b < a < c */
      return a;
    } else {
      /* b < a, c < a */
      const int BC = SORT_CMP(dst[b], dst[c]) < 0;

      if (BC) {
        /* b < c < a */
        return c;
      } else {
        /* c < b < a */
        return b;
      }
    }
  }
}
static __inline void HEAP_SIFT_DOWN(SORT_TYPE *dst, const int start, const int end) {
  int root = start;

  while ((root << 1) <= end) {
    int child = root << 1;

    if ((child < end) && (SORT_CMP(dst[child], dst[child + 1]) < 0)) {
      child++;
    }

    if (SORT_CMP(dst[root], dst[child]) < 0) {
      SORT_SWAP(dst[root], dst[child]);
      root = child;
    } else {
      return;
    }
  }
}
static __inline void HEAPIFY(SORT_TYPE *dst, const int size) {
  int start = size >> 1;

  while (1) {
    HEAP_SIFT_DOWN(dst, start, size - 1);

    if (start == 0) {
      break;
    }

    start--;
  }
}
void HEAP_SORT(SORT_TYPE *dst, const int size) {
  int end = size - 1;

  /* don't bother sorting an array of size <= 1 */
  if (size <= 1) {
    return;
  }

  HEAPIFY(dst, size);

  while (end > 0) {
    SORT_SWAP(dst[end], dst[0]);
    HEAP_SIFT_DOWN(dst, 0, end - 1);
    end--;
  }
}
static __inline int BINARY_INSERTION_FIND(SORT_TYPE *dst, const SORT_TYPE x,
    const int size) {
  int l, c, r;
  SORT_TYPE cx;
  l = 0;
  r = size - 1;
  c = r >> 1;

  /* check for out of bounds at the beginning. */
  if (SORT_CMP(x, dst[0]) < 0) {
    return 0;
  } else if (SORT_CMP(x, dst[r]) > 0) {
    return r;
  }

  cx = dst[c];

  while (1) {
    const int val = SORT_CMP(x, cx);

    if (val < 0) {
      if (c - l <= 1) {
        return c;
      }

      r = c;
    } else { /* allow = for stability. The binary search favors the right. */
      if (r - c <= 1) {
        return c + 1;
      }

      l = c;
    }

    c = l + ((r - l) >> 1);
    cx = dst[c];
  }
}
static __inline int QUICK_SORT_PARTITION(SORT_TYPE *dst, const int left,
    const int right, const int pivot) {
  SORT_TYPE value = dst[pivot];
  int index = left;
  int i;
  int not_all_same = 0;
  /* move the pivot to the right */
  SORT_SWAP(dst[pivot], dst[right]);

  for (i = left; i < right; i++) {
    int cmp = SORT_CMP(dst[i], value);
    /* check if everything is all the same */
    not_all_same |= cmp;

    if (cmp < 0) {
      SORT_SWAP(dst[i], dst[index]);
      index++;
    }
  }

  SORT_SWAP(dst[right], dst[index]);

  /* avoid degenerate case */
  if (not_all_same == 0) {
    return SIZE_MAX;
  }

  return index;
}


static void QUICK_SORT_RECURSIVE(SORT_TYPE *dst, const int original_left,
                                 const int original_right) {
  int left;
  int right;
  int pivot;
  int new_pivot;
  int middle;
  int loop_count = 0;
  const int max_loops = 64 - CLZ(original_right - original_left); /* ~lg N */
  left = original_left;
  right = original_right;

  while (1) {
    if (right <= left) {
      return;
    }

    if ((right - left + 1U) <= SMALL_SORT_BND) {
      SMALL_SORT(&dst[left], right - left + 1U);
      return;
    }

    if (++loop_count >= max_loops) {
      /* we have recursed / looped too many times; switch to heap sort */
      HEAP_SORT(&dst[left], right - left + 1U);
      return;
    }

    /* median of 5 */
    middle = left + ((right - left) >> 1);
    pivot = MEDIAN((const SORT_TYPE *) dst, left, middle, right);
    pivot = MEDIAN((const SORT_TYPE *) dst, left + ((middle - left) >> 1), pivot,
                   middle + ((right - middle) >> 1));
    new_pivot = QUICK_SORT_PARTITION(dst, left, right, pivot);

    /* check for partition all equal */
    if (new_pivot == SIZE_MAX) {
      return;
    }

    /* recurse only on the small part to avoid degenerate stack sizes */
    /* and manually do tail call on the large part */
    if (new_pivot - 1U - left > right - new_pivot - 1U) {
      /* left is bigger than right */
      QUICK_SORT_RECURSIVE(dst, new_pivot + 1U, right);
      /* tail call for left */
      right = new_pivot - 1U;
    } else {
      /* right is bigger than left */
      QUICK_SORT_RECURSIVE(dst, left, new_pivot - 1U);
      /* tail call for right */
      left = new_pivot + 1U;
    }
  }
}
void QUICK_SORT(SORT_TYPE *dst, const int size) {
  /* don't bother sorting an array of size 1 */
  if (size <= 1) {
    return;
  }

  QUICK_SORT_RECURSIVE(dst, 0U, size - 1U);
}
static void BINARY_INSERTION_SORT_START(SORT_TYPE *dst, const int start, const int size) {
  int i;

  for (i = start; i < size; i++) {
    int j;
    SORT_TYPE x;
    int location;

    /* If this entry is already correct, just move along */
    if (SORT_CMP(dst[i - 1], dst[i]) <= 0) {
      continue;
    }

    /* Else we need to find the right place, shift everything over, and squeeze in */
    x = dst[i];
    location = BINARY_INSERTION_FIND(dst, x, i);

    for (j = i - 1; j >= location; j--) {
      dst[j + 1] = dst[j];

      if (j == 0) { /* check edge case because j is unsigned */
        break;
      }
    }

    dst[location] = x;
  }
}

/* Binary insertion sort */
void BINARY_INSERTION_SORT(SORT_TYPE *dst, const int size) {
  /* don't bother sorting an array of size <= 1 */
  if (size <= 1) {
    return;
  }

  BINARY_INSERTION_SORT_START(dst, 1, size);
}
#define BITONIC_SORT_2          SORT_MAKE_STR(bitonic_sort_2)
static __inline void BITONIC_SORT_2(SORT_TYPE *dst) {
  SORT_CSWAP(dst[0], dst[1]);
}


#define BITONIC_SORT_3          SORT_MAKE_STR(bitonic_sort_3)
static __inline void BITONIC_SORT_3(SORT_TYPE *dst) {
  SORT_CSWAP(dst[1], dst[2]);
  SORT_CSWAP(dst[0], dst[2]);
  SORT_CSWAP(dst[0], dst[1]);
}


#define BITONIC_SORT_4          SORT_MAKE_STR(bitonic_sort_4)
static __inline void BITONIC_SORT_4(SORT_TYPE *dst) {
  SORT_CSWAP(dst[0], dst[1]);
  SORT_CSWAP(dst[2], dst[3]);
  SORT_CSWAP(dst[0], dst[2]);
  SORT_CSWAP(dst[1], dst[3]);
  SORT_CSWAP(dst[1], dst[2]);
}


#define BITONIC_SORT_5          SORT_MAKE_STR(bitonic_sort_5)
static __inline void BITONIC_SORT_5(SORT_TYPE *dst) {
  SORT_CSWAP(dst[0], dst[1]);
  SORT_CSWAP(dst[3], dst[4]);
  SORT_CSWAP(dst[2], dst[4]);
  SORT_CSWAP(dst[2], dst[3]);
  SORT_CSWAP(dst[1], dst[4]);
  SORT_CSWAP(dst[0], dst[3]);
  SORT_CSWAP(dst[0], dst[2]);
  SORT_CSWAP(dst[1], dst[3]);
  SORT_CSWAP(dst[1], dst[2]);
}


#define BITONIC_SORT_6          SORT_MAKE_STR(bitonic_sort_6)
static __inline void BITONIC_SORT_6(SORT_TYPE *dst) {
  SORT_CSWAP(dst[1], dst[2]);
  SORT_CSWAP(dst[4], dst[5]);
  SORT_CSWAP(dst[0], dst[2]);
  SORT_CSWAP(dst[3], dst[5]);
  SORT_CSWAP(dst[0], dst[1]);
  SORT_CSWAP(dst[3], dst[4]);
  SORT_CSWAP(dst[2], dst[5]);
  SORT_CSWAP(dst[0], dst[3]);
  SORT_CSWAP(dst[1], dst[4]);
  SORT_CSWAP(dst[2], dst[4]);
  SORT_CSWAP(dst[1], dst[3]);
  SORT_CSWAP(dst[2], dst[3]);
}


#define BITONIC_SORT_7          SORT_MAKE_STR(bitonic_sort_7)
static __inline void BITONIC_SORT_7(SORT_TYPE *dst) {
  SORT_CSWAP(dst[1], dst[2]);
  SORT_CSWAP(dst[3], dst[4]);
  SORT_CSWAP(dst[5], dst[6]);
  SORT_CSWAP(dst[0], dst[2]);
  SORT_CSWAP(dst[3], dst[5]);
  SORT_CSWAP(dst[4], dst[6]);
  SORT_CSWAP(dst[0], dst[1]);
  SORT_CSWAP(dst[4], dst[5]);
  SORT_CSWAP(dst[2], dst[6]);
  SORT_CSWAP(dst[0], dst[4]);
  SORT_CSWAP(dst[1], dst[5]);
  SORT_CSWAP(dst[0], dst[3]);
  SORT_CSWAP(dst[2], dst[5]);
  SORT_CSWAP(dst[1], dst[3]);
  SORT_CSWAP(dst[2], dst[4]);
  SORT_CSWAP(dst[2], dst[3]);
}


#define BITONIC_SORT_8          SORT_MAKE_STR(bitonic_sort_8)
static __inline void BITONIC_SORT_8(SORT_TYPE *dst) {
  SORT_CSWAP(dst[0], dst[1]);
  SORT_CSWAP(dst[2], dst[3]);
  SORT_CSWAP(dst[4], dst[5]);
  SORT_CSWAP(dst[6], dst[7]);
  SORT_CSWAP(dst[0], dst[2]);
  SORT_CSWAP(dst[1], dst[3]);
  SORT_CSWAP(dst[4], dst[6]);
  SORT_CSWAP(dst[5], dst[7]);
  SORT_CSWAP(dst[1], dst[2]);
  SORT_CSWAP(dst[5], dst[6]);
  SORT_CSWAP(dst[0], dst[4]);
  SORT_CSWAP(dst[3], dst[7]);
  SORT_CSWAP(dst[1], dst[5]);
  SORT_CSWAP(dst[2], dst[6]);
  SORT_CSWAP(dst[1], dst[4]);
  SORT_CSWAP(dst[3], dst[6]);
  SORT_CSWAP(dst[2], dst[4]);
  SORT_CSWAP(dst[3], dst[5]);
  SORT_CSWAP(dst[3], dst[4]);
}


#define BITONIC_SORT_9          SORT_MAKE_STR(bitonic_sort_9)
static __inline void BITONIC_SORT_9(SORT_TYPE *dst) {
  SORT_CSWAP(dst[0], dst[1]);
  SORT_CSWAP(dst[3], dst[4]);
  SORT_CSWAP(dst[6], dst[7]);
  SORT_CSWAP(dst[1], dst[2]);
  SORT_CSWAP(dst[4], dst[5]);
  SORT_CSWAP(dst[7], dst[8]);
  SORT_CSWAP(dst[0], dst[1]);
  SORT_CSWAP(dst[3], dst[4]);
  SORT_CSWAP(dst[6], dst[7]);
  SORT_CSWAP(dst[2], dst[5]);
  SORT_CSWAP(dst[0], dst[3]);
  SORT_CSWAP(dst[1], dst[4]);
  SORT_CSWAP(dst[5], dst[8]);
  SORT_CSWAP(dst[3], dst[6]);
  SORT_CSWAP(dst[4], dst[7]);
  SORT_CSWAP(dst[2], dst[5]);
  SORT_CSWAP(dst[0], dst[3]);
  SORT_CSWAP(dst[1], dst[4]);
  SORT_CSWAP(dst[5], dst[7]);
  SORT_CSWAP(dst[2], dst[6]);
  SORT_CSWAP(dst[1], dst[3]);
  SORT_CSWAP(dst[4], dst[6]);
  SORT_CSWAP(dst[2], dst[4]);
  SORT_CSWAP(dst[5], dst[6]);
  SORT_CSWAP(dst[2], dst[3]);
}


#define BITONIC_SORT_10          SORT_MAKE_STR(bitonic_sort_10)
static __inline void BITONIC_SORT_10(SORT_TYPE *dst) {
  SORT_CSWAP(dst[4], dst[9]);
  SORT_CSWAP(dst[3], dst[8]);
  SORT_CSWAP(dst[2], dst[7]);
  SORT_CSWAP(dst[1], dst[6]);
  SORT_CSWAP(dst[0], dst[5]);
  SORT_CSWAP(dst[1], dst[4]);
  SORT_CSWAP(dst[6], dst[9]);
  SORT_CSWAP(dst[0], dst[3]);
  SORT_CSWAP(dst[5], dst[8]);
  SORT_CSWAP(dst[0], dst[2]);
  SORT_CSWAP(dst[3], dst[6]);
  SORT_CSWAP(dst[7], dst[9]);
  SORT_CSWAP(dst[0], dst[1]);
  SORT_CSWAP(dst[2], dst[4]);
  SORT_CSWAP(dst[5], dst[7]);
  SORT_CSWAP(dst[8], dst[9]);
  SORT_CSWAP(dst[1], dst[2]);
  SORT_CSWAP(dst[4], dst[6]);
  SORT_CSWAP(dst[7], dst[8]);
  SORT_CSWAP(dst[3], dst[5]);
  SORT_CSWAP(dst[2], dst[5]);
  SORT_CSWAP(dst[6], dst[8]);
  SORT_CSWAP(dst[1], dst[3]);
  SORT_CSWAP(dst[4], dst[7]);
  SORT_CSWAP(dst[2], dst[3]);
  SORT_CSWAP(dst[6], dst[7]);
  SORT_CSWAP(dst[3], dst[4]);
  SORT_CSWAP(dst[5], dst[6]);
  SORT_CSWAP(dst[4], dst[5]);
}


#define BITONIC_SORT_11          SORT_MAKE_STR(bitonic_sort_11)
static __inline void BITONIC_SORT_11(SORT_TYPE *dst) {
  SORT_CSWAP(dst[0], dst[1]);
  SORT_CSWAP(dst[2], dst[3]);
  SORT_CSWAP(dst[4], dst[5]);
  SORT_CSWAP(dst[6], dst[7]);
  SORT_CSWAP(dst[8], dst[9]);
  SORT_CSWAP(dst[1], dst[3]);
  SORT_CSWAP(dst[5], dst[7]);
  SORT_CSWAP(dst[0], dst[2]);
  SORT_CSWAP(dst[4], dst[6]);
  SORT_CSWAP(dst[8], dst[10]);
  SORT_CSWAP(dst[1], dst[2]);
  SORT_CSWAP(dst[5], dst[6]);
  SORT_CSWAP(dst[9], dst[10]);
  SORT_CSWAP(dst[0], dst[4]);
  SORT_CSWAP(dst[3], dst[7]);
  SORT_CSWAP(dst[1], dst[5]);
  SORT_CSWAP(dst[6], dst[10]);
  SORT_CSWAP(dst[4], dst[8]);
  SORT_CSWAP(dst[5], dst[9]);
  SORT_CSWAP(dst[2], dst[6]);
  SORT_CSWAP(dst[0], dst[4]);
  SORT_CSWAP(dst[3], dst[8]);
  SORT_CSWAP(dst[1], dst[5]);
  SORT_CSWAP(dst[6], dst[10]);
  SORT_CSWAP(dst[2], dst[3]);
  SORT_CSWAP(dst[8], dst[9]);
  SORT_CSWAP(dst[1], dst[4]);
  SORT_CSWAP(dst[7], dst[10]);
  SORT_CSWAP(dst[3], dst[5]);
  SORT_CSWAP(dst[6], dst[8]);
  SORT_CSWAP(dst[2], dst[4]);
  SORT_CSWAP(dst[7], dst[9]);
  SORT_CSWAP(dst[5], dst[6]);
  SORT_CSWAP(dst[3], dst[4]);
  SORT_CSWAP(dst[7], dst[8]);
}


#define BITONIC_SORT_12          SORT_MAKE_STR(bitonic_sort_12)
static __inline void BITONIC_SORT_12(SORT_TYPE *dst) {
  SORT_CSWAP(dst[0], dst[1]);
  SORT_CSWAP(dst[2], dst[3]);
  SORT_CSWAP(dst[4], dst[5]);
  SORT_CSWAP(dst[6], dst[7]);
  SORT_CSWAP(dst[8], dst[9]);
  SORT_CSWAP(dst[10], dst[11]);
  SORT_CSWAP(dst[1], dst[3]);
  SORT_CSWAP(dst[5], dst[7]);
  SORT_CSWAP(dst[9], dst[11]);
  SORT_CSWAP(dst[0], dst[2]);
  SORT_CSWAP(dst[4], dst[6]);
  SORT_CSWAP(dst[8], dst[10]);
  SORT_CSWAP(dst[1], dst[2]);
  SORT_CSWAP(dst[5], dst[6]);
  SORT_CSWAP(dst[9], dst[10]);
  SORT_CSWAP(dst[0], dst[4]);
  SORT_CSWAP(dst[7], dst[11]);
  SORT_CSWAP(dst[1], dst[5]);
  SORT_CSWAP(dst[6], dst[10]);
  SORT_CSWAP(dst[3], dst[7]);
  SORT_CSWAP(dst[4], dst[8]);
  SORT_CSWAP(dst[5], dst[9]);
  SORT_CSWAP(dst[2], dst[6]);
  SORT_CSWAP(dst[0], dst[4]);
  SORT_CSWAP(dst[7], dst[11]);
  SORT_CSWAP(dst[3], dst[8]);
  SORT_CSWAP(dst[1], dst[5]);
  SORT_CSWAP(dst[6], dst[10]);
  SORT_CSWAP(dst[2], dst[3]);
  SORT_CSWAP(dst[8], dst[9]);
  SORT_CSWAP(dst[1], dst[4]);
  SORT_CSWAP(dst[7], dst[10]);
  SORT_CSWAP(dst[3], dst[5]);
  SORT_CSWAP(dst[6], dst[8]);
  SORT_CSWAP(dst[2], dst[4]);
  SORT_CSWAP(dst[7], dst[9]);
  SORT_CSWAP(dst[5], dst[6]);
  SORT_CSWAP(dst[3], dst[4]);
  SORT_CSWAP(dst[7], dst[8]);
}


#define BITONIC_SORT_13          SORT_MAKE_STR(bitonic_sort_13)
static __inline void BITONIC_SORT_13(SORT_TYPE *dst) {
  SORT_CSWAP(dst[1], dst[7]);
  SORT_CSWAP(dst[9], dst[11]);
  SORT_CSWAP(dst[3], dst[4]);
  SORT_CSWAP(dst[5], dst[8]);
  SORT_CSWAP(dst[0], dst[12]);
  SORT_CSWAP(dst[2], dst[6]);
  SORT_CSWAP(dst[0], dst[1]);
  SORT_CSWAP(dst[2], dst[3]);
  SORT_CSWAP(dst[4], dst[6]);
  SORT_CSWAP(dst[8], dst[11]);
  SORT_CSWAP(dst[7], dst[12]);
  SORT_CSWAP(dst[5], dst[9]);
  SORT_CSWAP(dst[0], dst[2]);
  SORT_CSWAP(dst[3], dst[7]);
  SORT_CSWAP(dst[10], dst[11]);
  SORT_CSWAP(dst[1], dst[4]);
  SORT_CSWAP(dst[6], dst[12]);
  SORT_CSWAP(dst[7], dst[8]);
  SORT_CSWAP(dst[11], dst[12]);
  SORT_CSWAP(dst[4], dst[9]);
  SORT_CSWAP(dst[6], dst[10]);
  SORT_CSWAP(dst[3], dst[4]);
  SORT_CSWAP(dst[5], dst[6]);
  SORT_CSWAP(dst[8], dst[9]);
  SORT_CSWAP(dst[10], dst[11]);
  SORT_CSWAP(dst[1], dst[7]);
  SORT_CSWAP(dst[2], dst[6]);
  SORT_CSWAP(dst[9], dst[11]);
  SORT_CSWAP(dst[1], dst[3]);
  SORT_CSWAP(dst[4], dst[7]);
  SORT_CSWAP(dst[8], dst[10]);
  SORT_CSWAP(dst[0], dst[5]);
  SORT_CSWAP(dst[2], dst[5]);
  SORT_CSWAP(dst[6], dst[8]);
  SORT_CSWAP(dst[9], dst[10]);
  SORT_CSWAP(dst[1], dst[2]);
  SORT_CSWAP(dst[3], dst[5]);
  SORT_CSWAP(dst[7], dst[8]);
  SORT_CSWAP(dst[4], dst[6]);
  SORT_CSWAP(dst[2], dst[3]);
  SORT_CSWAP(dst[4], dst[5]);
  SORT_CSWAP(dst[6], dst[7]);
  SORT_CSWAP(dst[8], dst[9]);
  SORT_CSWAP(dst[3], dst[4]);
  SORT_CSWAP(dst[5], dst[6]);
}


#define BITONIC_SORT_14          SORT_MAKE_STR(bitonic_sort_14)
static __inline void BITONIC_SORT_14(SORT_TYPE *dst) {
  SORT_CSWAP(dst[0], dst[1]);
  SORT_CSWAP(dst[2], dst[3]);
  SORT_CSWAP(dst[4], dst[5]);
  SORT_CSWAP(dst[6], dst[7]);
  SORT_CSWAP(dst[8], dst[9]);
  SORT_CSWAP(dst[10], dst[11]);
  SORT_CSWAP(dst[12], dst[13]);
  SORT_CSWAP(dst[0], dst[2]);
  SORT_CSWAP(dst[4], dst[6]);
  SORT_CSWAP(dst[8], dst[10]);
  SORT_CSWAP(dst[1], dst[3]);
  SORT_CSWAP(dst[5], dst[7]);
  SORT_CSWAP(dst[9], dst[11]);
  SORT_CSWAP(dst[0], dst[4]);
  SORT_CSWAP(dst[8], dst[12]);
  SORT_CSWAP(dst[1], dst[5]);
  SORT_CSWAP(dst[9], dst[13]);
  SORT_CSWAP(dst[2], dst[6]);
  SORT_CSWAP(dst[3], dst[7]);
  SORT_CSWAP(dst[0], dst[8]);
  SORT_CSWAP(dst[1], dst[9]);
  SORT_CSWAP(dst[2], dst[10]);
  SORT_CSWAP(dst[3], dst[11]);
  SORT_CSWAP(dst[4], dst[12]);
  SORT_CSWAP(dst[5], dst[13]);
  SORT_CSWAP(dst[5], dst[10]);
  SORT_CSWAP(dst[6], dst[9]);
  SORT_CSWAP(dst[3], dst[12]);
  SORT_CSWAP(dst[7], dst[11]);
  SORT_CSWAP(dst[1], dst[2]);
  SORT_CSWAP(dst[4], dst[8]);
  SORT_CSWAP(dst[1], dst[4]);
  SORT_CSWAP(dst[7], dst[13]);
  SORT_CSWAP(dst[2], dst[8]);
  SORT_CSWAP(dst[5], dst[6]);
  SORT_CSWAP(dst[9], dst[10]);
  SORT_CSWAP(dst[2], dst[4]);
  SORT_CSWAP(dst[11], dst[13]);
  SORT_CSWAP(dst[3], dst[8]);
  SORT_CSWAP(dst[7], dst[12]);
  SORT_CSWAP(dst[6], dst[8]);
  SORT_CSWAP(dst[10], dst[12]);
  SORT_CSWAP(dst[3], dst[5]);
  SORT_CSWAP(dst[7], dst[9]);
  SORT_CSWAP(dst[3], dst[4]);
  SORT_CSWAP(dst[5], dst[6]);
  SORT_CSWAP(dst[7], dst[8]);
  SORT_CSWAP(dst[9], dst[10]);
  SORT_CSWAP(dst[11], dst[12]);
  SORT_CSWAP(dst[6], dst[7]);
  SORT_CSWAP(dst[8], dst[9]);
}


#define BITONIC_SORT_15          SORT_MAKE_STR(bitonic_sort_15)
static __inline void BITONIC_SORT_15(SORT_TYPE *dst) {
  SORT_CSWAP(dst[0], dst[1]);
  SORT_CSWAP(dst[2], dst[3]);
  SORT_CSWAP(dst[4], dst[5]);
  SORT_CSWAP(dst[6], dst[7]);
  SORT_CSWAP(dst[8], dst[9]);
  SORT_CSWAP(dst[10], dst[11]);
  SORT_CSWAP(dst[12], dst[13]);
  SORT_CSWAP(dst[0], dst[2]);
  SORT_CSWAP(dst[4], dst[6]);
  SORT_CSWAP(dst[8], dst[10]);
  SORT_CSWAP(dst[12], dst[14]);
  SORT_CSWAP(dst[1], dst[3]);
  SORT_CSWAP(dst[5], dst[7]);
  SORT_CSWAP(dst[9], dst[11]);
  SORT_CSWAP(dst[0], dst[4]);
  SORT_CSWAP(dst[8], dst[12]);
  SORT_CSWAP(dst[1], dst[5]);
  SORT_CSWAP(dst[9], dst[13]);
  SORT_CSWAP(dst[2], dst[6]);
  SORT_CSWAP(dst[10], dst[14]);
  SORT_CSWAP(dst[3], dst[7]);
  SORT_CSWAP(dst[0], dst[8]);
  SORT_CSWAP(dst[1], dst[9]);
  SORT_CSWAP(dst[2], dst[10]);
  SORT_CSWAP(dst[3], dst[11]);
  SORT_CSWAP(dst[4], dst[12]);
  SORT_CSWAP(dst[5], dst[13]);
  SORT_CSWAP(dst[6], dst[14]);
  SORT_CSWAP(dst[5], dst[10]);
  SORT_CSWAP(dst[6], dst[9]);
  SORT_CSWAP(dst[3], dst[12]);
  SORT_CSWAP(dst[13], dst[14]);
  SORT_CSWAP(dst[7], dst[11]);
  SORT_CSWAP(dst[1], dst[2]);
  SORT_CSWAP(dst[4], dst[8]);
  SORT_CSWAP(dst[1], dst[4]);
  SORT_CSWAP(dst[7], dst[13]);
  SORT_CSWAP(dst[2], dst[8]);
  SORT_CSWAP(dst[11], dst[14]);
  SORT_CSWAP(dst[5], dst[6]);
  SORT_CSWAP(dst[9], dst[10]);
  SORT_CSWAP(dst[2], dst[4]);
  SORT_CSWAP(dst[11], dst[13]);
  SORT_CSWAP(dst[3], dst[8]);
  SORT_CSWAP(dst[7], dst[12]);
  SORT_CSWAP(dst[6], dst[8]);
  SORT_CSWAP(dst[10], dst[12]);
  SORT_CSWAP(dst[3], dst[5]);
  SORT_CSWAP(dst[7], dst[9]);
  SORT_CSWAP(dst[3], dst[4]);
  SORT_CSWAP(dst[5], dst[6]);
  SORT_CSWAP(dst[7], dst[8]);
  SORT_CSWAP(dst[9], dst[10]);
  SORT_CSWAP(dst[11], dst[12]);
  SORT_CSWAP(dst[6], dst[7]);
  SORT_CSWAP(dst[8], dst[9]);
}


#define BITONIC_SORT_16          SORT_MAKE_STR(bitonic_sort_16)
static __inline void BITONIC_SORT_16(SORT_TYPE *dst) {
  SORT_CSWAP(dst[0], dst[1]);
  SORT_CSWAP(dst[2], dst[3]);
  SORT_CSWAP(dst[4], dst[5]);
  SORT_CSWAP(dst[6], dst[7]);
  SORT_CSWAP(dst[8], dst[9]);
  SORT_CSWAP(dst[10], dst[11]);
  SORT_CSWAP(dst[12], dst[13]);
  SORT_CSWAP(dst[14], dst[15]);
  SORT_CSWAP(dst[0], dst[2]);
  SORT_CSWAP(dst[4], dst[6]);
  SORT_CSWAP(dst[8], dst[10]);
  SORT_CSWAP(dst[12], dst[14]);
  SORT_CSWAP(dst[1], dst[3]);
  SORT_CSWAP(dst[5], dst[7]);
  SORT_CSWAP(dst[9], dst[11]);
  SORT_CSWAP(dst[13], dst[15]);
  SORT_CSWAP(dst[0], dst[4]);
  SORT_CSWAP(dst[8], dst[12]);
  SORT_CSWAP(dst[1], dst[5]);
  SORT_CSWAP(dst[9], dst[13]);
  SORT_CSWAP(dst[2], dst[6]);
  SORT_CSWAP(dst[10], dst[14]);
  SORT_CSWAP(dst[3], dst[7]);
  SORT_CSWAP(dst[11], dst[15]);
  SORT_CSWAP(dst[0], dst[8]);
  SORT_CSWAP(dst[1], dst[9]);
  SORT_CSWAP(dst[2], dst[10]);
  SORT_CSWAP(dst[3], dst[11]);
  SORT_CSWAP(dst[4], dst[12]);
  SORT_CSWAP(dst[5], dst[13]);
  SORT_CSWAP(dst[6], dst[14]);
  SORT_CSWAP(dst[7], dst[15]);
  SORT_CSWAP(dst[5], dst[10]);
  SORT_CSWAP(dst[6], dst[9]);
  SORT_CSWAP(dst[3], dst[12]);
  SORT_CSWAP(dst[13], dst[14]);
  SORT_CSWAP(dst[7], dst[11]);
  SORT_CSWAP(dst[1], dst[2]);
  SORT_CSWAP(dst[4], dst[8]);
  SORT_CSWAP(dst[1], dst[4]);
  SORT_CSWAP(dst[7], dst[13]);
  SORT_CSWAP(dst[2], dst[8]);
  SORT_CSWAP(dst[11], dst[14]);
  SORT_CSWAP(dst[5], dst[6]);
  SORT_CSWAP(dst[9], dst[10]);
  SORT_CSWAP(dst[2], dst[4]);
  SORT_CSWAP(dst[11], dst[13]);
  SORT_CSWAP(dst[3], dst[8]);
  SORT_CSWAP(dst[7], dst[12]);
  SORT_CSWAP(dst[6], dst[8]);
  SORT_CSWAP(dst[10], dst[12]);
  SORT_CSWAP(dst[3], dst[5]);
  SORT_CSWAP(dst[7], dst[9]);
  SORT_CSWAP(dst[3], dst[4]);
  SORT_CSWAP(dst[5], dst[6]);
  SORT_CSWAP(dst[7], dst[8]);
  SORT_CSWAP(dst[9], dst[10]);
  SORT_CSWAP(dst[11], dst[12]);
  SORT_CSWAP(dst[6], dst[7]);
  SORT_CSWAP(dst[8], dst[9]);
}
void BITONIC_SORT(SORT_TYPE *dst, const int size) {
  switch (size) {
  case 0:
  case 1:
    break;

  case 2:
    BITONIC_SORT_2(dst);
    break;

  case 3:
    BITONIC_SORT_3(dst);
    break;

  case 4:
    BITONIC_SORT_4(dst);
    break;

  case 5:
    BITONIC_SORT_5(dst);
    break;

  case 6:
    BITONIC_SORT_6(dst);
    break;

  case 7:
    BITONIC_SORT_7(dst);
    break;

  case 8:
    BITONIC_SORT_8(dst);
    break;

  case 9:
    BITONIC_SORT_9(dst);
    break;

  case 10:
    BITONIC_SORT_10(dst);
    break;

  case 11:
    BITONIC_SORT_11(dst);
    break;

  case 12:
    BITONIC_SORT_12(dst);
    break;

  case 13:
    BITONIC_SORT_13(dst);
    break;

  case 14:
    BITONIC_SORT_14(dst);
    break;

  case 15:
    BITONIC_SORT_15(dst);
    break;

  case 16:
    BITONIC_SORT_16(dst);
    break;

  default:
    BINARY_INSERTION_SORT(dst, size);
  }
}