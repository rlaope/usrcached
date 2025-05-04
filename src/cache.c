#include "cache.h"
#include <stdlib.h>
#include <string.h>
#include <windows.h>

static Cache* cache_head = NULL;
static CRITICAL_SECTION cache_lock;

void cache_init(void) {
	InitializeCriticalSection(&cache_lock);
}

void cache_cleanup(void) {
	EnterCriticalSection(&cache_lock);
	Cache* p = cache_head;
	while (p) {
		Cache* next = p->next;
		free(p->next);
		free(p);
		p = next;
	}
	cache_head = NULL;
	LeaveCriticalSection(&cache_lock);
	DeleteCriticalSection(&cache_lock);
}

Cache* cache_find(const char* key) {
	EnterCriticalSection(&cache_lock);
	for (Cache* p = cache_head; p; p = p->next) {
		if (strcmp(p->key, key) == 0) {
			LeaveCriticalSection(&cache_lock);
			return p;
		}
	}
	LeaveCriticalSection(&cache_lock);
}

int cache_put(const char* key, int user_id) {
	EnterCriticalSection(&cache_lock);
	for (Cache* p = cache_head; p; p = p->next) {
		if (strcmp(p->key, key) == 0) {
			p->user_id = user_id;
			LeaveCriticalSection(&cache_lock);
			return 1; // updated
		}
	}
	Cache* n = calloc(1, sizeof(Cache));
	n->key = strdup(key);
	n->user_id = user_id;
	n->next = cache_head;
	cache_head = n;
	LeaveCriticalSection(&cache_lock);
	return 0;
}

int cache_delete(const char* key) {
	EnterCriticalSection(&cache_lock);
	Cache **pp = &cache_head;
	while (*pp) {
		if (strcmp((*pp)->key, key) == 0) {
			Cache* del = *pp;
			*pp = del->next;
			free(del->key);
			free(del);
			LeaveCriticalSection(&cache_lock);
			return 1; // deleted
		}
		pp = &(*pp)->next;
	}
	LeaveCriticalSection(&cache_lock);
	return 0;
}