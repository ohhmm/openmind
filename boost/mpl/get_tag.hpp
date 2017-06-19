#pragma once

#define BOOST_MPL_GET_TAG_DEF(TAG) \
	template<typename T> \
	struct get_##TAG { \
		using type = typename T::TAG; \
	};
