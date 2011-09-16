/*
 * =====================================================================================
 *
 *       Filename:  comparator.c
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
#include "comparator.h"

/* callback object used for comparator. */
PyObject *desctructor_callback = NULL;
PyObject *compare_callbak = NULL;
PyObject *name_callback = NULL;

/* helper functions used for comparator. */

#if 0
__attribute__((unused))
static void _destructor(void *arg __attribute__((unused)))
{
	fprintf(stderr, "hello, in _destructor.\n");
	PyObject *ret_pyfunc = NULL;

	ret_pyfunc = PyEval_CallObject(desctructor_callback, NULL);

	if (ret_pyfunc == NULL) {
		PyErr_Format(LevelDBError, "error occurs in comparator destructor operation.\n");
		return; 
	}
	Py_XDECREF(ret_pyfunc);
	return;

}
#else 

__attribute__((unused))
static void _destructor(void *arg __attribute__((unused)))
{
	VERBOSE("%s", "Hello, I'm now in _destructor.\n");
	return;

}
#endif

#if 0
__attribute__((unused))
static int _compare(void *arg __attribute__((unused)), 
		const char *a, size_t alen,
		const char *b, size_t blen)
{
	VERBOSE("%s", "Hello, I'm in _compare.\n");
	int ret = 0;
	PyObject *arglist;
	PyObject *ret_pyfunc = NULL;
	arglist = Py_BuildValue("(s#s#)", a, alen, b, blen);

	ret_pyfunc = PyEval_CallObject(compare_callbak, arglist);
	Py_DECREF(arglist);

	if (ret_pyfunc == NULL) {
		PyErr_Format(LevelDBError, "error occurs in compare operation.\n");
		return 0;
	}

	ret = (int)PyInt_AsLong(ret_pyfunc);
	Py_XDECREF(ret_pyfunc);

	return ret;
}
#else 
__attribute__((unused))
static int _compare(void *arg __attribute__((unused)), 
		const char *a, size_t alen,
		const char *b, size_t blen)
{
	fprintf(stderr, "hello, in _compare.\n");
	int ret = 0;
	PyObject *ret_pyfunc = NULL;

	char *abuf = (char *)malloc(sizeof(char) * (alen + 1));
	char *bbuf = (char *)malloc(sizeof(char) * (blen + 1));

	memcpy(abuf, a, alen);
	memcpy(bbuf, b, blen);
	abuf[alen] = '\0';
	bbuf[blen] = '\0';

	if (abuf != NULL && bbuf != NULL) {
		VERBOSE( "abuf: %s, bbuf: %s\n", abuf, bbuf);
	}
	/* How to call python object in c ?
	 * And here is an example. 
	 * */
	//arglist = PyTuple_New(2);
	//PyTuple_SetItem(arglist, 0, Py_BuildValue("s", abuf));
	//PyTuple_SetItem(arglist, 1, Py_BuildValue("s", bbuf));

	if (compare_callbak != NULL) {
		if(PyCallable_Check(compare_callbak)) {
			fprintf(stderr, "yes, it is callable, i'm now in python callback in _compare.\n");
			ret_pyfunc = PyObject_CallFunction(compare_callbak, "ss", abuf, bbuf);
			VERBOSE("%s","O.k., end of callback now.\n");
		}
	}else {
		fprintf(stderr, "compare_callbak is NULL.\n");
	}

	if (ret_pyfunc == NULL) {
		PyErr_Format(LevelDBError, "Error occurs in compare operation.\n");
		return 0;
	}

	ret = (int)PyInt_AsLong(ret_pyfunc);
	Py_XDECREF(ret_pyfunc);

	return ret;
}

__attribute__((unused))
static int compare(void *arg __attribute__((unused)), 
		const char *a, size_t alen,
		const char *b, size_t blen)
{
	VERBOSE("%s", "Hello, in compare.\n");
	int ret = 0;
	ret = _compare(arg, a, alen, b, blen);
	return ret;
}
#endif

#if 0
__attribute__((unused))
static const char *_name(void *arg __attribute__((unused)))
{

	VERBOSE("%s", "Hello, in _name.\n");
	char *ret = NULL;
	PyObject *ret_pyfunc = NULL;

	ret_pyfunc = PyEval_CallObject(name_callback, NULL);

	if (ret_pyfunc == NULL) {
		PyErr_Format(LevelDBError, "Error occurs in name operation.\n");
		return NULL;
	}

	ret = (char *)PyString_AsString(ret_pyfunc);
	Py_XDECREF(ret_pyfunc);

	return ret;
}
#else

__attribute__((unused))
static const char *_name(void *arg __attribute__((unused)))
{

	VERBOSE("%s", "Hello, in _name.\n");
	return "hello, dummy.\n";
}
#endif

__attribute__((unused))
static void CmpDestroy(void* arg) { }

