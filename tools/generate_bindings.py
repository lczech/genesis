#!/usr/bin/python

import os
import re
import xml.etree.ElementTree

#~ cpp over doxygen to boost python binding generator
#~ codygen
#~ copygen
#~ codbpbg

# TODO resolve typdefs
# TODO return policies

# ==============================================================================
#     Helper Functions
# ==============================================================================

def CamelCaseToUnderscore(name):
    s1 = re.sub('(.)([A-Z][a-z]+)', r'\1_\2', name)
    return re.sub('([a-z0-9])([A-Z])', r'\1_\2', s1).lower()

# ==============================================================================
#     Class: C++ Parameter
# ==============================================================================

class CppParameter:
    def __init__ (self):
        self.type  = ""
        self.name  = ""
        self.value = ""

    def cpp_string(self):
        return self.type+" "+self.name+("="+self.value if self.value != "" else "")

# ==============================================================================
#     Class: C++ Function
# ==============================================================================

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

    def cpp_full_name (self):
        return self.parent.cpp_full_name() + "::" + self.name

    def cpp_string (self):
        val  = ("static " if self.static else "")
        val  = (self.type + " " if self.type != "" else "") + self.name
        val += " (" + ', '.join(x.cpp_string() for x in self.params) + ")"
        val += (" const" if self.const else "")
        return val

# ==============================================================================
#     Class: C++ Iterator
# ==============================================================================

class CppIterator:
    # TODO add iterators with parameters
    def __init__ (self):
        self.parent = None
        self.name   = ""

        self.begin  = ""
        self.end    = ""

# ==============================================================================
#     Class: C++ Class
# ==============================================================================

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

    def dump (self, indent=0):
        in_str0 = " " * 4 * indent
        in_str1 = " " * 4 * (indent + 1)
        in_str2 = " " * 4 * (indent + 2)

        print in_str0 + "Class " + self.name
        print in_str1 + "@ " + self.location
        if self.briefdescription != "":
            print in_str1 + self.briefdescription.strip()

        if len(self.ctors) > 0:
            print in_str1 + "Constructors:"
            for ctor in self.ctors:
                print in_str2 + ctor.cpp_string()

        if len(self.dtors) > 0:
            print in_str1 + "Destructors:"
            for dtor in self.dtors:
                print in_str2 + dtor.cpp_string()

        if len(self.methods) > 0:
            print in_str1 + "Methods:"
            for m in self.methods:
                print in_str2 + m.cpp_string()

        if len(self.operators) > 0:
            print in_str1 + "Operators:"
            for o in self.operators:
                print in_str2 + o.cpp_string()

        if len(self.iterators) > 0:
            print in_str1 + "Iterators:"
            for it in self.iterators:
                print in_str2 + it.name

# ==============================================================================
#     Class: C++ Namespace
# ==============================================================================

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
        for cls in self.classes:
            classlist.append(self.classes[cls])
        for ns in self.namespaces:
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
            self.namespaces[ns].shorten_location_prefix()

    def dump (self, indent=0):
        print " " * 4 * indent + "Namespace " + self.name
        for ns in self.namespaces:
            self.namespaces[ns].dump(indent+1)
        for cls in self.classes:
            self.classes[cls].dump(indent+1)

# ==============================================================================
#     Class: Doxygen Reader
# ==============================================================================

