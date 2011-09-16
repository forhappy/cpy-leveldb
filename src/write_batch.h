/*
 * =====================================================================================
 *
 *       Filename:  write_batch.h
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
#ifndef _WRITE_BATCH_H
#define _WRITE_BATCH_H
#include "leveldb.h"

typedef struct {
	PyObject_HEAD
	leveldb_writebatch_t *_writebatch;
}WriteBatch;

#endif
