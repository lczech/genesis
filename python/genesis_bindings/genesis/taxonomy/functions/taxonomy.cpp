#include <functional>
#include <genesis/taxonomy/formats/taxopath_generator.hpp>
#include <genesis/taxonomy/functions/taxonomy.hpp>
#include <genesis/taxonomy/functions/taxopath.hpp>
#include <genesis/taxonomy/taxon.hpp>
#include <genesis/taxonomy/taxon_data.hpp>
#include <genesis/taxonomy/taxonomy.hpp>
#include <genesis/taxonomy/taxopath.hpp>
#include <iterator>
#include <memory>
#include <sstream> // __str__
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <functional>
#include <pybind11/pybind11.h>
#include <string>
#include <pybind11/functional.h>
#include <../python/custom_bindings/extensions/utils/matrix.hpp>
#include <genesis/utils/color/functions.hpp>
#include <../python/custom_bindings/extensions/utils/bitvector.hpp>
#include <../python/custom_bindings/extensions/utils/range.hpp>
#include <../python/custom_bindings/extensions/string_target.hpp>
#include <../python/custom_bindings/extensions/sequence/quality.hpp>
#include <../python/custom_bindings/extensions/sequence/sequence_set.hpp>
#include <../python/custom_bindings/extensions/sequence/sequence_dict.hpp>
#include <../python/custom_bindings/extensions/sequence/fasta_input_iterator.hpp>
#include <../python/custom_bindings/extensions/sequence/fastq_input_iterator.hpp>
#include <../python/custom_bindings/extensions/sequence/reference_genome.hpp>
#include <../python/custom_bindings/extensions/taxonomy/taxopath.hpp>
#include <../python/custom_bindings/extensions/taxonomy/iterator.hpp>
#include <../python/custom_bindings/extensions/tree/tree.hpp>
#include <../python/custom_bindings/extensions/tree/functions_tree.hpp>
#include <../python/custom_bindings/extensions/placement/helper.hpp>
#include <pybind11/stl.h>


#ifndef BINDER_PYBIND11_TYPE_CASTER
	#define BINDER_PYBIND11_TYPE_CASTER
	PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>)
	PYBIND11_DECLARE_HOLDER_TYPE(T, T*)
	PYBIND11_MAKE_OPAQUE(std::shared_ptr<void>)
#endif

