#!/usr/bin/python

# Genesis - A toolkit for working with phylogenetic data.
# Copyright (C) 2014-2016 Lucas Czech
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# Contact:
# Lucas Czech <lucas.czech@h-its.org>
# Exelixis Lab, Heidelberg Institute for Theoretical Studies
# Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany

import os
import re

# ==================================================================================================
#     Class: C++ Parameter
# ==================================================================================================

class CppParameter:
    def __init__ (self):
        self.type  = ""
        self.name  = ""
        self.value = ""

    def cpp_signature(self):
        if self.value == None:
            self.value = ""
        return self.type+" "+self.name+("="+self.value if self.value != "" else "")

# ==================================================================================================
#     Class: C++ Function
# ==================================================================================================

class CppFunction:
    def __init__ (self):
        self.parent  = None
        self.name    = ""
        self.template_params = None

        self.prot    = "public"
        self.static  = False
        self.const   = False
        self.virtual = False

        self.type    = ""
        self.params  = []

        self.briefdescription    = ""
        self.detaileddescription = ""

        self.location = ""

    def cpp_full_name (self):
        return self.parent.cpp_full_name() + "::" + self.name

    def cpp_signature (self, full=True):
        val  = ("static " if self.static else "")
        val += (self.type + " " if self.type != "" else "")
        val += (self.parent.cpp_full_name() + "::" if full else "") + self.name
        val += " (" + ', '.join(x.cpp_signature() for x in self.params) + ")"
        val += (" const" if self.const else "")
        return val

    def dump (self, indent=0, detail=1):
        print " " * 4 * indent       + "\x1b[34mFunction " + self.name + "\x1b[0m"

        if detail >= 1:
            print " " * 4 * (indent + 1) + "\x1b[90m" + self.location + "\x1b[0m"
            print " " * 4 * (indent + 1) + "\x1b[90m" + self.cpp_signature() + "\x1b[0m"

# ==================================================================================================
#     Class: C++ Iterator
# ==================================================================================================

class CppIterator:
    # TODO add iterators with parameters
    def __init__ (self):
        self.parent = None
        self.name   = ""

        self.begin  = ""
        self.end    = ""

    def dump (self, indent=0, detail=1):
        print " " * 4 * indent       + "\x1b[34mIterator " + self.name + "\x1b[0m"

        if detail >= 1:
            print " " * 4 * (indent + 1) + "\x1b[90m["+self.begin+",",self.end+")\x1b[0m"

# ==================================================================================================
#     Class: C++ Class
# ==================================================================================================

class CppClass:
    def __init__ (self, name):
        self.parent = None
        self.name   = name
        self.template_params = None

        self.ctors = []
        self.dtors = []

        self.methods   = []
        self.operators = []
        self.iterators = []

        self.briefdescription    = ""
        self.detaileddescription = ""

        self.location = ""

    def cpp_full_name (self):
        return self.parent.cpp_full_name() + "::" + self.name

    def add_function (self, func):
        if func is None:
            return
        if func.name == self.name:
            self.ctors.append(func)
        elif func.name == "~" + self.name:
            self.dtors.append(func)
        elif func.name.startswith("operator"):
            self.operators.append(func)
        else:
            self.methods.append(func)

    def add_named_iterator (self, py_name, begin, end):
        it = CppIterator()
        it.parent = self
        it.name   = py_name
        it.begin  = begin
        it.end    = end
        self.iterators.append(it)

    def add_iterator (self, begin="begin", end="end"):
        self.add_named_iterator("__iter__", begin, end)

    def extract_iterators (self, all_named = False):
        """Extracts iterators from the class methods list. Typically, these are functions
        named 'begin' and 'end'. If all_named==True, all methods starting with '[Bb]egin...'
        and '[Ee]nd...' are extracted as iterators."""

        # process default iterators
        method_names = [func.name for func in self.methods]
        if "begin" in method_names and "end" in method_names:
            self.add_iterator()
            self.methods[:] = [f for f in self.methods if f.name not in [ "begin", "end" ]]

        # continue only if we want to extract all named iterators
        if not all_named:
            return

        # process all iterators starting with "[Bb]egin..." and "[Ee]nd..."
        extracted_iters = []
        for mn in method_names:
            if not mn.lower().startswith("begin") or mn == "begin":
                continue

            it_name    = mn[5:].strip('_')
            begin_name = mn
            end_name   = re.sub('^begin', 'end', re.sub('^Begin', 'End', mn))

            # do not extract if no appropriate end function present
            if end_name not in method_names:
                continue

            # extract each iterator only once
            if it_name in extracted_iters:
                continue
            else:
                extracted_iters.append(it_name)

            self.add_named_iterator(it_name, begin_name, end_name)
            self.methods[:] = [f for f in self.methods if f.name not in [ begin_name, end_name ]]

    def shorten_location_prefix (self, prefix = ""):
        if prefix == "":
            return

        for func in self.ctors:
            if not func.location.startswith(prefix):
                print "Location of function", func, "does not start with prefix", prefix
                continue
            func.location = func.location[len(prefix):]

        for func in self.dtors:
            if not func.location.startswith(prefix):
                print "Location of function", func, "does not start with prefix", prefix
                continue
            func.location = func.location[len(prefix):]

        for func in self.methods:
            if not func.location.startswith(prefix):
                print "Location of function", func, "does not start with prefix", prefix
                continue
            func.location = func.location[len(prefix):]

        for func in self.operators:
            if not func.location.startswith(prefix):
                print "Location of function", func, "does not start with prefix", prefix
                continue
            func.location = func.location[len(prefix):]

    def dump (self, indent=0, detail=1):
        in_str0 = " " * 4 * indent
        in_str1 = " " * 4 * (indent + 1)
        in_str2 = " " * 4 * (indent + 2)

        print in_str0 + "\x1b[33mClass", self.name + "\x1b[0m"

        if detail >= 1:
            print in_str1 + "\x1b[90m" + self.location + "\x1b[0m"
            print in_str1 + "\x1b[90m" + self.cpp_full_name() + "\x1b[0m"

        if detail < 2:
            return

        # if self.briefdescription != "":
        #     print in_str1 + self.briefdescription.strip()

        def print_funcs(name, func_set):
            if len(func_set) > 0:
                print in_str1 + "\x1b[34m"+name+":\x1b[0m"
                for func in sorted(func_set):
                    if detail == 2:
                        print in_str2 + "\x1b[34m"+func.name+"\x1b[0m"
                    else:
                        func.dump(indent+2, 1)

        print_funcs( "Constructors", self.ctors )
        print_funcs( "Destructors",  self.dtors )
        print_funcs( "Methods",      self.methods )
        print_funcs( "Operators",    self.operators )
        print_funcs( "Iterators",    self.iterators )

