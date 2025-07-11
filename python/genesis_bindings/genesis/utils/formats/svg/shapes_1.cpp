#include <functional>
#include <genesis/utils/color/color.hpp>
#include <genesis/utils/formats/nexus/block.hpp>
#include <genesis/utils/formats/nexus/taxa.hpp>
#include <genesis/utils/formats/nexus/trees.hpp>
#include <genesis/utils/formats/svg/attributes.hpp>
#include <genesis/utils/formats/svg/group.hpp>
#include <genesis/utils/formats/svg/helper.hpp>
#include <genesis/utils/formats/svg/object.hpp>
#include <genesis/utils/formats/svg/pie_chart.hpp>
#include <genesis/utils/formats/svg/shapes.hpp>
#include <ios>
#include <iterator>
#include <locale>
#include <memory>
#include <ostream>
#include <sstream> // __str__
#include <streambuf>
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

// genesis::utils::NexusBlock file:genesis/utils/formats/nexus/block.hpp line:47
struct PyCallBack_genesis_utils_NexusBlock : public genesis::utils::NexusBlock {
	using genesis::utils::NexusBlock::NexusBlock;

	std::string block_name() const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const genesis::utils::NexusBlock *>(this), "block_name");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>();
			if (pybind11::detail::cast_is_temporary_value_reference<std::string>::value) {
				static pybind11::detail::override_caster_t<std::string> caster;
				return pybind11::detail::cast_ref<std::string>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<std::string>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"NexusBlock::block_name\"");
	}
	void to_stream(std::ostream & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const genesis::utils::NexusBlock *>(this), "to_stream");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<void>::value) {
				static pybind11::detail::override_caster_t<void> caster;
				return pybind11::detail::cast_ref<void>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<void>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"NexusBlock::to_stream\"");
	}
};

// genesis::utils::NexusTaxa file:genesis/utils/formats/nexus/taxa.hpp line:61
struct PyCallBack_genesis_utils_NexusTaxa : public genesis::utils::NexusTaxa {
	using genesis::utils::NexusTaxa::NexusTaxa;

	std::string block_name() const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const genesis::utils::NexusTaxa *>(this), "block_name");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>();
			if (pybind11::detail::cast_is_temporary_value_reference<std::string>::value) {
				static pybind11::detail::override_caster_t<std::string> caster;
				return pybind11::detail::cast_ref<std::string>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<std::string>(std::move(o));
		}
		return NexusTaxa::block_name();
	}
	void to_stream(std::ostream & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const genesis::utils::NexusTaxa *>(this), "to_stream");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<void>::value) {
				static pybind11::detail::override_caster_t<void> caster;
				return pybind11::detail::cast_ref<void>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<void>(std::move(o));
		}
		return NexusTaxa::to_stream(a0);
	}
};

// genesis::utils::NexusTrees file:genesis/utils/formats/nexus/trees.hpp line:62
struct PyCallBack_genesis_utils_NexusTrees : public genesis::utils::NexusTrees {
	using genesis::utils::NexusTrees::NexusTrees;

	std::string block_name() const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const genesis::utils::NexusTrees *>(this), "block_name");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>();
			if (pybind11::detail::cast_is_temporary_value_reference<std::string>::value) {
				static pybind11::detail::override_caster_t<std::string> caster;
				return pybind11::detail::cast_ref<std::string>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<std::string>(std::move(o));
		}
		return NexusTrees::block_name();
	}
	void to_stream(std::ostream & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const genesis::utils::NexusTrees *>(this), "to_stream");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<void>::value) {
				static pybind11::detail::override_caster_t<void> caster;
				return pybind11::detail::cast_ref<void>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<void>(std::move(o));
		}
		return NexusTrees::to_stream(a0);
	}
};

