#define _GNU_SOURCE
#include <link.h>
#include <stdio.h>
#include <stdlib.h>

static int header_handler(struct dl_phdr_info* info, size_t size, void* data) {
  printf("name=%s (%d segments) address=%p\n", info->dlpi_name, info->dlpi_phnum,
         (void*)info->dlpi_addr);
  for (int j = 0; j < info->dlpi_phnum; j++) {
    printf("\t\t header %2d: address=%10p\n", j,
           (void*)(info->dlpi_addr + info->dlpi_phdr[j].p_vaddr));
    printf("\t\t\t type=%u, flags=0x%X\n", info->dlpi_phdr[j].p_type, info->dlpi_phdr[j].p_flags);
  }
  printf("\n");
  return 0;
}

extern int ml_func(int, int);
extern int myglob;

int main(int argc, const char* argv[]) {
  dl_iterate_phdr(header_handler, NULL);

  printf("addr myglob = %p\n", (void*)&myglob);
  int t = ml_func(argc, argc);
  return t;
}
