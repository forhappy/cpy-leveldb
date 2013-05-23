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

#include "leveldb.h"
#include "range_iterator.h"

static void RangeIterator_clean(RangeIterator *iter)
{
    VERBOSE("%s", "Entering RangeIterator_clean...");
    Py_XDECREF(iter->_db);
	_XDECREF(iter->_iterator);
	_XDECREF(iter->_to);
	iter->_db = NULL;
	iter->_iterator = NULL;
	iter->_to = NULL;
	iter->_include_value = 0;
    VERBOSE("%s", "Step out of RangeIterator_clean...");
}

static void RangeIterator_dealloc(RangeIterator *self)
{
    VERBOSE("%s", "Entering RangeIterator_dealloc...");
	Py_BEGIN_ALLOW_THREADS
	if (self->_iterator != NULL) {
		leveldb_iter_destroy(self->_iterator);
		self->_iterator = NULL;
		_XDECREF(self->_iterator);
	}

	Py_END_ALLOW_THREADS

    RangeIterator_clean(self);
    PyObject_GC_Del(self);

//	self->_iterator = NULL;

//	self->ob_type->tp_free(self);
}


static int RangeIterator_traverse(RangeIterator* iter, visitproc visit, void* arg)
{

    VERBOSE("%s", "Entering RangeIterator_traverse...");
	Py_VISIT((PyObject*)iter->_db);
	return 0;
}

static PyObject* RangeIterator_next(RangeIterator  *iter)
{

    VERBOSE("%s", "Entering RangeIterator_next...");
	if (iter->_db == 0 || !leveldb_iter_valid(iter->_iterator)) {
		RangeIterator_clean(iter);
		return 0;
	}
	if (iter->_to != NULL) {
        const char *tkey; /* c style key */
        size_t key_len; /* key length */
        if (iter->_iterator != NULL) {
            tkey = leveldb_iter_key(iter->_iterator, &key_len);
        } else return NULL;
      
        int limit_len = (strlen(iter->_to) > key_len) ? key_len: strlen(iter->_to);
 
        int c = memcmp(iter->_to, tkey, limit_len);
		if (!(0 <= c)) {
            VERBOSE("%s", "Invalid iterator................................");
			RangeIterator_clean(iter);
			return 0;
		}
	}
 

	// get key and (optional) value
    const char *ckey; /* c style key */
    size_t key_len; /* key length */
	if (iter->_iterator != NULL) {
		ckey = leveldb_iter_key(iter->_iterator, &key_len);
	} else return NULL;

    VERBOSE("The key is %s, length is %d", ckey, key_len);
	PyObject* key = PyString_FromStringAndSize(ckey, key_len);
	PyObject* value = 0;
	PyObject* ret = key;

	if (key == 0) return 0;

	const char *cvalue; /* c style value */
	size_t value_len; /* value length */
	if (iter->_iterator != NULL) {
		cvalue = leveldb_iter_value(iter->_iterator, &value_len);
	} else return NULL;
	if (iter->_include_value) {
		value = PyString_FromStringAndSize(cvalue, value_len);

		if (value == 0) {
			Py_XDECREF(key);
			return 0;
		}
	}

	// key/value pairs are returned as 2-tuples
	if (value != NULL) {
		ret = PyTuple_New(2);

		if (ret == 0) {
			Py_DECREF(key);
			Py_XDECREF(value);
			return 0;
		}

		PyTuple_SET_ITEM(ret, 0, key);
		PyTuple_SET_ITEM(ret, 1, value);
	}

	// get next value
    leveldb_iter_next(iter->_iterator);

	// return k/v pair or single key
	return ret;
}

PyTypeObject RangeIterator_Type = {
    PyObject_HEAD_INIT(NULL)
	0,
    (char*)"leveldb.RangeIterator",       /* tp_name */
    sizeof(RangeIterator),             /* tp_basicsize */
    0,                               /* tp_itemsize */
    (destructor)RangeIterator_dealloc, /* tp_dealloc */
    0,                               /* tp_print */
    0,                               /* tp_getattr */
    0,                               /* tp_setattr */
    0,                               /* tp_compare */
    0,                               /* tp_repr */
    0,                               /* tp_as_number */
    0,                               /* tp_as_sequence */
    0,                               /* tp_as_mapping */
    0,                               /* tp_hash */
    0,                               /* tp_call */
    0,                               /* tp_str */
    PyObject_GenericGetAttr,         /* tp_getattro */
    0,                               /* tp_setattro */
    0,                               /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT  | Py_TPFLAGS_HAVE_GC, /* tp_flags */
    0,                               /* tp_doc */
    (traverseproc)RangeIterator_traverse,  /* tp_traverse */
    0,                               /* tp_clear */
    0,                               /* tp_richcompare */
    0,                               /* tp_weaklistoffset */
    PyObject_SelfIter,               /* tp_iter */
    (iternextfunc)RangeIterator_next,  /* tp_iternext */
    0,                               /* tp_methods */
    0,
};

PyObject* RangeIterator_new(LevelDB *db, leveldb_iterator_t *iterator, char *to, int include_value)
{

    VERBOSE("%s", "Entering RangeIterator_new...");
	RangeIterator* iter = PyObject_GC_New(RangeIterator, &RangeIterator_Type);

	if (iter == 0) return NULL;

	Py_XINCREF(db);
	iter->_db = db;
	iter->_iterator = iterator;
	iter->_to = to;
	iter->_include_value = include_value;
	PyObject_GC_Track(iter);
	return (PyObject*)iter;
}

