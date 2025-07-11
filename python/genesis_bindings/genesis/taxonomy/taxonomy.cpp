#include <genesis/taxonomy/functions/taxonomy.hpp>
#include <genesis/taxonomy/taxon.hpp>
#include <genesis/taxonomy/taxon_data.hpp>
#include <genesis/taxonomy/taxonomy.hpp>
#include <ios>
#include <iterator>
#include <locale>
#include <memory>
#include <ostream>
#include <sstream> // __str__
#include <streambuf>
#include <string>

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

// genesis::taxonomy::Taxonomy file:genesis/taxonomy/taxonomy.hpp line:96
struct PyCallBack_genesis_taxonomy_Taxonomy : public genesis::taxonomy::Taxonomy {
	using genesis::taxonomy::Taxonomy::Taxonomy;

	class genesis::taxonomy::Taxon & add_child_(const class genesis::taxonomy::Taxon & a0, bool a1) override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const genesis::taxonomy::Taxonomy *>(this), "add_child_");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<class genesis::taxonomy::Taxon &>::value) {
				static pybind11::detail::override_caster_t<class genesis::taxonomy::Taxon &> caster;
				return pybind11::detail::cast_ref<class genesis::taxonomy::Taxon &>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<class genesis::taxonomy::Taxon &>(std::move(o));
		}
		return Taxonomy::add_child_(a0, a1);
	}
};

// genesis::taxonomy::Taxon file:genesis/taxonomy/taxon.hpp line:76
struct PyCallBack_genesis_taxonomy_Taxon : public genesis::taxonomy::Taxon {
	using genesis::taxonomy::Taxon::Taxon;

	class genesis::taxonomy::Taxon & add_child_(const class genesis::taxonomy::Taxon & a0, bool a1) override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const genesis::taxonomy::Taxon *>(this), "add_child_");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<class genesis::taxonomy::Taxon &>::value) {
				static pybind11::detail::override_caster_t<class genesis::taxonomy::Taxon &> caster;
				return pybind11::detail::cast_ref<class genesis::taxonomy::Taxon &>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<class genesis::taxonomy::Taxon &>(std::move(o));
		}
		return Taxon::add_child_(a0, a1);
	}
};

