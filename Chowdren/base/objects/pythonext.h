// Copyright (c) Mathias Kaerlev 2012-2015.
//
// This file is part of Anaconda.
//
// Anaconda is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Anaconda is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Anaconda.  If not, see <http://www.gnu.org/licenses/>.

#ifndef CHOWDREN_PYTHONEXT_H
#define CHOWDREN_PYTHONEXT_H

#include <string>
#include "frameobject.h"

typedef struct _object PyObject;

// macros for event handling

#define GET_PYTHON_ARG(i) PythonInterpreter::get_tuple_item(python_args, i)

class PythonInterpreter : public FrameObject
{
public:
    FRAMEOBJECT_HEAD(PythonInterpreter)

    static PyObject * main_module;
    static PyObject * interface_module;
    static PyObject * functions_module;
    PyObject * parameters;
    PyObject * returns;

    static void initialize();
    static void add_function(const char * name, void * fp);
    PythonInterpreter(int x, int y, int type_id);
    static void print_errors();
    void run_string(std::string str);
    void add_parameter(PyObject * v);
    void add_parameter(double v);

    static inline PyObject * to_object(double value)
    {
        return *((PyObject**)&value);
    }

    static inline PyObject * to_object(PyObject * value)
    {
        return value;
    }

    static PyObject * get_none();
    static PyObject * get_tuple_item(PyObject * t, int i);
    static double to_double(PyObject * value);
    static PyObject * create_object(const std::string & v);
    static PyObject * create_object(int value);
    static PyObject * eval(std::string str);
    static double list_append(double listd, PyObject * value);
    static double create_list();
    void call_global(const std::string & name);
    std::string as_string(PyObject * v);
    int as_number(PyObject * v);
    std::string as_string(double v);
};

#endif // CHOWDREN_PYTHONEXT_H
