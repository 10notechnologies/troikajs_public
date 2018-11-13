//==============================================================================
//
// Copyright (c) 2015 Smells Like Donkey Software Inc.  All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS `AS IS'
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
//==============================================================================

#include "troika_common.hpp"
#include <iostream>
#include <sstream>

#ifdef linux
    #include <Python.h>
#else
    #include <Python/Python.h>
#endif

#define EXPORT __attribute__((visibility("default")))

static PyObject *gExcept;

//==============================================================================
//==============================================================================

void parse_cards_list (PyObject *cards_obj, Hand &hand)
{
    PyObject *iter = PyObject_GetIter(cards_obj);
    if (!iter) {
        throw AIException("cards_obj not iterable");
    }

    while (true) {
        PyObject *next = PyIter_Next(iter);
        if (!next)
            break;

        // If list element is not a string then abort
        if (!PyString_Check(next) && !PyUnicode_Check(next)) {
            throw AIException("List element not a string");
        }

        char *str = PyString_AsString(next);
                
        // Add the card to the hand
        Card card = chars_to_card(str);
        add_card(&hand, card);
    }
}

void parse_bids_list (PyObject *bids_obj, Bid bids[4])
{
    PyObject *iter = PyObject_GetIter(bids_obj);
    if (!iter) {
        throw AIException("bids_obj not iterable");
    }
    
    for (int32_t b = 0; b < 4; ++b) {
        bids[b].bid = 0;
        bids[b].trump = TRUMP_UNKNOWN;
    }

    int32_t bid_index = 0;
    while (true) {
        PyObject *next = PyIter_Next(iter);
        if (!next)
            break;
            
        // If list element is none then skip
        if (next == Py_None)
            continue;

        // If list element is not a string then abort
        if (!PyString_Check(next) && !PyUnicode_Check(next)) {
            throw AIException("List element not a string");
        }

        // Too many bids
        if (bid_index > 3) {
            throw AIException("Too many bids");
        }

        char *str = PyString_AsString(next);
                
        // Add the card to the hand
        bids[bid_index] = chars_to_bid(str);
        ++bid_index;
    }
}

//==============================================================================
// in:  int32_t     (rule_pass_cards), 
//      int32_t     (rule_no_trump_bidout), 
//      [string]    (my_cards)
// out: [string]
//==============================================================================

void ai_get_pass_cards  (int32_t    rule_pass_cards,
                        int32_t     rule_no_trump_bidout,
                        int32_t     rule_minimum_bid,
                        Hand        *my_cards,
                        Card        *out_cards);

static PyObject* plugin_ai_get_pass_cards(PyObject *self, PyObject *args)
{
    try {
    
        int32_t rule_pass_cards;
        int32_t rule_no_trump_bidout;
        int32_t rule_minimum_bid;
        PyObject *my_cards_obj;
    
        Hand my_cards;
        my_cards.num_cards = 0;

        //
        // Parse arguments
        //
    
        if (!PyArg_ParseTuple(args, "iiiO", &rule_pass_cards, &rule_no_trump_bidout, &rule_minimum_bid, &my_cards_obj)) {
            throw AIException("PyArg_ParseTuple failed");
        }
                
        // Parse cards
        parse_cards_list (my_cards_obj, my_cards);
    
        //
        // Call function
        //
    
        Card out_cards[2];
        ai_get_pass_cards (rule_pass_cards, rule_no_trump_bidout, rule_minimum_bid, &my_cards, out_cards);

        //
        // Build result list
        //

        PyObject *results = PyList_New(2);
    
        for (int32_t i = 0; i < 2; ++i) {
            char cs[2];
            card_to_chars (out_cards[i], cs);
    
            PyObject *item = PyString_FromStringAndSize(cs, 2);
            PyList_SetItem(results, i, item);
        }
    
        return results;
        
    }
    catch (AIException &e) {
        PyErr_SetString(gExcept, e.what());
        return NULL;
    }
}

//==============================================================================
// in:  int32_t     (rule_pass_cards), 
//      int32_t     (rule_no_trump_bidout), 
//      [string]    (my_cards)
// out: [string]
//==============================================================================

