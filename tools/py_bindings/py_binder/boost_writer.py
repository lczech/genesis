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

from cpp_entities import *

# ==================================================================================================
#     Helper Functions
# ==================================================================================================

def CamelCaseToUnderscore(name):
    s1 = re.sub('(.)([A-Z][a-z]+)', r'\1_\2', name)
    return re.sub('([a-z0-9])([A-Z])', r'\1_\2', s1).lower()

def CppEscapeString(txt):
    return txt.replace("\\", "\\\\").replace("\"", "\\\"").replace("\n", "\\n")

# Helper struct that collects all the information that goes into one file.
class ExportFile:
    def __init__ (self):
        self.scope            = ""
        self.using            = ""
        self.includes         = []
        self.class_strings    = {}
        self.function_strings = []
        self.func_templates   = {}

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
    #     Generate Free Functions
    # ----------------------------------------------------------------

    @staticmethod
    def generate_function_body (func, py_name = ""):
        val  = "    boost::python::def(\n"
        val += "        \"" + (func.name if py_name == "" else py_name) + "\",\n"
        val += "        ( " + func.type + " ( * )( "
        val += ", ".join(
            (
                param.type
            ) for param in func.params
        )
        val += " )"
        val += ")( &" + func.cpp_full_name() + " )"
        if len(func.params) > 0:
            val += ",\n        ( " + ", ".join (
                (
                    "boost::python::arg(\"" + param.name + "\")" + (
                        "" if param.value == "" else
                        "=(" + param.type + ")(" + param.value + ")"
                    )
                ) for param in func.params
            ) + " )"
        if func.type.strip().endswith("*") or func.type.strip().endswith("&"):
            val += ",\n        boost::python::return_value_policy<boost::python::reference_existing_object>()"
        if func.briefdescription != "":
            # val += ",\n            \"" + func.briefdescription + "\"\n"
            val += ",\n        get_docstring(\"" + func.cpp_signature() + "\")\n"
        else:
            val += "\n"
        val += "    );\n"
        return val

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
    def generate_class_header (clss):
        if len(clss.ctors) > 0:
            ctor_val  = ", " + BoostPythonWriter.generate_class_constructor(clss.ctors[0])
        else:
            ctor_val  = ""

        if clss.template_params is None:
            ctype = clss.cpp_full_name()
            name  = "\"" + clss.name + "\""
        else:
            ctype = clss.name + "Type"
            name  = "name.c_str()"

        val  = "    boost::python::class_< " + ctype + " > "
        val += "( " + name + ctor_val + " )\n"
        if len(clss.ctors) > 1:
            for i in range(1, len(clss.ctors)):
                # Skip move constructor
                if len(clss.ctors[i].params) == 1 and clss.ctors[i].params[0].type == clss.name + " &&":
                    continue

                val += "        .def( "
                val += BoostPythonWriter.generate_class_constructor(clss.ctors[i])
                val += " )\n"
        return val

    # ----------------------------------------------------------------
    #     Generate Class Methods
    # ----------------------------------------------------------------

    @staticmethod
    def generate_class_function_body (func, ctype=None, py_name=None):
        # For class templates, the function type argument is not the class name itself,
        # so it needs to be provided. For normales classes, we however infer it from the class name:
        if ctype is None:
            ctype = func.parent.cpp_full_name()

        val  = "        .def(\n"
        val += "            \"" + (func.name if py_name == None else py_name) + "\",\n"
        val += "            ( " + func.type + " ( "
        val += "*" if func.static else ctype + "::*"
        val += " )( "
        val += ", ".join(
            (
                param.type
            ) for param in func.params
        )
        val += " )"
        val += " const " if func.const else ""
        val += ")( &" + ctype + "::" + func.name + " )"
        if len(func.params) > 0:
            val += ",\n            ( " + ", ".join (
                (
                    "boost::python::arg(\"" + param.name + "\")" + (
                        "" if param.value == "" else
                        "=(" + param.type + ")(" + param.value + ")"
                    )
                ) for param in func.params
            ) + " )"
        if func.type.strip().endswith("*") or func.type.strip().endswith("&"):
            val += ",\n            boost::python::return_value_policy<boost::python::reference_existing_object>()"
        if func.briefdescription != "":
            # val += ",\n            \"" + func.briefdescription + "\"\n"
            val += ",\n            get_docstring(\"" + func.cpp_signature() + "\")\n"
        else:
            val += "\n"
        val += "        )\n"

        # TODO if there are overloaded static functions, the static delcarations needs to come
        # after all of them! so maybe, add this to the end of the class definition instead.
        if func.static:
            val += "        .staticmethod(\""
            val += func.name if py_name == None else py_name
            val += "\")\n"
        return val

    @staticmethod
    def generate_class_methods (clss):
        if len(clss.methods) == 0:
            return ""

        if clss.template_params is None:
            ctype = None
        else:
            ctype = clss.name + "Type"

        val = "\n        // Public Member Functions\n\n"
        m_list = []
        for func in clss.methods:
            m_list.append(BoostPythonWriter.generate_class_function_body (func, ctype=ctype))

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

        symbol = op.name[len("operator"):].strip()
        if symbol in [ "+=", "-=", "*=", "/=", "%=", ">>=", "<<=", "&=", "^=", "|=" ]:
            return (symbol, "inplace")

        if symbol in [ "==", "!=", "<", ">", "<=", ">=" ]:
            return (symbol, "comparison")

        if symbol in [ "-", "+", "~", "!" ]:
            return (symbol, "unary")

        if symbol == "[]":
            return (symbol, "array")

        if symbol == "()":
            return (symbol, "access")

        if symbol == "<<":
            return (symbol, "ostream")

        if symbol in [ "*", "->" ]:
            return (symbol, "dereference")

        if symbol in [ "++", "--" ]:
            return (symbol, "crement")

        if symbol == "=":
            return (symbol, "assignment")

        if symbol in [ "bool" ]:
            return (symbol, "conversion")

        return None

    @staticmethod
    def generate_class_operators (clss):
        if len(clss.operators) == 0:
            return ""

        if clss.template_params is None:
            ctype = None
        else:
            ctype = clss.name + "Type"

        # TODO missing doc strings here!
        val = ""
        for operator in clss.operators:
            op_class = BoostPythonWriter.classify_operator(operator)
            if op_class is None:
                print "Weird. Empty operator in class", clss.name
                pass

            elif op_class[1] in [ "inplace", "comparison" ]:
                val += "        .def( boost::python::self " + op_class[0] + " boost::python::self )\n"

            elif op_class[1] == "unary":
                val += "        .def( " + op_class[0] + "boost::python::self )\n"

            elif op_class[1] == "array":
                val += BoostPythonWriter.generate_class_function_body (operator, ctype=ctype, py_name="__getitem__")

            elif op_class[1] == "access":
                pass

            elif op_class[1] == "ostream":
                val += "        .def( boost::python::self_ns::str( boost::python::self ) )\n"

            elif op_class[1] in [ "dereference", "crement", "assignment", "conversion" ]:
                pass

            else:
                print "Operator type not handled:", op_class[1], operator.name
                pass

        # If we actually added operators, make a section for them.
        if val != "":
            val = "\n        // Operators\n\n" + val
        return val

    # ----------------------------------------------------------------
    #     Generate Class Iterators
    # ----------------------------------------------------------------

    @staticmethod
    def generate_class_iterators (clss):
        if len(clss.iterators) == 0:
            return ""

        if clss.template_params is None:
            ctype = clss.cpp_full_name()
        else:
            ctype = clss.name + "Type"

        # TODO missing doc strings here!
        # TODO add iterators with parameters
        val = "\n        // Iterators\n\n"
        for it in clss.iterators:
            if it.name == "__iter__":
                val += "        .def"
            else:
                val += "        .add_property"
            val += "(\n            \"" + it.name + "\",\n            boost::python::range ( &"
            val += ctype + "::" + it.begin + ", &"
            val += ctype + "::" + it.end
            val += " )\n        )\n"

        return val

    # ----------------------------------------------------------------
    #     Generate Class
    # ----------------------------------------------------------------

    @staticmethod
    def generate_class (clss):
        val = BoostPythonWriter.make_section_header_minor ("Class " + clss.name)

        if clss.template_params is None:
            val += BoostPythonWriter.generate_class_header (clss)
        else:
            val += "    using namespace " + clss.parent.cpp_full_name() + ";\n\n"
            val += "    using " + clss.name + "Type = "
            val += clss.name + "<" + ", ".join(clss.template_params) + ">;\n\n"
            val += BoostPythonWriter.generate_class_header (clss)

        val += BoostPythonWriter.generate_class_methods (clss)
        val += BoostPythonWriter.generate_class_operators (clss)
        val += BoostPythonWriter.generate_class_iterators (clss)
        val += "    ;\n"
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
    #     Generate Docstring File
    # ----------------------------------------------------------------

    @staticmethod
    def generate_docstring_file (namespace, directory):
        written_signatures = []
        fn = os.path.join(directory, "docstrings.cpp")
        f = open(fn, 'w')

        f.write("/**\n")
        f.write("* @brief Documentation strings for the Python module.\n")
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

        def write_docstring( func ):
            if func.briefdescription != "" or func.detaileddescription != "":
                if func.cpp_signature() in written_signatures:
                    print "Warn: Signature already in docstring file:", func.cpp_signature()
                else:
                    written_signatures.append( func.cpp_signature() )

                f.write("    {\"" + func.cpp_signature() + "\", \"")
                if func.briefdescription != "":
                    f.write(CppEscapeString(func.briefdescription))
                if func.briefdescription != "" and func.detaileddescription != "":
                    f.write("\\n\\n")
                if func.detaileddescription != "":
                    f.write(CppEscapeString(func.detaileddescription))
                f.write("\"},\n")

        for clss in namespace.get_all_classes():
            for func in sorted(clss.methods, key=lambda x: x.name):
                write_docstring(func)
            f.write ("\n")

        for func in sorted(namespace.get_all_functions()):
            write_docstring(func)

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

    # ----------------------------------------------------------------
    #     Generate Docstring File
    # ----------------------------------------------------------------

    @staticmethod
    def generate_main_bindings_file (export_files, directory):
        """Helper function for the old export way, where the module main file calls all export
        functions explicitly. Not used any more, due to the export registry."""

        fn = os.path.join(directory, "bindings.cpp")
        f = open(fn, 'w')
        f.write ("#include <boost/python.hpp>\n")
        f.write (BoostPythonWriter.make_section_header_major("Forward declarations of all exported classes"))
        for fn, exp in export_files.iteritems():
             for clss_name, clss_str in exp.class_strings.iteritems():
                 f.write ("void BoostPythonExport_" + clss_name + "();\n")

        f.write (BoostPythonWriter.make_section_header_major("Boost Python Module"))
        f.write ("BOOST_PYTHON_MODULE(" + module_name + ")\n{\n")
        for fn, exp in export_files.iteritems():
             for clss_name, clss_str in exp.class_strings.iteritems():
                 f.write ("    BoostPythonExport_" + clss_name + "();\n")
        f.write ("}\n")
        f.close()

    # ----------------------------------------------------------------
    #     Write export files
    # ----------------------------------------------------------------

    @staticmethod
    def write_export_files (export_files, directory):
        # Write all the files.
        for filename, exp in export_files.iteritems():
            if filename.startswith(".") or filename.startswith("/"):
                filename = "unnamed" + filename
            fn = os.path.join(directory, filename)
            # print "Creating file", fn

            if not os.path.exists(os.path.dirname(fn)):
                os.makedirs(os.path.dirname(fn))

            if os.path.isfile(fn):
                print "Warn: File already exists:", fn

            # Write file intro.
            f = open(fn, 'w')
            f.write("/**\n")
            f.write(" * @brief\n")
            f.write(" *\n")
            f.write(" * @file\n")
            f.write(" * @ingroup python\n")
            f.write(" */\n\n")

            # Write includes.
            # f.write ("#include <boost/python.hpp>\n")
            f.write ("#include <python/src/common.hpp>\n\n")
            # for inc in set(exp.includes):
            #     f.write ("#include \"lib/" + inc + "\"\n")
            f.write ("#include \"lib/genesis.hpp\"\n")

            if exp.using != "":
                f.write( "\nusing namespace " + exp.using + ";\n" )

            # Write classes.
            for clss_name, clss_str in exp.class_strings.iteritems():
                f.write ("\n")
                # f.write ("void BoostPythonExport_" + clss_name + "()\n{")
                f.write (clss_str)
                # f.write ("\n}\n\n")

            # Write free functions.
            if len(exp.function_strings) > 0:
                identifier = os.path.splitext(filename)[0].replace("/", "_").replace(".", "_") + "_export"
                f.write("\nPYTHON_EXPORT_FUNCTIONS(" + identifier + ", \"" + exp.scope + "\")\n{\n")
                for func_str in exp.function_strings:
                    f.write ("\n")
                    # f.write ("void BoostPythonExport_" + clss_name + "()\n{")
                    f.write (func_str)
                    # f.write ("\n}\n\n")
                f.write("}\n")

            # Write function templates.
            if len(exp.func_templates) > 0:
                for tmpl_params, func_str in exp.func_templates.iteritems():
                    identifier  = os.path.splitext(filename)[0].replace("/", "_").replace(".", "_")
                    identifier += "_" + tmpl_params.replace("class", "").replace("typename", "").replace(" ", "").replace(",", "_")

                    f.write( "\ntemplate<" + tmpl_params + ">\n" )
                    f.write( "void python_export_function_" + identifier + " ()\n{\n" )
                    f.write( "\n".join(func_str) + "}\n" )
                f.write("\n")

            f.close()

    # ----------------------------------------------------------------
    #     Generate Files
    # ----------------------------------------------------------------

    @staticmethod
    def generate_files (namespace, directory, module_name):
        # Store a dict of file name -> file content.
        export_files = {}

        # Collect exports for all classes.
        for clss in namespace.get_all_classes():
            scope = clss.cpp_full_name().split("::")
            if scope[0] != "" or scope[1] != module_name:
                # print "Weird scope:", scope
                continue
            if len(scope) > 4:
                # print "Passing scope", scope
                continue
            scope = ".".join( scope[ 2 : len(scope)-1 ])

            inc_file  = os.path.splitext(clss.location)[0] + ".hpp"

            if clss.template_params is None:
                clss_file = os.path.splitext(clss.location)[0] + ".cpp"
            else:
                clss_file = os.path.splitext(clss.location)[0] + ".hpp"

            if not export_files.has_key(clss_file):
                export_files[clss_file] = ExportFile()
                export_files[clss_file].scope = scope

            if export_files[clss_file].scope != scope:
                print "Warn: Multiple scopes in one file:", export_files[clss_file].scope, "and", scope

            if export_files[clss_file].class_strings.has_key(clss.name):
                print "Warn. Export file", clss_file, "already has class", clss.name


            if clss.template_params is None:
                if export_files[clss_file].using not in [ "", clss.parent.cpp_full_name() ]:
                    print "Warn: using namespace already set to", export_files[clss_file].using, "instead of", clss.parent.cpp_full_name();
                export_files[clss_file].using = clss.parent.cpp_full_name()

                # clss_str  = "using namespace " + clss.parent.cpp_full_name() + ";\n\n"
                clss_str  = "PYTHON_EXPORT_CLASS (" + clss.name + ", \"" + scope + "\")\n{\n"
                clss_str += BoostPythonWriter.generate_class(clss)
                clss_str += "}\n"
            else:
                clss_str  = "template <" + ", ".join(clss.template_params) + ">\n"
                clss_str += "void PythonExportClass_" + clss.name + "(std::string name)\n{\n"
                clss_str += BoostPythonWriter.generate_class(clss)
                clss_str += "}\n"

            export_files[clss_file].includes.append( inc_file )
            export_files[clss_file].class_strings[clss.name] = clss_str

        # Collect exports for all free functions.
        for func in namespace.get_all_functions():
            scope = func.cpp_full_name().split("::")
            if scope[0] != "" or scope[1] != module_name:
                # print "Weird scope:", scope
                continue
            if len(scope) > 4:
                # print "Passing scope", scope
                continue
            scope = ".".join( scope[ 2 : len(scope)-1 ])

            inc_file  = os.path.splitext(func.location)[0] + ".hpp"

            if func.template_params is None:
                func_file = os.path.splitext(func.location)[0] + ".cpp"
            else:
                func_file = os.path.splitext(func.location)[0] + ".hpp"

            if not export_files.has_key(func_file):
                export_files[func_file] = ExportFile()
                export_files[func_file].scope = scope

            if export_files[func_file].scope != scope:
                print "Warn: Multiple scopes in one file:", export_files[func_file].scope, "and", scope

            if export_files[func_file].using not in [ "", func.parent.cpp_full_name() ]:
                print "Warn: using namespace already set to", export_files[func_file].using, "instead of", func.parent.cpp_full_name();

            export_files[func_file].using = func.parent.cpp_full_name()
            export_files[func_file].includes.append( inc_file )

            if func.template_params is None:
                func_str = BoostPythonWriter.generate_function_body(func)
                export_files[func_file].function_strings.append( func_str )
            else:
                tmpl_params = ", ".join(func.template_params)
                func_str = BoostPythonWriter.generate_function_body(func)
                if not export_files[func_file].func_templates.has_key(tmpl_params):
                    export_files[func_file].func_templates[tmpl_params] = []
                export_files[func_file].func_templates[tmpl_params].append(func_str)

        # Write all files.
        BoostPythonWriter.write_export_files( export_files, directory )
        BoostPythonWriter.generate_docstring_file( namespace, directory )
        # BoostPythonWriter.generate_main_bindings_file( export_files, directory )

    @staticmethod
    def generate (namespace):
        return BoostPythonWriter.generate_namespace (namespace)

# ==================================================================================================
#     Main
# ==================================================================================================

if __name__ == "__main__":
    print "This has no main function."
    sys.exit()
