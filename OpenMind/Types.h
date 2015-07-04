/*
 * <one line to give the library's name and an idea of what it does.>
 * Copyright (C) 2015  <copyright holder> <email>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#pragma once

#include "trie.h"
#include <vector>

class Type {

public:
	typedef wchar_t char_t;
	typedef std::shared_ptr<Type> 	ptr_t;
	typedef std::vector<ptr_t> 		collection_t;

	const char_t* type_id;
	const collection_t& construction_params;
	
	Type(const char_t* id, const collection_t& construction_params)
	: type_id(id)
	, construction_params(construction_params)
	{
	
		
	}
	
};


class Types
{
	Trie<Type> types;
	
	Types();
	~Types();
public:
	Types& Instance() {
		static Types types;
		return types;
	}
	
	void Register(Type::ptr_t type){
	
		types[type->type_id] = type;
	}

	Type::ptr_t& operator[](const Type::char_t* id) {
		return types[id];
	}

	Type::ptr_t& operator[](const Type& type) {
		return types[type.type_id];
	}

};

