#!/usr/bin/python

import xml.etree.ElementTree

#~ cpp over doxygen boost python binding generator
#~ codygen
#~ copygen
#~ codbpbg

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
        self.name    = ""
        self.prot    = "public"
        self.static  = False
        self.const   = False
        self.virtual = False

        self.type    = ""
        self.params  = []

        self.briefdescription    = ""
        self.detaileddescription = ""

    def cpp_string (self):
        val  = ("static " if self.static else "")
        val  = (self.type + " " if self.type != "" else "") + self.name
        val += " (" + ', '.join(x.cpp_string() for x in self.params) + ")"
        val += (" const" if self.const else "")
        return val

# ==============================================================================
#     Class: C++ class
# ==============================================================================

class CppClass:
    def __init__ (self, name):
        self.name = name

        self.ctors = []
        self.dtors = []

        self.methods   = []
        self.operators = []

        self.briefdescription    = ""
        self.detaileddescription = ""

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
                if section.tag != "sectiondef" or section.attrib["kind"] != "public-func":
                    continue

                for member in section:
                    cls.add_function (DoxygenReader.parse_member_function(member))

            classes.append(cls)

        classes = sorted(classes, key=lambda cls: cls.name)
        return classes

# ==============================================================================
#     Class: Boost Python Writer
# ==============================================================================

class BoostPythonWriter:

    # ----------------------------------------------------------------
    #     Generate Header
    # ----------------------------------------------------------------

    @staticmethod
    def generate_header (ctors):
        if len(ctors) > 0:
            ctor = ctors[0]
            ctor_val  = ", boost::python::init< "
            ctor_val += ', '.join (
                (param.type if param.value == "" else "boost::python::optional< " + param.type + " >")
                for param in ctor.params
            )
            ctor_val += " >(( "
            ctor_val += ", ".join (
                (
                    "boost::python::arg(\"" + param.name + "\")" + (
                        "" if param.value == "" else
                        "=(" + param.type + ")(" + param.value + ")"
                    )
                ) for param in ctor.params
            )
            ctor_val += " ))"
        else:
            ctor_val  = ""
        return "    boost::python::class_< " + cls.name + " > ( \"" + cls.name + "\"" + ctor_val + " )\n"

    # ----------------------------------------------------------------
    #     Generate Methods
    # ----------------------------------------------------------------

    @staticmethod
    def generate_methods (methods):
        if len(methods) == 0:
            return ""

        val = "\n        // Public Member Functions\n\n"
        for func in methods:
            val += "        .def(\n"
            val += "            \"" + func.name + "\",\n"
            val += "            ( " + func.type + " ( " + cls.name + "::*" + " )( "
            val += ", ".join(
                (
                    param.type
                ) for param in func.params
            )
            val += " )" + (" const " if func.const else "") + ")( &" + cls.name + "::" + func.name + " )"
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
        return val

    # ----------------------------------------------------------------
    #     Generate Operators
    # ----------------------------------------------------------------

   # TODO need to add friend (non-member) operators: http://www.boost.org/doc/libs/1_35_0/libs/python/doc/v2/operators.html

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
    def generate_operators (operators):
        if len(operators) == 0:
            return ""

        # TODO missing doc strings here!
        val = "\n        // Operators\n\n"
        for operator in operators:
            op_class = BoostPythonWriter.classify_operator(operator)
            if op_class is None:
                print "Unknown operator:", operator.name
            elif op_class[1] in [ "inplace", "comparison" ]:
                val += "        .def( boost::python::self " + op_class[0] + " boost::python::self )\n"
            elif op_class[1] == "unary":
                val += "        .def( " + op_class[0] + "boost::python::self )\n"
            elif op_class[1] == "array":
                val += "        .def(\n"
                val += "            \"__getitem__\",\n"

                # TODO the following code is copied from generate_methods. not nice.
                val += "            ( " + operator.type + " ( " + cls.name + "::*" + " )( "
                val += ", ".join(
                    (
                        param.type
                    ) for param in operator.params
                )
                val += " )" + (" const " if operator.const else "") + ")( &" + cls.name + "::" + operator.name + " )"
                if len(operator.params) > 0:
                    val += ",\n            ( " + ", ".join (
                        (
                            "boost::python::arg(\"" + param.name + "\")" + (
                                "" if param.value == "" else
                                "=(" + param.type + ")(" + param.value + ")"
                            )
                        ) for param in operator.params
                    ) + " )"
                if operator.briefdescription != "":
                    val += ",\n            \"" + operator.briefdescription + "\"\n"
                else:
                    val += "\n"
                val += "        )\n"
            else:
                print "Operator type not handled:", op_class[1]
        return val

    # ----------------------------------------------------------------
    #     Generate
    # ----------------------------------------------------------------

    @staticmethod
    def generate (cls):
        val  = BoostPythonWriter.generate_header (cls.ctors)
        val += BoostPythonWriter.generate_methods (cls.methods)
        val += BoostPythonWriter.generate_operators (cls.operators)
        val += "    ;"
        return val

# ==============================================================================
#     Main
# ==============================================================================

if __name__ == "__main__":
    classes = DoxygenReader.parse_class_file ("classgenesis_1_1Bitvector.xml")

    for cls in classes:
        cls.methods = sorted(cls.methods, key=lambda member: str.lower(member.name))
        print BoostPythonWriter.generate(cls)
    exit()

    for cls in classes:
        for ctor in cls.ctors:
            print ctor.cpp_string()
        for dtor in cls.dtors:
            print dtor.cpp_string()
        for func in cls.methods:
            print func.cpp_string()
        for op in cls.operators:
            print op.cpp_string()
