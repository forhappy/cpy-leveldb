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
#include "snapshot.h"

static void Snapshot_dealloc(Snapshot *self)
{
	Py_BEGIN_ALLOW_THREADS

	_XDECREF((void *)self->_snapshot);

	Py_END_ALLOW_THREADS

	self->_leveldb = NULL;
	self->_snapshot = NULL;

	self->ob_type->tp_free(self);
}

static PyObject* Snapshot_new(PyTypeObject* type, PyObject* args, PyObject* kwds)
{
	Snapshot * self = (Snapshot *)type->tp_alloc(type, 0);

	if (self != NULL) {
		self->_leveldb = NULL;
		self->_snapshot = NULL;
	}
	return (PyObject*)self;
}

static int Snapshot_init(Snapshot* self, PyObject* args, PyObject* kwds)
{
	static char* kwargs[] = {"db", 0};
	LevelDB *leveldb = NULL;

	if (self->_snapshot) {
		Py_BEGIN_ALLOW_THREADS
		_XDECREF((void *)self->_snapshot);
		Py_END_ALLOW_THREADS
		
		self->_snapshot = NULL;
		self->_leveldb = NULL;
	}
	if (!PyArg_ParseTupleAndKeywords(args, kwds, (const char*)"O!", kwargs, &LevelDBType, &leveldb))
		return -1;

	if (leveldb != NULL && leveldb->_db != NULL) {
		self->_leveldb = leveldb;
		self->_snapshot = leveldb_create_snapshot(leveldb->_db);
		assert(self->_snapshot != NULL);
	}

	if (self->_snapshot== NULL ) {
		PyErr_Format(LevelDBError, "Failed to create snapshot.\n");
		return -1;
	}

	return 0;
}

static PyObject * Snapshot_Set(Snapshot *self, PyObject *args)
{

	assert(self->_snapshot != NULL);

	if (self->_snapshot != NULL ) {
		leveldb_readoptions_set_snapshot(self->_leveldb->_roptions, self->_snapshot);
	} else {
		PyErr_Format(LevelDBError, "Unable to set snapshot.\n");
		return NULL;
	}

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject * Snapshot_Reset(Snapshot *self, PyObject *args)
{

	assert(self->_snapshot != NULL);

	leveldb_readoptions_set_snapshot(self->_leveldb->_roptions, NULL);

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject * Snapshot_Release(Snapshot *self, PyObject *args)
{
	leveldb_release_snapshot(self->_leveldb->_db, self->_snapshot);
	Py_INCREF(Py_None);
	return Py_None;
}

PyMethodDef Snapshot_methods[] = {
	{(char*)"Set",    (PyCFunction)Snapshot_Set,    METH_VARARGS, (char*)"set snapshot" },
	{(char*)"Reset", (PyCFunction)Snapshot_Reset, METH_VARARGS, (char*)"reset snapshot to the current state" },
	{(char*)"Release",    (PyCFunction)Snapshot_Release,    METH_VARARGS, (char*)"release snapshot" },
	{NULL}
};

PyTypeObject SnapshotType = {
	PyObject_HEAD_INIT(NULL)
	0,                             /*ob_size*/
	(char*)"leveldb.Snapshot",      /*tp_name*/
	sizeof(Snapshot),             /*tp_basicsize*/
	0,                             /*tp_itemsize*/
	(destructor)Snapshot_dealloc, /*tp_dealloc*/
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
	(char*)"leveldb Snapshot",   /*tp_doc */
	0,                             /*tp_traverse */
	0,                             /*tp_clear */
	0,                             /*tp_richcompare */
	0,                             /*tp_weaklistoffset */
	0,                             /*tp_iter */
	0,                             /*tp_iternext */
	Snapshot_methods,             /*tp_methods */
	0,                             /*tp_members */
	0,                             /*tp_getset */
	0,                             /*tp_base */
	0,                             /*tp_dict */
	0,                             /*tp_descr_get */
	0,                             /*tp_descr_set */
	0,                             /*tp_dictoffset */
	(initproc)Snapshot_init,      /*tp_init */
	0,                             /*tp_alloc */
	Snapshot_new,                 /*tp_new */
};

