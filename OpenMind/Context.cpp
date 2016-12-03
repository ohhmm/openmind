//
// Created by Sergio on 13.08.2015.
//

#include "Context.h"

#ifdef __GNUC__
#ifndef __clang__
constexpr EnglishLanguageTraits::char_t EnglishLanguageTraits::word_delimeters[];
constexpr EnglishLanguageTraits::char_t EnglishLanguageTraits::sentence_delimeters[];
#endif
#endif
