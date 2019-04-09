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

#include <emscripten/bind.h>
//#include <emscripten/emscripten.h>

#include "troika_common.hpp"
#include <iostream>
#include <sstream>
#include <vector>
#include <string>

using namespace emscripten;

//==============================================================================
//==============================================================================

std::string card_to_string (Card c)
{
#if DEBUG_LOG
    std::cout << "card_to_string: " << (int) c << std::endl;
#endif

    char out_chars[3] = {0};
    card_to_chars(c, out_chars);
    return out_chars;
}

std::string bid_to_string (Bid bid)
{
    if (bid.bid == 0)
        return "pass";
    
    std::string s;
    s = std::to_string(bid.bid);
    
    if (bid.trump != TRUMP_UNKNOWN) {
        char trump;
        trump_to_char(bid.trump, &trump);
        s = s + trump;
    }
    
    return s;
}


void parse_cards_list(std::vector<std::string> cards_sv, Hand *hand)
{
#if DEBUG_LOG
    std::cout << "parse_cards_list" << std::endl;
#endif
 
    for (auto &c : cards_sv) {
    
#if DEBUG_LOG
        std::cout << c << std::endl;
#endif

        if (c.size() != 2)
            throw AIException("parse_cards_list failed");
    
        // Add the card to the hand
        Card card = chars_to_card(c.data());
        add_card(hand, card);
    }
    
#if DEBUG_LOG
    std::cout << "parse_cards_list DONE" << std::endl;
#endif
}

void parse_bids_list(std::vector<std::string> bids_sv, Bid bids[4])
{
#if DEBUG_LOG
    std::cout << "parse_bids_list" << std::endl;
#endif

    if (bids_sv.size() != 4)
        throw AIException("parse_bids_list failed");

    for (uint8_t i = 0; i < 4; ++i) {
    
#if DEBUG_LOG
        std::cout << bids_sv[i] << std::endl;
#endif
    
        Bid bid = chars_to_bid(bids_sv[i].data());
        bids[i] = bid;
    }
    
#if DEBUG_LOG
    std::cout << "parse_bids_list DONE" << std::endl;
#endif
}

//==============================================================================
//==============================================================================

void ai_get_pass_cards (int32_t     rule_pass_cards,
                        int32_t     rule_no_trump_bidout,
                        int32_t     rule_minimum_bid,
                        Hand        *my_cards,
                        Card        *out_cards);

std::vector<std::string> plugin_ai_get_pass_cards  (int32_t rule_pass_cards,
                                                    int32_t rule_no_trump_bidout,
                                                    int32_t rule_minimum_bid,
                                                    std::vector<std::string> my_cards_sv)
{
#if DEBUG_LOG
    std::cout << "ENTER plugin_ai_get_pass_cards" << std::endl;
#endif

    Hand my_cards = { 0 };
    
    parse_cards_list(my_cards_sv, &my_cards);
    Card out_cards[2] = { 0 };
    
    try {
        ai_get_pass_cards (rule_pass_cards, rule_no_trump_bidout, rule_minimum_bid, &my_cards, out_cards);
    }
    catch (std::exception &e) {
#if DEBUG_LOG
        std::cout << "AI EXCEPTION: " << e.what();
#endif
    }

    std::vector<std::string> result;
    result.push_back(card_to_string(out_cards[0]));
    result.push_back(card_to_string(out_cards[1]));

#if DEBUG_LOG
    std::cout << "EXIT plugin_ai_get_pass_cards" << std::endl;
#endif
    return result;
}

//==============================================================================
//==============================================================================

Bid ai_get_bid (int32_t     rule_pass_cards,
                int32_t     rule_no_trump_bidout,
                int32_t     rule_minimum_bid,
                Bid         bids[4],
                Hand        *my_cards,
                int32_t     my_index,
                int32_t     dealer_index);

std::string plugin_ai_get_bid(  int32_t rule_pass_cards,
                                int32_t rule_no_trump_bidout,
                                int32_t rule_minimum_bid,
                                std::vector<std::string> bids_sv,
                                std::vector<std::string> my_cards_sv,
                                int32_t my_index,
                                int32_t dealer_index)
{
#if DEBUG_LOG
    std::cout << "ENTER plugin_ai_get_bid" << std::endl;
#endif

    Hand my_cards = { 0 };
    Bid bids[4];
    
    parse_cards_list(my_cards_sv, &my_cards);
    parse_bids_list(bids_sv, bids);

    Bid bid;
    
    try {
        bid = ai_get_bid (rule_pass_cards, rule_no_trump_bidout, rule_minimum_bid, bids, &my_cards, my_index, dealer_index);
    }
    catch (std::exception &e) {
#if DEBUG_LOG
        std::cout << "AI EXCEPTION: " << e.what();
#endif
    }

#if DEBUG_LOG
    std::cout << "EXIT plugin_ai_get_bid" << std::endl;
#endif
    return bid_to_string(bid);
}

