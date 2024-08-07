// Copyright 2020 Joshua J Baker. All rights reserved.
// Use of this source code is governed by an MIT-style
// License can be found in header

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include "std.h"

#define GROW_AT   0.60 /* 60% */
#define SHRINK_AT 0.10 /* 10% */

#ifndef HASHMAP_LOAD_FACTOR
#define HASHMAP_LOAD_FACTOR GROW_AT
#endif

static vptr_t (*__malloc)(size_t) = NULL;
static vptr_t (*__realloc)(vptr_t , size_t) = NULL;
static void (*__free)(vptr_t ) = NULL;

void hashmap_set_allocator(vptr_t (*malloc)(size_t), void (*free)(vptr_t )) {
    __malloc = malloc;
    __free = free;
}

struct bucket {
    u64 hash:48;
    u64 dib:16;
};

struct hashmap_t {
    vptr_t (*malloc)(size_t);
    vptr_t (*realloc)(vptr_t , size_t);
    void (*free)(vptr_t );
    size_t elsize;
    size_t cap;
    u64 seed0;
    u64 seed1;
    u64 (*hash)(const vptr_t item, u64 seed0, u64 seed1);
    i32 (*compare)(const vptr_t a, const vptr_t b, vptr_t udata);
    void (*elfree)(vptr_t item);
    vptr_t udata;
    size_t bucketsz;
    size_t nbuckets;
    size_t count;
    size_t mask;
    size_t growat;
    size_t shrinkat;
    u8 loadfactor;
    u8 growpower;
    b8 oom;
    vptr_t buckets;
    vptr_t spare;
    vptr_t edata;
};

void hashmap_set_grow_by_power(struct hashmap_t *map, size_t power) {
    map->growpower = (u8)(power < 1 ? 1 : (power > 16 ? 16 : power));
}

static f64 clamp_load_factor(f64 factor, f64 default_factor) {
    if (factor != factor)   return default_factor;
    if (factor < 0.50)      return 0.50;
    if (factor > 0.95)      return 0.95;

    return factor;
}

void hashmap_set_load_factor(struct hashmap_t *map, f64 factor) {
    factor = clamp_load_factor(factor, (f64)(map->loadfactor) / 100.0);
    map->loadfactor = (u8)(factor * 100);
    map->growat = (u64)(map->nbuckets * factor);
    map->shrinkat = (u64)(map->nbuckets * SHRINK_AT);
}

static struct bucket *bucket_at0(vptr_t buckets, size_t bucketsz, size_t i) {
    return (struct bucket*)(((char*)buckets)+(bucketsz*i));
}

static struct bucket *bucket_at(struct hashmap_t *map, size_t index) {
    return bucket_at0(map->buckets, map->bucketsz, index);
}

static vptr_t bucket_item(struct bucket *entry) {
    return ((char*)entry)+sizeof(struct bucket);
}

static u64 clip_hash(u64 hash) {
    return hash & 0xFFFFFFFFFFFF;
}

static u64 get_hash(struct hashmap_t *map, const vptr_t key) {
    return clip_hash(map->hash(key, map->seed0, map->seed1));
}

struct hashmap_t *hashmap_new_with_allocator(vptr_t (*_malloc)(size_t), 
    vptr_t (*_realloc)(vptr_t , size_t), void (*_free)(vptr_t ),
    size_t elsize, size_t cap, u64 seed0, u64 seed1,
    u64 (*hash)(const vptr_t item, u64 seed0, u64 seed1),
    i32 (*compare)(const vptr_t a, const vptr_t b, vptr_t udata),
    void (*elfree)(vptr_t item),
    vptr_t udata)
{
    _malloc = _malloc ? _malloc : __malloc ? __malloc : malloc;
    _realloc = _realloc ? _realloc : __realloc ? __realloc : realloc;
    _free = _free ? _free : __free ? __free : free;
    size_t ncap = 16;
    if (cap < ncap) {
        cap = ncap;
    } else {
        while (ncap < cap) {
            ncap *= 2;
        }
        cap = ncap;
    }
    size_t bucketsz = sizeof(struct bucket) + elsize;
    while (bucketsz & (sizeof(uintptr_t)-1)) {
        bucketsz++;
    }

    size_t size = sizeof(struct hashmap_t)+bucketsz*2;
    struct hashmap_t *map = _malloc(size);
    if (!map) {
        return NULL;
    }
    memset(map, 0, sizeof(struct hashmap_t));
    map->elsize = elsize;
    map->bucketsz = bucketsz;
    map->seed0 = seed0;
    map->seed1 = seed1;
    map->hash = hash;
    map->compare = compare;
    map->elfree = elfree;
    map->udata = udata;
    map->spare = ((char*)map)+sizeof(struct hashmap_t);
    map->edata = (char*)map->spare+bucketsz;
    map->cap = cap;
    map->nbuckets = cap;
    map->mask = map->nbuckets-1;
    map->buckets = _malloc(map->bucketsz*map->nbuckets);
    if (!map->buckets) {
        _free(map);
        return NULL;
    }
    memset(map->buckets, 0, map->bucketsz*map->nbuckets);
    map->growpower = 1;
    map->loadfactor = (u8)(clamp_load_factor(HASHMAP_LOAD_FACTOR, GROW_AT) * 100.0);
    map->growat = map->nbuckets * (size_t)(map->loadfactor / 100);
    map->shrinkat = (size_t)((double)map->nbuckets * SHRINK_AT);
    map->malloc = _malloc;
    map->realloc = _realloc;
    map->free = _free;
    return map;  
}