Bid ai_get_bid  (int32_t    rule_pass_cards,
                int32_t     rule_no_trump_bidout,
                int32_t     rule_minimum_bid,
                Bid         bids[4],
                Hand        *my_cards,
                int32_t     my_index,
                int32_t     dealer_index);

static PyObject* plugin_ai_get_bid(PyObject *self, PyObject *args)
{
    try {

        int32_t rule_pass_cards;
        int32_t rule_no_trump_bidout;
        int32_t rule_minimum_bid;
        PyObject *bids_obj;
        PyObject *my_cards_obj;
        int32_t my_index;
        int32_t dealer_index;

        Bid bids[4];
        Hand my_cards;
        my_cards.num_cards = 0;

        //
        // Parse arguments
        //
    
        if (!PyArg_ParseTuple(args, "iiiOOii", &rule_pass_cards, &rule_no_trump_bidout, &rule_minimum_bid, &bids_obj, &my_cards_obj, &my_index, &dealer_index)) {
            throw AIException("PyArg_ParseTuple failed");
        }
                
        // Parse cards
        parse_cards_list (my_cards_obj, my_cards);
        parse_bids_list (bids_obj, bids);

        //
        // Call function
        //
    
        Bid bid = ai_get_bid (rule_pass_cards, rule_no_trump_bidout, rule_minimum_bid, bids, &my_cards, my_index, dealer_index);

        //
        // Build result string
        //
        
        std::string bid_str;
        if (bid.bid == 0) {
            bid_str = "pass";
        } else {
            std::stringstream ss;
            ss << (int) bid.bid;
        
            bid_str = ss.str();
            if (bid.trump == TRUMP_N) {
                bid_str += "N";
            }
        }
    
        PyObject *result = PyString_FromStringAndSize(bid_str.c_str(), bid_str.size());
        return result;

    }
    catch (AIException &e) {
        PyErr_SetString(gExcept, e.what());
        return NULL;
    }
}

//==============================================================================
// in:  int32_t     (rule_pass_cards), 
//      int32_t     (rule_no_trump_bidout), 
//      [string]    (my_cards)
// out: [string]
//==============================================================================

TrumpSuit ai_get_trump  (int32_t    rule_pass_cards,
                        int32_t     rule_no_trump_bidout,
                        int32_t     rule_minimum_bid,
                        Hand        *my_cards);

static PyObject* plugin_ai_get_trump(PyObject *self, PyObject *args)
{
    try {
        int32_t rule_pass_cards;
        int32_t rule_no_trump_bidout;
        int32_t rule_minimum_bid;
        PyObject *my_cards_obj;
    
        Hand my_cards;
        my_cards.num_cards = 0;

        //
        // Parse arguments
        //
    
        if (!PyArg_ParseTuple(args, "iiiO", &rule_pass_cards, &rule_no_trump_bidout, &rule_minimum_bid, &my_cards_obj)) {
            throw AIException("PyArg_ParseTuple failed");
        }
                
        // Parse cards
        parse_cards_list (my_cards_obj, my_cards);
    
        //
        // Call function
        //
    
        TrumpSuit trump = ai_get_trump (rule_pass_cards, rule_no_trump_bidout, rule_minimum_bid, &my_cards);

        //
        // Build result list
        //
    
        char ts[2] = {'0','0'};
        trump_to_char (trump, &ts[1]);
        
        PyObject *result = PyString_FromStringAndSize(ts, 2);
        return result;
        
    }
    catch (AIException &e) {
        PyErr_SetString(gExcept, e.what());
        return NULL;
    }
}

//==============================================================================
// in:  int32_t     (rule_pass_cards), 
//      int32_t     (rule_no_trump_bidout), 
//      [string]    (my_cards)
// out: [string]
//==============================================================================

Card ai_get_play_cards  (int32_t    rule_pass_cards,
                        int32_t     rule_no_trump_bidout,
                        int32_t     rule_minimum_bid,
                        int32_t     highest_bidder,
                        Bid         highest_bid,
                        Hand        *p0_played_cards,
                        Hand        *p1_played_cards,
                        Hand        *p2_played_cards,
                        Hand        *p3_played_cards,
                        Hand        *my_cards,
                        int32_t     my_index);

