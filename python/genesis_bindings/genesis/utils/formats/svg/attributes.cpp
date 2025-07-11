#include <genesis/utils/color/color.hpp>
#include <genesis/utils/formats/svg/attributes.hpp>
#include <genesis/utils/formats/svg/helper.hpp>
#include <genesis/utils/formats/svg/text.hpp>
#include <ios>
#include <iterator>
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

void bind_genesis_utils_formats_svg_attributes(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	{ // genesis::utils::SvgTransform file:genesis/utils/formats/svg/attributes.hpp line:238
		pybind11::class_<genesis::utils::SvgTransform, std::shared_ptr<genesis::utils::SvgTransform>> cl(M("genesis::utils"), "SvgTransform", "");
		cl.def( pybind11::init( [](){ return new genesis::utils::SvgTransform(); } ) );
		cl.def( pybind11::init( [](genesis::utils::SvgTransform const &o){ return new genesis::utils::SvgTransform(o); } ) );
		cl.def_readwrite("transformations", &genesis::utils::SvgTransform::transformations);
		cl.def("assign", (struct genesis::utils::SvgTransform & (genesis::utils::SvgTransform::*)(const struct genesis::utils::SvgTransform &)) &genesis::utils::SvgTransform::operator=, "C++: genesis::utils::SvgTransform::operator=(const struct genesis::utils::SvgTransform &) --> struct genesis::utils::SvgTransform &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("append", (void (genesis::utils::SvgTransform::*)(const class genesis::utils::SvgTransform::Transformation &)) &genesis::utils::SvgTransform::append, "C++: genesis::utils::SvgTransform::append(const class genesis::utils::SvgTransform::Transformation &) --> void", pybind11::arg("t"));
		cl.def("apply", (struct genesis::utils::SvgPoint (genesis::utils::SvgTransform::*)(const struct genesis::utils::SvgPoint &) const) &genesis::utils::SvgTransform::apply, "Apply all transformations to a point, and return the new transformed coordinate.\n\nC++: genesis::utils::SvgTransform::apply(const struct genesis::utils::SvgPoint &) const --> struct genesis::utils::SvgPoint", pybind11::arg("p"));
		cl.def("apply", (struct genesis::utils::SvgBox (genesis::utils::SvgTransform::*)(const struct genesis::utils::SvgBox &) const) &genesis::utils::SvgTransform::apply, "Apply all transformations to a box, and return the new transformed coordinates.\n\n As our implmentation is not a full svg renderer, we cheat here and just transform the\n corners of the box. That means, with rotations etc, there could be parts of an object\n where this model fails us. Note that we transform all four corners, and hence compute\n a box that is too large under rotations.\n\n Still, this is good enough for our puposes of determining the rough outlines of objects,\n so that we can scale the document around them.\n\nC++: genesis::utils::SvgTransform::apply(const struct genesis::utils::SvgBox &) const --> struct genesis::utils::SvgBox", pybind11::arg("b"));
		cl.def("write", (void (genesis::utils::SvgTransform::*)(std::ostream &) const) &genesis::utils::SvgTransform::write, "C++: genesis::utils::SvgTransform::write(std::ostream &) const --> void", pybind11::arg("out"));
		cl.def("clear", (void (genesis::utils::SvgTransform::*)()) &genesis::utils::SvgTransform::clear, "C++: genesis::utils::SvgTransform::clear() --> void");

		{ // genesis::utils::SvgTransform::Translate file:genesis/utils/formats/svg/attributes.hpp line:252
			auto & enclosing_class = cl;
			pybind11::class_<genesis::utils::SvgTransform::Translate, std::shared_ptr<genesis::utils::SvgTransform::Translate>> cl(enclosing_class, "Translate", "");
			cl.def( pybind11::init( [](){ return new genesis::utils::SvgTransform::Translate(); } ) );
			cl.def( pybind11::init<double, double>(), pybind11::arg("tx"), pybind11::arg("ty") );

			cl.def( pybind11::init<struct genesis::utils::SvgPoint>(), pybind11::arg("offset") );

			cl.def( pybind11::init( [](genesis::utils::SvgTransform::Translate const &o){ return new genesis::utils::SvgTransform::Translate(o); } ) );
			cl.def_readwrite("tx", &genesis::utils::SvgTransform::Translate::tx);
			cl.def_readwrite("ty", &genesis::utils::SvgTransform::Translate::ty);
			cl.def("assign", (struct genesis::utils::SvgTransform::Translate & (genesis::utils::SvgTransform::Translate::*)(const struct genesis::utils::SvgTransform::Translate &)) &genesis::utils::SvgTransform::Translate::operator=, "C++: genesis::utils::SvgTransform::Translate::operator=(const struct genesis::utils::SvgTransform::Translate &) --> struct genesis::utils::SvgTransform::Translate &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
			cl.def("write", (void (genesis::utils::SvgTransform::Translate::*)(std::ostream &) const) &genesis::utils::SvgTransform::Translate::write, "C++: genesis::utils::SvgTransform::Translate::write(std::ostream &) const --> void", pybind11::arg("out"));
			cl.def("apply", (struct genesis::utils::SvgPoint (genesis::utils::SvgTransform::Translate::*)(const struct genesis::utils::SvgPoint &) const) &genesis::utils::SvgTransform::Translate::apply, "C++: genesis::utils::SvgTransform::Translate::apply(const struct genesis::utils::SvgPoint &) const --> struct genesis::utils::SvgPoint", pybind11::arg("p"));
		}

		{ // genesis::utils::SvgTransform::Rotate file:genesis/utils/formats/svg/attributes.hpp line:298
			auto & enclosing_class = cl;
			pybind11::class_<genesis::utils::SvgTransform::Rotate, std::shared_ptr<genesis::utils::SvgTransform::Rotate>> cl(enclosing_class, "Rotate", "");
			cl.def( pybind11::init( [](){ return new genesis::utils::SvgTransform::Rotate(); } ) );
			cl.def( pybind11::init<double>(), pybind11::arg("angle") );

			cl.def( pybind11::init<double, double, double>(), pybind11::arg("angle"), pybind11::arg("cx"), pybind11::arg("cy") );

			cl.def( pybind11::init<double, struct genesis::utils::SvgPoint>(), pybind11::arg("angle"), pybind11::arg("offset") );

			cl.def( pybind11::init( [](genesis::utils::SvgTransform::Rotate const &o){ return new genesis::utils::SvgTransform::Rotate(o); } ) );
			cl.def_readwrite("a", &genesis::utils::SvgTransform::Rotate::a);
			cl.def_readwrite("cx", &genesis::utils::SvgTransform::Rotate::cx);
			cl.def_readwrite("cy", &genesis::utils::SvgTransform::Rotate::cy);
			cl.def("assign", (struct genesis::utils::SvgTransform::Rotate & (genesis::utils::SvgTransform::Rotate::*)(const struct genesis::utils::SvgTransform::Rotate &)) &genesis::utils::SvgTransform::Rotate::operator=, "C++: genesis::utils::SvgTransform::Rotate::operator=(const struct genesis::utils::SvgTransform::Rotate &) --> struct genesis::utils::SvgTransform::Rotate &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
			cl.def("write", (void (genesis::utils::SvgTransform::Rotate::*)(std::ostream &) const) &genesis::utils::SvgTransform::Rotate::write, "C++: genesis::utils::SvgTransform::Rotate::write(std::ostream &) const --> void", pybind11::arg("out"));
			cl.def("apply", (struct genesis::utils::SvgPoint (genesis::utils::SvgTransform::Rotate::*)(const struct genesis::utils::SvgPoint &) const) &genesis::utils::SvgTransform::Rotate::apply, "C++: genesis::utils::SvgTransform::Rotate::apply(const struct genesis::utils::SvgPoint &) const --> struct genesis::utils::SvgPoint", pybind11::arg("p"));
		}

		{ // genesis::utils::SvgTransform::Scale file:genesis/utils/formats/svg/attributes.hpp line:359
			auto & enclosing_class = cl;
			pybind11::class_<genesis::utils::SvgTransform::Scale, std::shared_ptr<genesis::utils::SvgTransform::Scale>> cl(enclosing_class, "Scale", "");
			cl.def( pybind11::init( [](){ return new genesis::utils::SvgTransform::Scale(); } ) );
			cl.def( pybind11::init<double>(), pybind11::arg("sxy") );

			cl.def( pybind11::init<double, double>(), pybind11::arg("sx"), pybind11::arg("sy") );

			cl.def( pybind11::init( [](genesis::utils::SvgTransform::Scale const &o){ return new genesis::utils::SvgTransform::Scale(o); } ) );
			cl.def_readwrite("sx", &genesis::utils::SvgTransform::Scale::sx);
			cl.def_readwrite("sy", &genesis::utils::SvgTransform::Scale::sy);
			cl.def("assign", (struct genesis::utils::SvgTransform::Scale & (genesis::utils::SvgTransform::Scale::*)(const struct genesis::utils::SvgTransform::Scale &)) &genesis::utils::SvgTransform::Scale::operator=, "C++: genesis::utils::SvgTransform::Scale::operator=(const struct genesis::utils::SvgTransform::Scale &) --> struct genesis::utils::SvgTransform::Scale &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
			cl.def("write", (void (genesis::utils::SvgTransform::Scale::*)(std::ostream &) const) &genesis::utils::SvgTransform::Scale::write, "C++: genesis::utils::SvgTransform::Scale::write(std::ostream &) const --> void", pybind11::arg("out"));
			cl.def("apply", (struct genesis::utils::SvgPoint (genesis::utils::SvgTransform::Scale::*)(const struct genesis::utils::SvgPoint &) const) &genesis::utils::SvgTransform::Scale::apply, "C++: genesis::utils::SvgTransform::Scale::apply(const struct genesis::utils::SvgPoint &) const --> struct genesis::utils::SvgPoint", pybind11::arg("p"));
		}

		{ // genesis::utils::SvgTransform::SkewX file:genesis/utils/formats/svg/attributes.hpp line:406
			auto & enclosing_class = cl;
			pybind11::class_<genesis::utils::SvgTransform::SkewX, std::shared_ptr<genesis::utils::SvgTransform::SkewX>> cl(enclosing_class, "SkewX", "");
			cl.def( pybind11::init( [](){ return new genesis::utils::SvgTransform::SkewX(); } ) );
			cl.def( pybind11::init<double>(), pybind11::arg("a") );

			cl.def( pybind11::init( [](genesis::utils::SvgTransform::SkewX const &o){ return new genesis::utils::SvgTransform::SkewX(o); } ) );
			cl.def_readwrite("ax", &genesis::utils::SvgTransform::SkewX::ax);
			cl.def("assign", (struct genesis::utils::SvgTransform::SkewX & (genesis::utils::SvgTransform::SkewX::*)(const struct genesis::utils::SvgTransform::SkewX &)) &genesis::utils::SvgTransform::SkewX::operator=, "C++: genesis::utils::SvgTransform::SkewX::operator=(const struct genesis::utils::SvgTransform::SkewX &) --> struct genesis::utils::SvgTransform::SkewX &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
			cl.def("write", (void (genesis::utils::SvgTransform::SkewX::*)(std::ostream &) const) &genesis::utils::SvgTransform::SkewX::write, "C++: genesis::utils::SvgTransform::SkewX::write(std::ostream &) const --> void", pybind11::arg("out"));
			cl.def("apply", (struct genesis::utils::SvgPoint (genesis::utils::SvgTransform::SkewX::*)(const struct genesis::utils::SvgPoint &) const) &genesis::utils::SvgTransform::SkewX::apply, "C++: genesis::utils::SvgTransform::SkewX::apply(const struct genesis::utils::SvgPoint &) const --> struct genesis::utils::SvgPoint", pybind11::arg("p"));
		}

		{ // genesis::utils::SvgTransform::SkewY file:genesis/utils/formats/svg/attributes.hpp line:449
			auto & enclosing_class = cl;
			pybind11::class_<genesis::utils::SvgTransform::SkewY, std::shared_ptr<genesis::utils::SvgTransform::SkewY>> cl(enclosing_class, "SkewY", "");
			cl.def( pybind11::init( [](){ return new genesis::utils::SvgTransform::SkewY(); } ) );
			cl.def( pybind11::init<double>(), pybind11::arg("a") );

			cl.def( pybind11::init( [](genesis::utils::SvgTransform::SkewY const &o){ return new genesis::utils::SvgTransform::SkewY(o); } ) );
			cl.def_readwrite("ay", &genesis::utils::SvgTransform::SkewY::ay);
			cl.def("assign", (struct genesis::utils::SvgTransform::SkewY & (genesis::utils::SvgTransform::SkewY::*)(const struct genesis::utils::SvgTransform::SkewY &)) &genesis::utils::SvgTransform::SkewY::operator=, "C++: genesis::utils::SvgTransform::SkewY::operator=(const struct genesis::utils::SvgTransform::SkewY &) --> struct genesis::utils::SvgTransform::SkewY &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
			cl.def("write", (void (genesis::utils::SvgTransform::SkewY::*)(std::ostream &) const) &genesis::utils::SvgTransform::SkewY::write, "C++: genesis::utils::SvgTransform::SkewY::write(std::ostream &) const --> void", pybind11::arg("out"));
			cl.def("apply", (struct genesis::utils::SvgPoint (genesis::utils::SvgTransform::SkewY::*)(const struct genesis::utils::SvgPoint &) const) &genesis::utils::SvgTransform::SkewY::apply, "C++: genesis::utils::SvgTransform::SkewY::apply(const struct genesis::utils::SvgPoint &) const --> struct genesis::utils::SvgPoint", pybind11::arg("p"));
		}

		{ // genesis::utils::SvgTransform::Matrix file:genesis/utils/formats/svg/attributes.hpp line:492
			auto & enclosing_class = cl;
			pybind11::class_<genesis::utils::SvgTransform::Matrix, std::shared_ptr<genesis::utils::SvgTransform::Matrix>> cl(enclosing_class, "Matrix", "");
			cl.def( pybind11::init( [](){ return new genesis::utils::SvgTransform::Matrix(); } ) );
			cl.def( pybind11::init<double, double, double, double, double, double>(), pybind11::arg("a"), pybind11::arg("b"), pybind11::arg("c"), pybind11::arg("d"), pybind11::arg("e"), pybind11::arg("f") );

			cl.def( pybind11::init( [](genesis::utils::SvgTransform::Matrix const &o){ return new genesis::utils::SvgTransform::Matrix(o); } ) );
			cl.def_readwrite("a", &genesis::utils::SvgTransform::Matrix::a);
			cl.def_readwrite("b", &genesis::utils::SvgTransform::Matrix::b);
			cl.def_readwrite("c", &genesis::utils::SvgTransform::Matrix::c);
			cl.def_readwrite("d", &genesis::utils::SvgTransform::Matrix::d);
			cl.def_readwrite("e", &genesis::utils::SvgTransform::Matrix::e);
			cl.def_readwrite("f", &genesis::utils::SvgTransform::Matrix::f);
			cl.def("assign", (struct genesis::utils::SvgTransform::Matrix & (genesis::utils::SvgTransform::Matrix::*)(const struct genesis::utils::SvgTransform::Matrix &)) &genesis::utils::SvgTransform::Matrix::operator=, "C++: genesis::utils::SvgTransform::Matrix::operator=(const struct genesis::utils::SvgTransform::Matrix &) --> struct genesis::utils::SvgTransform::Matrix &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
			cl.def("write", (void (genesis::utils::SvgTransform::Matrix::*)(std::ostream &) const) &genesis::utils::SvgTransform::Matrix::write, "C++: genesis::utils::SvgTransform::Matrix::write(std::ostream &) const --> void", pybind11::arg("out"));
			cl.def("apply", (struct genesis::utils::SvgPoint (genesis::utils::SvgTransform::Matrix::*)(const struct genesis::utils::SvgPoint &) const) &genesis::utils::SvgTransform::Matrix::apply, "C++: genesis::utils::SvgTransform::Matrix::apply(const struct genesis::utils::SvgPoint &) const --> struct genesis::utils::SvgPoint", pybind11::arg("p"));
		}

		{ // genesis::utils::SvgTransform::Transformation file:genesis/utils/formats/svg/attributes.hpp line:546
			auto & enclosing_class = cl;
			pybind11::class_<genesis::utils::SvgTransform::Transformation, std::shared_ptr<genesis::utils::SvgTransform::Transformation>> cl(enclosing_class, "Transformation", "Internal helper class used as an abstraction to be able to store Transformations\n without need for inheritance and virtual functions within the different transformation types.");
			cl.def( pybind11::init( [](genesis::utils::SvgTransform::Transformation const &o){ return new genesis::utils::SvgTransform::Transformation(o); } ) );
			cl.def("assign", (class genesis::utils::SvgTransform::Transformation & (genesis::utils::SvgTransform::Transformation::*)(class genesis::utils::SvgTransform::Transformation)) &genesis::utils::SvgTransform::Transformation::operator=, "C++: genesis::utils::SvgTransform::Transformation::operator=(class genesis::utils::SvgTransform::Transformation) --> class genesis::utils::SvgTransform::Transformation &", pybind11::return_value_policy::reference_internal, pybind11::arg("other"));
			cl.def("apply", (struct genesis::utils::SvgPoint (genesis::utils::SvgTransform::Transformation::*)(const struct genesis::utils::SvgPoint &) const) &genesis::utils::SvgTransform::Transformation::apply, "C++: genesis::utils::SvgTransform::Transformation::apply(const struct genesis::utils::SvgPoint &) const --> struct genesis::utils::SvgPoint", pybind11::arg("p"));
			cl.def("write", (void (genesis::utils::SvgTransform::Transformation::*)(std::ostream &) const) &genesis::utils::SvgTransform::Transformation::write, "C++: genesis::utils::SvgTransform::Transformation::write(std::ostream &) const --> void", pybind11::arg("out"));
		}

	}
	{ // genesis::utils::SvgText file:genesis/utils/formats/svg/text.hpp line:47
		pybind11::class_<genesis::utils::SvgText, std::shared_ptr<genesis::utils::SvgText>> cl(M("genesis::utils"), "SvgText", "");
		cl.def( pybind11::init( [](){ return new genesis::utils::SvgText(); } ), "doc" );
		cl.def( pybind11::init( [](const std::string & a0){ return new genesis::utils::SvgText(a0); } ), "doc" , pybind11::arg("text"));
		cl.def( pybind11::init( [](const std::string & a0, const struct genesis::utils::SvgPoint & a1){ return new genesis::utils::SvgText(a0, a1); } ), "doc" , pybind11::arg("text"), pybind11::arg("position"));
		cl.def( pybind11::init( [](const std::string & a0, const struct genesis::utils::SvgPoint & a1, const struct genesis::utils::SvgFont & a2){ return new genesis::utils::SvgText(a0, a1, a2); } ), "doc" , pybind11::arg("text"), pybind11::arg("position"), pybind11::arg("font"));
		cl.def( pybind11::init( [](const std::string & a0, const struct genesis::utils::SvgPoint & a1, const struct genesis::utils::SvgFont & a2, const struct genesis::utils::SvgFill & a3){ return new genesis::utils::SvgText(a0, a1, a2, a3); } ), "doc" , pybind11::arg("text"), pybind11::arg("position"), pybind11::arg("font"), pybind11::arg("fill"));
		cl.def( pybind11::init<const std::string &, const struct genesis::utils::SvgPoint &, const struct genesis::utils::SvgFont &, const struct genesis::utils::SvgFill &, const struct genesis::utils::SvgStroke &>(), pybind11::arg("text"), pybind11::arg("position"), pybind11::arg("font"), pybind11::arg("fill"), pybind11::arg("stroke") );

		cl.def( pybind11::init( [](genesis::utils::SvgText const &o){ return new genesis::utils::SvgText(o); } ) );

		pybind11::enum_<genesis::utils::SvgText::Anchor>(cl, "Anchor", "")
			.value("kNone", genesis::utils::SvgText::Anchor::kNone)
			.value("kStart", genesis::utils::SvgText::Anchor::kStart)
			.value("kMiddle", genesis::utils::SvgText::Anchor::kMiddle)
			.value("kEnd", genesis::utils::SvgText::Anchor::kEnd);


		pybind11::enum_<genesis::utils::SvgText::DominantBaseline>(cl, "DominantBaseline", "")
			.value("kNone", genesis::utils::SvgText::DominantBaseline::kNone)
			.value("kAuto", genesis::utils::SvgText::DominantBaseline::kAuto)
			.value("kUseScript", genesis::utils::SvgText::DominantBaseline::kUseScript)
			.value("kNoChange", genesis::utils::SvgText::DominantBaseline::kNoChange)
			.value("kResetSize", genesis::utils::SvgText::DominantBaseline::kResetSize)
			.value("kIdeographic", genesis::utils::SvgText::DominantBaseline::kIdeographic)
			.value("kAlphabetic", genesis::utils::SvgText::DominantBaseline::kAlphabetic)
			.value("kHanging", genesis::utils::SvgText::DominantBaseline::kHanging)
			.value("kMathematical", genesis::utils::SvgText::DominantBaseline::kMathematical)
			.value("kCentral", genesis::utils::SvgText::DominantBaseline::kCentral)
			.value("kMiddle", genesis::utils::SvgText::DominantBaseline::kMiddle)
			.value("kTextAfterEdge", genesis::utils::SvgText::DominantBaseline::kTextAfterEdge)
			.value("kTextBeforeEdge", genesis::utils::SvgText::DominantBaseline::kTextBeforeEdge)
			.value("kInherit", genesis::utils::SvgText::DominantBaseline::kInherit);


		pybind11::enum_<genesis::utils::SvgText::AlignmentBaseline>(cl, "AlignmentBaseline", "")
			.value("kNone", genesis::utils::SvgText::AlignmentBaseline::kNone)
			.value("kAuto", genesis::utils::SvgText::AlignmentBaseline::kAuto)
			.value("kBaseline", genesis::utils::SvgText::AlignmentBaseline::kBaseline)
			.value("kBeforeEdge", genesis::utils::SvgText::AlignmentBaseline::kBeforeEdge)
			.value("kTextBeforeEdge", genesis::utils::SvgText::AlignmentBaseline::kTextBeforeEdge)
			.value("kMiddle", genesis::utils::SvgText::AlignmentBaseline::kMiddle)
			.value("kCentral", genesis::utils::SvgText::AlignmentBaseline::kCentral)
			.value("kAfterEdge", genesis::utils::SvgText::AlignmentBaseline::kAfterEdge)
			.value("kTextAfterEdge", genesis::utils::SvgText::AlignmentBaseline::kTextAfterEdge)
			.value("kIdeographic", genesis::utils::SvgText::AlignmentBaseline::kIdeographic)
			.value("kAlphabetic", genesis::utils::SvgText::AlignmentBaseline::kAlphabetic)
			.value("kHanging", genesis::utils::SvgText::AlignmentBaseline::kHanging)
			.value("kMathematical", genesis::utils::SvgText::AlignmentBaseline::kMathematical)
			.value("kInherit", genesis::utils::SvgText::AlignmentBaseline::kInherit);

		cl.def_readwrite("id", &genesis::utils::SvgText::id);
		cl.def_readwrite("text", &genesis::utils::SvgText::text);
		cl.def_readwrite("position", &genesis::utils::SvgText::position);
		cl.def_readwrite("font", &genesis::utils::SvgText::font);
		cl.def_readwrite("fill", &genesis::utils::SvgText::fill);
		cl.def_readwrite("stroke", &genesis::utils::SvgText::stroke);
		cl.def_readwrite("anchor", &genesis::utils::SvgText::anchor);
		cl.def_readwrite("dominant_baseline", &genesis::utils::SvgText::dominant_baseline);
		cl.def_readwrite("alignment_baseline", &genesis::utils::SvgText::alignment_baseline);
		cl.def_readwrite("kerning", &genesis::utils::SvgText::kerning);
		cl.def_readwrite("letter_spacing", &genesis::utils::SvgText::letter_spacing);
		cl.def_readwrite("word_spacing", &genesis::utils::SvgText::word_spacing);
		cl.def_readwrite("dx", &genesis::utils::SvgText::dx);
		cl.def_readwrite("dy", &genesis::utils::SvgText::dy);
		cl.def_readwrite("transform", &genesis::utils::SvgText::transform);
		cl.def("assign", (struct genesis::utils::SvgText & (genesis::utils::SvgText::*)(const struct genesis::utils::SvgText &)) &genesis::utils::SvgText::operator=, "C++: genesis::utils::SvgText::operator=(const struct genesis::utils::SvgText &) --> struct genesis::utils::SvgText &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("bounding_box", (struct genesis::utils::SvgBox (genesis::utils::SvgText::*)() const) &genesis::utils::SvgText::bounding_box, "C++: genesis::utils::SvgText::bounding_box() const --> struct genesis::utils::SvgBox");
		cl.def("write", [](genesis::utils::SvgText const &o, std::ostream & a0) -> void { return o.write(a0); }, "", pybind11::arg("out"));
		cl.def("write", [](genesis::utils::SvgText const &o, std::ostream & a0, unsigned long const & a1) -> void { return o.write(a0, a1); }, "", pybind11::arg("out"), pybind11::arg("indent"));
		cl.def("write", (void (genesis::utils::SvgText::*)(std::ostream &, unsigned long, const struct genesis::utils::SvgDrawingOptions &) const) &genesis::utils::SvgText::write, "C++: genesis::utils::SvgText::write(std::ostream &, unsigned long, const struct genesis::utils::SvgDrawingOptions &) const --> void", pybind11::arg("out"), pybind11::arg("indent"), pybind11::arg("options"));
		cl.def_static("anchor_to_string", (std::string (*)(enum genesis::utils::SvgText::Anchor)) &genesis::utils::SvgText::anchor_to_string, "C++: genesis::utils::SvgText::anchor_to_string(enum genesis::utils::SvgText::Anchor) --> std::string", pybind11::arg("value"));
		cl.def_static("dominant_baseline_to_string", (std::string (*)(enum genesis::utils::SvgText::DominantBaseline)) &genesis::utils::SvgText::dominant_baseline_to_string, "C++: genesis::utils::SvgText::dominant_baseline_to_string(enum genesis::utils::SvgText::DominantBaseline) --> std::string", pybind11::arg("value"));
		cl.def_static("alignment_baseline_to_string", (std::string (*)(enum genesis::utils::SvgText::AlignmentBaseline)) &genesis::utils::SvgText::alignment_baseline_to_string, "C++: genesis::utils::SvgText::alignment_baseline_to_string(enum genesis::utils::SvgText::AlignmentBaseline) --> std::string", pybind11::arg("value"));
	}
}