class DoxygenReader:

    # ----------------------------------------------------------------
    #     Parse Member Function
    # ----------------------------------------------------------------

    @staticmethod
    def parse_member_function (xml_elem):
        if xml_elem.tag != "memberdef":
            print "Invalid xml tag:", xml_elem.tag
            return None
        if xml_elem.attrib["kind"] != "function":
            print "Invalid member kind:", xml_elem.attrib["kind"]
            return None

        func = CppFunction()
        func.name = xml_elem.find("name").text
        func.type = ''.join(xml_elem.find("type").itertext())

        func.prot    = (xml_elem.attrib["prot"])
        func.static  = (xml_elem.attrib["static"] == "yes")
        func.const   = (xml_elem.attrib["const"]  == "yes")
        func.virtual = (xml_elem.attrib["virt"]   == "virtual")

        for p in xml_elem.findall("param"):
            param = CppParameter()
            param.type = ''.join(p.find("type").itertext()).strip()
            if p.find("declname") is not None:
                param.name = p.find("declname").text
            if p.find("defval") is not None:
                param.value = p.find("defval").text
            func.params.append(param)

        func.briefdescription    = ''.join(xml_elem.find("briefdescription").itertext()).strip()
        func.detaileddescription = ''.join(xml_elem.find("detaileddescription").itertext()).strip()

        # unused properties of the xml element:
        #~ print "definition:",x.find("definition").text
        #~ print "argsstring:",x.find("argsstring").text

        return func

    # ----------------------------------------------------------------
    #     Parse Class File
    # ----------------------------------------------------------------

    @staticmethod
    def parse_class_file (filename):
        tree = xml.etree.ElementTree.parse(filename)
        root = tree.getroot()

        classes = []
        #~ for compound in root.iter("compound"):
        for compound in root:
            if compound.attrib["kind"] != "class":
                continue

            cls_name = compound.find("compoundname").text.rsplit('::', 1)[1]
            cls = CppClass(cls_name)

            cls.briefdescription    = ''.join(compound.find("briefdescription").itertext()).strip()
            cls.detaileddescription = ''.join(compound.find("detaileddescription").itertext()).strip()

            for section in compound:
                if section.tag != "sectiondef":
                    continue

                if not section.attrib["kind"] in [ "public-func", "public-static-func" ]:
                    continue

                for member in section:
                    func = DoxygenReader.parse_member_function(member)
                    func.parent = cls
                    cls.add_function (func)

            cls.location = compound.find("location").attrib["file"]
            classes.append(cls)

        classes = sorted(classes, key=lambda cls: cls.name)
        return classes

    # ----------------------------------------------------------------
    #     Parse Doxygen Index
    # ----------------------------------------------------------------

    @staticmethod
    def parse (directory):
        tree = xml.etree.ElementTree.parse(os.path.join(directory, "index.xml"))
        root = tree.getroot()

        ns_global = CppNamespace("")
        for compound in root:
            if compound.attrib["kind"] == "class":
                cls_full_name = compound.find("name").text
                cls_ns_list   = cls_full_name.split("::")
                cls_name      = cls_ns_list.pop()

                ns_local = ns_global
                for ns in cls_ns_list:
                    ns_local.create_namespace(ns)
                    ns_tmp   = ns_local
                    ns_local = ns_local.namespaces[ns]
                    ns_local.parent = ns_tmp

                xml_file = os.path.join(directory, compound.attrib["refid"] + ".xml")
                for cls in DoxygenReader.parse_class_file (xml_file):
                    cls.parent = ns_local
                    ns_local.add_class (cls)

        return ns_global

# ==============================================================================
#     Class: Boost Python Writer
# ==============================================================================

