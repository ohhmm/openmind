/*
 * openmind - AI framework
 * Copyright (C) 2014  Sergej Krivonos sergeikrivonos@gmail.com
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "stdafx.h"
#include "trie.h"
#include <cstdlib>

#ifndef _MSC_VER
//constexpr EnglishTrieTraits::range_t EnglishTrieTraits::alphabet[];
#endif

template <>
std::basic_string<char> String(const char* str, std::size_t sz) {
    return std::string(str);
}

template <>
std::basic_string<wchar_t> String(const char* str, std::size_t sz) {
    if(!sz)
    {
        if(!*str)
            return std::basic_string<wchar_t>();
        else
            sz = strlen(str);
    }
    std::wstring res( sz, L'#' );
    mbstowcs( &res[0], str, sz );
    return res;
}