void bind_genesis_taxonomy_taxonomy(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	// genesis::taxonomy::swap(class genesis::taxonomy::Taxonomy &, class genesis::taxonomy::Taxonomy &) file:genesis/taxonomy/taxonomy.hpp line:46
	M("genesis::taxonomy").def("swap", (void (*)(class genesis::taxonomy::Taxonomy &, class genesis::taxonomy::Taxonomy &)) &genesis::taxonomy::swap, "C++: genesis::taxonomy::swap(class genesis::taxonomy::Taxonomy &, class genesis::taxonomy::Taxonomy &) --> void", pybind11::arg("lhs"), pybind11::arg("rhs"));

	{ // genesis::taxonomy::Taxonomy file:genesis/taxonomy/taxonomy.hpp line:96
		pybind11::class_<genesis::taxonomy::Taxonomy, std::shared_ptr<genesis::taxonomy::Taxonomy>, PyCallBack_genesis_taxonomy_Taxonomy> cl(M("genesis::taxonomy"), "Taxonomy", "Store a Taxonomy, i.e., a nested hierarchy of \n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
		cl.def( pybind11::init( [](){ return new genesis::taxonomy::Taxonomy(); }, [](){ return new PyCallBack_genesis_taxonomy_Taxonomy(); } ) );
		cl.def( pybind11::init( [](PyCallBack_genesis_taxonomy_Taxonomy const &o){ return new PyCallBack_genesis_taxonomy_Taxonomy(o); } ) );
		cl.def( pybind11::init( [](genesis::taxonomy::Taxonomy const &o){ return new genesis::taxonomy::Taxonomy(o); } ) );
		cl.def("assign", (class genesis::taxonomy::Taxonomy & (genesis::taxonomy::Taxonomy::*)(const class genesis::taxonomy::Taxonomy &)) &genesis::taxonomy::Taxonomy::operator=, "Copy assignment operator.\n\n We need a custom version of this in order to set the Taxon::parent() pointers of all children\n correctly when copying.\n\nC++: genesis::taxonomy::Taxonomy::operator=(const class genesis::taxonomy::Taxonomy &) --> class genesis::taxonomy::Taxonomy &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("size", (unsigned long (genesis::taxonomy::Taxonomy::*)() const) &genesis::taxonomy::Taxonomy::size, "Return the number of immediate child \n\n\n\n\nC++: genesis::taxonomy::Taxonomy::size() const --> unsigned long");
		cl.def("has_child", (bool (genesis::taxonomy::Taxonomy::*)(std::string) const) &genesis::taxonomy::Taxonomy::has_child, "Return whether an immediate child Taxon with the given name exists.\n\nC++: genesis::taxonomy::Taxonomy::has_child(std::string) const --> bool", pybind11::arg("name"));
		cl.def("get_child", (class genesis::taxonomy::Taxon & (genesis::taxonomy::Taxonomy::*)(std::string)) &genesis::taxonomy::Taxonomy::get_child, "Return the child Taxon with a given name if it exists, or throw otherwise.\n\nC++: genesis::taxonomy::Taxonomy::get_child(std::string) --> class genesis::taxonomy::Taxon &", pybind11::return_value_policy::reference_internal, pybind11::arg("name"));
		cl.def("__getitem__", (class genesis::taxonomy::Taxon & (genesis::taxonomy::Taxonomy::*)(std::string)) &genesis::taxonomy::Taxonomy::operator[], "Return the child Taxon with a given name if it exists, or throw otherwise.\n\nC++: genesis::taxonomy::Taxonomy::operator[](std::string) --> class genesis::taxonomy::Taxon &", pybind11::return_value_policy::reference_internal, pybind11::arg("name"));
		cl.def("at", (class genesis::taxonomy::Taxon & (genesis::taxonomy::Taxonomy::*)(unsigned long)) &genesis::taxonomy::Taxonomy::at, "Return the child Taxon at the given index.\n\n The function throws an exception if the index in invalid, i.e., `>=` size().\n\nC++: genesis::taxonomy::Taxonomy::at(unsigned long) --> class genesis::taxonomy::Taxon &", pybind11::return_value_policy::reference_internal, pybind11::arg("index"));
		cl.def("__getitem__", (class genesis::taxonomy::Taxon & (genesis::taxonomy::Taxonomy::*)(unsigned long)) &genesis::taxonomy::Taxonomy::operator[], "Return the child Taxon at the given index.\n\n The function does not check whether the provided index is within the valid range of size().\n\nC++: genesis::taxonomy::Taxonomy::operator[](unsigned long) --> class genesis::taxonomy::Taxon &", pybind11::return_value_policy::reference_internal, pybind11::arg("index"));
		cl.def("index_of", (unsigned long (genesis::taxonomy::Taxonomy::*)(const std::string &) const) &genesis::taxonomy::Taxonomy::index_of, "C++: genesis::taxonomy::Taxonomy::index_of(const std::string &) const --> unsigned long", pybind11::arg("name"));
		cl.def("add_child", [](genesis::taxonomy::Taxonomy &o, const class genesis::taxonomy::Taxon & a0) -> genesis::taxonomy::Taxon & { return o.add_child(a0); }, "", pybind11::return_value_policy::reference_internal, pybind11::arg("child"));
		cl.def("add_child", (class genesis::taxonomy::Taxon & (genesis::taxonomy::Taxonomy::*)(const class genesis::taxonomy::Taxon &, bool)) &genesis::taxonomy::Taxonomy::add_child, "Add a child Taxon as a copy of a given Taxon and return it.\n\n If  is `true` (default), the Taxonomy is checked for a child Taxon with\n the same name, and if one exists, it is recursively merged with the given Taxon.\n Otherwise ( is `false`), the Taxon is added, even if this creates\n another child Taxon with the same name.\n\nC++: genesis::taxonomy::Taxonomy::add_child(const class genesis::taxonomy::Taxon &, bool) --> class genesis::taxonomy::Taxon &", pybind11::return_value_policy::reference_internal, pybind11::arg("child"), pybind11::arg("merge_duplicates"));
		cl.def("add_child", [](genesis::taxonomy::Taxonomy &o, const std::string & a0) -> genesis::taxonomy::Taxon & { return o.add_child(a0); }, "", pybind11::return_value_policy::reference_internal, pybind11::arg("name"));
		cl.def("add_child", (class genesis::taxonomy::Taxon & (genesis::taxonomy::Taxonomy::*)(const std::string &, bool)) &genesis::taxonomy::Taxonomy::add_child, "Add a child Taxon by creating a new one with the given name and return it.\n\n If  is `true` (default), the Taxonomy is checked for a child Taxon with\n the same name, and if one exists, nothing is added.\n Otherwise ( is `false`), a new Taxon is added, even if this creates\n another child Taxon with the same name.\n\nC++: genesis::taxonomy::Taxonomy::add_child(const std::string &, bool) --> class genesis::taxonomy::Taxon &", pybind11::return_value_policy::reference_internal, pybind11::arg("name"), pybind11::arg("merge_duplicates"));
		cl.def("remove_child", (void (genesis::taxonomy::Taxonomy::*)(const std::string &)) &genesis::taxonomy::Taxonomy::remove_child, "Remove a child Taxon with a certain name.\n\n The Taxon (and all its sub-taxa) are destroyed.\n Throws an `std::runtime_error` if there is no Taxon with the given name.\n\nC++: genesis::taxonomy::Taxonomy::remove_child(const std::string &) --> void", pybind11::arg("name"));
		cl.def("remove_at", (void (genesis::taxonomy::Taxonomy::*)(unsigned long)) &genesis::taxonomy::Taxonomy::remove_at, "Remove a child Taxon at a certain index.\n\n The Taxon (and all its sub-taxa) are destroyed.\n Throws an `std::runtime_error` if the index is out of bounds.\n\nC++: genesis::taxonomy::Taxonomy::remove_at(unsigned long) --> void", pybind11::arg("index"));
		cl.def("clear_children", (void (genesis::taxonomy::Taxonomy::*)()) &genesis::taxonomy::Taxonomy::clear_children, "Remove all children.\n\nC++: genesis::taxonomy::Taxonomy::clear_children() --> void");

		cl.def("__str__", [](genesis::taxonomy::Taxonomy const &o) -> std::string { std::ostringstream s; genesis::taxonomy::operator<<(s, o); return s.str(); } );
	}
	// genesis::taxonomy::swap(class genesis::taxonomy::Taxon &, class genesis::taxonomy::Taxon &) file:genesis/taxonomy/taxon.hpp line:48
	M("genesis::taxonomy").def("swap", (void (*)(class genesis::taxonomy::Taxon &, class genesis::taxonomy::Taxon &)) &genesis::taxonomy::swap, "C++: genesis::taxonomy::swap(class genesis::taxonomy::Taxon &, class genesis::taxonomy::Taxon &) --> void", pybind11::arg("lhs"), pybind11::arg("rhs"));

	{ // genesis::taxonomy::Taxon file:genesis/taxonomy/taxon.hpp line:76
		pybind11::class_<genesis::taxonomy::Taxon, std::shared_ptr<genesis::taxonomy::Taxon>, PyCallBack_genesis_taxonomy_Taxon, genesis::taxonomy::Taxonomy> cl(M("genesis::taxonomy"), "Taxon", "Store a Taxon, i.e., an element in a Taxonomy, with its name, rank, ID and sub-taxa.\n\n This class models a taxon within the hierarchy of a Taxonomy. Each such taxon can have a\n \n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
		cl.def( pybind11::init( [](){ return new genesis::taxonomy::Taxon(); }, [](){ return new PyCallBack_genesis_taxonomy_Taxon(); } ) );
		cl.def( pybind11::init( [](const std::string & a0){ return new genesis::taxonomy::Taxon(a0); }, [](const std::string & a0){ return new PyCallBack_genesis_taxonomy_Taxon(a0); } ), "doc");
		cl.def( pybind11::init( [](const std::string & a0, const std::string & a1){ return new genesis::taxonomy::Taxon(a0, a1); }, [](const std::string & a0, const std::string & a1){ return new PyCallBack_genesis_taxonomy_Taxon(a0, a1); } ), "doc");
		cl.def( pybind11::init<const std::string &, const std::string &, const std::string &>(), pybind11::arg("name"), pybind11::arg("rank"), pybind11::arg("id") );

		cl.def( pybind11::init( [](PyCallBack_genesis_taxonomy_Taxon const &o){ return new PyCallBack_genesis_taxonomy_Taxon(o); } ) );
		cl.def( pybind11::init( [](genesis::taxonomy::Taxon const &o){ return new genesis::taxonomy::Taxon(o); } ) );
		cl.def("assign", (class genesis::taxonomy::Taxon & (genesis::taxonomy::Taxon::*)(const class genesis::taxonomy::Taxon &)) &genesis::taxonomy::Taxon::operator=, "Copy assignment operator.\n\n We need a custom version of this in order to set the Taxon::parent() pointers of all children\n correctly, and to treat the data correctlty when copying.\n\n See the \n\n\nC++: genesis::taxonomy::Taxon::operator=(const class genesis::taxonomy::Taxon &) --> class genesis::taxonomy::Taxon &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("name", (const std::string & (genesis::taxonomy::Taxon::*)() const) &genesis::taxonomy::Taxon::name, "Return the name of this taxon.\n\nC++: genesis::taxonomy::Taxon::name() const --> const std::string &", pybind11::return_value_policy::reference_internal);
		cl.def("name", (void (genesis::taxonomy::Taxon::*)(const std::string &)) &genesis::taxonomy::Taxon::name, "Set the name of this taxon.\n\nC++: genesis::taxonomy::Taxon::name(const std::string &) --> void", pybind11::arg("value"));
		cl.def("rank", (const std::string & (genesis::taxonomy::Taxon::*)() const) &genesis::taxonomy::Taxon::rank, "Return the rank of this taxon.\n\nC++: genesis::taxonomy::Taxon::rank() const --> const std::string &", pybind11::return_value_policy::reference_internal);
		cl.def("rank", (void (genesis::taxonomy::Taxon::*)(const std::string &)) &genesis::taxonomy::Taxon::rank, "C++: genesis::taxonomy::Taxon::rank(const std::string &) --> void", pybind11::arg("value"));
		cl.def("id", (const std::string & (genesis::taxonomy::Taxon::*)() const) &genesis::taxonomy::Taxon::id, "Set the ID of this taxon.\n\nC++: genesis::taxonomy::Taxon::id() const --> const std::string &", pybind11::return_value_policy::reference_internal);
		cl.def("id", (void (genesis::taxonomy::Taxon::*)(const std::string &)) &genesis::taxonomy::Taxon::id, "C++: genesis::taxonomy::Taxon::id(const std::string &) --> void", pybind11::arg("value"));
		cl.def("parent", (class genesis::taxonomy::Taxon * (genesis::taxonomy::Taxon::*)()) &genesis::taxonomy::Taxon::parent, "Return a pointer to the parent of this taxon, or a `nullptr` if this is the top level taxon.\n\nC++: genesis::taxonomy::Taxon::parent() --> class genesis::taxonomy::Taxon *", pybind11::return_value_policy::reference_internal);
		cl.def("has_data", (bool (genesis::taxonomy::Taxon::*)() const) &genesis::taxonomy::Taxon::has_data, "Return `true` if the Taxon has a data object assigned to it.\n\nC++: genesis::taxonomy::Taxon::has_data() const --> bool");
	}
	{ // genesis::taxonomy::DepthFirstSearch file:genesis/taxonomy/functions/taxonomy.hpp line:54
		pybind11::class_<genesis::taxonomy::DepthFirstSearch, std::shared_ptr<genesis::taxonomy::DepthFirstSearch>> cl(M("genesis::taxonomy"), "DepthFirstSearch", "Tag used for find_taxon().");
		cl.def( pybind11::init( [](){ return new genesis::taxonomy::DepthFirstSearch(); } ) );
	}
	{ // genesis::taxonomy::BreadthFirstSearch file:genesis/taxonomy/functions/taxonomy.hpp line:59
		pybind11::class_<genesis::taxonomy::BreadthFirstSearch, std::shared_ptr<genesis::taxonomy::BreadthFirstSearch>> cl(M("genesis::taxonomy"), "BreadthFirstSearch", "Tag used for find_taxon().");
		cl.def( pybind11::init( [](){ return new genesis::taxonomy::BreadthFirstSearch(); } ) );
	}
	// genesis::taxonomy::find_taxon_by_name(const class genesis::taxonomy::Taxonomy &, const std::string &) file:genesis/taxonomy/functions/taxonomy.hpp line:151
	M("genesis::taxonomy").def("find_taxon_by_name", (const class genesis::taxonomy::Taxon * (*)(const class genesis::taxonomy::Taxonomy &, const std::string &)) &genesis::taxonomy::find_taxon_by_name, "Alias for find_taxon_by_name(..., DepthFirstSearch{}).\n\nC++: genesis::taxonomy::find_taxon_by_name(const class genesis::taxonomy::Taxonomy &, const std::string &) --> const class genesis::taxonomy::Taxon *", pybind11::return_value_policy::reference_internal, pybind11::arg("tax"), pybind11::arg("name"));

	// genesis::taxonomy::find_taxon_by_name(class genesis::taxonomy::Taxonomy &, const std::string &) file:genesis/taxonomy/functions/taxonomy.hpp line:156
	M("genesis::taxonomy").def("find_taxon_by_name", (class genesis::taxonomy::Taxon * (*)(class genesis::taxonomy::Taxonomy &, const std::string &)) &genesis::taxonomy::find_taxon_by_name, "Alias for find_taxon_by_name(..., DepthFirstSearch{}).\n\nC++: genesis::taxonomy::find_taxon_by_name(class genesis::taxonomy::Taxonomy &, const std::string &) --> class genesis::taxonomy::Taxon *", pybind11::return_value_policy::reference_internal, pybind11::arg("tax"), pybind11::arg("name"));

	// genesis::taxonomy::find_taxon_by_id(const class genesis::taxonomy::Taxonomy &, const std::string &) file:genesis/taxonomy/functions/taxonomy.hpp line:161
	M("genesis::taxonomy").def("find_taxon_by_id", (const class genesis::taxonomy::Taxon * (*)(const class genesis::taxonomy::Taxonomy &, const std::string &)) &genesis::taxonomy::find_taxon_by_id, "Alias for find_taxon_by_id(..., DepthFirstSearch{}).\n\nC++: genesis::taxonomy::find_taxon_by_id(const class genesis::taxonomy::Taxonomy &, const std::string &) --> const class genesis::taxonomy::Taxon *", pybind11::return_value_policy::reference_internal, pybind11::arg("tax"), pybind11::arg("id"));

	// genesis::taxonomy::find_taxon_by_id(class genesis::taxonomy::Taxonomy &, const std::string &) file:genesis/taxonomy/functions/taxonomy.hpp line:166
	M("genesis::taxonomy").def("find_taxon_by_id", (class genesis::taxonomy::Taxon * (*)(class genesis::taxonomy::Taxonomy &, const std::string &)) &genesis::taxonomy::find_taxon_by_id, "Alias for find_taxon_by_id(..., DepthFirstSearch{}).\n\nC++: genesis::taxonomy::find_taxon_by_id(class genesis::taxonomy::Taxonomy &, const std::string &) --> class genesis::taxonomy::Taxon *", pybind11::return_value_policy::reference_internal, pybind11::arg("tax"), pybind11::arg("id"));

	// genesis::taxonomy::taxon_level(const class genesis::taxonomy::Taxon &) file:genesis/taxonomy/functions/taxonomy.hpp line:223
	M("genesis::taxonomy").def("taxon_level", (unsigned long (*)(const class genesis::taxonomy::Taxon &)) &genesis::taxonomy::taxon_level, "Return the level of depth of a given Taxon.\n\n This level is the number of parents the Taxon has, excluding the Taxonomy which contains them.\n That means, the immediate children of a Taxonomy all have level 0, their children level 1,\n and so on.\n\nC++: genesis::taxonomy::taxon_level(const class genesis::taxonomy::Taxon &) --> unsigned long", pybind11::arg("taxon"));

	// genesis::taxonomy::total_taxa_count(const class genesis::taxonomy::Taxonomy &) file:genesis/taxonomy/functions/taxonomy.hpp line:240
	M("genesis::taxonomy").def("total_taxa_count", (unsigned long (*)(const class genesis::taxonomy::Taxonomy &)) &genesis::taxonomy::total_taxa_count, "Return the total number of taxa contained in the Taxomony, i.e., the number of\n (non-unique) names of all children (recursively).\n\n Example: The Taxonomy\n\n     Tax_1\n         Tax_2\n             Tax_3\n         Tax_4\n             Tax_3\n     Tax_5\n\n contains a total of 6 taxa. The name `Tax_3` appears twice and is counted twice.\n\nC++: genesis::taxonomy::total_taxa_count(const class genesis::taxonomy::Taxonomy &) --> unsigned long", pybind11::arg("tax"));

}
