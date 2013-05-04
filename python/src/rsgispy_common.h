#ifndef RSGISPY_COMMON_H
#define RSGISPY_COMMON_H

#include <Python.h>
#include <string>
#include <string.h>

#include "common/RSGISCommons.h"

// hides differences between Python2 and 3. 
// PyString for Python2 - PyUnicode for Python3
inline bool RSGISPY_CHECK_STRING(PyObject *o)
{
#if PY_MAJOR_VERSION >= 3
    return PyUnicode_Check(o);
#else
    return PyString_Check(o);
#endif
}

// returns a std::string with the contents of o as a string
// hides differences between Python 2 and 3 - does encoding for Python3
inline std::string RSGISPY_STRING_EXTRACT(PyObject *o)
{
    std::string sVal;
#if PY_MAJOR_VERSION >= 3
    // convert from a unicode to a bytes
    PyObject *pBytes = PyUnicode_AsEncodedString(o, NULL, NULL);
    sVal = PyBytes_AsString(pBytes);
    Py_DECREF(pBytes);
#else
    sVal = PyString_AsString(o);    
#endif
    return sVal;
}

inline PyObject *RSGISPY_CREATE_STRING(const char *psz)
{
#if PY_MAJOR_VERSION >= 3
    return PyUnicode_FromString(psz);
#else
    return PyString_FromString(psz);
#endif
}

inline long RSGISPY_CHECK_INT(PyObject *o)
{
#if PY_MAJOR_VERSION >= 3
    return PyLong_Check(o);
#else
    return PyInt_Check(o);
#endif
}

inline long RSGISPY_INT_EXTRACT(PyObject *o)
{
#if PY_MAJOR_VERSION >= 3
    return PyLong_AsLong(o);
#else
    return PyInt_AsLong(o);
#endif
}

#endif // RSGISPY_COMMON_H