class BoostPythonWriter:

    # ----------------------------------------------------------------
    #     Section Headers
    # ----------------------------------------------------------------

    @staticmethod
    def make_section_header (symbol, title, indent = 0, length = 80):
        val  = "\n" + " " * indent + "// " + symbol * (length-3) + "\n"
        val += " " * indent + "//     " + title + "\n"
        val += " " * indent + "// " + symbol * (length-3) + "\n\n"
        return val

    @staticmethod
    def make_section_header_major (title, indent = 0, length = 80):
        return BoostPythonWriter.make_section_header ("=", title, indent, length)

    @staticmethod
    def make_section_header_minor(title, indent = 4, length = 70):
        return BoostPythonWriter.make_section_header ("-", title, indent, length)

    # ----------------------------------------------------------------
    #     Generate Class Header
    # ----------------------------------------------------------------

    @staticmethod
    def generate_class_constructor (ctor):
        val  = "boost::python::init< "
        val += ", ".join (
                (
                    param.type if param.value == "" else
                    "boost::python::optional< " + param.type + " >"
                ) for param in ctor.params
            )
        val += " >("
        val += "( " if len(ctor.params) > 0 else " "
        val += ", ".join (
            (
                "boost::python::arg(\"" + param.name + "\")" + (
                    "" if param.value == "" else
                    "=(" + param.type + ")(" + param.value + ")"
                )
            ) for param in ctor.params
        )
        val += " )"
        val += ")" if len(ctor.params) > 0 else ""
        return val

    @staticmethod
    def generate_class_header (cls):
        if len(cls.ctors) > 0:
            ctor_val  = ", " + BoostPythonWriter.generate_class_constructor(cls.ctors[0])
        else:
            ctor_val  = ""
        val  = "    boost::python::class_< " + cls.cpp_full_name() + " > "
        val += "( \"" + cls.name + "\"" + ctor_val + " )\n"
        if len(cls.ctors) > 1:
            for i in range(1, len(cls.ctors)):
                val += "        .def( "
                val += BoostPythonWriter.generate_class_constructor(cls.ctors[i])
                val += " )\n"
        return val

    # ----------------------------------------------------------------
    #     Generate Class Methods
    # ----------------------------------------------------------------

    @staticmethod
    def generate_class_function_body (func, py_name = ""):
        val  = "        .def(\n"
        val += "            \"" + (func.name if py_name == "" else py_name) + "\",\n"
        val += "            ( " + func.type + " ( "
        val += "*" if func.static else func.parent.cpp_full_name() + "::*"
        val += " )( "
        val += ", ".join(
            (
                param.type
            ) for param in func.params
        )
        val += " )"
        val += " const " if func.const else ""
        val += ")( &" + func.cpp_full_name() + " )"
        if len(func.params) > 0:
            val += ",\n            ( " + ", ".join (
                (
                    "boost::python::arg(\"" + param.name + "\")" + (
                        "" if param.value == "" else
                        "=(" + param.type + ")(" + param.value + ")"
                    )
                ) for param in func.params
            ) + " )"
        if func.briefdescription != "":
            val += ",\n            \"" + func.briefdescription + "\"\n"
        else:
            val += "\n"
        val += "        )\n"
        if func.static:
            val += "        .staticmethod(\""
            val += func.name if py_name == "" else py_name
            val += "\")\n"
        return val

    @staticmethod
    def generate_class_methods (cls):
        if len(cls.methods) == 0:
            return ""

        val = "\n        // Public Member Functions\n\n"
        for func in cls.methods:
            val += BoostPythonWriter.generate_class_function_body (func)

        return val

    # ----------------------------------------------------------------
    #     Generate Class Operators
    # ----------------------------------------------------------------

   # TODO add friend (non-member) operators: http://www.boost.org/doc/libs/1_35_0/libs/python/doc/v2/operators.html

    @staticmethod
    def classify_operator (op):
        if not op.name.startswith("operator"):
            return None

        symbol = op.name[len("operator"):]
        if symbol in [ "+=", "-=", "*=", "/=", "%=", ">>=", "<<=", "&=", "^=", "|=" ]:
            return (symbol, "inplace")
        if symbol in [ "==", "!=", "<", ">", "<=", ">=" ]:
            return (symbol, "comparison")
        if symbol in [ "-", "+", "~", "!" ]:
            return (symbol, "unary")
        if symbol == "[]":
            return (symbol, "array")

        return None

    @staticmethod
    def generate_class_operators (cls):
        if len(cls.operators) == 0:
            return ""

        # TODO missing doc strings here!
        val = "\n        // Operators\n\n"
        for operator in cls.operators:
            op_class = BoostPythonWriter.classify_operator(operator)
            if op_class is None:
                print "Unknown operator:", operator.name
            elif op_class[1] in [ "inplace", "comparison" ]:
                val += "        .def( boost::python::self " + op_class[0] + " boost::python::self )\n"
            elif op_class[1] == "unary":
                val += "        .def( " + op_class[0] + "boost::python::self )\n"
            elif op_class[1] == "array":
                val += BoostPythonWriter.generate_class_function_body (operator, "__getitem__")
            else:
                print "Operator type not handled:", op_class[1]
        return val

    # ----------------------------------------------------------------
    #     Generate Class Iterators
    # ----------------------------------------------------------------

    @staticmethod
    def generate_class_iterators (cls):
        if len(cls.iterators) == 0:
            return ""

        # TODO missing doc strings here!
        # TODO add iterators with parameters
        val = "\n        // Iterators\n\n"
        for it in cls.iterators:
            if it.name == "__iter__":
                val += "        .def"
            else:
                val += "        .add_property"
            val += "(\n            \"" + it.name + "\",\n            boost::python::range ( &"
            val += it.parent.cpp_full_name() + "::" + it.begin + ", &"
            val += it.parent.cpp_full_name() + "::" + it.end
            val += " )\n        )\n"

        return val

    # ----------------------------------------------------------------
    #     Generate Class
    # ----------------------------------------------------------------

    @staticmethod
    def generate_class (cls):
        val = BoostPythonWriter.make_section_header_minor ("Class " + cls.name)
        val += BoostPythonWriter.generate_class_header (cls)
        val += BoostPythonWriter.generate_class_methods (cls)
        val += BoostPythonWriter.generate_class_operators (cls)
        val += BoostPythonWriter.generate_class_iterators (cls)
        val += "    ;"
        return val

    # ----------------------------------------------------------------
    #     Generate Namespace
    # ----------------------------------------------------------------

    @staticmethod
    def generate_namespace (namespace):
        val = ""

        if len(namespace.classes) > 0:
            val += BoostPythonWriter.make_section_header_major ("Classes")
        for name in sorted(namespace.classes):
            val += BoostPythonWriter.generate_class (namespace.classes[name]) + "\n"

        for name in sorted(namespace.namespaces):
            val += BoostPythonWriter.make_section_header_major ("Namespace " + name)
            val += BoostPythonWriter.generate_namespace (namespace.namespaces[name]) + "\n"
        return val

    # ----------------------------------------------------------------
    #     Generate
    # ----------------------------------------------------------------

    @staticmethod
    def generate_files (namespace, directory, module_name):
        class ExportFile:
            def __init__ (self):
                self.includes      = []
                self.class_strings = {}

        export_files = {}
        for cls in namespace.get_all_classes():
            cls_str  = BoostPythonWriter.generate_class(cls)
            cls_file = os.path.splitext(cls.location)[0] + ".cpp"

            if not export_files.has_key(cls_file):
                export_files[cls_file] = ExportFile()

            export_files[cls_file].includes.append(cls.location)
            export_files[cls_file].class_strings[cls.name] = cls_str

        for fn, exp in export_files.iteritems():
            fn = os.path.join(directory, fn)
            if not os.path.exists(os.path.dirname(fn)):
                os.makedirs(os.path.dirname(fn))

            f = open(fn, 'w')
            f.write ("#include <boost/python.hpp>\n")
            for inc in set(exp.includes):
                f.write ("#include \"" + inc + "\"\n")
            f.write ("\n")

            for cls_name, cls_str in exp.class_strings.iteritems():
                f.write ("void BoostPythonExport_" + cls_name + "()\n{")
                f.write (cls_str)
                f.write ("\n}\n\n")
            f.close()

        fn = os.path.join(directory, "bindings.cpp")
        f = open(fn, 'w')
        f.write ("#include <boost/python.hpp>\n")
        f.write (BoostPythonWriter.make_section_header_major("Forward declarations of all exported classes"))
        for fn, exp in export_files.iteritems():
             for cls_name, cls_str in exp.class_strings.iteritems():
                 f.write ("void BoostPythonExport_" + cls_name + "();\n")

        f.write (BoostPythonWriter.make_section_header_major("Boost Python Module"))
        f.write ("BOOST_PYTHON_MODULE(" + module_name + ")\n{\n")
        for fn, exp in export_files.iteritems():
             for cls_name, cls_str in exp.class_strings.iteritems():
                 f.write ("    BoostPythonExport_" + cls_name + "();\n")
        f.write ("}\n")
        f.close()

    @staticmethod
    def generate (namespace):
        return BoostPythonWriter.generate_namespace (namespace)

# ==============================================================================
#     Main
# ==============================================================================

if __name__ == "__main__":
    hierarchy = DoxygenReader.parse ("../doc/api/xml")
    hierarchy.extract_iterators(True)
    hierarchy.shorten_location_prefix()
    #~ print BoostPythonWriter.generate (hierarchy)
    BoostPythonWriter.generate_files (hierarchy, "test", "genesis")
