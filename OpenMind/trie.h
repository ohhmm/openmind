/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2015  <copyright holder> <email>
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

#pragma once

#include <boost/serialization/serialization.hpp>
#include <tuple>
#include <array>
#include <functional>
#include <vector>
#include <memory>
#include <algorithm>

#include <boost/mpl/accumulate.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/mpl/range_c.hpp>
#include <boost/mpl/size.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/vector_c.hpp>

struct BaseLanguageTraits {
    typedef wchar_t char_t;

    // should be a range typedef
    template<char_t Start, char_t End>
    using range_t = boost::mpl::range_c<char_t, Start, End>;
};

struct ramges_plus_mpl
{
    template <class T1, class T2>
    struct apply
    {
        typedef typename boost::mpl::plus<
			boost::mpl::size<T1>,
			boost::mpl::size<T2> >::type type;
    };
};

template<typename T>
struct AlphabetSizeMixin : public BaseLanguageTraits {

    static constexpr std::size_t alphabet_size() {
		using namespace boost::mpl;
		return accumulate<
				typename T::alphabet_ranges_t,
				int_<0>,
				plus< _1, size<_2> >
			>::type::value;
    }

};

struct EnglishTrieTraits : public AlphabetSizeMixin<EnglishTrieTraits> {

	typedef boost::mpl::vector<
		boost::mpl::range_c<char_t, L' ', L'!'>,
		boost::mpl::range_c<char_t, L'$', L'9'>,
		boost::mpl::range_c<char_t, L'A', L'Z'>,
		boost::mpl::range_c<char_t, L'a', L'z'> > alphabet_ranges_t;
    static constexpr std::size_t sz = boost::mpl::size<alphabet_ranges_t>::value;
};

template<class ObjectT, class LangTraitsT = EnglishTrieTraits>
class Trie : public LangTraitsT
{
    typedef Trie<ObjectT, LangTraitsT> self_t;
public:
    typedef typename std::shared_ptr<self_t> ptr_t;
    typedef typename LangTraitsT::char_t    char_t;
    typedef const char_t                    const_char_t;
	typedef std::basic_string<char_t>		string_t;
    using typename LangTraitsT::alphabet_ranges_t;

private:
	char_t ch;
//    ptr_t root, parent;
	using LangTraitsT::alphabet_size;
	std::array<ptr_t, alphabet_size()> children;
	
	typedef std::shared_ptr<ObjectT> obj_ptr_t;
	obj_ptr_t object;

    struct VisitRangesT{
        std::function<bool (char_t, char_t)> _f;
        bool _break = false;

        template<class U>
        void operator()(U u)
        {
            if(!_break)
                if(!_f(U::start::value, U::finish::value))
                    _break=true;
        }
    };

    ptr_t& operator[](char_t c) {

		bool isInAlphabet = false;
		int index = c;
		VisitRangesT t;
		t._f = [&](char_t first, char_t last)
			-> bool // continue enumeration
		{
            index -= first;
			if (c >= first && c <= last) {
				isInAlphabet = true;
				return false;
			}
            index += last;
            return true;
		};

		boost::mpl::for_each<alphabet_ranges_t>(t);

		if(!isInAlphabet)
			throw string_t(L"The symbol ")+c+L" is not part of the context alphabet. Ypu may want to try dynamic context instead.";

		return children[index];
    }

    Trie(const_char_t* text, obj_ptr_t object=nullptr) {
        if(*text)
        {
            ch = *text;
            ++text;
            if(*text)
                (*this)[*text].reset(new Trie(text, object));
			else
				this->object = object;
        }
    }

public:
    Trie() = default;

	void AddToRoot(const_char_t* text, obj_ptr_t object=nullptr) {
        if(*text) {
            ptr_t next = operator[](*text);
            if(static_cast<ptr_t>(next))
                next->AddToRoot(text+1, object);
            else
                operator[](*text).reset(new Trie(text, object));
        }
    }
    
    obj_ptr_t& operator[](const_char_t* key) {
		if(*key) {
			ptr_t next = operator[](*key);
			if(!static_cast<ptr_t>(next))
				operator[](*key).reset(new Trie(key));
			return next->operator[](key+1);
		} else {
			return object;
		}
	}

    void Remove(const_char_t* text){

    }

};
