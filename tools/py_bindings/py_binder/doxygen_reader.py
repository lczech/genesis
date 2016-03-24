#!/usr/bin/python

import os
import re
import xml.etree.ElementTree

from cpp_entities import *

# ==================================================================================================
#     Class: Doxygen Reader
# ==================================================================================================

class DoxygenReader:

    # ----------------------------------------------------------------
    #     Parse Member Function
    # ----------------------------------------------------------------

    @staticmethod
    def parse_function (xml_elem):
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
        func.location            = xml_elem.find("location").attrib["file"]

        # unused properties of the xml element:
        # print "definition:",x.find("definition").text
        # print "argsstring:",x.find("argsstring").text

        return func

    # ----------------------------------------------------------------
    #     Parse Class File
    # ----------------------------------------------------------------

    @staticmethod
    def parse_class_file (filename):
        tree = xml.etree.ElementTree.parse(filename)
        root = tree.getroot()

        classes = []
        # for compound in root.iter("compound"):
        for compound in root:
            if not compound.attrib["kind"] in [ "class", "struct" ]:
                continue

            clss_name = compound.find("compoundname").text.rsplit('::', 1)[1]
            clss = CppClass(clss_name)

            clss.briefdescription    = ''.join(compound.find("briefdescription").itertext()).strip()
            clss.detaileddescription = ''.join(compound.find("detaileddescription").itertext()).strip()

            # doxygen sets the class-wide location to the file where the class is first used,
            # which often is a forward declaration, but not the actual definition of the class.
            # so we need to check the location attributes of the functions instead.
            locations = set()

            for section in compound:
                if section.tag != "sectiondef":
                    continue

                if not section.attrib["kind"] in [ "public-func", "public-static-func" ]:
                    continue

                for member in section:
                    if not member.attrib["kind"] == "function":
                        print "Weird. Member in section '"+section.attrib["kind"]+"' that is not a function."
                        continue

                    func = DoxygenReader.parse_function(member)
                    func.parent = clss
                    locations.add(func.location)
                    clss.add_function (func)

            if len(locations) > 1:
                print "Weird. Multiple locations for class", clss_name, ":", locations
            if len(locations) == 0:
                clss.location = compound.find("location").attrib["file"]
            else:
                clss.location = locations.pop()

            classes.append(clss)

        classes = sorted(classes, key=lambda clss: clss.name)
        return classes

    # ----------------------------------------------------------------
    #     Parse Namespace
    # ----------------------------------------------------------------

    @staticmethod
    def parse_namespace_file (filename):
        tree = xml.etree.ElementTree.parse(filename)
        root = tree.getroot()

        functions = []
        for compound in root:
            if not compound.attrib["kind"] == "namespace":
                continue

            for section in compound:
                if section.tag != "sectiondef":
                    continue

                if not section.attrib["kind"] in [ "func" ]:
                    continue

                for member in section:
                    if not member.attrib["kind"] == "function":
                        print "Weird. Member in section '"+section.attrib["kind"]+"' that is not a function."
                        continue

                    func = DoxygenReader.parse_function(member)
                    functions.append(func)

        functions = sorted(functions, key=lambda func: func.name)
        return functions

    # ----------------------------------------------------------------
    #     Parse Doxygen Index
    # ----------------------------------------------------------------

    @staticmethod
    def parse_xml_dir (directory):
        tree = xml.etree.ElementTree.parse(os.path.join(directory, "index.xml"))
        root = tree.getroot()

        ns_global = CppNamespace("")
        for compound in root:

            # Process all elements of the namespace.
            if compound.attrib["kind"] in [ "class", "struct", "namespace" ]:

                # Get basic class information: name and namespace.
                elem_full_name = compound.find("name").text
                elem_ns_list   = elem_full_name.split("::")

                # If the element is a class or struct, it's name is not used as a namespace,
                # so we pop it from the namespace list.
                # However, if the element is a namespace, we skip this, as we need the element
                # itself as a namespace.
                if not compound.attrib["kind"] == "namespace":
                    elem_name  = elem_ns_list.pop()

                # Move into the namespace of that element,
                # create parent namespaces when necessary.
                ns_local = ns_global
                for ns in elem_ns_list:
                    ns_local.create_namespace(ns)
                    ns_tmp   = ns_local
                    ns_local = ns_local.namespaces[ns]
                    ns_local.parent = ns_tmp

                # Get the details file for the class and parse it.
                xml_file = os.path.join(directory, compound.attrib["refid"] + ".xml")

                # Parse if the element is a class or struct.
                if compound.attrib["kind"] in [ "class", "struct" ]:
                    for clss in DoxygenReader.parse_class_file (xml_file):
                        clss.parent = ns_local
                        ns_local.add_class (clss)

                # Parse if the element is a namespace.
                # Currently, the namespace parser only returns functions.
                # If this changes, the following needs to be adapted accordingly.
                if compound.attrib["kind"] == "namespace":
                    for func in DoxygenReader.parse_namespace_file (xml_file):
                        func.parent = ns_local
                        ns_local.add_function (func)

        return ns_global

# ==================================================================================================
#     Main
# ==================================================================================================

if __name__ == "__main__":
    print "This has no main function."
    sys.exit()
