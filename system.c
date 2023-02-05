#define _GNU_SOURCE
#include <Python.h>
#include <sys/syscall.h>
#include <sys/wait.h>

#define SYSTEM_MODULE_DOC "Module for calling system calls"
#define UNSHARE_DOC ".. py:function:: unshare(flags)"

#define STACK_SIZE 32768

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

#define SETHOSTNAME_DOC ".. py:function:: sethostname(hostname)\n"

static PyObject *
_sethostname(PyObject *self, PyObject *args) {
	const char *hostname;

	if (!PyArg_ParseTuple(args, "s", &hostname))
		return NULL;

	if (sethostname(hostname, strlen(hostname)) == -1) {
		PyErr_SetFromErrno(PyExc_RuntimeError);
		return NULL;
	}

	Py_INCREF(Py_None);
	return Py_None;
}

struct py_clone_args {
	PyObject *callback;
	PyObject *callback_args;
};

static int clone_callback(void *args) {
	PyObject *result;
	struct py_clone_args *call_args = (struct py_clone_args *)args;

	if ((result = PyObject_CallObject(call_args->callback, call_args->callback_args)) == NULL) {
		PyErr_Print();
		return -1;
	} else {
		Py_DECREF(result);
	}
	return 0;
}

#define CLONE_DOC   ".. py:function:: clone(callback, flags, callback_args)\n"
static PyObject *
_clone(PyObject *self, PyObject *args) {
	PyObject *callback, *callback_args;
	void *child_stack;
	int flags;
	pid_t child_pid;

	child_stack = malloc(STACK_SIZE);

	if (!PyArg_ParseTuple(args, "OiO", &callback, &flags, &callback_args))
		return NULL;

	if (!PyCallable_Check(callback)) {
		PyErr_SetString(PyExc_TypeError, "parameter must be callable");
        return NULL;
    }

    struct py_clone_args call_args;
    call_args.callback = callback;
    call_args.callback_args = callback_args;

	if ((child_pid = clone(&clone_callback, child_stack + STACK_SIZE, flags | SIGCHLD, &call_args)) == -1) {
			PyErr_SetFromErrno(PyExc_RuntimeError);
			return Py_BuildValue("i", -1);
	} else {
		return Py_BuildValue("i", child_pid);
	}
}

static PyMethodDef SystemMethods[] = {
	{"unshare", _unshare, METH_VARARGS, UNSHARE_DOC},
	{"sethostname", _sethostname, METH_VARARGS, SETHOSTNAME_DOC},
	{"clone", _clone, METH_VARARGS, CLONE_DOC},
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