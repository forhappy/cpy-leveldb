/*
 * =====================================================================================
 *
 *       Filename:  leveldb-capi-python.c
 *
 *    Description:  python binding for leveldb C API 
 *
 *        Version:  1.0
 *        Created:  08/28/2011 11:31:39 AM
 *       Revision:  r1
 *       Compiler:  gcc
 *
 *         Author:  Fu Haiping
 *        Company:  ICT
 *
 * =====================================================================================
 */
#include <Python.h>
#include "structmember.h"
#include "leveldb/c.h"

typedef struct {
	PyObject_HEAD
	leveldb_t *_db;
	leveldb_options_t *_options;
	leveldb_cache_t *_cache;
	leveldb_readoptions_t *_roptions;
	leveldb_writeoptions_t *_woptions;
	leveldb_env_t *_env;
} LevelDB;

static void LevelDB_dealloc(LevelDB* self)
{
	Py_BEGIN_ALLOW_THREADS
	free(self->_db);
	free(self->_options);
	free(self->_cache);
	free(self->_roptions);
	free(self->_woptions);
	free(self->_env);
	Py_END_ALLOW_THREADS

	self->_db = NULL;
	self->_options = NULL;
	self->_cache = NULL;
	self->_roptions = NULL;
	self->_woptions = NULL;
	self->_env = NULL;

	self->ob_type->tp_free((PyObject *)self);
}


static PyObject* LevelDB_new(PyTypeObject* type, PyObject* args, PyObject* kwds)
{
	LevelDB* self = (LevelDB*)type->tp_alloc(type, 0);

	if (self != NULL) {
		self->_db = NULL;
		self->_options = NULL;
		self->_cache = NULL;
		self->_roptions = NULL;
		self->_woptions = NULL;
		self->_env = NULL;
	}

	return (PyObject*)self;
}


