#define _GNU_SOURCE
#include <Python.h>
#include <sys/syscall.h>

#define SYSTEM_MODULE_DOC "Module for calling system calls"
#define UNSHARE_DOC ".. py:function:: unshare(flags)"

static PyObject *
_unshare(PyObject *self, PyObject *args) {
	int clone_flags;

	if (!PyArg_ParseTuple(args, "i", &clone_flags))
		return NULL;

	if (unshare(clone_flags) == -1) {
		PyErr_SetFromErrno(PyExc_RuntimeError);
		return NULL;
	} else {
		Py_INCREF(Py_None);
		return Py_None;
	}
}

static PyMethodDef SystemMethods[] = {
	{"unshare", _unshare, METH_VARARGS, UNSHARE_DOC},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

static struct PyModuleDef SystemCalls =
{
    PyModuleDef_HEAD_INIT,
    "system", /* name of module */
    "usage: use for system calls in linux kernel\n", /* module documentation, may be NULL */
    -1,   /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
    SystemMethods
};

PyMODINIT_FUNC PyInit_system(void)
{
    PyObject *module = PyModule_Create(&SystemCalls);
	// clone constants
	PyModule_AddIntConstant(module, "CLONE_NEWNS", CLONE_NEWNS);     // mount namespace
	PyModule_AddIntConstant(module, "CLONE_NEWUTS", CLONE_NEWUTS);   // UTS (hostname) namespace
	PyModule_AddIntConstant(module, "CLONE_NEWPID", CLONE_NEWPID);   // PID namespace
	PyModule_AddIntConstant(module, "CLONE_NEWUSER", CLONE_NEWUSER); // users namespace
	PyModule_AddIntConstant(module, "CLONE_NEWIPC", CLONE_NEWIPC);   // IPC namespace
	PyModule_AddIntConstant(module, "CLONE_NEWNET", CLONE_NEWNET);   // network namespace
	PyModule_AddIntConstant(module, "CLONE_THREAD", CLONE_THREAD);
    return module;
}