#include <genesis/utils/color/norm_linear.hpp>
#include <genesis/utils/formats/svg/helper.hpp>
#include <genesis/utils/tools/tickmarks.hpp>
#include <iterator>
#include <memory>
#include <sstream> // __str__
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

// genesis::utils::ColorNormalizationLinear file:genesis/utils/color/norm_linear.hpp line:59
struct PyCallBack_genesis_utils_ColorNormalizationLinear : public genesis::utils::ColorNormalizationLinear {
	using genesis::utils::ColorNormalizationLinear::ColorNormalizationLinear;

	bool is_valid_() const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const genesis::utils::ColorNormalizationLinear *>(this), "is_valid_");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>();
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::override_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return ColorNormalizationLinear::is_valid_();
	}
	void is_valid_or_throw_() const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const genesis::utils::ColorNormalizationLinear *>(this), "is_valid_or_throw_");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>();
			if (pybind11::detail::cast_is_temporary_value_reference<void>::value) {
				static pybind11::detail::override_caster_t<void> caster;
				return pybind11::detail::cast_ref<void>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<void>(std::move(o));
		}
		return ColorNormalizationLinear::is_valid_or_throw_();
	}
	double normalize_(double a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const genesis::utils::ColorNormalizationLinear *>(this), "normalize_");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<double>::value) {
				static pybind11::detail::override_caster_t<double> caster;
				return pybind11::detail::cast_ref<double>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<double>(std::move(o));
		}
		return ColorNormalizationLinear::normalize_(a0);
	}
	void update_hook_(double a0, double a1) override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const genesis::utils::ColorNormalizationLinear *>(this), "update_hook_");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<void>::value) {
				static pybind11::detail::override_caster_t<void> caster;
				return pybind11::detail::cast_ref<void>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<void>(std::move(o));
		}
		return ColorNormalizationLinear::update_hook_(a0, a1);
	}
};

