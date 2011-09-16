/*
 * =====================================================================================
 *
 *       Filename:  comparator.h
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

#ifndef _COMPARATOR_H
#define _COMPARATOR_H
#include "leveldb.h"

typedef struct {
	PyObject_HEAD
	leveldb_comparator_t *_comparator;
} Comparator;

#endif
