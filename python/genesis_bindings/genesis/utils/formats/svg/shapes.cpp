#include <genesis/utils/color/color.hpp>
#include <genesis/utils/formats/svg/attributes.hpp>
#include <genesis/utils/formats/svg/helper.hpp>
#include <genesis/utils/formats/svg/shapes.hpp>
#include <ios>
#include <iterator>
#include <memory>
#include <ostream>
#include <sstream> // __str__
#include <streambuf>
#include <string>
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

void bind_genesis_utils_formats_svg_shapes(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	{ // genesis::utils::SvgRect file:genesis/utils/formats/svg/shapes.hpp line:112
		pybind11::class_<genesis::utils::SvgRect, std::shared_ptr<genesis::utils::SvgRect>> cl(M("genesis::utils"), "SvgRect", "");
		cl.def( pybind11::init( [](const struct genesis::utils::SvgPoint & a0, const struct genesis::utils::SvgSize & a1){ return new genesis::utils::SvgRect(a0, a1); } ), "doc" , pybind11::arg("position"), pybind11::arg("size"));
		cl.def( pybind11::init( [](const struct genesis::utils::SvgPoint & a0, const struct genesis::utils::SvgSize & a1, const struct genesis::utils::SvgStroke & a2){ return new genesis::utils::SvgRect(a0, a1, a2); } ), "doc" , pybind11::arg("position"), pybind11::arg("size"), pybind11::arg("stroke"));
		cl.def( pybind11::init<const struct genesis::utils::SvgPoint &, const struct genesis::utils::SvgSize &, const struct genesis::utils::SvgStroke &, const struct genesis::utils::SvgFill &>(), pybind11::arg("position"), pybind11::arg("size"), pybind11::arg("stroke"), pybind11::arg("fill") );

		cl.def( pybind11::init( [](double const & a0, double const & a1, double const & a2, double const & a3){ return new genesis::utils::SvgRect(a0, a1, a2, a3); } ), "doc" , pybind11::arg("x"), pybind11::arg("y"), pybind11::arg("w"), pybind11::arg("h"));
		cl.def( pybind11::init( [](double const & a0, double const & a1, double const & a2, double const & a3, const struct genesis::utils::SvgStroke & a4){ return new genesis::utils::SvgRect(a0, a1, a2, a3, a4); } ), "doc" , pybind11::arg("x"), pybind11::arg("y"), pybind11::arg("w"), pybind11::arg("h"), pybind11::arg("stroke"));
		cl.def( pybind11::init<double, double, double, double, const struct genesis::utils::SvgStroke &, const struct genesis::utils::SvgFill &>(), pybind11::arg("x"), pybind11::arg("y"), pybind11::arg("w"), pybind11::arg("h"), pybind11::arg("stroke"), pybind11::arg("fill") );

		cl.def( pybind11::init( [](genesis::utils::SvgRect const &o){ return new genesis::utils::SvgRect(o); } ) );
		cl.def_readwrite("id", &genesis::utils::SvgRect::id);
		cl.def_readwrite("position", &genesis::utils::SvgRect::position);
		cl.def_readwrite("size", &genesis::utils::SvgRect::size);
		cl.def_readwrite("stroke", &genesis::utils::SvgRect::stroke);
		cl.def_readwrite("fill", &genesis::utils::SvgRect::fill);
		cl.def_readwrite("rx", &genesis::utils::SvgRect::rx);
		cl.def_readwrite("ry", &genesis::utils::SvgRect::ry);
		cl.def_readwrite("transform", &genesis::utils::SvgRect::transform);
		cl.def("assign", (struct genesis::utils::SvgRect & (genesis::utils::SvgRect::*)(const struct genesis::utils::SvgRect &)) &genesis::utils::SvgRect::operator=, "C++: genesis::utils::SvgRect::operator=(const struct genesis::utils::SvgRect &) --> struct genesis::utils::SvgRect &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("bounding_box", (struct genesis::utils::SvgBox (genesis::utils::SvgRect::*)() const) &genesis::utils::SvgRect::bounding_box, "C++: genesis::utils::SvgRect::bounding_box() const --> struct genesis::utils::SvgBox");
		cl.def("write", [](genesis::utils::SvgRect const &o, std::ostream & a0) -> void { return o.write(a0); }, "", pybind11::arg("out"));
		cl.def("write", [](genesis::utils::SvgRect const &o, std::ostream & a0, unsigned long const & a1) -> void { return o.write(a0, a1); }, "", pybind11::arg("out"), pybind11::arg("indent"));
		cl.def("write", (void (genesis::utils::SvgRect::*)(std::ostream &, unsigned long, const struct genesis::utils::SvgDrawingOptions &) const) &genesis::utils::SvgRect::write, "C++: genesis::utils::SvgRect::write(std::ostream &, unsigned long, const struct genesis::utils::SvgDrawingOptions &) const --> void", pybind11::arg("out"), pybind11::arg("indent"), pybind11::arg("options"));
	}
	{ // genesis::utils::SvgCircle file:genesis/utils/formats/svg/shapes.hpp line:183
		pybind11::class_<genesis::utils::SvgCircle, std::shared_ptr<genesis::utils::SvgCircle>> cl(M("genesis::utils"), "SvgCircle", "");
		cl.def( pybind11::init( [](const struct genesis::utils::SvgPoint & a0, double const & a1){ return new genesis::utils::SvgCircle(a0, a1); } ), "doc" , pybind11::arg("center"), pybind11::arg("radius"));
		cl.def( pybind11::init( [](const struct genesis::utils::SvgPoint & a0, double const & a1, const struct genesis::utils::SvgStroke & a2){ return new genesis::utils::SvgCircle(a0, a1, a2); } ), "doc" , pybind11::arg("center"), pybind11::arg("radius"), pybind11::arg("stroke"));
		cl.def( pybind11::init<const struct genesis::utils::SvgPoint &, double, const struct genesis::utils::SvgStroke &, const struct genesis::utils::SvgFill &>(), pybind11::arg("center"), pybind11::arg("radius"), pybind11::arg("stroke"), pybind11::arg("fill") );

		cl.def( pybind11::init( [](double const & a0, double const & a1, double const & a2){ return new genesis::utils::SvgCircle(a0, a1, a2); } ), "doc" , pybind11::arg("cx"), pybind11::arg("cy"), pybind11::arg("radius"));
		cl.def( pybind11::init( [](double const & a0, double const & a1, double const & a2, const struct genesis::utils::SvgStroke & a3){ return new genesis::utils::SvgCircle(a0, a1, a2, a3); } ), "doc" , pybind11::arg("cx"), pybind11::arg("cy"), pybind11::arg("radius"), pybind11::arg("stroke"));
		cl.def( pybind11::init<double, double, double, const struct genesis::utils::SvgStroke &, const struct genesis::utils::SvgFill &>(), pybind11::arg("cx"), pybind11::arg("cy"), pybind11::arg("radius"), pybind11::arg("stroke"), pybind11::arg("fill") );

		cl.def( pybind11::init( [](genesis::utils::SvgCircle const &o){ return new genesis::utils::SvgCircle(o); } ) );
		cl.def_readwrite("id", &genesis::utils::SvgCircle::id);
		cl.def_readwrite("center", &genesis::utils::SvgCircle::center);
		cl.def_readwrite("radius", &genesis::utils::SvgCircle::radius);
		cl.def_readwrite("stroke", &genesis::utils::SvgCircle::stroke);
		cl.def_readwrite("fill", &genesis::utils::SvgCircle::fill);
		cl.def_readwrite("transform", &genesis::utils::SvgCircle::transform);
		cl.def("assign", (struct genesis::utils::SvgCircle & (genesis::utils::SvgCircle::*)(const struct genesis::utils::SvgCircle &)) &genesis::utils::SvgCircle::operator=, "C++: genesis::utils::SvgCircle::operator=(const struct genesis::utils::SvgCircle &) --> struct genesis::utils::SvgCircle &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("bounding_box", (struct genesis::utils::SvgBox (genesis::utils::SvgCircle::*)() const) &genesis::utils::SvgCircle::bounding_box, "C++: genesis::utils::SvgCircle::bounding_box() const --> struct genesis::utils::SvgBox");
		cl.def("write", [](genesis::utils::SvgCircle const &o, std::ostream & a0) -> void { return o.write(a0); }, "", pybind11::arg("out"));
		cl.def("write", [](genesis::utils::SvgCircle const &o, std::ostream & a0, unsigned long const & a1) -> void { return o.write(a0, a1); }, "", pybind11::arg("out"), pybind11::arg("indent"));
		cl.def("write", (void (genesis::utils::SvgCircle::*)(std::ostream &, unsigned long, const struct genesis::utils::SvgDrawingOptions &) const) &genesis::utils::SvgCircle::write, "C++: genesis::utils::SvgCircle::write(std::ostream &, unsigned long, const struct genesis::utils::SvgDrawingOptions &) const --> void", pybind11::arg("out"), pybind11::arg("indent"), pybind11::arg("options"));
	}
	{ // genesis::utils::SvgEllipse file:genesis/utils/formats/svg/shapes.hpp line:251
		pybind11::class_<genesis::utils::SvgEllipse, std::shared_ptr<genesis::utils::SvgEllipse>> cl(M("genesis::utils"), "SvgEllipse", "");
		cl.def( pybind11::init( [](const struct genesis::utils::SvgPoint & a0, double const & a1, double const & a2){ return new genesis::utils::SvgEllipse(a0, a1, a2); } ), "doc" , pybind11::arg("center"), pybind11::arg("rx"), pybind11::arg("ry"));
		cl.def( pybind11::init( [](const struct genesis::utils::SvgPoint & a0, double const & a1, double const & a2, const struct genesis::utils::SvgStroke & a3){ return new genesis::utils::SvgEllipse(a0, a1, a2, a3); } ), "doc" , pybind11::arg("center"), pybind11::arg("rx"), pybind11::arg("ry"), pybind11::arg("stroke"));
		cl.def( pybind11::init<const struct genesis::utils::SvgPoint &, double, double, const struct genesis::utils::SvgStroke &, const struct genesis::utils::SvgFill &>(), pybind11::arg("center"), pybind11::arg("rx"), pybind11::arg("ry"), pybind11::arg("stroke"), pybind11::arg("fill") );

		cl.def( pybind11::init( [](double const & a0, double const & a1, double const & a2, double const & a3){ return new genesis::utils::SvgEllipse(a0, a1, a2, a3); } ), "doc" , pybind11::arg("cx"), pybind11::arg("cy"), pybind11::arg("rx"), pybind11::arg("ry"));
		cl.def( pybind11::init( [](double const & a0, double const & a1, double const & a2, double const & a3, const struct genesis::utils::SvgStroke & a4){ return new genesis::utils::SvgEllipse(a0, a1, a2, a3, a4); } ), "doc" , pybind11::arg("cx"), pybind11::arg("cy"), pybind11::arg("rx"), pybind11::arg("ry"), pybind11::arg("stroke"));
		cl.def( pybind11::init<double, double, double, double, const struct genesis::utils::SvgStroke &, const struct genesis::utils::SvgFill &>(), pybind11::arg("cx"), pybind11::arg("cy"), pybind11::arg("rx"), pybind11::arg("ry"), pybind11::arg("stroke"), pybind11::arg("fill") );

		cl.def( pybind11::init( [](genesis::utils::SvgEllipse const &o){ return new genesis::utils::SvgEllipse(o); } ) );
		cl.def_readwrite("id", &genesis::utils::SvgEllipse::id);
		cl.def_readwrite("center", &genesis::utils::SvgEllipse::center);
		cl.def_readwrite("rx", &genesis::utils::SvgEllipse::rx);
		cl.def_readwrite("ry", &genesis::utils::SvgEllipse::ry);
		cl.def_readwrite("stroke", &genesis::utils::SvgEllipse::stroke);
		cl.def_readwrite("fill", &genesis::utils::SvgEllipse::fill);
		cl.def_readwrite("transform", &genesis::utils::SvgEllipse::transform);
		cl.def("assign", (struct genesis::utils::SvgEllipse & (genesis::utils::SvgEllipse::*)(const struct genesis::utils::SvgEllipse &)) &genesis::utils::SvgEllipse::operator=, "C++: genesis::utils::SvgEllipse::operator=(const struct genesis::utils::SvgEllipse &) --> struct genesis::utils::SvgEllipse &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("bounding_box", (struct genesis::utils::SvgBox (genesis::utils::SvgEllipse::*)() const) &genesis::utils::SvgEllipse::bounding_box, "C++: genesis::utils::SvgEllipse::bounding_box() const --> struct genesis::utils::SvgBox");
		cl.def("write", [](genesis::utils::SvgEllipse const &o, std::ostream & a0) -> void { return o.write(a0); }, "", pybind11::arg("out"));
		cl.def("write", [](genesis::utils::SvgEllipse const &o, std::ostream & a0, unsigned long const & a1) -> void { return o.write(a0, a1); }, "", pybind11::arg("out"), pybind11::arg("indent"));
		cl.def("write", (void (genesis::utils::SvgEllipse::*)(std::ostream &, unsigned long, const struct genesis::utils::SvgDrawingOptions &) const) &genesis::utils::SvgEllipse::write, "C++: genesis::utils::SvgEllipse::write(std::ostream &, unsigned long, const struct genesis::utils::SvgDrawingOptions &) const --> void", pybind11::arg("out"), pybind11::arg("indent"), pybind11::arg("options"));
	}
	{ // genesis::utils::SvgPolyline file:genesis/utils/formats/svg/shapes.hpp line:320
		pybind11::class_<genesis::utils::SvgPolyline, std::shared_ptr<genesis::utils::SvgPolyline>> cl(M("genesis::utils"), "SvgPolyline", "");
		cl.def( pybind11::init( [](){ return new genesis::utils::SvgPolyline(); } ), "doc" );
		cl.def( pybind11::init( [](const struct genesis::utils::SvgStroke & a0){ return new genesis::utils::SvgPolyline(a0); } ), "doc" , pybind11::arg("stroke"));
		cl.def( pybind11::init<const struct genesis::utils::SvgStroke &, const struct genesis::utils::SvgFill &>(), pybind11::arg("stroke"), pybind11::arg("fill") );

		cl.def( pybind11::init( [](class std::vector<struct genesis::utils::SvgPoint, class std::allocator<struct genesis::utils::SvgPoint> > const & a0){ return new genesis::utils::SvgPolyline(a0); } ), "doc" , pybind11::arg("points"));
		cl.def( pybind11::init( [](class std::vector<struct genesis::utils::SvgPoint, class std::allocator<struct genesis::utils::SvgPoint> > const & a0, const struct genesis::utils::SvgStroke & a1){ return new genesis::utils::SvgPolyline(a0, a1); } ), "doc" , pybind11::arg("points"), pybind11::arg("stroke"));
		cl.def( pybind11::init<class std::vector<struct genesis::utils::SvgPoint, class std::allocator<struct genesis::utils::SvgPoint> >, const struct genesis::utils::SvgStroke &, const struct genesis::utils::SvgFill &>(), pybind11::arg("points"), pybind11::arg("stroke"), pybind11::arg("fill") );

		cl.def( pybind11::init( [](genesis::utils::SvgPolyline const &o){ return new genesis::utils::SvgPolyline(o); } ) );
		cl.def_readwrite("id", &genesis::utils::SvgPolyline::id);
		cl.def_readwrite("points", &genesis::utils::SvgPolyline::points);
		cl.def_readwrite("stroke", &genesis::utils::SvgPolyline::stroke);
		cl.def_readwrite("fill", &genesis::utils::SvgPolyline::fill);
		cl.def_readwrite("transform", &genesis::utils::SvgPolyline::transform);
		cl.def("assign", (struct genesis::utils::SvgPolyline & (genesis::utils::SvgPolyline::*)(const struct genesis::utils::SvgPolyline &)) &genesis::utils::SvgPolyline::operator=, "C++: genesis::utils::SvgPolyline::operator=(const struct genesis::utils::SvgPolyline &) --> struct genesis::utils::SvgPolyline &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("add", (struct genesis::utils::SvgPolyline & (genesis::utils::SvgPolyline::*)(double, double)) &genesis::utils::SvgPolyline::add, "C++: genesis::utils::SvgPolyline::add(double, double) --> struct genesis::utils::SvgPolyline &", pybind11::return_value_policy::reference_internal, pybind11::arg("x"), pybind11::arg("y"));
		cl.def("add", (struct genesis::utils::SvgPolyline & (genesis::utils::SvgPolyline::*)(struct genesis::utils::SvgPoint)) &genesis::utils::SvgPolyline::add, "C++: genesis::utils::SvgPolyline::add(struct genesis::utils::SvgPoint) --> struct genesis::utils::SvgPolyline &", pybind11::return_value_policy::reference_internal, pybind11::arg("p"));
		cl.def("__lshift__", (struct genesis::utils::SvgPolyline & (genesis::utils::SvgPolyline::*)(struct genesis::utils::SvgPoint)) &genesis::utils::SvgPolyline::operator<<, "C++: genesis::utils::SvgPolyline::operator<<(struct genesis::utils::SvgPoint) --> struct genesis::utils::SvgPolyline &", pybind11::return_value_policy::reference_internal, pybind11::arg("p"));
		cl.def("bounding_box", (struct genesis::utils::SvgBox (genesis::utils::SvgPolyline::*)() const) &genesis::utils::SvgPolyline::bounding_box, "C++: genesis::utils::SvgPolyline::bounding_box() const --> struct genesis::utils::SvgBox");
		cl.def("write", [](genesis::utils::SvgPolyline const &o, std::ostream & a0) -> void { return o.write(a0); }, "", pybind11::arg("out"));
		cl.def("write", [](genesis::utils::SvgPolyline const &o, std::ostream & a0, unsigned long const & a1) -> void { return o.write(a0, a1); }, "", pybind11::arg("out"), pybind11::arg("indent"));
		cl.def("write", (void (genesis::utils::SvgPolyline::*)(std::ostream &, unsigned long, const struct genesis::utils::SvgDrawingOptions &) const) &genesis::utils::SvgPolyline::write, "C++: genesis::utils::SvgPolyline::write(std::ostream &, unsigned long, const struct genesis::utils::SvgDrawingOptions &) const --> void", pybind11::arg("out"), pybind11::arg("indent"), pybind11::arg("options"));
	}
	{ // genesis::utils::SvgPolygon file:genesis/utils/formats/svg/shapes.hpp line:389
		pybind11::class_<genesis::utils::SvgPolygon, std::shared_ptr<genesis::utils::SvgPolygon>> cl(M("genesis::utils"), "SvgPolygon", "");
		cl.def( pybind11::init( [](){ return new genesis::utils::SvgPolygon(); } ), "doc" );
		cl.def( pybind11::init( [](const struct genesis::utils::SvgStroke & a0){ return new genesis::utils::SvgPolygon(a0); } ), "doc" , pybind11::arg("stroke"));
		cl.def( pybind11::init<const struct genesis::utils::SvgStroke &, const struct genesis::utils::SvgFill &>(), pybind11::arg("stroke"), pybind11::arg("fill") );

		cl.def( pybind11::init( [](const class std::vector<struct genesis::utils::SvgPoint, class std::allocator<struct genesis::utils::SvgPoint> > & a0){ return new genesis::utils::SvgPolygon(a0); } ), "doc" , pybind11::arg("points"));
		cl.def( pybind11::init( [](const class std::vector<struct genesis::utils::SvgPoint, class std::allocator<struct genesis::utils::SvgPoint> > & a0, const struct genesis::utils::SvgStroke & a1){ return new genesis::utils::SvgPolygon(a0, a1); } ), "doc" , pybind11::arg("points"), pybind11::arg("stroke"));
		cl.def( pybind11::init<const class std::vector<struct genesis::utils::SvgPoint, class std::allocator<struct genesis::utils::SvgPoint> > &, const struct genesis::utils::SvgStroke &, const struct genesis::utils::SvgFill &>(), pybind11::arg("points"), pybind11::arg("stroke"), pybind11::arg("fill") );

		cl.def( pybind11::init( [](genesis::utils::SvgPolygon const &o){ return new genesis::utils::SvgPolygon(o); } ) );
		cl.def_readwrite("id", &genesis::utils::SvgPolygon::id);
		cl.def_readwrite("points", &genesis::utils::SvgPolygon::points);
		cl.def_readwrite("stroke", &genesis::utils::SvgPolygon::stroke);
		cl.def_readwrite("fill", &genesis::utils::SvgPolygon::fill);
		cl.def_readwrite("transform", &genesis::utils::SvgPolygon::transform);
		cl.def("assign", (struct genesis::utils::SvgPolygon & (genesis::utils::SvgPolygon::*)(const struct genesis::utils::SvgPolygon &)) &genesis::utils::SvgPolygon::operator=, "C++: genesis::utils::SvgPolygon::operator=(const struct genesis::utils::SvgPolygon &) --> struct genesis::utils::SvgPolygon &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("add", (struct genesis::utils::SvgPolygon & (genesis::utils::SvgPolygon::*)(double, double)) &genesis::utils::SvgPolygon::add, "C++: genesis::utils::SvgPolygon::add(double, double) --> struct genesis::utils::SvgPolygon &", pybind11::return_value_policy::reference_internal, pybind11::arg("x"), pybind11::arg("y"));
		cl.def("add", (struct genesis::utils::SvgPolygon & (genesis::utils::SvgPolygon::*)(struct genesis::utils::SvgPoint)) &genesis::utils::SvgPolygon::add, "C++: genesis::utils::SvgPolygon::add(struct genesis::utils::SvgPoint) --> struct genesis::utils::SvgPolygon &", pybind11::return_value_policy::reference_internal, pybind11::arg("p"));
		cl.def("__lshift__", (struct genesis::utils::SvgPolygon & (genesis::utils::SvgPolygon::*)(struct genesis::utils::SvgPoint)) &genesis::utils::SvgPolygon::operator<<, "C++: genesis::utils::SvgPolygon::operator<<(struct genesis::utils::SvgPoint) --> struct genesis::utils::SvgPolygon &", pybind11::return_value_policy::reference_internal, pybind11::arg("p"));
		cl.def("bounding_box", (struct genesis::utils::SvgBox (genesis::utils::SvgPolygon::*)() const) &genesis::utils::SvgPolygon::bounding_box, "C++: genesis::utils::SvgPolygon::bounding_box() const --> struct genesis::utils::SvgBox");
		cl.def("write", [](genesis::utils::SvgPolygon const &o, std::ostream & a0) -> void { return o.write(a0); }, "", pybind11::arg("out"));
		cl.def("write", [](genesis::utils::SvgPolygon const &o, std::ostream & a0, unsigned long const & a1) -> void { return o.write(a0, a1); }, "", pybind11::arg("out"), pybind11::arg("indent"));
		cl.def("write", (void (genesis::utils::SvgPolygon::*)(std::ostream &, unsigned long, const struct genesis::utils::SvgDrawingOptions &) const) &genesis::utils::SvgPolygon::write, "C++: genesis::utils::SvgPolygon::write(std::ostream &, unsigned long, const struct genesis::utils::SvgDrawingOptions &) const --> void", pybind11::arg("out"), pybind11::arg("indent"), pybind11::arg("options"));
	}
}
