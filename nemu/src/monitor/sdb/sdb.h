#ifndef __SDB_H__
#define __SDB_H__

#include <common.h>

word_t expr(char *e, bool *success);

void init_regex();
void init_wp_pool();

#endif