struct hashmap_t *hashmap_new(size_t elsize, size_t cap, u64 seed0, 
    u64 seed1,
    u64 (*hash)(const vptr_t item, u64 seed0, u64 seed1),
    i32 (*compare)(const vptr_t a, const vptr_t b, vptr_t udata),
    void (*elfree)(vptr_t item),
    vptr_t udata)
{
    return hashmap_new_with_allocator(NULL, NULL, NULL, elsize, cap, seed0, 
        seed1, hash, compare, elfree, udata);
}

static void free_elements(struct hashmap_t *map) {
    if (map->elfree) {
        for (size_t i = 0; i < map->nbuckets; i++) {
            struct bucket *bucket = bucket_at(map, i);
            if (bucket->dib) map->elfree(bucket_item(bucket));
        }
    }
}

void hashmap_clear(struct hashmap_t *map, b8 update_cap) {
    map->count = 0;
    free_elements(map);
    if (update_cap) {
        map->cap = map->nbuckets;
    } else if (map->nbuckets != map->cap) {
        vptr_t new_buckets = map->malloc(map->bucketsz*map->cap);
        if (new_buckets) {
            map->free(map->buckets);
            map->buckets = new_buckets;
        }
        map->nbuckets = map->cap;
    }
    memset(map->buckets, 0, map->bucketsz*map->nbuckets);
    map->mask = map->nbuckets-1;
    map->growat = map->nbuckets * (size_t)((f64)map->loadfactor / 100.0) ;
    map->shrinkat = (u64)((f64)map->nbuckets * SHRINK_AT);
}

static b8 resize0(struct hashmap_t *map, size_t new_cap) {
    struct hashmap_t *map2 = hashmap_new_with_allocator(map->malloc, map->realloc, 
        map->free, map->elsize, new_cap, map->seed0, map->seed1, map->hash, 
        map->compare, map->elfree, map->udata);
    if (!map2) return false;
    for (size_t i = 0; i < map->nbuckets; i++) {
        struct bucket *entry = bucket_at(map, i);
        if (!entry->dib) {
            continue;
        }
        entry->dib = 1;
        size_t j = entry->hash & map2->mask;
        while(1) {
            struct bucket *bucket = bucket_at(map2, j);
            if (bucket->dib == 0) {
                memcpy(bucket, entry, map->bucketsz);
                break;
            }
            if (bucket->dib < entry->dib) {
                memcpy(map2->spare, bucket, map->bucketsz);
                memcpy(bucket, entry, map->bucketsz);
                memcpy(entry, map2->spare, map->bucketsz);
            }
            j = (j + 1) & map2->mask;
            entry->dib += 1;
        }
    }
    map->free(map->buckets);
    map->buckets = map2->buckets;
    map->nbuckets = map2->nbuckets;
    map->mask = map2->mask;
    map->growat = map2->growat;
    map->shrinkat = map2->shrinkat;
    map->free(map2);
    return true;
}

static b8 resize(struct hashmap_t *map, size_t new_cap) {
    return resize0(map, new_cap);
}

