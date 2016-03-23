#!/usr/bin/python

import os
import re

from cpp_entities import *

# ==================================================================================================
#     Helper Functions
# ==================================================================================================

def CamelCaseToUnderscore(name):
    s1 = re.sub('(.)([A-Z][a-z]+)', r'\1_\2', name)
    return re.sub('([a-z0-9])([A-Z])', r'\1_\2', s1).lower()

def CppEscapeString(txt):
    return txt.replace("\\", "\\\\").replace("\"", "\\\"").replace("\n", "\\n")

# ==================================================================================================
#     Class: Boost Python Writer
# ==================================================================================================

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
                    "" if param.value == None or param.value == "" else
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
            # val += ",\n            \"" + func.briefdescription + "\"\n"
            val += ",\n            get_docstring(\"" + func.cpp_signature() + "\")\n"
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
        m_list = []
        for func in cls.methods:
            m_list.append(BoostPythonWriter.generate_class_function_body (func))

        for d in sorted(set(m_list)):
            val += d

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
        if symbol == "<<":
            return (symbol, "ostream")

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
                # print "Unknown operator:", operator.name
                pass
            elif op_class[1] in [ "inplace", "comparison" ]:
                val += "        .def( boost::python::self " + op_class[0] + " boost::python::self )\n"
            elif op_class[1] == "unary":
                val += "        .def( " + op_class[0] + "boost::python::self )\n"
            elif op_class[1] == "array":
                val += BoostPythonWriter.generate_class_function_body (operator, "__getitem__")
            elif op_class[1] == "ostream":
                val += "        .def( boost::python::self_ns::str( boost::python::self ) )\n"
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

        # collect export functions for all classes
        export_files = {}
        for cls in namespace.get_all_classes():
            cls_str  = BoostPythonWriter.generate_class(cls)
            cls_file = os.path.splitext(cls.location)[0] + ".cpp"

            if not export_files.has_key(cls_file):
                export_files[cls_file] = ExportFile()

            export_files[cls_file].includes.append(cls.location)
            export_files[cls_file].class_strings[cls.name] = cls_str

        # write them to files
        for fn, exp in export_files.iteritems():
            if fn.startswith(".") or fn.startswith("/"):
                fn = "unnamed" + fn
            fn = os.path.join(directory, fn)
            print "Creating file", fn

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

        # write main bindings file
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

        # write doc string file
        fn = os.path.join(directory, "docstrings.cpp")
        f = open(fn, 'w')
        f.write("/**\n")
        f.write("* @brief Documentation strings for the genesis Python module.\n")
        f.write(" *\n")
        f.write(" * @file\n")
        f.write(" * @ingroup python\n")
        f.write(" */\n")
        f.write("\n")
        f.write("#include <python/src/common.hpp>\n")
        f.write("\n")
        f.write("#include <map>\n")
        f.write("#include <string>\n")
        f.write("\n")
        f.write ("static std::map<std::string, std::string> doc_strings_ = {\n")
        for cls in namespace.get_all_classes():
            for func in sorted(cls.methods, key=lambda x: x.name):
                if func.briefdescription != "" or func.detaileddescription != "":
                    f.write("    {\"" + func.cpp_signature() + "\", \"")
                    if func.briefdescription != "":
                        f.write(CppEscapeString(func.briefdescription))
                    if func.briefdescription != "" and func.detaileddescription != "":
                        f.write("\\n\\n")
                    if func.detaileddescription != "":
                        f.write(CppEscapeString(func.detaileddescription))
                    f.write("\"},\n")
        f.write ("};\n")
        f.write ("\n")
        f.write ("const char* get_docstring (const std::string& signature)\n")
        f.write ("{\n")
        f.write ("    if (doc_strings_.count(signature) > 0) {\n")
        f.write ("        return doc_strings_[signature].c_str();\n")
        f.write ("    } else {\n")
        f.write ("        return \"\";\n")
        f.write ("    }\n")
        f.write ("}\n")
        f.close()

    @staticmethod
    def generate (namespace):
        return BoostPythonWriter.generate_namespace (namespace)

# ==================================================================================================
#     Main
# ==================================================================================================

if __name__ == "__main__":
    print "This has no  main function."
    sys.exit()
