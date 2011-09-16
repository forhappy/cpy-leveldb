/*
 * =====================================================================================
 *
 *       Filename:  snapshot.h
 *
 *    Description:  python binding for leveldb based on c api.
 *
 *        Version:  0.3.0
 *        Created:  09/16/2011 08:18:48 PM
 *       Revision:  r16 
 *       Compiler:  gcc
 *
 *         Author:  Fu Haiping <haipingf@gmail.com> 
 *        Company:  ICT
 *
 * =====================================================================================
 */
#ifndef _SNAPSHOT_T
#define _SNAPSHOT_T 

#include "leveldb.h"

typedef struct {
	PyObject_HEAD
	LevelDB *_leveldb;
	const leveldb_snapshot_t *_snapshot;
} Snapshot;

#endif