//==============================================================================
//==============================================================================

TrumpSuit ai_get_trump (int32_t     rule_pass_cards,
                        int32_t     rule_no_trump_bidout,
                        int32_t     rule_minimum_bid,
                        Hand        *my_cards);

std::string plugin_ai_get_trump(int32_t rule_pass_cards,
                                int32_t rule_no_trump_bidout,
                                int32_t rule_minimum_bid,
                                std::vector<std::string> my_cards_sv)
{
#if DEBUG_LOG
    std::cout << "ENTER plugin_ai_get_trump" << std::endl;
#endif

    Hand my_cards = { 0 };
    parse_cards_list(my_cards_sv, &my_cards);

    TrumpSuit trump = TRUMP_UNKNOWN;
    
    try {
        trump = ai_get_trump (rule_pass_cards, rule_no_trump_bidout, rule_minimum_bid, &my_cards);
    }
    catch (std::exception &e) {
#if DEBUG_LOG
        std::cout << "AI EXCEPTION: " << e.what();
#endif
    }

    char trump_char[2] = {0};
    trump_to_char (trump, trump_char);

#if DEBUG_LOG
    std::cout << "EXIT plugin_ai_get_trump" << std::endl;
#endif
    return std::string(trump_char);
}

//==============================================================================
//==============================================================================

Card ai_get_play_cards (int32_t     rule_pass_cards,
                        int32_t     rule_no_trump_bidout,
                        int32_t     rule_minimum_bid,
    
                        int32_t     highest_bidder,
                        Bid         highest_bid,

                        Hand        *p0_played_cards,
                        Hand        *p1_played_cards,
                        Hand        *p2_played_cards,
                        Hand        *p3_played_cards,
                        Hand        *my_passed_cards,
                        
                        Hand        *my_cards,
                        int32_t     my_index);

std::string plugin_ai_get_play_cards(   int32_t rule_pass_cards,
                                        int32_t rule_no_trump_bidout,
                                        int32_t rule_minimum_bid,
                                        int32_t highest_bidder,
                                        std::string highest_bid_s,
                                        std::vector<std::string> p0_played_cards_sv,
                                        std::vector<std::string> p1_played_cards_sv,
                                        std::vector<std::string> p2_played_cards_sv,
                                        std::vector<std::string> p3_played_cards_sv,
                                        std::vector<std::string> my_passed_cards_sv,
                                        std::vector<std::string> my_cards_sv,
                                        int32_t my_index)
{
#if DEBUG_LOG
    std::cout << "ENTER plugin_ai_get_play_cards" << std::endl;
#endif

    Hand p0_played_cards = { 0 };
    parse_cards_list(p0_played_cards_sv, &p0_played_cards);

    Hand p1_played_cards = { 0 };
    parse_cards_list(p1_played_cards_sv, &p1_played_cards);

    Hand p2_played_cards = { 0 };
    parse_cards_list(p2_played_cards_sv, &p2_played_cards);

    Hand p3_played_cards = { 0 };
    parse_cards_list(p3_played_cards_sv, &p3_played_cards);

    Hand my_passed_cards = { 0 };
    parse_cards_list(my_passed_cards_sv, &my_passed_cards);

    Hand my_cards = { 0 };
    parse_cards_list(my_cards_sv, &my_cards);


    Bid highest_bid = chars_to_bid(highest_bid_s.data());
    
    Card card;
    try {
        card = ai_get_play_cards (  rule_pass_cards, rule_no_trump_bidout, rule_minimum_bid,
                                    highest_bidder, highest_bid,
                                    &p0_played_cards, &p1_played_cards, &p2_played_cards, &p3_played_cards,
                                    &my_passed_cards, &my_cards, my_index );
    }
    catch (std::exception &e) {
#if DEBUG_LOG
        std::cout << "AI EXCEPTION: " << e.what();
#endif
    }

    char out_chars[3] = {0};
    card_to_chars(card, out_chars);
    
#if DEBUG_LOG
    std::cout << "EXIT plugin_ai_get_play_cards" << std::endl;
#endif
    return out_chars;

}


//==============================================================================
//==============================================================================

EMSCRIPTEN_BINDINGS(aicpp) {
    function("ai_get_pass_cards", &plugin_ai_get_pass_cards);
    function("ai_get_bid", &plugin_ai_get_bid);
    function("ai_get_trump", &plugin_ai_get_trump);
    function("ai_get_play_cards", &plugin_ai_get_play_cards);

    register_vector<std::string>("StringArray");
}

//==============================================================================
//==============================================================================
