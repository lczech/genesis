#!/usr/bin/python

# Genesis - A toolkit for working with phylogenetic data.
# Copyright (C) 2014-2017 Lucas Czech
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
import xml.etree.ElementTree

from cpp_entities import *

# ==================================================================================================
#     Class: Doxygen Reader
# ==================================================================================================

class DoxygenReader:

    # ----------------------------------------------------------------
    #     Parse Template Parameters
    # ----------------------------------------------------------------

    @staticmethod
    def parse_template_parameters (compound):
        if compound.find("templateparamlist") is not None:
            param_list = []
            for param in compound.find("templateparamlist"):
                if not param.tag == "param":
                    print "Warn: Unknown template parameter tag:", param.tag

                param_str = ''.join(param.find("type").itertext()).strip()
                if param.find("declname") is not None:
                    param_str += " " + ''.join(param.find("declname").itertext()).strip()
                param_list.append(param_str)
            return param_list
        return None

    # ----------------------------------------------------------------
    #     Parse Member Function
    # ----------------------------------------------------------------

    @staticmethod
    def parse_function (member):
        if member.tag != "memberdef":
            print "Invalid xml tag:", member.tag
            return None
        if member.attrib["kind"] != "function":
            print "Invalid member kind:", member.attrib["kind"]
            return None

        func = CppFunction()
        func.name = member.find("name").text
        func.type = ''.join(member.find("type").itertext())

        func.template_params = DoxygenReader.parse_template_parameters(member)

        func.prot    = (member.attrib["prot"])
        func.static  = (member.attrib["static"] == "yes")
        func.const   = (member.attrib["const"]  == "yes")
        func.virtual = (member.attrib["virt"]   == "virtual")

        for p in member.findall("param"):
            param = CppParameter()
            param.type = ''.join(p.find("type").itertext()).strip()
            if p.find("declname") is not None:
                param.name = p.find("declname").text
            if p.find("defval") is not None:
                param.value = p.find("defval").text
            if param.value is None:
                param.value = ""
            func.params.append(param)

        func.briefdescription    = ''.join(member.find("briefdescription").itertext()).strip()
        func.detaileddescription = ''.join(member.find("detaileddescription").itertext()).strip()
        func.location            = member.find("location").attrib["file"]

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

            clss.template_params     = DoxygenReader.parse_template_parameters(compound)
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
