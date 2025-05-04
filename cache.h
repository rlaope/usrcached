#ifndef CACHE_H
#define CACHE_H

typedef struct Cache {
    char* key;
    int   user_id;
    struct Cache* next;
} Cache;

void cache_init(void);
void cache_cleanup(void);

Cache* cache_find(const char* key);
int    cache_put(const char* key, int user_id);   // 0=created, 1=updated
int    cache_delete(const char* key);               // 1=deleted, 0=not found

#endif 
