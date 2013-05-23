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

#ifndef _CPY_LEVELDB_H
#define _CPY_LEVELDB_H
#include <Python.h>
#include "structmember.h"
#include "leveldb/c.h"

/* switch CPY_LEVLEDB_DEBUG off to surpress debug information. */
//#define CPY_LEVLEDB_DEBUG 1 

/* switch ENABLE_COMPARATOR off to compile without comparator enabled. */
//#define ENABLE_COMPARATOR 1 


/* Helper micros */
#if !defined(_XDECREF)
#define _XDECREF(ptr) do { if ((ptr) != NULL) free((ptr));} while(0)
#else
#undef _XDECREF
#define _XDECREF(ptr) do { if ((ptr) != NULL) free((ptr));} while(0)
#endif

#define LEVELDB_DEFINE_KVBUF(buf) const char * s_##buf = NULL; size_t i_##buf = 0

/* Version micros */
#define CPY_LEVELDB_MODULE_VERSION "0.4.0"
#define CPY_LEVELDB_VERSION_DATE   "2012-1-21"

/* Type check micros */
#define LevelDB_Check(op) PyObject_TypeCheck(op, &LevelDBType)
#define WriteBatch_Check(op) PyObject_TypeCheck(op, &WriteBatchType)
#define Snapshot_Check(op) PyObject_TypeCheck(op, &SnapshotType)
#define Iterator_Check(op) PyObject_TypeCheck(op, &IteratorType)

/* Debug utils. */
#ifdef CPY_LEVELDB_DEBUG
 #define VERBOSE(format, ...) fprintf(stderr,\
		 "---***---\nFilename:%s\nLine number:%d\nFunction name:%s\n***---***\n"format"\n\n",\
		 __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#else
 #define VERBOSE(format, ...) (void)0 /* do nothing */
#endif


/* Types. */
extern PyTypeObject LevelDBType;
extern PyTypeObject WriteBatchType;
extern PyTypeObject SnapshotType;
extern PyTypeObject IteratorType;

/* Object methods */


extern PyMethodDef LevelDB_methods[];
extern PyMethodDef WriteBatch_methods[];
extern PyMethodDef Snapshot_methods[];
extern PyMethodDef Iterator_methods[];

/* Error object. */
extern PyObject *LevelDBError;

/* LevelDB definition. */
typedef struct {
	PyObject_HEAD
	leveldb_t *_db;
	leveldb_options_t *_options;
	leveldb_cache_t *_cache;
	leveldb_env_t *_env;

	/* In order to support snapshot, we have to put leveldb_readoptions_t *_roptions 
	 * in LevelDB struct to record the readoptions operation 
	 * in LevelDB_Get(), since we must use the same readoptions
	 * struct in Snapshot_Set as we use in LevelDB_Get. 
	 *
	 * */
	leveldb_readoptions_t *_roptions;

} LevelDB;

#include "iterator.h"
#include "snapshot.h"
#include "write_batch.h"
#include "range_iterator.h"
#endif
