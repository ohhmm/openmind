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
#include <vector>
#include <memory>
#include <algorithm>


struct BaseLanguageTraits {
    typedef wchar_t char_t;
    typedef std::pair<char_t, char_t> range_t;
};

template<class T>
struct AlphabetSizeMixin : public BaseLanguageTraits {

    static constexpr std::size_t ranges_count() {
        return sizeof(decltype(T::alphabet))/sizeof(decltype(T::alphabet[0]));
    }

    static constexpr std::size_t alphabet_size() {
        return alphabet_size(ranges_count());
    }

    static constexpr std::size_t alphabet_size(int n) {
        return n ? T::alphabet[n-1].second - T::alphabet[n-1].first + alphabet_size(n-1) : 0;
    }

};

struct EnglishTrieTraits : public AlphabetSizeMixin<EnglishTrieTraits> {
    static constexpr const range_t alphabet[] = {
            { L'A',L'Z' },
            { L'a',L'z' }
    };
};

template<class ObjectT, class LangTraitsT = EnglishTrieTraits>
class Trie : public LangTraitsT
{
    typedef Trie<ObjectT, LangTraitsT> self_t;
public:
    typedef typename std::shared_ptr<self_t> ptr_t;
    typedef typename LangTraitsT::char_t    char_t;
    typedef const char_t                    const_char_t;
private:
    using LangTraitsT::ranges_count;
    using LangTraitsT::alphabet_size;
    using LangTraitsT::alphabet;
    
//    wchar_t* seq;
    wchar_t ch;
//    ptr_t root, parent;
    std::array<ptr_t, alphabet_size()> children;
	
	typedef std::shared_ptr<ObjectT> obj_ptr_t;
	obj_ptr_t object;

    ptr_t& operator[](char_t c) {
        
        // get its range
        int index = c;
        for(int rangeIdx = ranges_count(); rangeIdx--;) {
            index -= alphabet[rangeIdx].first;
            if(c >= alphabet[rangeIdx].first && c<= alphabet[rangeIdx].second)
                break;
            index += alphabet[rangeIdx].second;
        }

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
