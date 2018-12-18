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

#include <iostream>
#include "easyunit/test.h"
#include "troika_common.hpp"

//==============================================================================
//==============================================================================

using namespace easyunit;

//==============================================================================
//==============================================================================

void ai_get_pass_cards (int32_t     rule_pass_cards,
                        int32_t     rule_no_trump_bidout,
                        int32_t     rule_minimum_bid,
                        Hand        *my_cards,
                        Card        *out_cards);

//==============================================================================
//==============================================================================

TEST(pass_cards,test_pass_cards_1)
{
    Hand    my = { 0 };
    
    // Partner cards
    add_card (&my, card (CARD_5, SUIT_H));
    add_card (&my, card (CARD_T, SUIT_D));
    add_card (&my, card (CARD_K, SUIT_D));
    add_card (&my, card (CARD_Q, SUIT_D));
    add_card (&my, card (CARD_8, SUIT_C));
    add_card (&my, card (CARD_K, SUIT_H));
    add_card (&my, card (CARD_A, SUIT_H));
    add_card (&my, card (CARD_Q, SUIT_H));
    
    std::cout << std::endl << "---TEST(get_get_bid,test_simple_bid_1)---" << std::endl;
    
    Card cards[2] = {0,0};
    ai_get_pass_cards (true, RULE_NO_TRUMP_BIDOUT, RULE_MINIMUM_BID, &my, cards);
    
    ASSERT_TRUE(cards[0] != cards[1]);
    ASSERT_TRUE(cards[0] != 0);
    ASSERT_TRUE(cards[1] != 0);
}

//==============================================================================
//==============================================================================

TEST(pass_cards,test_pass_cards_2)
{
    Hand    my = { 0 };
    
    // Partner cards
    add_card (&my, card (CARD_5, SUIT_H));
    add_card (&my, card (CARD_T, SUIT_D));
    add_card (&my, card (CARD_K, SUIT_D));
    add_card (&my, card (CARD_Q, SUIT_C));
    add_card (&my, card (CARD_8, SUIT_C));
    add_card (&my, card (CARD_K, SUIT_H));
    add_card (&my, card (CARD_A, SUIT_H));
    add_card (&my, card (CARD_Q, SUIT_H));
    
    std::cout << std::endl << "---TEST(get_get_bid,test_simple_bid_1)---" << std::endl;
    
    Card cards[2] = {0,0};
    ai_get_pass_cards (true, RULE_NO_TRUMP_BIDOUT, RULE_MINIMUM_BID, &my, cards);
    
    ASSERT_TRUE(cards[0] != cards[1]);
    ASSERT_TRUE(cards[0] == card (CARD_T, SUIT_D));
    ASSERT_TRUE(cards[1] == card (CARD_K, SUIT_D));
}

//==============================================================================
//==============================================================================

TEST(pass_cards,test_pass_cards_3)
{
    Hand    my = { 0 };
    
    // Partner cards
    add_card (&my, card (CARD_5, SUIT_H));
    add_card (&my, card (CARD_T, SUIT_D));
    add_card (&my, card (CARD_K, SUIT_C));
    add_card (&my, card (CARD_Q, SUIT_C));
    add_card (&my, card (CARD_8, SUIT_C));
    add_card (&my, card (CARD_K, SUIT_H));
    add_card (&my, card (CARD_A, SUIT_H));
    add_card (&my, card (CARD_Q, SUIT_H));
    
    std::cout << std::endl << "---TEST(get_get_bid,test_simple_bid_1)---" << std::endl;
    
    Card cards[2] = {0,0};
    ai_get_pass_cards (true, RULE_NO_TRUMP_BIDOUT, RULE_MINIMUM_BID, &my, cards);
    
    ASSERT_TRUE(cards[0] != cards[1]);
    ASSERT_TRUE(cards[0] == card (CARD_T, SUIT_D));
    ASSERT_TRUE(cards[1] == card (CARD_5, SUIT_H));
}

//==============================================================================
//==============================================================================
