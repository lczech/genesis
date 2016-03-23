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
            if not compound.attrib["kind"] in [ "class", "struct" ]:
                continue

            cls_name = compound.find("compoundname").text.rsplit('::', 1)[1]
            cls = CppClass(cls_name)

            cls.briefdescription    = ''.join(compound.find("briefdescription").itertext()).strip()
            cls.detaileddescription = ''.join(compound.find("detaileddescription").itertext()).strip()

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
                    func = DoxygenReader.parse_member_function(member)
                    func.parent = cls
                    locations.add(member.find("location").attrib["file"])
                    cls.add_function (func)

            if len(locations) > 1:
                print "Weird. Multiple locations for class", cls_name, ":", locations
            if len(locations) == 0:
                cls.location = compound.find("location").attrib["file"]
            else:
                cls.location = locations.pop()

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
            if compound.attrib["kind"] in [ "class", "struct" ]:
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

# ==================================================================================================
#     Main
# ==================================================================================================

if __name__ == "__main__":
    print "This has no main function."
    sys.exit()