void bind_genesis_utils_tools_tickmarks(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	{ // genesis::utils::Tickmarks file:genesis/utils/tools/tickmarks.hpp line:48
		pybind11::class_<genesis::utils::Tickmarks, std::shared_ptr<genesis::utils::Tickmarks>> cl(M("genesis::utils"), "Tickmarks", "Helper class to find \"nice\" tickmark intervals for creating scales and axes.");
		cl.def( pybind11::init( [](){ return new genesis::utils::Tickmarks(); } ) );
		cl.def( pybind11::init( [](genesis::utils::Tickmarks const &o){ return new genesis::utils::Tickmarks(o); } ) );
		cl.def_readwrite("include_min", &genesis::utils::Tickmarks::include_min);
		cl.def_readwrite("include_max", &genesis::utils::Tickmarks::include_max);
		cl.def_readwrite("undershoot_at_min", &genesis::utils::Tickmarks::undershoot_at_min);
		cl.def_readwrite("overshoot_at_max", &genesis::utils::Tickmarks::overshoot_at_max);
		cl.def_readwrite("relative_epsilon", &genesis::utils::Tickmarks::relative_epsilon);
		cl.def("assign", (struct genesis::utils::Tickmarks & (genesis::utils::Tickmarks::*)(const struct genesis::utils::Tickmarks &)) &genesis::utils::Tickmarks::operator=, "C++: genesis::utils::Tickmarks::operator=(const struct genesis::utils::Tickmarks &) --> struct genesis::utils::Tickmarks &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def_static("step_size", (double (*)(double, unsigned long)) &genesis::utils::Tickmarks::step_size, "Calculate a step size that fills the `interval_size` in approximately `target_steps`\n many steps.\n\n The resulting step size is calculated to be a \"nice\" size, i.e. fits `[ 1 | 2 | 5 ] * 10^n`.\n Thus, exemplary nice step sizes are 0.01, 20 and 500.\n\nC++: genesis::utils::Tickmarks::step_size(double, unsigned long) --> double", pybind11::arg("interval_size"), pybind11::arg("target_steps"));
		cl.def("linear_ticks", (class std::vector<double, class std::allocator<double> > (genesis::utils::Tickmarks::*)(double, double, unsigned long)) &genesis::utils::Tickmarks::linear_ticks, "Calculate a set of ticks that linearily span from `min` to `max` in approximately\n `target_steps` many steps, where each tick sits on a \"nice\" number.\n\n See step_size() for what is meant by nice numbers.\n\nC++: genesis::utils::Tickmarks::linear_ticks(double, double, unsigned long) --> class std::vector<double, class std::allocator<double> >", pybind11::arg("min"), pybind11::arg("max"), pybind11::arg("target_steps"));
		cl.def("linear_labels", (class std::vector<struct genesis::utils::Tickmarks::LabeledTick, class std::allocator<struct genesis::utils::Tickmarks::LabeledTick> > (genesis::utils::Tickmarks::*)(double, double, unsigned long)) &genesis::utils::Tickmarks::linear_labels, "Return a set of labels with relative positions between `min` and `max`, where the\n labels correspond to the linear_ticks() numbers.\n\n This function is a convenience wrapper for linear_ticks(), that turns the tick positions\n into relative values, while keeping the actual tick value as a label. Relative position 0.0\n corresponds to `min`, relative position 1.0 to `max`.\n\nC++: genesis::utils::Tickmarks::linear_labels(double, double, unsigned long) --> class std::vector<struct genesis::utils::Tickmarks::LabeledTick, class std::allocator<struct genesis::utils::Tickmarks::LabeledTick> >", pybind11::arg("min"), pybind11::arg("max"), pybind11::arg("target_steps"));
		cl.def("logarithmic_labels", [](genesis::utils::Tickmarks &o, double const & a0, double const & a1) -> std::vector<struct genesis::utils::Tickmarks::LabeledTick, class std::allocator<struct genesis::utils::Tickmarks::LabeledTick> > { return o.logarithmic_labels(a0, a1); }, "", pybind11::arg("min"), pybind11::arg("max"));
		cl.def("logarithmic_labels", (class std::vector<struct genesis::utils::Tickmarks::LabeledTick, class std::allocator<struct genesis::utils::Tickmarks::LabeledTick> > (genesis::utils::Tickmarks::*)(double, double, double)) &genesis::utils::Tickmarks::logarithmic_labels, "Return a set of labels with relative positions between `min` and `max`, where the\n labels correspond to logarithmic ticks.\n\n This function is the equivalent of linear_labels(), but instead uses logarithmic scaling.\n Also, it inserts as many ticks as are needed to go up to  in powers of \n and thus has no target steps.\n\n Example: Given `min == 1.0`, `max == 350`, with `overshoot_at_max == false` and\n `include_max == true`, the resulting vector contains entries with labels `1, 10, 100, 350`.\n\n Remark: There is no logarithmic equivalent for linear_ticks(), as those would simply be\n powers of  up to `max`.\n\nC++: genesis::utils::Tickmarks::logarithmic_labels(double, double, double) --> class std::vector<struct genesis::utils::Tickmarks::LabeledTick, class std::allocator<struct genesis::utils::Tickmarks::LabeledTick> >", pybind11::arg("min"), pybind11::arg("max"), pybind11::arg("base"));

		{ // genesis::utils::Tickmarks::LabeledTick file:genesis/utils/tools/tickmarks.hpp line:56
			auto & enclosing_class = cl;
			pybind11::class_<genesis::utils::Tickmarks::LabeledTick, std::shared_ptr<genesis::utils::Tickmarks::LabeledTick>> cl(enclosing_class, "LabeledTick", "");
			cl.def( pybind11::init<double, double>(), pybind11::arg("rel_pos"), pybind11::arg("lab") );

			cl.def_readwrite("relative_position", &genesis::utils::Tickmarks::LabeledTick::relative_position);
			cl.def_readwrite("label", &genesis::utils::Tickmarks::LabeledTick::label);
		}

	}
	{ // genesis::utils::ColorNormalizationLinear file:genesis/utils/color/norm_linear.hpp line:59
		pybind11::class_<genesis::utils::ColorNormalizationLinear, std::shared_ptr<genesis::utils::ColorNormalizationLinear>, PyCallBack_genesis_utils_ColorNormalizationLinear, genesis::utils::ColorNormalization> cl(M("genesis::utils"), "ColorNormalizationLinear", "Default Color normalization, using a sequential linear scaling in the\n range `[ min, max ]`.");
		cl.def( pybind11::init( [](){ return new genesis::utils::ColorNormalizationLinear(); }, [](){ return new PyCallBack_genesis_utils_ColorNormalizationLinear(); } ) );
		cl.def( pybind11::init<double, double>(), pybind11::arg("min"), pybind11::arg("max") );

		cl.def( pybind11::init<const class std::vector<double, class std::allocator<double> > &>(), pybind11::arg("values") );

		cl.def( pybind11::init( [](PyCallBack_genesis_utils_ColorNormalizationLinear const &o){ return new PyCallBack_genesis_utils_ColorNormalizationLinear(o); } ) );
		cl.def( pybind11::init( [](genesis::utils::ColorNormalizationLinear const &o){ return new genesis::utils::ColorNormalizationLinear(o); } ) );
		cl.def("autoscale", (class genesis::utils::ColorNormalizationLinear & (genesis::utils::ColorNormalizationLinear::*)(class __gnu_cxx::__normal_iterator<const double *, class std::vector<double, class std::allocator<double> > >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double, class std::allocator<double> > >)) &genesis::utils::ColorNormalizationLinear::autoscale<__gnu_cxx::__normal_iterator<const double *, std::vector<double, std::allocator<double> > >>, "C++: genesis::utils::ColorNormalizationLinear::autoscale(class __gnu_cxx::__normal_iterator<const double *, class std::vector<double, class std::allocator<double> > >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double, class std::allocator<double> > >) --> class genesis::utils::ColorNormalizationLinear &", pybind11::return_value_policy::reference_internal, pybind11::arg("first"), pybind11::arg("last"));
		cl.def("autoscale_min", (class genesis::utils::ColorNormalizationLinear & (genesis::utils::ColorNormalizationLinear::*)(class __gnu_cxx::__normal_iterator<const double *, class std::vector<double, class std::allocator<double> > >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double, class std::allocator<double> > >)) &genesis::utils::ColorNormalizationLinear::autoscale_min<__gnu_cxx::__normal_iterator<const double *, std::vector<double, std::allocator<double> > >>, "C++: genesis::utils::ColorNormalizationLinear::autoscale_min(class __gnu_cxx::__normal_iterator<const double *, class std::vector<double, class std::allocator<double> > >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double, class std::allocator<double> > >) --> class genesis::utils::ColorNormalizationLinear &", pybind11::return_value_policy::reference_internal, pybind11::arg("first"), pybind11::arg("last"));
		cl.def("autoscale_max", (class genesis::utils::ColorNormalizationLinear & (genesis::utils::ColorNormalizationLinear::*)(class __gnu_cxx::__normal_iterator<const double *, class std::vector<double, class std::allocator<double> > >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double, class std::allocator<double> > >)) &genesis::utils::ColorNormalizationLinear::autoscale_max<__gnu_cxx::__normal_iterator<const double *, std::vector<double, std::allocator<double> > >>, "C++: genesis::utils::ColorNormalizationLinear::autoscale_max(class __gnu_cxx::__normal_iterator<const double *, class std::vector<double, class std::allocator<double> > >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double, class std::allocator<double> > >) --> class genesis::utils::ColorNormalizationLinear &", pybind11::return_value_policy::reference_internal, pybind11::arg("first"), pybind11::arg("last"));
		cl.def("assign", (class genesis::utils::ColorNormalizationLinear & (genesis::utils::ColorNormalizationLinear::*)(const class genesis::utils::ColorNormalizationLinear &)) &genesis::utils::ColorNormalizationLinear::operator=, "C++: genesis::utils::ColorNormalizationLinear::operator=(const class genesis::utils::ColorNormalizationLinear &) --> class genesis::utils::ColorNormalizationLinear &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("min_value", (double (genesis::utils::ColorNormalizationLinear::*)() const) &genesis::utils::ColorNormalizationLinear::min_value, "Minimum value, that is, where to begin the color scale.\n\nC++: genesis::utils::ColorNormalizationLinear::min_value() const --> double");
		cl.def("max_value", (double (genesis::utils::ColorNormalizationLinear::*)() const) &genesis::utils::ColorNormalizationLinear::max_value, "Minimum value, that is, where to end the color scale.\n\nC++: genesis::utils::ColorNormalizationLinear::max_value() const --> double");
		cl.def("scale", (class genesis::utils::ColorNormalizationLinear & (genesis::utils::ColorNormalizationLinear::*)(double, double)) &genesis::utils::ColorNormalizationLinear::scale, "C++: genesis::utils::ColorNormalizationLinear::scale(double, double) --> class genesis::utils::ColorNormalizationLinear &", pybind11::return_value_policy::reference_internal, pybind11::arg("min"), pybind11::arg("max"));
		cl.def("autoscale", (class genesis::utils::ColorNormalizationLinear & (genesis::utils::ColorNormalizationLinear::*)(const class std::vector<double, class std::allocator<double> > &)) &genesis::utils::ColorNormalizationLinear::autoscale, "C++: genesis::utils::ColorNormalizationLinear::autoscale(const class std::vector<double, class std::allocator<double> > &) --> class genesis::utils::ColorNormalizationLinear &", pybind11::return_value_policy::reference_internal, pybind11::arg("values"));
		cl.def("autoscale_min", (class genesis::utils::ColorNormalizationLinear & (genesis::utils::ColorNormalizationLinear::*)(const class std::vector<double, class std::allocator<double> > &)) &genesis::utils::ColorNormalizationLinear::autoscale_min, "C++: genesis::utils::ColorNormalizationLinear::autoscale_min(const class std::vector<double, class std::allocator<double> > &) --> class genesis::utils::ColorNormalizationLinear &", pybind11::return_value_policy::reference_internal, pybind11::arg("values"));
		cl.def("autoscale_max", (class genesis::utils::ColorNormalizationLinear & (genesis::utils::ColorNormalizationLinear::*)(const class std::vector<double, class std::allocator<double> > &)) &genesis::utils::ColorNormalizationLinear::autoscale_max, "C++: genesis::utils::ColorNormalizationLinear::autoscale_max(const class std::vector<double, class std::allocator<double> > &) --> class genesis::utils::ColorNormalizationLinear &", pybind11::return_value_policy::reference_internal, pybind11::arg("values"));
		cl.def("min_value", (class genesis::utils::ColorNormalizationLinear & (genesis::utils::ColorNormalizationLinear::*)(double)) &genesis::utils::ColorNormalizationLinear::min_value, "min_value()\n\nC++: genesis::utils::ColorNormalizationLinear::min_value(double) --> class genesis::utils::ColorNormalizationLinear &", pybind11::return_value_policy::reference_internal, pybind11::arg("value"));
		cl.def("max_value", (class genesis::utils::ColorNormalizationLinear & (genesis::utils::ColorNormalizationLinear::*)(double)) &genesis::utils::ColorNormalizationLinear::max_value, "max_value()\n\nC++: genesis::utils::ColorNormalizationLinear::max_value(double) --> class genesis::utils::ColorNormalizationLinear &", pybind11::return_value_policy::reference_internal, pybind11::arg("value"));
	}
	// genesis::utils::base64_encode(const class std::vector<unsigned char, class std::allocator<unsigned char> > &, unsigned long) file: line:45
	M("genesis::utils").def("base64_encode", [](const class std::vector<unsigned char, class std::allocator<unsigned char> > & a0) -> std::string { return genesis::utils::base64_encode(a0); }, "", pybind11::arg("input"));
	M("genesis::utils").def("base64_encode", (std::string (*)(const class std::vector<unsigned char, class std::allocator<unsigned char> > &, unsigned long)) &genesis::utils::base64_encode, "C++: genesis::utils::base64_encode(const class std::vector<unsigned char, class std::allocator<unsigned char> > &, unsigned long) --> std::string", pybind11::arg("input"), pybind11::arg("line_length"));

	// genesis::utils::base64_encode(const std::string &, unsigned long) file: line:46
	M("genesis::utils").def("base64_encode", [](const std::string & a0) -> std::string { return genesis::utils::base64_encode(a0); }, "", pybind11::arg("input"));
	M("genesis::utils").def("base64_encode", (std::string (*)(const std::string &, unsigned long)) &genesis::utils::base64_encode, "C++: genesis::utils::base64_encode(const std::string &, unsigned long) --> std::string", pybind11::arg("input"), pybind11::arg("line_length"));

	// genesis::utils::base64_decode_uint8(const std::string &) file: line:48
	M("genesis::utils").def("base64_decode_uint8", (class std::vector<unsigned char, class std::allocator<unsigned char> > (*)(const std::string &)) &genesis::utils::base64_decode_uint8, "C++: genesis::utils::base64_decode_uint8(const std::string &) --> class std::vector<unsigned char, class std::allocator<unsigned char> >", pybind11::arg("input"));

	// genesis::utils::base64_decode_string(const std::string &) file: line:49
	M("genesis::utils").def("base64_decode_string", (std::string (*)(const std::string &)) &genesis::utils::base64_decode_string, "C++: genesis::utils::base64_decode_string(const std::string &) --> std::string", pybind11::arg("input"));

	{ // genesis::utils::SvgPoint file:genesis/utils/formats/svg/helper.hpp line:57
		pybind11::class_<genesis::utils::SvgPoint, std::shared_ptr<genesis::utils::SvgPoint>> cl(M("genesis::utils"), "SvgPoint", "");
		cl.def( pybind11::init( [](){ return new genesis::utils::SvgPoint(); } ), "doc" );
		cl.def( pybind11::init( [](double const & a0){ return new genesis::utils::SvgPoint(a0); } ), "doc" , pybind11::arg("x"));
		cl.def( pybind11::init<double, double>(), pybind11::arg("x"), pybind11::arg("y") );

		cl.def( pybind11::init( [](genesis::utils::SvgPoint const &o){ return new genesis::utils::SvgPoint(o); } ) );
		cl.def_readwrite("x", &genesis::utils::SvgPoint::x);
		cl.def_readwrite("y", &genesis::utils::SvgPoint::y);
		cl.def("__add__", (struct genesis::utils::SvgPoint (genesis::utils::SvgPoint::*)(const struct genesis::utils::SvgPoint &) const) &genesis::utils::SvgPoint::operator+, "C++: genesis::utils::SvgPoint::operator+(const struct genesis::utils::SvgPoint &) const --> struct genesis::utils::SvgPoint", pybind11::arg("other"));
		cl.def("assign", (struct genesis::utils::SvgPoint & (genesis::utils::SvgPoint::*)(const struct genesis::utils::SvgPoint &)) &genesis::utils::SvgPoint::operator=, "C++: genesis::utils::SvgPoint::operator=(const struct genesis::utils::SvgPoint &) --> struct genesis::utils::SvgPoint &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
	}
}
