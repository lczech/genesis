#!/usr/bin/python

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

    def dump (self):
        print "Function " + self.cpp_full_name()

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

# ==================================================================================================
#     Class: C++ Class
# ==================================================================================================

class CppClass:
    def __init__ (self, name):
        self.parent = None
        self.name   = name

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

    def dump (self, indent=0, full=False):
        in_str0 = " " * 4 * indent
        in_str1 = " " * 4 * (indent + 1)
        in_str2 = " " * 4 * (indent + 2)

        print in_str0 + "\x1b[33mClass", self.name + "\x1b[0m"
        print in_str1 + "  \x1b[90m" + self.location + "\x1b[0m"
        if full == False:
            return

        # if self.briefdescription != "":
        #     print in_str1 + self.briefdescription.strip()

        if len(self.ctors) > 0:
            print in_str1 + "Constructors:"
            for ctor in self.ctors:
                print in_str2 + ctor.cpp_signature()

        if len(self.dtors) > 0:
            print in_str1 + "Destructors:"
            for dtor in self.dtors:
                print in_str2 + dtor.cpp_signature()

        if len(self.methods) > 0:
            print in_str1 + "Methods:"
            for m in self.methods:
                print in_str2 + m.cpp_signature()

        if len(self.operators) > 0:
            print in_str1 + "Operators:"
            for o in self.operators:
                print in_str2 + o.cpp_signature()

        if len(self.iterators) > 0:
            print in_str1 + "Iterators:"
            for it in self.iterators:
                print in_str2 + it.name

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

    def add_class (self, cls):
        if cls is None:
            return
        if self.classes.has_key (cls.name):
            print "Namespace", self.name, "already contains a class named", cls.name
            return
        self.classes[cls.name] = cls

    def get_all_classes (self):
        classlist = []
        for cls in sorted(self.classes):
            classlist.append(self.classes[cls])
        for ns in sorted(self.namespaces):
            for cls in self.namespaces[ns].get_all_classes():
                classlist.append(cls)
        return classlist

    def add_function (self, func):
        if func is None:
            return
        self.functions[func.name] = func

    def extract_iterators (self, named = False):
        for cls in self.classes:
            self.classes[cls].extract_iterators(named)
        for ns in self.namespaces:
            self.namespaces[ns].extract_iterators(named)

    def get_file_locations (self):
        locations = []
        for cls in self.classes:
            if self.classes[cls].location == "":
                print "Warn: Class without location:", self.classes[cls].name
            else:
                locations.append(self.classes[cls].location)
        for ns in self.namespaces:
            for loc in self.namespaces[ns].get_file_locations():
                locations.append(loc)
        return locations

    def get_location_prefix (self):
        return os.path.commonprefix(self.get_file_locations())

    def shorten_location_prefix (self, prefix = ""):
        if prefix == "":
            prefix = self.get_location_prefix()

        for cls in self.classes:
            if not self.classes[cls].location.startswith(prefix):
                print "Location of class", cls, "does not start with prefix", prefix
                continue
            self.classes[cls].location = self.classes[cls].location[len(prefix):]

        for ns in self.namespaces:
            self.namespaces[ns].shorten_location_prefix(prefix)

    def dump (self, indent=0, full=False):
        print " " * 4 * indent + "\x1b[31mNamespace " + self.name + "\x1b[0m"
        for ns in self.namespaces:
            self.namespaces[ns].dump(indent+1, full)
        for cls in self.classes:
            self.classes[cls].dump(indent+1, full)

# ==================================================================================================
#     Main
# ==================================================================================================

if __name__ == "__main__":
    print "This has no main function."
    sys.exit()
