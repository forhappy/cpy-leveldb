/*
 * =============================================================================
 *
 *       Filename:  range_iterator.h
 *
 *    Description:  range iterator for leveldb.
 *
 *        Version:  0.4.0
 *        Created:  01/22/2012 12:14:42 PM
 *       Revision:  r1
 *       Compiler:  gcc (Ubuntu/Linaro 4.4.4-14ubuntu5) 4.4.5
 *
 *         Author:  Fu Haiping (forhappy), haipingf@gmail.com
 *        Company:  ICT ( Institute Of Computing Technology, CAS )
 *
 * =============================================================================
 */
#ifndef _RANGE_ITERATOR_H
#define _RANGE_ITERATOR_H
#include "leveldb.h"


typedef struct {
	PyObject_HEAD
    LevelDB *_db;
	leveldb_iterator_t *_iterator;
    char *_to;
    int _include_value;
} RangeIterator;

extern PyObject* RangeIterator_new(LevelDB *db, leveldb_iterator_t *iterator, char *to, int include_value);
#endif
