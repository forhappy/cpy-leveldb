/**
 * cpy-leveldb: Python bindings for leveldb using leveldb c api
 * (http://code.google.com/p/leveldb/)
 *
 * Copyright (C) 2011-2013  Fu Haiping (haipingf@gmail.com)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301 USA
 **/

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