#if 0
static int CmpCompare(void* arg, const char* a, size_t alen,
                      const char* b, size_t blen) {
  int n = (alen < blen) ? alen : blen;
  int r = memcmp(a, b, n);
  if (r == 0) {
    if (alen < blen) r = -1;
    else if (alen > blen) r = +1;
  }
  return r;
}
#else
__attribute__((unused))
static int CmpCompare(void* arg, const char* a, size_t alen,
                      const char* b, size_t blen) {
  int n = (alen < blen) ? alen : blen;
  int r = memcmp(a, b, n);
  if (r > 0) return -1;
  if (r < 0) return 1;
  if (r == 0) {
    if (alen < blen) r = 1;
    else if (alen > blen) r = -1;
  }
  return r;
}
#endif

__attribute__((unused))
static const char* CmpName(void* arg) {
  return "hello, dummy.\n";
}


static void Comparator_dealloc(Comparator *self)
{
	Py_BEGIN_ALLOW_THREADS
	if (self->_comparator != NULL) {
		leveldb_comparator_destroy(self->_comparator);
		self->_comparator = NULL;
		_XDECREF(self->_comparator);
	}

	Py_END_ALLOW_THREADS

	self->_comparator= NULL;

	self->ob_type->tp_free(self);
}


static PyObject* Comparator_new(PyTypeObject* type, PyObject* args, PyObject* kwds)
{
	Comparator *self = (Comparator *)type->tp_alloc(type, 0);

	if (self != NULL) {
		self->_comparator= NULL;
	}

	return (PyObject*)self;
}


static int Comparator_init(Comparator *self, PyObject* args, PyObject* kwds)
{
	VERBOSE("%s", "Hi, Comparator_init.\n");

	
	static char* kwargs[] = {"destructor", "compare", "name",0};

	PyObject *t_desctructor_callback = NULL;
	PyObject *t_compare_callbak = NULL;
	PyObject *t_name_callback = NULL;

	if (self->_comparator != NULL) {
		Py_BEGIN_ALLOW_THREADS
		leveldb_comparator_destroy(self->_comparator);
		self->_comparator = NULL;
		_XDECREF(self->_comparator);
		Py_END_ALLOW_THREADS
		
		self->_comparator = NULL;
	}

	VERBOSE("%s", "Beginning parsing arguments now...\n ");
	if (PyArg_ParseTupleAndKeywords(args, kwds, (const char*)"OOO", kwargs, &t_desctructor_callback, &t_compare_callbak, &t_name_callback)) {

		if (!PyCallable_Check(t_desctructor_callback)
				|| !PyCallable_Check(t_compare_callbak)
				|| !PyCallable_Check(t_name_callback)) {
            PyErr_SetString(PyExc_TypeError, "parameter in Comparator_init() must be callable");
            return -1;
        }

		VERBOSE("%s","Arguments parsing is O.K.\n");

		Py_XINCREF(t_desctructor_callback);
        Py_XDECREF(desctructor_callback);
        desctructor_callback = t_desctructor_callback;		

		Py_XINCREF(t_compare_callbak);
        Py_XDECREF(compare_callbak);
        compare_callbak = t_compare_callbak;		

		Py_XINCREF(t_name_callback);
        Py_XDECREF(name_callback);
        name_callback = t_name_callback;		
#if 1

		PyObject *ret = NULL;
		ret = PyObject_CallFunction(compare_callbak, "ss","aaa","bbb");
		int result = PyInt_AsLong(ret);
		VERBOSE("Hello, calling compare_callbak, return value is %d.\n", result);
#endif
		VERBOSE("%s", "Hello, beginning leveldb_comparator_create.\n");
		self->_comparator = leveldb_comparator_create(NULL, _destructor, CmpCompare, _name);
		//self->_comparator = leveldb_comparator_create(NULL, CmpDestroy, CmpCompare, CmpName);
		//self->_comparator = leveldb_comparator_create(NULL, _destructor, compare, _name);
	
		if (self->_comparator == NULL) {
			PyErr_Format(LevelDBError, "Failed to create comparator.\n");
			return -1;
		}
	} else {
		return -1;
	}
	VERBOSE("%s", "Bye, end of Comparator_init.\n");
	return 0;
}

PyMethodDef Comparator_methods[] = {
	{NULL}
};

PyTypeObject ComparatorType = {
	PyObject_HEAD_INIT(NULL)
	0,                             /*ob_size*/
	(char*)"leveldb.Comparator",      /*tp_name*/
	sizeof(Comparator),             /*tp_basicsize*/
	0,                             /*tp_itemsize*/
	(destructor)Comparator_dealloc, /*tp_dealloc*/
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
	(char*)"LevelDB comparator",   /*tp_doc */
	0,                             /*tp_traverse */
	0,                             /*tp_clear */
	0,                             /*tp_richcompare */
	0,                             /*tp_weaklistoffset */
	0,                             /*tp_iter */
	0,                             /*tp_iternext */
	Comparator_methods,             /*tp_methods */
	0,                             /*tp_members */
	0,                             /*tp_getset */
	0,                             /*tp_base */
	0,                             /*tp_dict */
	0,                             /*tp_descr_get */
	0,                             /*tp_descr_set */
	0,                             /*tp_dictoffset */
	(initproc)Comparator_init,      /*tp_init */
	0,                             /*tp_alloc */
	Comparator_new,                 /*tp_new */
};

