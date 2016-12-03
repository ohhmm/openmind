//
// Created by Сергей Кривонос on 25.10.16.
//

#pragma once

#include "Context.h"

template<
        class ObjectT = Named::ptr_t,
        class LanguageTraitsT = EnglishLanguageTraits,
        class TraitsT = BaseContextTraits
>
class ContextRequirement : public Context<ObjectT, LanguageTraitsT, TraitsT> {

};