static PyObject* plugin_ai_get_play_cards(PyObject *self, PyObject *args)
{
    try {
        int32_t rule_pass_cards;
        int32_t rule_no_trump_bidout;
        int32_t rule_minimum_bid;
        int32_t highest_bidder;
        char *highest_bid_str;
        PyObject *p0_played_cards_obj;
        PyObject *p1_played_cards_obj;
        PyObject *p2_played_cards_obj;
        PyObject *p3_played_cards_obj;
        PyObject *my_cards_obj;
        int32_t my_index;

        // Hands
        Hand p0_played_cards;
        p0_played_cards.num_cards = 0;

        Hand p1_played_cards;
        p1_played_cards.num_cards = 0;

        Hand p2_played_cards;
        p2_played_cards.num_cards = 0;

        Hand p3_played_cards;
        p3_played_cards.num_cards = 0;

        Hand my_cards;
        my_cards.num_cards = 0;

        //
        // Parse arguments
        //
    
        if (!PyArg_ParseTuple(args, "iiiisOOOOOi", 
                &rule_pass_cards, 
                &rule_no_trump_bidout, 
                &rule_minimum_bid, 
                &highest_bidder,
                &highest_bid_str,
                &p0_played_cards_obj,
                &p1_played_cards_obj,
                &p2_played_cards_obj,
                &p3_played_cards_obj,
                &my_cards_obj,
                &my_index)) {
            throw AIException("PyArg_ParseTuple failed");
        }
        
        // Parse cards
//         std::cout << " A" << std::endl;
        parse_cards_list (p0_played_cards_obj, p0_played_cards);
//         std::cout << " B" << std::endl;
        parse_cards_list (p1_played_cards_obj, p1_played_cards);
//         std::cout << " C" << std::endl;
        parse_cards_list (p2_played_cards_obj, p2_played_cards);
//         std::cout << " D" << std::endl;
        parse_cards_list (p3_played_cards_obj, p3_played_cards);
//         std::cout << " E" << std::endl;
        parse_cards_list (my_cards_obj, my_cards);
//         std::cout << " F" << std::endl;

        Bid highest_bid = chars_to_bid(highest_bid_str);
        
//         std::cout << "rule_pass_cards: " << rule_pass_cards << std::endl;
//         std::cout << "rule_no_trump_bidout: " << rule_no_trump_bidout << std::endl;
//         std::cout << "rule_minimum_bid: " << rule_minimum_bid << std::endl;
//         std::cout << "highest_bidder: " << highest_bidder << std::endl;
//         std::cout << "my_index: " << my_index << std::endl;
        
        // Call the function
        Card out_card = ai_get_play_cards   (rule_pass_cards, rule_no_trump_bidout, rule_minimum_bid,
                                            highest_bidder, highest_bid,
                                            &p0_played_cards, &p1_played_cards, 
                                            &p2_played_cards, &p3_played_cards, 
                                            &my_cards, my_index);
          
        // Build result string
        char c[2];                           
        card_to_chars (out_card, c);
       
        PyObject *result = PyString_FromStringAndSize(c, 2);
        return result;

    }
    catch (AIException &e) {
        PyErr_SetString(gExcept, e.what());
        return NULL;
    }

}

//==============================================================================
//==============================================================================

static PyMethodDef PluginMethods[] = {
    {"ai_get_pass_cards",   plugin_ai_get_pass_cards,   METH_VARARGS, "AI Pass Cards."},
    {"ai_get_bid",          plugin_ai_get_bid,          METH_VARARGS, "AI Get Bid."},
    {"ai_get_trump",        plugin_ai_get_trump,        METH_VARARGS, "AI Get Trump."},
    {"ai_get_play_cards",   plugin_ai_get_play_cards,   METH_VARARGS, "AI Get Play Cards."},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

#ifdef linux
extern "C" void inittroika_ai(void)
#else
EXPORT PyMODINIT_FUNC inittroika_ai(void)
#endif
{
    PyObject *m;

    m = Py_InitModule("troika_ai", PluginMethods);
    if (m == NULL)
        return;
        
    gExcept = PyErr_NewException("troika_ai.error", NULL, NULL);
    Py_INCREF(gExcept);
    PyModule_AddObject(m, "error", gExcept);

}

//==============================================================================
//==============================================================================