void bind_genesis_utils_formats_svg_shapes_1(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	{ // genesis::utils::SvgPath file:genesis/utils/formats/svg/shapes.hpp line:458
		pybind11::class_<genesis::utils::SvgPath, std::shared_ptr<genesis::utils::SvgPath>> cl(M("genesis::utils"), "SvgPath", "");
		cl.def( pybind11::init( [](){ return new genesis::utils::SvgPath(); } ), "doc" );
		cl.def( pybind11::init( [](const struct genesis::utils::SvgStroke & a0){ return new genesis::utils::SvgPath(a0); } ), "doc" , pybind11::arg("stroke"));
		cl.def( pybind11::init<const struct genesis::utils::SvgStroke &, const struct genesis::utils::SvgFill &>(), pybind11::arg("stroke"), pybind11::arg("fill") );

		cl.def( pybind11::init( [](const class std::vector<std::string, class std::allocator<std::string > > & a0){ return new genesis::utils::SvgPath(a0); } ), "doc" , pybind11::arg("elements"));
		cl.def( pybind11::init( [](const class std::vector<std::string, class std::allocator<std::string > > & a0, const struct genesis::utils::SvgStroke & a1){ return new genesis::utils::SvgPath(a0, a1); } ), "doc" , pybind11::arg("elements"), pybind11::arg("stroke"));
		cl.def( pybind11::init<const class std::vector<std::string, class std::allocator<std::string > > &, const struct genesis::utils::SvgStroke &, const struct genesis::utils::SvgFill &>(), pybind11::arg("elements"), pybind11::arg("stroke"), pybind11::arg("fill") );

		cl.def( pybind11::init( [](genesis::utils::SvgPath const &o){ return new genesis::utils::SvgPath(o); } ) );
		cl.def_readwrite("id", &genesis::utils::SvgPath::id);
		cl.def_readwrite("elements", &genesis::utils::SvgPath::elements);
		cl.def_readwrite("stroke", &genesis::utils::SvgPath::stroke);
		cl.def_readwrite("fill", &genesis::utils::SvgPath::fill);
		cl.def_readwrite("transform", &genesis::utils::SvgPath::transform);
		cl.def("assign", (struct genesis::utils::SvgPath & (genesis::utils::SvgPath::*)(const struct genesis::utils::SvgPath &)) &genesis::utils::SvgPath::operator=, "C++: genesis::utils::SvgPath::operator=(const struct genesis::utils::SvgPath &) --> struct genesis::utils::SvgPath &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("add", (struct genesis::utils::SvgPath & (genesis::utils::SvgPath::*)(std::string)) &genesis::utils::SvgPath::add, "C++: genesis::utils::SvgPath::add(std::string) --> struct genesis::utils::SvgPath &", pybind11::return_value_policy::reference_internal, pybind11::arg("elem"));
		cl.def("__lshift__", (struct genesis::utils::SvgPath & (genesis::utils::SvgPath::*)(std::string)) &genesis::utils::SvgPath::operator<<, "C++: genesis::utils::SvgPath::operator<<(std::string) --> struct genesis::utils::SvgPath &", pybind11::return_value_policy::reference_internal, pybind11::arg("elem"));
		cl.def("bounding_box", (struct genesis::utils::SvgBox (genesis::utils::SvgPath::*)() const) &genesis::utils::SvgPath::bounding_box, "Compute the bounding box of the path.\n\n Note that we have not implemented a full svg renderer here. Hence, we only use the immediate\n coordinates as given in the path commands, but ignore any parts that might go outside of\n that, such as Bezier curves. Still, this bounding box should give a good enough outline for\n many purposes, e.g., to scale a group or set the document margins roughly around the path.\n\nC++: genesis::utils::SvgPath::bounding_box() const --> struct genesis::utils::SvgBox");
		cl.def("write", [](genesis::utils::SvgPath const &o, std::ostream & a0) -> void { return o.write(a0); }, "", pybind11::arg("out"));
		cl.def("write", [](genesis::utils::SvgPath const &o, std::ostream & a0, unsigned long const & a1) -> void { return o.write(a0, a1); }, "", pybind11::arg("out"), pybind11::arg("indent"));
		cl.def("write", (void (genesis::utils::SvgPath::*)(std::ostream &, unsigned long, const struct genesis::utils::SvgDrawingOptions &) const) &genesis::utils::SvgPath::write, "C++: genesis::utils::SvgPath::write(std::ostream &, unsigned long, const struct genesis::utils::SvgDrawingOptions &) const --> void", pybind11::arg("out"), pybind11::arg("indent"), pybind11::arg("options"));
	}
	{ // genesis::utils::SvgUse file:genesis/utils/formats/svg/shapes.hpp line:534
		pybind11::class_<genesis::utils::SvgUse, std::shared_ptr<genesis::utils::SvgUse>> cl(M("genesis::utils"), "SvgUse", "");
		cl.def( pybind11::init<const class genesis::utils::SvgObject &>(), pybind11::arg("other") );

		cl.def( pybind11::init<const class genesis::utils::SvgObject &, const struct genesis::utils::SvgPoint &>(), pybind11::arg("other"), pybind11::arg("offset") );

		cl.def( pybind11::init<const class genesis::utils::SvgObject &, double, double>(), pybind11::arg("other"), pybind11::arg("offset_x"), pybind11::arg("offset_y") );

		cl.def( pybind11::init( [](genesis::utils::SvgUse const &o){ return new genesis::utils::SvgUse(o); } ) );
		cl.def_readwrite("id", &genesis::utils::SvgUse::id);
		cl.def_readwrite("offset", &genesis::utils::SvgUse::offset);
		cl.def_readwrite("transform", &genesis::utils::SvgUse::transform);
		cl.def("assign", (struct genesis::utils::SvgUse & (genesis::utils::SvgUse::*)(const struct genesis::utils::SvgUse &)) &genesis::utils::SvgUse::operator=, "C++: genesis::utils::SvgUse::operator=(const struct genesis::utils::SvgUse &) --> struct genesis::utils::SvgUse &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("bounding_box", (struct genesis::utils::SvgBox (genesis::utils::SvgUse::*)() const) &genesis::utils::SvgUse::bounding_box, "C++: genesis::utils::SvgUse::bounding_box() const --> struct genesis::utils::SvgBox");
		cl.def("write", [](genesis::utils::SvgUse const &o, std::ostream & a0) -> void { return o.write(a0); }, "", pybind11::arg("out"));
		cl.def("write", [](genesis::utils::SvgUse const &o, std::ostream & a0, unsigned long const & a1) -> void { return o.write(a0, a1); }, "", pybind11::arg("out"), pybind11::arg("indent"));
		cl.def("write", (void (genesis::utils::SvgUse::*)(std::ostream &, unsigned long, const struct genesis::utils::SvgDrawingOptions &) const) &genesis::utils::SvgUse::write, "C++: genesis::utils::SvgUse::write(std::ostream &, unsigned long, const struct genesis::utils::SvgDrawingOptions &) const --> void", pybind11::arg("out"), pybind11::arg("indent"), pybind11::arg("options"));
	}
	// genesis::utils::make_svg_pie_chart(const class std::vector<double, class std::allocator<double> > &, const class std::vector<class genesis::utils::Color, class std::allocator<class genesis::utils::Color> > &, double, double, bool) file:genesis/utils/formats/svg/pie_chart.hpp line:70
	M("genesis::utils").def("make_svg_pie_chart", [](const class std::vector<double, class std::allocator<double> > & a0, const class std::vector<class genesis::utils::Color, class std::allocator<class genesis::utils::Color> > & a1) -> genesis::utils::SvgGroup { return genesis::utils::make_svg_pie_chart(a0, a1); }, "", pybind11::arg("values"), pybind11::arg("colors"));
	M("genesis::utils").def("make_svg_pie_chart", [](const class std::vector<double, class std::allocator<double> > & a0, const class std::vector<class genesis::utils::Color, class std::allocator<class genesis::utils::Color> > & a1, double const & a2) -> genesis::utils::SvgGroup { return genesis::utils::make_svg_pie_chart(a0, a1, a2); }, "", pybind11::arg("values"), pybind11::arg("colors"), pybind11::arg("radius"));
	M("genesis::utils").def("make_svg_pie_chart", [](const class std::vector<double, class std::allocator<double> > & a0, const class std::vector<class genesis::utils::Color, class std::allocator<class genesis::utils::Color> > & a1, double const & a2, double const & a3) -> genesis::utils::SvgGroup { return genesis::utils::make_svg_pie_chart(a0, a1, a2, a3); }, "", pybind11::arg("values"), pybind11::arg("colors"), pybind11::arg("radius"), pybind11::arg("start_angle"));
	M("genesis::utils").def("make_svg_pie_chart", (struct genesis::utils::SvgGroup (*)(const class std::vector<double, class std::allocator<double> > &, const class std::vector<class genesis::utils::Color, class std::allocator<class genesis::utils::Color> > &, double, double, bool)) &genesis::utils::make_svg_pie_chart, "Make a pie chart in SVG.\n\n The given  are automatically re-scaled to fill the circle.\n If there are more  than  only the first ones needed are used;\n if given  have fewer entries thatn there are  an exception is thrown.\n\n The  of the char defaults to 1.0, and the  to `-pi/2`, so that the chart\n starts from the top (most typical). By default, we also draw the values \n (that is, reversed form the mathematical direction).\n\nC++: genesis::utils::make_svg_pie_chart(const class std::vector<double, class std::allocator<double> > &, const class std::vector<class genesis::utils::Color, class std::allocator<class genesis::utils::Color> > &, double, double, bool) --> struct genesis::utils::SvgGroup", pybind11::arg("values"), pybind11::arg("colors"), pybind11::arg("radius"), pybind11::arg("start_angle"), pybind11::arg("clockwise"));

	{ // genesis::utils::NexusBlock file:genesis/utils/formats/nexus/block.hpp line:47
		pybind11::class_<genesis::utils::NexusBlock, std::shared_ptr<genesis::utils::NexusBlock>, PyCallBack_genesis_utils_NexusBlock> cl(M("genesis::utils"), "NexusBlock", "");
		cl.def( pybind11::init( [](){ return new PyCallBack_genesis_utils_NexusBlock(); } ) );
		cl.def(pybind11::init<PyCallBack_genesis_utils_NexusBlock const &>());
		cl.def("block_name", (std::string (genesis::utils::NexusBlock::*)() const) &genesis::utils::NexusBlock::block_name, "C++: genesis::utils::NexusBlock::block_name() const --> std::string");
		cl.def("to_stream", (void (genesis::utils::NexusBlock::*)(std::ostream &) const) &genesis::utils::NexusBlock::to_stream, "C++: genesis::utils::NexusBlock::to_stream(std::ostream &) const --> void", pybind11::arg("os"));

		cl.def("__str__", [](genesis::utils::NexusBlock const &o) -> std::string { std::ostringstream s; genesis::utils::operator<<(s, o); return s.str(); } );
	}
	{ // genesis::utils::NexusTaxa file:genesis/utils/formats/nexus/taxa.hpp line:61
		pybind11::class_<genesis::utils::NexusTaxa, std::shared_ptr<genesis::utils::NexusTaxa>, PyCallBack_genesis_utils_NexusTaxa, genesis::utils::NexusBlock> cl(M("genesis::utils"), "NexusTaxa", "");
		cl.def( pybind11::init( [](){ return new genesis::utils::NexusTaxa(); }, [](){ return new PyCallBack_genesis_utils_NexusTaxa(); } ) );
		cl.def( pybind11::init( [](PyCallBack_genesis_utils_NexusTaxa const &o){ return new PyCallBack_genesis_utils_NexusTaxa(o); } ) );
		cl.def( pybind11::init( [](genesis::utils::NexusTaxa const &o){ return new genesis::utils::NexusTaxa(o); } ) );
		cl.def("assign", (class genesis::utils::NexusTaxa & (genesis::utils::NexusTaxa::*)(const class genesis::utils::NexusTaxa &)) &genesis::utils::NexusTaxa::operator=, "C++: genesis::utils::NexusTaxa::operator=(const class genesis::utils::NexusTaxa &) --> class genesis::utils::NexusTaxa &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("swap", (void (genesis::utils::NexusTaxa::*)(class genesis::utils::NexusTaxa &)) &genesis::utils::NexusTaxa::swap, "C++: genesis::utils::NexusTaxa::swap(class genesis::utils::NexusTaxa &) --> void", pybind11::arg("other"));
		cl.def("empty", (bool (genesis::utils::NexusTaxa::*)() const) &genesis::utils::NexusTaxa::empty, "C++: genesis::utils::NexusTaxa::empty() const --> bool");
		cl.def("size", (unsigned long (genesis::utils::NexusTaxa::*)() const) &genesis::utils::NexusTaxa::size, "C++: genesis::utils::NexusTaxa::size() const --> unsigned long");
		cl.def("has_taxon", (bool (genesis::utils::NexusTaxa::*)(const std::string &) const) &genesis::utils::NexusTaxa::has_taxon, "C++: genesis::utils::NexusTaxa::has_taxon(const std::string &) const --> bool", pybind11::arg("name"));
		cl.def("begin", (class __gnu_cxx::__normal_iterator<const std::string *, class std::vector<std::string, class std::allocator<std::string > > > (genesis::utils::NexusTaxa::*)() const) &genesis::utils::NexusTaxa::begin, "C++: genesis::utils::NexusTaxa::begin() const --> class __gnu_cxx::__normal_iterator<const std::string *, class std::vector<std::string, class std::allocator<std::string > > >");
		cl.def("end", (class __gnu_cxx::__normal_iterator<const std::string *, class std::vector<std::string, class std::allocator<std::string > > > (genesis::utils::NexusTaxa::*)() const) &genesis::utils::NexusTaxa::end, "C++: genesis::utils::NexusTaxa::end() const --> class __gnu_cxx::__normal_iterator<const std::string *, class std::vector<std::string, class std::allocator<std::string > > >");
		cl.def("add_taxon", (void (genesis::utils::NexusTaxa::*)(const std::string &)) &genesis::utils::NexusTaxa::add_taxon, "C++: genesis::utils::NexusTaxa::add_taxon(const std::string &) --> void", pybind11::arg("name"));
		cl.def("add_taxa", (void (genesis::utils::NexusTaxa::*)(const class std::vector<std::string, class std::allocator<std::string > > &)) &genesis::utils::NexusTaxa::add_taxa, "C++: genesis::utils::NexusTaxa::add_taxa(const class std::vector<std::string, class std::allocator<std::string > > &) --> void", pybind11::arg("taxa"));
		cl.def("clear", (void (genesis::utils::NexusTaxa::*)()) &genesis::utils::NexusTaxa::clear, "C++: genesis::utils::NexusTaxa::clear() --> void");
		cl.def("block_name", (std::string (genesis::utils::NexusTaxa::*)() const) &genesis::utils::NexusTaxa::block_name, "C++: genesis::utils::NexusTaxa::block_name() const --> std::string");
		cl.def("to_stream", (void (genesis::utils::NexusTaxa::*)(std::ostream &) const) &genesis::utils::NexusTaxa::to_stream, "C++: genesis::utils::NexusTaxa::to_stream(std::ostream &) const --> void", pybind11::arg("os"));
	}
	{ // genesis::utils::NexusTrees file:genesis/utils/formats/nexus/trees.hpp line:62
		pybind11::class_<genesis::utils::NexusTrees, std::shared_ptr<genesis::utils::NexusTrees>, PyCallBack_genesis_utils_NexusTrees, genesis::utils::NexusBlock> cl(M("genesis::utils"), "NexusTrees", "");
		cl.def( pybind11::init( [](){ return new genesis::utils::NexusTrees(); }, [](){ return new PyCallBack_genesis_utils_NexusTrees(); } ) );
		cl.def( pybind11::init( [](PyCallBack_genesis_utils_NexusTrees const &o){ return new PyCallBack_genesis_utils_NexusTrees(o); } ) );
		cl.def( pybind11::init( [](genesis::utils::NexusTrees const &o){ return new genesis::utils::NexusTrees(o); } ) );
		cl.def("assign", (class genesis::utils::NexusTrees & (genesis::utils::NexusTrees::*)(const class genesis::utils::NexusTrees &)) &genesis::utils::NexusTrees::operator=, "C++: genesis::utils::NexusTrees::operator=(const class genesis::utils::NexusTrees &) --> class genesis::utils::NexusTrees &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("swap", (void (genesis::utils::NexusTrees::*)(class genesis::utils::NexusTrees &)) &genesis::utils::NexusTrees::swap, "C++: genesis::utils::NexusTrees::swap(class genesis::utils::NexusTrees &) --> void", pybind11::arg("other"));
		cl.def("empty", (bool (genesis::utils::NexusTrees::*)() const) &genesis::utils::NexusTrees::empty, "C++: genesis::utils::NexusTrees::empty() const --> bool");
		cl.def("size", (unsigned long (genesis::utils::NexusTrees::*)() const) &genesis::utils::NexusTrees::size, "C++: genesis::utils::NexusTrees::size() const --> unsigned long");
		cl.def("has_tree", (bool (genesis::utils::NexusTrees::*)(const std::string &) const) &genesis::utils::NexusTrees::has_tree, "C++: genesis::utils::NexusTrees::has_tree(const std::string &) const --> bool", pybind11::arg("name"));
		cl.def("begin", (class __gnu_cxx::__normal_iterator<const struct genesis::utils::NexusTrees::Entry *, class std::vector<struct genesis::utils::NexusTrees::Entry, class std::allocator<struct genesis::utils::NexusTrees::Entry> > > (genesis::utils::NexusTrees::*)() const) &genesis::utils::NexusTrees::begin, "C++: genesis::utils::NexusTrees::begin() const --> class __gnu_cxx::__normal_iterator<const struct genesis::utils::NexusTrees::Entry *, class std::vector<struct genesis::utils::NexusTrees::Entry, class std::allocator<struct genesis::utils::NexusTrees::Entry> > >");
		cl.def("end", (class __gnu_cxx::__normal_iterator<const struct genesis::utils::NexusTrees::Entry *, class std::vector<struct genesis::utils::NexusTrees::Entry, class std::allocator<struct genesis::utils::NexusTrees::Entry> > > (genesis::utils::NexusTrees::*)() const) &genesis::utils::NexusTrees::end, "C++: genesis::utils::NexusTrees::end() const --> class __gnu_cxx::__normal_iterator<const struct genesis::utils::NexusTrees::Entry *, class std::vector<struct genesis::utils::NexusTrees::Entry, class std::allocator<struct genesis::utils::NexusTrees::Entry> > >");
		cl.def("add_tree", (void (genesis::utils::NexusTrees::*)(const std::string &, const std::string &)) &genesis::utils::NexusTrees::add_tree, "C++: genesis::utils::NexusTrees::add_tree(const std::string &, const std::string &) --> void", pybind11::arg("name"), pybind11::arg("tree"));
		cl.def("erase_tree", (void (genesis::utils::NexusTrees::*)(const std::string &)) &genesis::utils::NexusTrees::erase_tree, "C++: genesis::utils::NexusTrees::erase_tree(const std::string &) --> void", pybind11::arg("name"));
		cl.def("clear", (void (genesis::utils::NexusTrees::*)()) &genesis::utils::NexusTrees::clear, "C++: genesis::utils::NexusTrees::clear() --> void");
		cl.def("block_name", (std::string (genesis::utils::NexusTrees::*)() const) &genesis::utils::NexusTrees::block_name, "C++: genesis::utils::NexusTrees::block_name() const --> std::string");
		cl.def("to_stream", (void (genesis::utils::NexusTrees::*)(std::ostream &) const) &genesis::utils::NexusTrees::to_stream, "C++: genesis::utils::NexusTrees::to_stream(std::ostream &) const --> void", pybind11::arg("os"));

		{ // genesis::utils::NexusTrees::Entry file:genesis/utils/formats/nexus/trees.hpp line:71
			auto & enclosing_class = cl;
			pybind11::class_<genesis::utils::NexusTrees::Entry, std::shared_ptr<genesis::utils::NexusTrees::Entry>> cl(enclosing_class, "Entry", "");
			cl.def( pybind11::init( [](genesis::utils::NexusTrees::Entry const &o){ return new genesis::utils::NexusTrees::Entry(o); } ) );
			cl.def( pybind11::init( [](){ return new genesis::utils::NexusTrees::Entry(); } ) );
			cl.def_readwrite("name", &genesis::utils::NexusTrees::Entry::name);
			cl.def_readwrite("properties", &genesis::utils::NexusTrees::Entry::properties);
			cl.def_readwrite("tree", &genesis::utils::NexusTrees::Entry::tree);
			cl.def("assign", (struct genesis::utils::NexusTrees::Entry & (genesis::utils::NexusTrees::Entry::*)(const struct genesis::utils::NexusTrees::Entry &)) &genesis::utils::NexusTrees::Entry::operator=, "C++: genesis::utils::NexusTrees::Entry::operator=(const struct genesis::utils::NexusTrees::Entry &) --> struct genesis::utils::NexusTrees::Entry &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		}

	}
}
