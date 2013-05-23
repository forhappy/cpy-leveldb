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
#include "write_batch.h"

static void WriteBatch_dealloc(WriteBatch* self)
{
	Py_BEGIN_ALLOW_THREADS
	if (self->_writebatch != NULL) {
		leveldb_writebatch_destroy(self->_writebatch);
		self->_writebatch = NULL;
		_XDECREF(self->_writebatch);
	}
	Py_END_ALLOW_THREADS

	self->_writebatch = NULL;

	self->ob_type->tp_free(self);
}


static PyObject* WriteBatch_new(PyTypeObject* type, PyObject* args, PyObject* kwds)
{
	WriteBatch* self = (WriteBatch *)type->tp_alloc(type, 0);

	if (self != NULL) {
		self->_writebatch = NULL;
	}
	return (PyObject*)self;
}

static int WriteBatch_init(WriteBatch* self, PyObject* args, PyObject* kwds)
{
	static char* kwargs[] = {0};

	if (self->_writebatch != NULL) {
		Py_BEGIN_ALLOW_THREADS
		leveldb_writebatch_destroy(self->_writebatch);
		self->_writebatch = NULL;
		_XDECREF(self->_writebatch);
		Py_END_ALLOW_THREADS
		self->_writebatch = NULL;
	}
	if (!PyArg_ParseTupleAndKeywords(args, kwds, (char*)"", kwargs))
		return -1;

	self->_writebatch = leveldb_writebatch_create();
	if (self->_writebatch == NULL) {
		PyErr_Format(LevelDBError, "Failed to create writebatch.\n");
		return -1;
	}
	return 0;
}

static PyObject * WriteBatch_Put(WriteBatch *self, PyObject *args)
{

	LEVELDB_DEFINE_KVBUF(key);
	LEVELDB_DEFINE_KVBUF(value);

	if (!PyArg_ParseTuple(args, (char*)"t#t#", &s_key, &i_key, &s_value, &i_value))
		return NULL;

	if (self->_writebatch != NULL) {
		leveldb_writebatch_put(self->_writebatch, (const char *)s_key, (size_t)i_key, \
			(const char *)s_value, (size_t)i_value);
	} else 
		return NULL;
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject * WriteBatch_Delete(WriteBatch *self, PyObject *args)
{

	LEVELDB_DEFINE_KVBUF(key);

	if (!PyArg_ParseTuple(args, (char*)"t#", &s_key, &i_key))
		return NULL;

	if (self->_writebatch != NULL) {
		leveldb_writebatch_delete(self->_writebatch, (const char *)s_key, (size_t)i_key);
	} else 
		return NULL;

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject * WriteBatch_Clear(WriteBatch *self, PyObject *args)
{
	if (self->_writebatch != NULL) {
		leveldb_writebatch_clear(self->_writebatch);
	} else 
		return NULL;

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject * WriteBatch_Release(WriteBatch *self, PyObject *args)
{
	if (self->_writebatch != NULL) {
		leveldb_writebatch_destroy(self->_writebatch);
	} else 
		return NULL;

	Py_INCREF(Py_None);
	return Py_None;
}
PyMethodDef WriteBatch_methods[] = {
	{(char*)"Put",    (PyCFunction)WriteBatch_Put,    METH_VARARGS, (char*)"add a put operation to batch" },
	{(char*)"Delete", (PyCFunction)WriteBatch_Delete, METH_VARARGS, (char*)"add a delete operation to batch" },
	{(char*)"Clear",    (PyCFunction)WriteBatch_Clear,    METH_VARARGS, (char*)"clear batch" },
	{(char*)"Release",    (PyCFunction)WriteBatch_Release,    METH_VARARGS, (char*)"release a batch" },
	{NULL}
};

PyTypeObject WriteBatchType = {
	PyObject_HEAD_INIT(NULL)
	0,                             /*ob_size*/
	(char*)"leveldb.WriteBatch",      /*tp_name*/
	sizeof(WriteBatch),             /*tp_basicsize*/
	0,                             /*tp_itemsize*/
	(destructor)WriteBatch_dealloc, /*tp_dealloc*/
	0,                             /*tp_print*/
	0,                             /*tp_getattr*/
	0,                             /*tp_setattr*/
	0,                             /*tp_compare*/
	0,                             /*tp_repr*/
	0,                             /*tp_as_number*/
	0,                             /*tp_as_sequence*/
	0,                             /*tp_as_mapping*/
	0,                             /*tp_hash */
	0,                             /*tp_call*/
	0,                             /*tp_str*/
	0,                             /*tp_getattro*/
	0,                             /*tp_setattro*/
	0,                             /*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT,            /*tp_flags*/
	(char*)"leveldb WriteBatch",   /*tp_doc */
	0,                             /*tp_traverse */
	0,                             /*tp_clear */
	0,                             /*tp_richcompare */
	0,                             /*tp_weaklistoffset */
	0,                             /*tp_iter */
	0,                             /*tp_iternext */
	WriteBatch_methods,             /*tp_methods */
	0,                             /*tp_members */
	0,                             /*tp_getset */
	0,                             /*tp_base */
	0,                             /*tp_dict */
	0,                             /*tp_descr_get */
	0,                             /*tp_descr_set */
	0,                             /*tp_dictoffset */
	(initproc)WriteBatch_init,      /*tp_init */
	0,                             /*tp_alloc */
	WriteBatch_new,                 /*tp_new */
};

