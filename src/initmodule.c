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
#include "iterator.h"
#include "snapshot.h"

PyObject *LevelDBError = NULL;

PyMODINIT_FUNC
initleveldb(void)
{
	PyObject *dict, *value;
	PyObject* leveldb_module = Py_InitModule3((char*)"leveldb", LevelDB_methods, 0);

	if (leveldb_module == 0)
		return;

	if (PyType_Ready(&LevelDBType) < 0)
		return;

	if (PyType_Ready(&WriteBatchType) < 0)
		return;

	if (PyType_Ready(&SnapshotType) < 0)
		return;

	if (PyType_Ready(&IteratorType) < 0)
		return;
	dict = PyModule_GetDict(leveldb_module);

	value = PyString_FromString("Fu Haiping <haipingf@gmail.com>");
	PyDict_SetItemString(dict, "__author__", value);
	Py_DECREF(value);

	value = PyString_FromString(CPY_LEVELDB_MODULE_VERSION);
	PyDict_SetItemString(dict, "__version__", value);
	Py_DECREF(value);


	value = PyString_FromString(CPY_LEVELDB_VERSION_DATE);
	PyDict_SetItemString(dict, "__date__", value);
	Py_DECREF(value);

	LevelDBError = PyErr_NewException((char *)"leveldb.LevelDBError", NULL, NULL);
	if (LevelDBError == NULL) {
		PyErr_Format(LevelDBError, "Failed to create LevelDBError.\n");
	}
	// add custom types to the different modules
	Py_INCREF(&LevelDBType);
	if (PyModule_AddObject(leveldb_module, (char*)"LevelDB", (PyObject*)&LevelDBType) != 0)
		return;

	Py_INCREF(&WriteBatchType);
	if (PyModule_AddObject(leveldb_module, (char*)"WriteBatch", (PyObject*)&WriteBatchType) != 0)
		return;

	Py_INCREF(&SnapshotType);
	if (PyModule_AddObject(leveldb_module, (char*)"Snapshot", (PyObject*)&SnapshotType) != 0)
		return;

	Py_INCREF(&IteratorType);
	if (PyModule_AddObject(leveldb_module, (char*)"Iterator", (PyObject*)&IteratorType) != 0)
		return;

	Py_INCREF(LevelDBError);
	if (PyModule_AddObject(leveldb_module, (char *)"LevelDBError", LevelDBError) != 0)
		return;
}
/* 
 * vim:ts=4:sw=4
 **/