static int LevelDB_init(LevelDB* self, PyObject* args, PyObject* kwds)
{
	// cleanup
	if (self->_db || self->_cache || self->_options
			|| self->_roptions || self->_woptions
			|| self->_env) {
		Py_BEGIN_ALLOW_THREADS
		free(self->_db);
		free(self->_options);
		free(self->_cache);
		free(self->_roptions);
		free(self->_woptions);
		free(self->_env);
		Py_END_ALLOW_THREADS
		self->_db = NULL;
		self->_options = NULL;
		self->_cache = NULL;
		self->_roptions = NULL;
		self->_roptions = NULL;
		self->_env = NULL;
	}

	const char* db_dir = NULL;
	PyObject* create_if_missing = Py_True;
	PyObject* error_if_exists = Py_False;
	PyObject* paranoid_checks = Py_False;
	int block_cache_size = 8 * (2 << 20);
	int write_buffer_size = 4<<20;
	int block_size = 4096;
	int max_open_files = 1000;
	int block_restart_interval = 16;
	PyObject *compression = Py_False;
	const char* kwargs[] = {"filename", "create_if_missing", "error_if_exists", "paranoid_checks", "write_buffer_size", "block_size", "max_open_files", "block_restart_interval", "block_cache_size", "compression", 0};

	if (!PyArg_ParseTupleAndKeywords(args, kwds, (char*)"s|O!O!O!iiiiiO!", (char**)kwargs,
		&db_dir,
		&PyBool_Type, &create_if_missing,
		&PyBool_Type, &error_if_exists,
		&PyBool_Type, &paranoid_checks,
		&write_buffer_size,
		&block_size,
		&max_open_files,
		&block_restart_interval,
		&block_cache_size,
		&PyBool_Type, &compression))
		return -1;

	if (write_buffer_size < 0 || block_size < 0 || max_open_files < 0 || block_restart_interval < 0 || block_cache_size < 0) {
		return -1;
	}

	/* initializing LeveldbType fields */
	self->_options = leveldb_options_create(); 
	self->_cache = leveldb_cache_create_lru(block_cache_size); 
	self->_env = leveldb_create_default_env();

	self->_roptions = leveldb_readoptions_create();
	self->_woptions = leveldb_writeoptions_create();

	if (self->_options == NULL || self->_cache == NULL
			|| self->_roptions == NULL 
			|| self->_woptions == NULL
			|| self->_env == NULL) {
		free(self->_options);
		free(self->_cache);
		free(self->_roptions);
		free(self->_woptions);
		free(self->_env);

		self->_options = NULL;
		self->_cache = NULL;
		self->_roptions = NULL;
		self->_woptions = NULL;
		self->_env = NULL;
		return -1;
	}
	leveldb_options_set_create_if_missing(self->_options, (create_if_missing == Py_True) ? 1 : 0);
	leveldb_options_set_error_if_exists(self->_options, (error_if_exists == Py_True) ? 1 : 0);
	leveldb_options_set_paranoid_checks(self->_options, (paranoid_checks == Py_True) ? 1 : 0);
	leveldb_options_set_write_buffer_size(self->_options, write_buffer_size);
	leveldb_options_set_block_size(self->_options, block_size);
	leveldb_options_set_cache(self->_options, self->_cache);
	leveldb_options_set_max_open_files(self->_options, max_open_files);
	leveldb_options_set_block_restart_interval(self->_options, 8);
	leveldb_options_set_compression(self->_options, (compression == Py_True) ? 1 : 0);
	leveldb_options_set_env(self->_options, self->_env);
	leveldb_options_set_info_log(self->_options, NULL);
	
	char *err = NULL;

	Py_BEGIN_ALLOW_THREADS
	self->_db = leveldb_open(self->_options, db_dir, &err);

	if (err != NULL) {
		free(self->_db);
		free(self->_options);
		free(self->_cache);
		free(self->_roptions);
		free(self->_woptions);
		free(self->_env);

		self->_db = NULL;
		self->_options = NULL;
		self->_cache = NULL;
		self->_roptions = NULL;
		self->_roptions = NULL;
		self->_env = NULL;
	
		fprintf(stderr, "error occurs in opening leveldb:\n\t%s\n", err);
	}
	Py_END_ALLOW_THREADS
	return 0;
}
#define LEVELDB_DEFINE_KVBUF(buf) const char * s_##buf = NULL; size_t i_##buf

static PyObject* LevelDB_Put(LevelDB* self, PyObject* args, PyObject* kwds)
{
	const char* kwargs[] = {"key", "value", "sync", 0};
	PyObject* sync = Py_False;

	char *err = NULL;
	LEVELDB_DEFINE_KVBUF(key);
	LEVELDB_DEFINE_KVBUF(value);


	if (!PyArg_ParseTupleAndKeywords(args, kwds, (char*)"s#s#|O!", (char**)kwargs, &s_key, &i_key, &s_value, &i_value, &PyBool_Type, &sync)) {
		return 0;
	}

	printf("in Put: key = %s, value = %s\n", s_key, s_value);
	Py_BEGIN_ALLOW_THREADS
	leveldb_writeoptions_set_sync(self->_woptions, (sync == Py_True) ? 1 : 0);

	leveldb_put(self->_db, self->_woptions, (const char *)s_key, (size_t)i_key, (const char *)s_value, (size_t)i_value, &err);
	Py_END_ALLOW_THREADS
	if (err != NULL) {
		fprintf(stderr, "error occurs when put:\n\t%s\n", err);
	}

	Py_INCREF(Py_None);
	return Py_None;
}