# ==================================================================================================
#     Class: C++ Namespace
# ==================================================================================================

class CppNamespace:
    def __init__ (self, name):
        self.parent = None
        self.name   = name

        self.namespaces = {}
        self.classes    = {}
        self.functions  = {}

        self.briefdescription    = ""
        self.detaileddescription = ""

    def cpp_full_name (self):
        if self.parent is not None:
            return self.parent.cpp_full_name() + "::" + self.name
        else:
            return ""

    def create_namespace (self, ns):
        if ns is None:
            return
        if self.namespaces.has_key (ns):
            return
        self.namespaces[ns] = CppNamespace(ns)

    def add_class (self, clss):
        if clss is None:
            return
        if self.classes.has_key (clss.cpp_full_name()):
            print "Namespace", self.name, "already contains a class named", clss.cpp_full_name()
            return
        self.classes[clss.cpp_full_name()] = clss

    def get_all_classes (self):
        classlist = []
        for clss in sorted(self.classes):
            classlist.append(self.classes[clss])
        for ns in sorted(self.namespaces):
            for clss in self.namespaces[ns].get_all_classes():
                classlist.append(clss)
        return classlist

    def add_function (self, func):
        if func is None:
            return
        if self.functions.has_key (func.cpp_signature()):
            print "Namespace", self.name, "already contains a function named", func.cpp_signature()
            return
        self.functions[func.cpp_signature()] = func

    def get_all_functions( self ):
        funclist = []
        for func in sorted(self.functions):
            funclist.append(self.functions[func])
        for ns in sorted(self.namespaces):
            for func in self.namespaces[ns].get_all_functions():
                funclist.append(func)
        return funclist

    def extract_iterators (self, named = False):
        for clss in self.classes:
            self.classes[clss].extract_iterators(named)
        for ns in self.namespaces:
            self.namespaces[ns].extract_iterators(named)

    def get_file_locations (self):
        locations = []
        for clss in self.classes:
            if self.classes[clss].location == "":
                print "Warn: Class without location:", self.classes[clss].name
            else:
                locations.append(self.classes[clss].location)
        for ns in self.namespaces:
            for loc in self.namespaces[ns].get_file_locations():
                locations.append(loc)
        return locations

    def get_location_prefix (self):
        return os.path.commonprefix(self.get_file_locations())

    def shorten_location_prefix (self, prefix = ""):
        if prefix == "":
            prefix = self.get_location_prefix()

        for clss in self.classes:
            if not self.classes[clss].location.startswith(prefix):
                print "Location of class", clss, "does not start with prefix", prefix
                continue
            self.classes[clss].location = self.classes[clss].location[len(prefix):]
            self.classes[clss].shorten_location_prefix(prefix)

        for func in self.functions:
            if not self.functions[func].location.startswith(prefix):
                print "Location of function", func, "does not start with prefix", prefix
                continue
            self.functions[func].location = self.functions[func].location[len(prefix):]

        for ns in self.namespaces:
            self.namespaces[ns].shorten_location_prefix(prefix)

    def dump (self, indent=0, detail=1):
        """Print the content of this namespace: all sub-namespaces, classes and functions.
        Use of the detail parameter:
        0: just the names,
        1: with path and fully qualified names
        2: with all subfunctions (for classes)"""

        print " " * 4 * indent + "\x1b[31mNamespace " + self.name + "\x1b[0m"
        for ns in sorted(self.namespaces):
            self.namespaces[ns].dump(indent+1, detail)
        for clss in sorted(self.classes):
            self.classes[clss].dump(indent+1, detail)
        for func in sorted(self.functions):
            self.functions[func].dump(indent+1, detail)

# ==================================================================================================
#     Main
# ==================================================================================================

if __name__ == "__main__":
    print "This has no main function."
    sys.exit()
