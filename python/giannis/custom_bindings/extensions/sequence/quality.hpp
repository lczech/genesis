#ifndef CUSTOM_BINDING_QUALITY_INCLUDED_
#define CUSTOM_BINDING_QUALITY_INCLUDED_

#include "genesis/sequence/functions/quality.hpp"

inline void extend_quality(pybind11::module &handle)
{
	handle.def(
		"quality_decode_to_phred_score",
		[](std::string quality_codes)
		{
			return ::genesis::sequence::quality_decode_to_phred_score(quality_codes);
		},
		pybind11::arg("quality_codes")
	);

	handle.def(
		"quality_decode_to_phred_score",
		[](std::string quality_codes, ::genesis::sequence::QualityEncoding encoding)
		{
			return ::genesis::sequence::quality_decode_to_phred_score(quality_codes, encoding);
		},
		pybind11::arg("quality_codes"),
		pybind11::arg("encoding")
	);
}

#endif