const vptr_t hashmap_set_with_hash(struct hashmap_t *map, const vptr_t item,
    u64 hash)
{
    hash = clip_hash(hash);
    map->oom = false;
    if (map->count >= map->growat) {
        if (!resize(map, map->nbuckets * (u64)(1 <<map->growpower))) {
            map->oom = true;
            return NULL;
        }
    }

    struct bucket *entry = map->edata;
    entry->hash = hash;
    entry->dib = 1;
    vptr_t eitem = bucket_item(entry);
    memcpy(eitem, item, map->elsize);

    vptr_t bitem;
    size_t i = entry->hash & map->mask;
    while(1) {
        struct bucket *bucket = bucket_at(map, i);
        if (bucket->dib == 0) {
            memcpy(bucket, entry, map->bucketsz);
            map->count++;
            return NULL;
        }
        bitem = bucket_item(bucket);
        if (entry->hash == bucket->hash && (!map->compare ||
            map->compare(eitem, bitem, map->udata) == 0))
        {
            memcpy(map->spare, bitem, map->elsize);
            memcpy(bitem, eitem, map->elsize);
            return map->spare;
        }
        if (bucket->dib < entry->dib) {
            memcpy(map->spare, bucket, map->bucketsz);
            memcpy(bucket, entry, map->bucketsz);
            memcpy(entry, map->spare, map->bucketsz);
            eitem = bucket_item(entry);
        }
        i = (i + 1) & map->mask;
        entry->dib += 1;
    }
}

const vptr_t hashmap_set(struct hashmap_t *map, const vptr_t item) {
    return hashmap_set_with_hash(map, item, get_hash(map, item));
}

const vptr_t hashmap_get_with_hash(struct hashmap_t *map, const vptr_t key, 
    u64 hash)
{
    hash = clip_hash(hash);
    size_t i = hash & map->mask;
    while(1) {
        struct bucket *bucket = bucket_at(map, i);
        if (!bucket->dib) return NULL;
        if (bucket->hash == hash) {
            vptr_t bitem = bucket_item(bucket);
            if (!map->compare || map->compare(key, bitem, map->udata) == 0) {
                return bitem;
            }
        }
        i = (i + 1) & map->mask;
    }
}

const vptr_t hashmap_get(struct hashmap_t *map, const vptr_t key) {
    return hashmap_get_with_hash(map, key, get_hash(map, key));
}

const vptr_t hashmap_probe(struct hashmap_t *map, u64 position) {
    size_t i = position & map->mask;
    struct bucket *bucket = bucket_at(map, i);
    if (!bucket->dib) {
        return NULL;
    }
    return bucket_item(bucket);
}

const vptr_t hashmap_delete_with_hash(struct hashmap_t *map, const vptr_t key,
    u64 hash)
{
    hash = clip_hash(hash);
    map->oom = false;
    size_t i = hash & map->mask;
    while(1) {
        struct bucket *bucket = bucket_at(map, i);
        if (!bucket->dib) {
            return NULL;
        }
        vptr_t bitem = bucket_item(bucket);
        if (bucket->hash == hash && (!map->compare ||
            map->compare(key, bitem, map->udata) == 0))
        {
            memcpy(map->spare, bitem, map->elsize);
            bucket->dib = 0;
            while(1) {
                struct bucket *prev = bucket;
                i = (i + 1) & map->mask;
                bucket = bucket_at(map, i);
                if (bucket->dib <= 1) {
                    prev->dib = 0;
                    break;
                }
                memcpy(prev, bucket, map->bucketsz);
                prev->dib--;
            }
            map->count--;
            if (map->nbuckets > map->cap && map->count <= map->shrinkat) {
                resize(map, map->nbuckets/2);
            }
            return map->spare;
        }
        i = (i + 1) & map->mask;
    }
}

const vptr_t hashmap_delete(struct hashmap_t *map, const vptr_t key) {
    return hashmap_delete_with_hash(map, key, get_hash(map, key));
}

size_t hashmap_count(struct hashmap_t *map) {
    return map->count;
}

void hashmap_free(struct hashmap_t *map) {
    if (!map) return;
    free_elements(map);
    map->free(map->buckets);
    map->free(map);
}

b8 hashmap_oom(struct hashmap_t *map) {
    return map->oom;
}

b8 hashmap_scan(struct hashmap_t *map, 
    b8 (*iter)(const vptr_t item, vptr_t udata), vptr_t udata)
{
    for (size_t i = 0; i < map->nbuckets; i++) {
        struct bucket *bucket = bucket_at(map, i);
        if (bucket->dib && !iter(bucket_item(bucket), udata)) {
            return false;
        }
    }
    return true;
}

b8 hashmap_iter(struct hashmap_t *map, size_t *i, vptr_t *item) {
    struct bucket *bucket;
    do {
        if (*i >= map->nbuckets) return false;
        bucket = bucket_at(map, *i);
        (*i)++;
    } while (!bucket->dib);
    *item = bucket_item(bucket);
    return true;
}
