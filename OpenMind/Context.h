//
// Created by Sergio on 13.08.2015.
//

#pragma once

#include <thread>
#include <list>

#include "trie.h"
#include "Named.h"

struct EnglishLanguageTraits : public EnglishTrieTraits {
    static constexpr const char_t sentence_delimeters[] = {
            L'.', L'!', L'?'
    };

    static constexpr const char_t word_delimeters[] = {
            L',', L':', L';', L' ', L'\t', L'\n', L'\x10'
            , L'.', L'!', L'?' //sentence_delimeters (needs array concat)
    }/* TODO : .concat(sentence_delimeters)*/;

};

/**
 * Obj
 */
struct BaseContextTraits {

};

template<
        class ObjectT = Named::ptr_t,
        class LanguageTraitsT = EnglishLanguageTraits,
        class TraitsT = BaseContextTraits
>
class Context {
    typedef Trie<ObjectT, LanguageTraitsT> words_trie_t;
    static words_trie_t Words;  // all words

    typedef typename Trie<typename words_trie_t::ptr_t, LanguageTraitsT>::ptr_t  sentence_t;
    typedef std::list<sentence_t>   sentences_t;
    static sentences_t Sentences;  // all sentences

    sentences_t sentences;

public:

    using typename words_trie_t::const_char_t;

    void Add(const_char_t* text){
        typedef std::basic_string<typename words_trie_t::char_t>        str_t;
        typedef std::basic_stringstream<typename words_trie_t::char_t>  strstr_t;

        std::thread updateGlobalWordsAndSentences([](strstr_t text) {
            typedef std::istream_iterator<str_t>  token_iterator_t;
            std::for_each(token_iterator_t(text, ",;.!? \t\n\x10"), //begin
                          token_iterator_t(), //end
                          [&](token_iterator_t it){
                              Words.AddToRoot(*it);
                          }
            );
        }, text);

        /*std::thread updateGlobalSentences([](strstr_t text) {
            // TODO : need to add ending delimiter as well (be sure to add '[!?.]+')
            //std::for_each(token_iterator_t(text, ".!?"), //begin
                          token_iterator_t(), //end
                          [&](token_iterator_t it){
                              str_t sentence = *it;
                              sentence.replace("\n", " ");
                              sentence.replace("\x10", " ");
                              sentence.replace("\t", " ");
                              sentence.erase(0, sentence.find_first_not_of(' '));       //prefixing spaces
                              sentence.erase(sentence.find_last_not_of(' ')+1);         //surfixing spaces
                              sentence.replace("  ", " ");
                              Words.AddToRoot(sentence);
                          }
            );
        }, text);*/


    }
};