static PyObject* LevelDB_Get(LevelDB* self, PyObject* args, PyObject* kwds)
{
	PyObject* verify_checksums = Py_False;
	PyObject* fill_cache = Py_True;
	const char* kwargs[] = {"key", "verify_checksums", "fill_cache", 0};

	char *value;
	size_t value_len;
	char *err = NULL;
	LEVELDB_DEFINE_KVBUF(key);

	if (!PyArg_ParseTupleAndKeywords(args, kwds, (char*)"s#|O!O!", (char**)kwargs, &s_key, &i_key, &PyBool_Type, &verify_checksums, &PyBool_Type, &fill_cache))
		return 0;

	Py_BEGIN_ALLOW_THREADS

	leveldb_readoptions_set_verify_checksums(self->_roptions, (verify_checksums == Py_True) ? 1 : 0);
	leveldb_readoptions_set_fill_cache(self->_roptions, (fill_cache == Py_True) ? 1 : 0);

	value = leveldb_get(self->_db, self->_roptions, (const char *)s_key, (size_t)i_key, &value_len, &err);
	Py_END_ALLOW_THREADS

	if (err != NULL) {
		fprintf(stderr, "error occurs get:\n\t%s\n", err);
	}
	return PyString_FromStringAndSize(value, value_len);
}

static PyObject* LevelDB_Delete(LevelDB* self, PyObject* args, PyObject* kwds)
{
	PyObject* sync = Py_False;
	const char* kwargs[] = {"key", "sync", 0};

	LEVELDB_DEFINE_KVBUF(key);	


	char *err = NULL;
	if (!PyArg_ParseTupleAndKeywords(args, kwds, (char*)"s#|O!", (char**)kwargs, &s_key, &i_key, &PyBool_Type, &sync))
		return 0;
	
	leveldb_writeoptions_set_sync(self->_woptions, (sync == Py_True) ? 1 : 0);

	Py_BEGIN_ALLOW_THREADS

	leveldb_delete(self->_db, self->_woptions, (const char *)s_key, (size_t)i_key, &err);

	Py_END_ALLOW_THREADS


	if (err != NULL) {
		fprintf(stderr, "error occurs when delete:\n\t%s\n", err);
	}

	Py_INCREF(Py_None);
	return Py_None;
}


static PyMethodDef LevelDB_methods[] = {
	{(char*)"Put",       (PyCFunction)LevelDB_Put,       METH_KEYWORDS, (char*)"add a key/value pair to database, with an optional synchronous disk write" },
	{(char*)"Get",       (PyCFunction)LevelDB_Get,       METH_KEYWORDS, (char*)"get a value from the database" },
	{(char*)"Delete",    (PyCFunction)LevelDB_Delete,    METH_KEYWORDS, (char*)"delete a value in the database" },
	{NULL}
};
PyTypeObject LevelDBType = {
	PyObject_HEAD_INIT(NULL)
	0,                             /*ob_size*/
	(char*)"leveldb.LevelDB",      /*tp_name*/
	sizeof(LevelDB),             /*tp_basicsize*/
	0,                             /*tp_itemsize*/
	(destructor)LevelDB_dealloc, /*tp_dealloc*/
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
	(char*)"LevelDB bindings",   /*tp_doc */
	0,                             /*tp_traverse */
	0,                             /*tp_clear */
	0,                             /*tp_richcompare */
	0,                             /*tp_weaklistoffset */
	0,                             /*tp_iter */
	0,                             /*tp_iternext */
	LevelDB_methods,             /*tp_methods */
	0,                             /*tp_members */
	0,                             /*tp_getset */
	0,                             /*tp_base */
	0,                             /*tp_dict */
	0,                             /*tp_descr_get */
	0,                             /*tp_descr_set */
	0,                             /*tp_dictoffset */
	(initproc)LevelDB_init,      /*tp_init */
	0,                             /*tp_alloc */
	LevelDB_new,                 /*tp_new */
};

#define LevelDB_Check(op) PyObject_TypeCheck(op, &LevelDBType)

PyMODINIT_FUNC
initleveldb(void)
{
	PyObject* leveldb_module = Py_InitModule3((char*)"leveldb", LevelDB_methods, 0);

	if (leveldb_module == 0)
		return;

	if (PyType_Ready(&LevelDBType) < 0)
		return;


	// add custom types to the different modules
	Py_INCREF(&LevelDBType);

	if (PyModule_AddObject(leveldb_module, (char*)"LevelDB", (PyObject*)&LevelDBType) != 0)
		return;

}