void bind_genesis_taxonomy_functions_taxonomy(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	// genesis::taxonomy::taxa_count_lowest_levels(const class genesis::taxonomy::Taxonomy &) file:genesis/taxonomy/functions/taxonomy.hpp line:261
	M("genesis::taxonomy").def("taxa_count_lowest_levels", (unsigned long (*)(const class genesis::taxonomy::Taxonomy &)) &genesis::taxonomy::taxa_count_lowest_levels, "Return the number of lowest level \n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\nC++: genesis::taxonomy::taxa_count_lowest_levels(const class genesis::taxonomy::Taxonomy &) --> unsigned long", pybind11::arg("tax"));

	// genesis::taxonomy::taxa_count_at_level(const class genesis::taxonomy::Taxonomy &, unsigned long) file:genesis/taxonomy/functions/taxonomy.hpp line:274
	M("genesis::taxonomy").def("taxa_count_at_level", (unsigned long (*)(const class genesis::taxonomy::Taxonomy &, unsigned long)) &genesis::taxonomy::taxa_count_at_level, "Count the number of \n\n\n\n\n\n\n\n\n\n\nC++: genesis::taxonomy::taxa_count_at_level(const class genesis::taxonomy::Taxonomy &, unsigned long) --> unsigned long", pybind11::arg("tax"), pybind11::arg("level"));

	// genesis::taxonomy::taxa_count_levels(const class genesis::taxonomy::Taxonomy &) file:genesis/taxonomy/functions/taxonomy.hpp line:287
	M("genesis::taxonomy").def("taxa_count_levels", (class std::vector<unsigned long, class std::allocator<unsigned long> > (*)(const class genesis::taxonomy::Taxonomy &)) &genesis::taxonomy::taxa_count_levels, "Count the number of \n\n\n\n\n\n\n\n\n\n\nC++: genesis::taxonomy::taxa_count_levels(const class genesis::taxonomy::Taxonomy &) --> class std::vector<unsigned long, class std::allocator<unsigned long> >", pybind11::arg("tax"));

	// genesis::taxonomy::taxa_count_with_rank(const class genesis::taxonomy::Taxonomy &, const std::string &, bool) file:genesis/taxonomy/functions/taxonomy.hpp line:299
	M("genesis::taxonomy").def("taxa_count_with_rank", [](const class genesis::taxonomy::Taxonomy & a0, const std::string & a1) -> unsigned long { return genesis::taxonomy::taxa_count_with_rank(a0, a1); }, "", pybind11::arg("tax"), pybind11::arg("rank"));
	M("genesis::taxonomy").def("taxa_count_with_rank", (unsigned long (*)(const class genesis::taxonomy::Taxonomy &, const std::string &, bool)) &genesis::taxonomy::taxa_count_with_rank, "Count the number of \n\n\n\n\n\n\n\n\n\nC++: genesis::taxonomy::taxa_count_with_rank(const class genesis::taxonomy::Taxonomy &, const std::string &, bool) --> unsigned long", pybind11::arg("tax"), pybind11::arg("rank"), pybind11::arg("case_sensitive"));

	// genesis::taxonomy::taxa_count_ranks(const class genesis::taxonomy::Taxonomy &, bool) file:genesis/taxonomy/functions/taxonomy.hpp line:320
	M("genesis::taxonomy").def("taxa_count_ranks", [](const class genesis::taxonomy::Taxonomy & a0) -> std::unordered_map<std::string, unsigned long, struct std::hash<std::string>, struct std::equal_to<std::string >, class std::allocator<struct std::pair<const std::string, unsigned long> > > { return genesis::taxonomy::taxa_count_ranks(a0); }, "", pybind11::arg("tax"));
	M("genesis::taxonomy").def("taxa_count_ranks", (class std::unordered_map<std::string, unsigned long, struct std::hash<std::string>, struct std::equal_to<std::string >, class std::allocator<struct std::pair<const std::string, unsigned long> > > (*)(const class genesis::taxonomy::Taxonomy &, bool)) &genesis::taxonomy::taxa_count_ranks, "Count the number of \n\n\n\n\n\n\n\n\n\n\n\n\n\n\nC++: genesis::taxonomy::taxa_count_ranks(const class genesis::taxonomy::Taxonomy &, bool) --> class std::unordered_map<std::string, unsigned long, struct std::hash<std::string>, struct std::equal_to<std::string >, class std::allocator<struct std::pair<const std::string, unsigned long> > >", pybind11::arg("tax"), pybind11::arg("case_sensitive"));

	// genesis::taxonomy::has_unique_ids(const class genesis::taxonomy::Taxonomy &) file:genesis/taxonomy/functions/taxonomy.hpp line:328
	M("genesis::taxonomy").def("has_unique_ids", (bool (*)(const class genesis::taxonomy::Taxonomy &)) &genesis::taxonomy::has_unique_ids, "Return `true` iff all IDs of the \n\n\nC++: genesis::taxonomy::has_unique_ids(const class genesis::taxonomy::Taxonomy &) --> bool", pybind11::arg("tax"));

	// genesis::taxonomy::sort_by_name(class genesis::taxonomy::Taxonomy &, bool, bool) file:genesis/taxonomy/functions/taxonomy.hpp line:346
	M("genesis::taxonomy").def("sort_by_name", [](class genesis::taxonomy::Taxonomy & a0) -> void { return genesis::taxonomy::sort_by_name(a0); }, "", pybind11::arg("tax"));
	M("genesis::taxonomy").def("sort_by_name", [](class genesis::taxonomy::Taxonomy & a0, bool const & a1) -> void { return genesis::taxonomy::sort_by_name(a0, a1); }, "", pybind11::arg("tax"), pybind11::arg("recursive"));
	M("genesis::taxonomy").def("sort_by_name", (void (*)(class genesis::taxonomy::Taxonomy &, bool, bool)) &genesis::taxonomy::sort_by_name, "Sort the \n\n\n\n\n\n\n\n\n\n\n\nC++: genesis::taxonomy::sort_by_name(class genesis::taxonomy::Taxonomy &, bool, bool) --> void", pybind11::arg("tax"), pybind11::arg("recursive"), pybind11::arg("case_sensitive"));

	// genesis::taxonomy::remove_taxa_at_level(class genesis::taxonomy::Taxonomy &, unsigned long) file:genesis/taxonomy/functions/taxonomy.hpp line:358
	M("genesis::taxonomy").def("remove_taxa_at_level", (void (*)(class genesis::taxonomy::Taxonomy &, unsigned long)) &genesis::taxonomy::remove_taxa_at_level, "Remove all \n\n\n\n\n\n\n\n\n\nC++: genesis::taxonomy::remove_taxa_at_level(class genesis::taxonomy::Taxonomy &, unsigned long) --> void", pybind11::arg("tax"), pybind11::arg("level"));

	// genesis::taxonomy::validate(const class genesis::taxonomy::Taxonomy &, bool) file:genesis/taxonomy/functions/taxonomy.hpp line:386
	M("genesis::taxonomy").def("validate", [](const class genesis::taxonomy::Taxonomy & a0) -> bool { return genesis::taxonomy::validate(a0); }, "", pybind11::arg("taxonomy"));
	M("genesis::taxonomy").def("validate", (bool (*)(const class genesis::taxonomy::Taxonomy &, bool)) &genesis::taxonomy::validate, "Validate the internal data structures of a Taxonomy and its child\n \n\n\n\n\n\n\n\n\n\n\n\n\n\nC++: genesis::taxonomy::validate(const class genesis::taxonomy::Taxonomy &, bool) --> bool", pybind11::arg("taxonomy"), pybind11::arg("stop_at_first_error"));

	// genesis::taxonomy::add_from_taxopath(class genesis::taxonomy::Taxonomy &, const class genesis::taxonomy::Taxopath &, bool) file:genesis/taxonomy/functions/taxopath.hpp line:49
	M("genesis::taxonomy").def("add_from_taxopath", [](class genesis::taxonomy::Taxonomy & a0, const class genesis::taxonomy::Taxopath & a1) -> genesis::taxonomy::Taxon & { return genesis::taxonomy::add_from_taxopath(a0, a1); }, "", pybind11::return_value_policy::reference_internal, pybind11::arg("taxonomy"), pybind11::arg("taxopath"));
	M("genesis::taxonomy").def("add_from_taxopath", (class genesis::taxonomy::Taxon & (*)(class genesis::taxonomy::Taxonomy &, const class genesis::taxonomy::Taxopath &, bool)) &genesis::taxonomy::add_from_taxopath, "C++: genesis::taxonomy::add_from_taxopath(class genesis::taxonomy::Taxonomy &, const class genesis::taxonomy::Taxopath &, bool) --> class genesis::taxonomy::Taxon &", pybind11::return_value_policy::reference_internal, pybind11::arg("taxonomy"), pybind11::arg("taxopath"), pybind11::arg("expect_parents"));

	// genesis::taxonomy::find_taxon_by_taxopath(const class genesis::taxonomy::Taxonomy &, const class genesis::taxonomy::Taxopath &) file:genesis/taxonomy/functions/taxopath.hpp line:55
	M("genesis::taxonomy").def("find_taxon_by_taxopath", (const class genesis::taxonomy::Taxon * (*)(const class genesis::taxonomy::Taxonomy &, const class genesis::taxonomy::Taxopath &)) &genesis::taxonomy::find_taxon_by_taxopath, "C++: genesis::taxonomy::find_taxon_by_taxopath(const class genesis::taxonomy::Taxonomy &, const class genesis::taxonomy::Taxopath &) --> const class genesis::taxonomy::Taxon *", pybind11::return_value_policy::reference_internal, pybind11::arg("tax"), pybind11::arg("taxopath"));

	// genesis::taxonomy::find_taxon_by_taxopath(class genesis::taxonomy::Taxonomy &, const class genesis::taxonomy::Taxopath &) file:genesis/taxonomy/functions/taxopath.hpp line:56
	M("genesis::taxonomy").def("find_taxon_by_taxopath", (class genesis::taxonomy::Taxon * (*)(class genesis::taxonomy::Taxonomy &, const class genesis::taxonomy::Taxopath &)) &genesis::taxonomy::find_taxon_by_taxopath, "C++: genesis::taxonomy::find_taxon_by_taxopath(class genesis::taxonomy::Taxonomy &, const class genesis::taxonomy::Taxopath &) --> class genesis::taxonomy::Taxon *", pybind11::return_value_policy::reference_internal, pybind11::arg("tax"), pybind11::arg("taxopath"));

	{ // genesis::taxonomy::TaxopathGenerator file:genesis/taxonomy/formats/taxopath_generator.hpp line:74
		pybind11::class_<genesis::taxonomy::TaxopathGenerator, std::shared_ptr<genesis::taxonomy::TaxopathGenerator>> cl(M("genesis::taxonomy"), "TaxopathGenerator", "Helper class to generate a taxonomic path string from a Taxopath object\n or a Taxon.\n\n This class bundles the parameters used for generating a taxonomic path string.\n This is needed in order to allow customization of the generation process.\n The result of the process is a string representation of the taxonomic path, with all\n names of the super-taxa of the given taxon (and the taxon itself), concatenated using the\n \n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
		cl.def( pybind11::init( [](){ return new genesis::taxonomy::TaxopathGenerator(); } ) );
		cl.def( pybind11::init( [](genesis::taxonomy::TaxopathGenerator const &o){ return new genesis::taxonomy::TaxopathGenerator(o); } ) );

		pybind11::enum_<genesis::taxonomy::TaxopathGenerator::TaxonField>(cl, "TaxonField", "")
			.value("kName", genesis::taxonomy::TaxopathGenerator::TaxonField::kName)
			.value("kRank", genesis::taxonomy::TaxopathGenerator::TaxonField::kRank)
			.value("kId", genesis::taxonomy::TaxopathGenerator::TaxonField::kId);

		cl.def("assign", (class genesis::taxonomy::TaxopathGenerator & (genesis::taxonomy::TaxopathGenerator::*)(const class genesis::taxonomy::TaxopathGenerator &)) &genesis::taxonomy::TaxopathGenerator::operator=, "C++: genesis::taxonomy::TaxopathGenerator::operator=(const class genesis::taxonomy::TaxopathGenerator &) --> class genesis::taxonomy::TaxopathGenerator &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("to_string", (std::string (genesis::taxonomy::TaxopathGenerator::*)(const class genesis::taxonomy::Taxopath &) const) &genesis::taxonomy::TaxopathGenerator::to_string, "Return a string representation of a Taxopath.\n\n This generator function uses the settings of this class to generate the string.\n\nC++: genesis::taxonomy::TaxopathGenerator::to_string(const class genesis::taxonomy::Taxopath &) const --> std::string", pybind11::arg("taxopath"));
		cl.def("__call__", (std::string (genesis::taxonomy::TaxopathGenerator::*)(const class genesis::taxonomy::Taxopath &) const) &genesis::taxonomy::TaxopathGenerator::operator(), "Shortcut function alias for\n \n\n\n\n\n\nC++: genesis::taxonomy::TaxopathGenerator::operator()(const class genesis::taxonomy::Taxopath &) const --> std::string", pybind11::arg("taxopath"));
		cl.def("to_string", (std::string (genesis::taxonomy::TaxopathGenerator::*)(const class genesis::taxonomy::Taxon &) const) &genesis::taxonomy::TaxopathGenerator::to_string, "Return a string representation of a Taxon.\n\n This generator function uses the settings of this class to generate the string.\n\nC++: genesis::taxonomy::TaxopathGenerator::to_string(const class genesis::taxonomy::Taxon &) const --> std::string", pybind11::arg("taxon"));
		cl.def("__call__", (std::string (genesis::taxonomy::TaxopathGenerator::*)(const class genesis::taxonomy::Taxon &) const) &genesis::taxonomy::TaxopathGenerator::operator(), "Shortcut function alias for\n \n\n\n\n\n\nC++: genesis::taxonomy::TaxopathGenerator::operator()(const class genesis::taxonomy::Taxon &) const --> std::string", pybind11::arg("taxon"));
		cl.def("field", (class genesis::taxonomy::TaxopathGenerator & (genesis::taxonomy::TaxopathGenerator::*)(enum genesis::taxonomy::TaxopathGenerator::TaxonField)) &genesis::taxonomy::TaxopathGenerator::field, "Set the string field to use for Taxon input.\n\n The Taxon class stores three strings that describe a taxon: its name, its rank, and an ID.\n Using this setting, one can chose which of the fields to use when generating a taxopath.\n\nC++: genesis::taxonomy::TaxopathGenerator::field(enum genesis::taxonomy::TaxopathGenerator::TaxonField) --> class genesis::taxonomy::TaxopathGenerator &", pybind11::return_value_policy::reference_internal, pybind11::arg("value"));
		cl.def("field", (enum genesis::taxonomy::TaxopathGenerator::TaxonField (genesis::taxonomy::TaxopathGenerator::*)() const) &genesis::taxonomy::TaxopathGenerator::field, "Return the currelty set field to use for describing a Taxon.\n\n See \n\n\nC++: genesis::taxonomy::TaxopathGenerator::field() const --> enum genesis::taxonomy::TaxopathGenerator::TaxonField");
		cl.def("delimiter", (class genesis::taxonomy::TaxopathGenerator & (genesis::taxonomy::TaxopathGenerator::*)(const std::string &)) &genesis::taxonomy::TaxopathGenerator::delimiter, "Set the string used to join the taxonomic path string elements.\n\n This value is used in between the elements of the taxonomic path string.\n Default is ';', as this is the usual value in many databases. See Taxopath for details.\n\nC++: genesis::taxonomy::TaxopathGenerator::delimiter(const std::string &) --> class genesis::taxonomy::TaxopathGenerator &", pybind11::return_value_policy::reference_internal, pybind11::arg("value"));
		cl.def("delimiter", (std::string (genesis::taxonomy::TaxopathGenerator::*)() const) &genesis::taxonomy::TaxopathGenerator::delimiter, "Return the currelty set value used to join the taxonomic path string elements.\n\n See \n\n\nC++: genesis::taxonomy::TaxopathGenerator::delimiter() const --> std::string");
		cl.def("trim_nested_duplicates", (class genesis::taxonomy::TaxopathGenerator & (genesis::taxonomy::TaxopathGenerator::*)(bool)) &genesis::taxonomy::TaxopathGenerator::trim_nested_duplicates, "Set whether lower level taxa are omitted if they are the same as the next higher level\n one.\n\n If set to `true`, lower level names are set to empty if they are the same as higher level names.\n Default is `false`, that is, nothing is trimmed.\n\n Example: For a Taxopath\n\n     [ \"Tax_1\", \"Tax_1\", \"Tax_2\" ]\n\n \n\n\n\nC++: genesis::taxonomy::TaxopathGenerator::trim_nested_duplicates(bool) --> class genesis::taxonomy::TaxopathGenerator &", pybind11::return_value_policy::reference_internal, pybind11::arg("value"));
		cl.def("trim_nested_duplicates", (bool (genesis::taxonomy::TaxopathGenerator::*)() const) &genesis::taxonomy::TaxopathGenerator::trim_nested_duplicates, "Return the currently set value whether to trim nested duplicates of taxa names.\n\n See \n\n\nC++: genesis::taxonomy::TaxopathGenerator::trim_nested_duplicates() const --> bool");
		cl.def("append_delimiter", (class genesis::taxonomy::TaxopathGenerator & (genesis::taxonomy::TaxopathGenerator::*)(bool)) &genesis::taxonomy::TaxopathGenerator::append_delimiter, "Set whether to append the delimiter string to the generated string.\n\n In many taxonomic databases, the string representation ends with the delimiter char, for example\n\n     Animalia;Vertebrata;Mammalia;Carnivora;\n\n This setting determines whether this last delimiter is appended or not.\n\nC++: genesis::taxonomy::TaxopathGenerator::append_delimiter(bool) --> class genesis::taxonomy::TaxopathGenerator &", pybind11::return_value_policy::reference_internal, pybind11::arg("value"));
		cl.def("append_delimiter", (bool (genesis::taxonomy::TaxopathGenerator::*)() const) &genesis::taxonomy::TaxopathGenerator::append_delimiter, "Return whether currently a delimiter is appended to the taxonomic path string.\n\n See \n\n\nC++: genesis::taxonomy::TaxopathGenerator::append_delimiter() const --> bool");
	}
	{ // genesis::taxonomy::Taxopath file:genesis/taxonomy/taxopath.hpp line:83
		pybind11::class_<genesis::taxonomy::Taxopath, std::shared_ptr<genesis::taxonomy::Taxopath>> cl(M("genesis::taxonomy"), "Taxopath", "Helper class to store a taxonomic path.\n\n We call a string of the form\n\n     Animalia;Vertebrata;Mammalia;Carnivora\n\n a taxonomic path. Those strings are often used in taxonomic databases, and usually\n use semicola to separate their parts.\n\n This class stores such a string, splitted into its single elements. Each such element can be\n seen as a Taxon; thus, a Taxopath can be added to a Taxonomy.\n\n For example, the above taxonomic path would be stored as\n\n     [ \"Animalia\", \"Vertebrata\", \"Mammalia\", \"Carnivora\" ]\n\n Each of those strings is then called a taxonomic path element.\n\n This class serves as an intermediate broker between a taxonomic path string and a Taxon.\n It is useful to transition between a Taxonomy and its string representation, for storing it\n in a text file or some other database format. It is also useful for looking up certain Taxa\n in a Taxonomy by using their taxonomic path.\n\n It furthermore offers all comparison operators (`==`, `!=`, `<`, `<=`, `>`, `>=`),\n which behave naturally for sorting a set of Taxopath%s (akin to `std::lexicographical_compare`).\n\n See the TaxopathParser and TaxopathGenerator classes for populating a Taxopath and for\n turning it back into a string, respectively.");
		cl.def( pybind11::init( [](){ return new genesis::taxonomy::Taxopath(); } ) );
		cl.def( pybind11::init<const class std::vector<std::string, class std::allocator<std::string > > &>(), pybind11::arg("elements") );

		cl.def( pybind11::init( [](genesis::taxonomy::Taxopath const &o){ return new genesis::taxonomy::Taxopath(o); } ) );
		cl.def("assign", (class genesis::taxonomy::Taxopath & (genesis::taxonomy::Taxopath::*)(const class genesis::taxonomy::Taxopath &)) &genesis::taxonomy::Taxopath::operator=, "C++: genesis::taxonomy::Taxopath::operator=(const class genesis::taxonomy::Taxopath &) --> class genesis::taxonomy::Taxopath &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("swap", (void (genesis::taxonomy::Taxopath::*)(class genesis::taxonomy::Taxopath &)) &genesis::taxonomy::Taxopath::swap, "Swap the elements of two Taxopath%s.\n\nC++: genesis::taxonomy::Taxopath::swap(class genesis::taxonomy::Taxopath &) --> void", pybind11::arg("other"));
		cl.def("empty", (bool (genesis::taxonomy::Taxopath::*)() const) &genesis::taxonomy::Taxopath::empty, "Return whether the Taxopath is empty, i.e., does not contain any elements.\n\nC++: genesis::taxonomy::Taxopath::empty() const --> bool");
		cl.def("size", (unsigned long (genesis::taxonomy::Taxopath::*)() const) &genesis::taxonomy::Taxopath::size, "Return the number of elements of this Taxopath.\n\nC++: genesis::taxonomy::Taxopath::size() const --> unsigned long");
		cl.def("at", (std::string & (genesis::taxonomy::Taxopath::*)(unsigned long)) &genesis::taxonomy::Taxopath::at, "Return the taxonomic element at a certain position.\n\n The function throws if the given index is not within the size of the taxonomic elements.\n\nC++: genesis::taxonomy::Taxopath::at(unsigned long) --> std::string &", pybind11::return_value_policy::reference_internal, pybind11::arg("index"));
		cl.def("__getitem__", (std::string & (genesis::taxonomy::Taxopath::*)(unsigned long)) &genesis::taxonomy::Taxopath::operator[], "Return the taxonomic element at a certain position.\n\n The function does not check whether the given index is valid.\n\nC++: genesis::taxonomy::Taxopath::operator[](unsigned long) --> std::string &", pybind11::return_value_policy::reference_internal, pybind11::arg("index"));
		cl.def("elements", (class std::vector<std::string, class std::allocator<std::string > > & (genesis::taxonomy::Taxopath::*)()) &genesis::taxonomy::Taxopath::elements, "Return the elements of the Taxopath as a vector of strings.\n\nC++: genesis::taxonomy::Taxopath::elements() --> class std::vector<std::string, class std::allocator<std::string > > &", pybind11::return_value_policy::reference_internal);
		cl.def("assign", (void (genesis::taxonomy::Taxopath::*)(const class std::vector<std::string, class std::allocator<std::string > > &)) &genesis::taxonomy::Taxopath::assign, "Replace the current elements of the Taxopath by a list of strings.\n\n The strings are copied and used as Taxopath elements in the order in which they are stored\n in the vector.\n\nC++: genesis::taxonomy::Taxopath::assign(const class std::vector<std::string, class std::allocator<std::string > > &) --> void", pybind11::arg("from"));
		cl.def("push_back", (void (genesis::taxonomy::Taxopath::*)(const std::string &)) &genesis::taxonomy::Taxopath::push_back, "Add an element to the end of the Taxopath by copying it.\n\nC++: genesis::taxonomy::Taxopath::push_back(const std::string &) --> void", pybind11::arg("value"));
		cl.def("pop_back", (std::string (genesis::taxonomy::Taxopath::*)()) &genesis::taxonomy::Taxopath::pop_back, "Remove the last element of the Taxopath and return its value.\n\n The returned value is obtained as a copy of the last element before it is removed.\n\nC++: genesis::taxonomy::Taxopath::pop_back() --> std::string");
		cl.def("clear", (void (genesis::taxonomy::Taxopath::*)()) &genesis::taxonomy::Taxopath::clear, "Clear all taxonomic elements. This results in an empty Taxopath.\n\nC++: genesis::taxonomy::Taxopath::clear() --> void");
		cl.def("begin", (class __gnu_cxx::__normal_iterator<std::string *, class std::vector<std::string, class std::allocator<std::string > > > (genesis::taxonomy::Taxopath::*)()) &genesis::taxonomy::Taxopath::begin, "Return an iterator to the beginning of the taxonomic elements.\n\nC++: genesis::taxonomy::Taxopath::begin() --> class __gnu_cxx::__normal_iterator<std::string *, class std::vector<std::string, class std::allocator<std::string > > >");
		cl.def("end", (class __gnu_cxx::__normal_iterator<std::string *, class std::vector<std::string, class std::allocator<std::string > > > (genesis::taxonomy::Taxopath::*)()) &genesis::taxonomy::Taxopath::end, "Return an iterator to the end of the taxonomic elements.\n\nC++: genesis::taxonomy::Taxopath::end() --> class __gnu_cxx::__normal_iterator<std::string *, class std::vector<std::string, class std::allocator<std::string > > >");
		cl.def("rbegin", (class std::reverse_iterator<class __gnu_cxx::__normal_iterator<std::string *, class std::vector<std::string, class std::allocator<std::string > > > > (genesis::taxonomy::Taxopath::*)()) &genesis::taxonomy::Taxopath::rbegin, "Return a reverse iterator to the reverse beginning of the taxonomic elements.\n\nC++: genesis::taxonomy::Taxopath::rbegin() --> class std::reverse_iterator<class __gnu_cxx::__normal_iterator<std::string *, class std::vector<std::string, class std::allocator<std::string > > > >");
		cl.def("rend", (class std::reverse_iterator<class __gnu_cxx::__normal_iterator<std::string *, class std::vector<std::string, class std::allocator<std::string > > > > (genesis::taxonomy::Taxopath::*)()) &genesis::taxonomy::Taxopath::rend, "Return a reverse iterator to the reverse end of the taxonomic elements.\n\nC++: genesis::taxonomy::Taxopath::rend() --> class std::reverse_iterator<class __gnu_cxx::__normal_iterator<std::string *, class std::vector<std::string, class std::allocator<std::string > > > >");

		extend_taxopath(cl);
	}
}
