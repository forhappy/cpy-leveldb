/*
 * =====================================================================================
 *
 *       Filename:  iterator.c
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
#include "leveldb.h"
#include "iterator.h"

static void Iterator_dealloc(Iterator *self)
{
	Py_BEGIN_ALLOW_THREADS
	if (self->_iterator != NULL) {
		leveldb_iter_destroy(self->_iterator);
		self->_iterator = NULL;
		_XDECREF(self->_iterator);
	}

	Py_END_ALLOW_THREADS

	self->_iterator = NULL;

	self->ob_type->tp_free(self);
}

static PyObject* Iterator_new(PyTypeObject* type, PyObject* args, PyObject* kwds)
{
	Iterator * self = (Iterator *)type->tp_alloc(type, 0);

	if (self != NULL) {
		self->_iterator= NULL;
	}
	return (PyObject*)self;
}

static int Iterator_init(Iterator *self, PyObject* args, PyObject* kwds)
{
	static char* kwargs[] = {"leveldb", 0};
	LevelDB *leveldb = NULL;
	leveldb_iterator_t *iterator = NULL;
	leveldb_readoptions_t *roptions = NULL;

	if (self->_iterator) {
		Py_BEGIN_ALLOW_THREADS
		_XDECREF(self->_iterator);
		Py_END_ALLOW_THREADS
		
		self->_iterator= NULL;
	}

	roptions = leveldb_readoptions_create();
	if (roptions == NULL) {
		PyErr_Format(LevelDBError, "Failed to create readoptions.\n");
		return -1;
	}

	if (!PyArg_ParseTupleAndKeywords(args, kwds, (const char*)"O!", kwargs, &LevelDBType, &leveldb))
		return -1;
	if (leveldb != NULL) {
		iterator = leveldb_create_iterator(leveldb->_db, roptions);
		self->_iterator = iterator;
	}
	if (self->_iterator == NULL) {
		PyErr_Format(LevelDBError, "Failed to create snapshot.\n");
		return -1;
	}

	return 0;
}
static PyObject * Iterator_Valid(Iterator *self, PyObject *args)
{
	PyObject *result;
	unsigned char valid;
	if (self->_iterator != NULL) {
		valid = leveldb_iter_valid(self->_iterator);
	} else
		return NULL;
	result = Py_BuildValue("b", valid);
	return result;
}

static PyObject * Iterator_Seek_To_First(Iterator *self, PyObject *args)
{
	if (self->_iterator != NULL) {	
		leveldb_iter_seek_to_first(self->_iterator);
	} else 
		return NULL;

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject * Iterator_Seek_To_Last(Iterator *self, PyObject *args)
{
	if (self->_iterator != NULL) {
		leveldb_iter_seek_to_last(self->_iterator);
	} else
		return NULL;

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject * Iterator_Next(Iterator *self, PyObject *args)
{
	if (self->_iterator != NULL) {
		leveldb_iter_next(self->_iterator);
	} else 
		return NULL;

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject * Iterator_Prev(Iterator *self, PyObject *args)
{
	if (self->_iterator != NULL) {
		leveldb_iter_prev(self->_iterator);
	} else
		return NULL;

	Py_INCREF(Py_None);
	return Py_None;
}


static PyObject * Iterator_Seek(Iterator *self, PyObject *args)
{

	LEVELDB_DEFINE_KVBUF(key);

	if (!PyArg_ParseTuple(args, (char*)"t#", &s_key, &i_key))
		return NULL;

	if (self->_iterator != NULL) {
		leveldb_iter_seek(self->_iterator, s_key, i_key);
	} else 
		return NULL;

	Py_INCREF(Py_None);
	return Py_None;
}


static PyObject * Iterator_Get_Error(Iterator *self, PyObject *args)
{

	PyObject *result;
	char *err = NULL;

	if (self->_iterator != NULL) {
		leveldb_iter_get_error(self->_iterator, &err);
	} else 
		return NULL;
	result = Py_BuildValue("s#", err, strlen(err));
	_XDECREF(err);
	Py_INCREF(result);
	return result;
}



static PyObject * Iterator_Key(Iterator *self, PyObject *args)
{

	PyObject *result;

	const char *key;
	size_t len;
	if (self->_iterator != NULL) {
		key = leveldb_iter_key(self->_iterator, &len);
	} else 
		return NULL;
	result = Py_BuildValue("s#", key, len);
	return result;
}


static PyObject * Iterator_Value(Iterator *self, PyObject *args)
{

	PyObject *result;

	const char *value;
	size_t len;
	if (self->_iterator != NULL) {
		value = leveldb_iter_value(self->_iterator, &len);
	} else 
		return NULL;
	result = Py_BuildValue("s#", value, len);
	return result;
}


static PyObject * Iterator_Destroy(Iterator *self, PyObject *args)
{

	if (self->_iterator != NULL) {
		leveldb_iter_destroy(self->_iterator);
	} else 
		return NULL;
	Py_INCREF(Py_None);
	return Py_None;
}

PyMethodDef Iterator_methods[] = {
	{(char*)"Validate",    (PyCFunction)Iterator_Valid,    METH_KEYWORDS, (char*)"validate iterator" },
	{(char*)"First",    (PyCFunction)Iterator_Seek_To_First,    METH_KEYWORDS, (char*)"seek to first" },
	{(char*)"Last",    (PyCFunction)Iterator_Seek_To_Last,    METH_KEYWORDS, (char*)"seek to last" },
	{(char*)"Seek",    (PyCFunction)Iterator_Seek,    METH_KEYWORDS, (char*)"iterator seek" },
	{(char*)"Next",    (PyCFunction)Iterator_Next,    METH_KEYWORDS, (char*)"iterator to next" },
	{(char*)"Prev",    (PyCFunction)Iterator_Prev,    METH_KEYWORDS, (char*)"iterator to prev" },
	{(char*)"Key",    (PyCFunction)Iterator_Key,    METH_KEYWORDS, (char*)"get key throuth current iterator" },
	{(char*)"Value",    (PyCFunction)Iterator_Value,    METH_KEYWORDS, (char*)"get value throuth current iterator" },
	{(char*)"GetError",    (PyCFunction)Iterator_Get_Error,    METH_KEYWORDS, (char*)"get iterator error" },
	{(char*)"Destroy",    (PyCFunction)Iterator_Destroy,    METH_KEYWORDS, (char*)"destroy iterator" },
	{NULL}
};

PyTypeObject IteratorType = {
	PyObject_HEAD_INIT(NULL)
	0,                             /*ob_size*/
	(char*)"leveldb.Iterator",      /*tp_name*/
	sizeof(Iterator),             /*tp_basicsize*/
	0,                             /*tp_itemsize*/
	(destructor)Iterator_dealloc, /*tp_dealloc*/
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
	(char*)"leveldb Iterator",   /*tp_doc */
	0,                             /*tp_traverse */
	0,                             /*tp_clear */
	0,                             /*tp_richcompare */
	0,                             /*tp_weaklistoffset */
	0,                             /*tp_iter */
	0,                             /*tp_iternext */
	Iterator_methods,             /*tp_methods */
	0,                             /*tp_members */
	0,                             /*tp_getset */
	0,                             /*tp_base */
	0,                             /*tp_dict */
	0,                             /*tp_descr_get */
	0,                             /*tp_descr_set */
	0,                             /*tp_dictoffset */
	(initproc)Iterator_init,      /*tp_init */
	0,                             /*tp_alloc */
	Iterator_new,                 /*tp_new */
};

