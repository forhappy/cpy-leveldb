/*
 * =====================================================================================
 *
 *       Filename:  leveldb.h
 *
 *    Description:  python binding for leveldb based on c api.
 *
 *        Version:  0.3.0
 *        Created:  09/16/2011 05:44:40 PM
 *       Revision:  r15
 *       Compiler:  gcc
 *
 *         Author:  Fu Haiping <haipingf@gmail.com>
 *        Company:  ICT
 *
 * =====================================================================================
 */
#ifndef _CPY_LEVELDB_H
#define _CPY_LEVELDB_H
#include <Python.h>
#include "structmember.h"
#include "leveldb/c.h"

/* switch CPY_LEVLEDB_DEBUG off to surpress debug information. */
#define CPY_LEVLEDB_DEBUG 1 

/* switch ENABLE_COMPARATOR off to compile without comparator enabled. */
//#define ENABLE_COMPARATOR 1 


/* Helper micros */
#if !defined(_XDECREF)
#define _XDECREF(ptr) do { if ((ptr) != NULL) free((ptr));} while(0)
#else
#undef _XDECREF
#define _XDECREF(ptr) do { if ((ptr) != NULL) free((ptr));} while(0)
#endif

#define LEVELDB_DEFINE_KVBUF(buf) const char * s_##buf = NULL; size_t i_##buf

/* Version micros */
#define CPY_LEVELDB_MODULE_VERSION "0.3.0"
#define CPY_LEVELDB_VERSION_DATE   "2011-09-16"

/* Type check micros */
#define LevelDB_Check(op) PyObject_TypeCheck(op, &LevelDBType)
#define WriteBatch_Check(op) PyObject_TypeCheck(op, &WriteBatchType)
#define Snapshot_Check(op) PyObject_TypeCheck(op, &SnapshotType)
#define Iterator_Check(op) PyObject_TypeCheck(op, &IteratorType)
#define Comparator_Check(op) PyObject_TypeCheck(op, &ComparatorType)

/* Debug utils. */
#ifdef CPY_LEVLEDB_DEBUG
 #define VERBOSE(format, ...) fprintf(stderr,\
		 "---***---\nFilename:%s\nLine number:%d\nFunction name:%s\n***---***\n"format"\n\n",\
		 __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#else
 #define VERBOSE(format, ...) (void)0 /* do nothing */
#endif

/* callback function pointer used in Comparator. */
extern PyObject *desctructor_callback;
extern PyObject *compare_callbak;
extern PyObject *name_callback;

/* Types. */
extern PyTypeObject LevelDBType;
extern PyTypeObject WriteBatchType;
extern PyTypeObject SnapshotType;
extern PyTypeObject IteratorType;
extern PyTypeObject ComparatorType;

/* Object methods */


extern PyMethodDef LevelDB_methods[];
extern PyMethodDef WriteBatch_methods[];
extern PyMethodDef Snapshot_methods[];
extern PyMethodDef Iterator_methods[];
extern PyMethodDef Comparator_methods[];

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
#include "comparator.h"
#endif
