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

#include <stdint.h>
#if DEBUG_LOG
    #include <iostream>
#endif

//==============================================================================
// Returns the best cards to pass. out_cards must point to array of at least
// rule_pass_cards in size
//==============================================================================

TrumpSuit ai_get_trump  (int32_t    rule_pass_cards,
                        int32_t     rule_no_trump_bidout,
                        int32_t     rule_minimum_bid,
                        Hand        *my_cards)
{
    // Validate inputs
    if (rule_pass_cards != 0 && rule_pass_cards != 1) {
        throw AIException("rule_pass_cards out of range");
    }

    if (rule_no_trump_bidout != 52 && rule_no_trump_bidout != 62) {
        throw AIException("rule_no_trump_bidout out of range");
    }

    if (rule_minimum_bid < 5 || rule_minimum_bid >8) {
        throw AIException("rule_minimum_bid out of range");
    }


    // Count all the suits
    int32_t nums[4];
    count_suits_in_hand (my_cards, nums);
    
    // Sum up different suits
    float suits[4];
    suits[SUIT_C] = weighted_sum_for_suit(SUIT_C, my_cards);
    suits[SUIT_D] = weighted_sum_for_suit(SUIT_D, my_cards);
    suits[SUIT_H] = weighted_sum_for_suit(SUIT_H, my_cards);
    suits[SUIT_S] = weighted_sum_for_suit(SUIT_S, my_cards);

    // Kevin: Ace and other heart, can bid heart.
    if (has_card(my_cards, card(CARD_A,SUIT_H)) && nums[SUIT_H] >= 2) {
        // Do nothing
    } else {
        nums[SUIT_H] = 0;
        suits[SUIT_H] = 0.0f;
    }

    uint32_t max_i = 0;
    float max_weight_i = suits[max_i];
    for (uint32_t i = 1; i < 4; ++i) {
        if (nums[i] > nums[max_i]) {
            max_i = i;
            max_weight_i = suits[i];
        }
        else if (nums[i] == nums[max_i]) {
            if (suits[i] > max_weight_i) {
                max_i = i;
                max_weight_i = suits[i];
            }
        }
    }
    
    if (max_i == SUIT_H)        return TRUMP_H;
    else if (max_i == SUIT_D)   return TRUMP_D;
    else if (max_i == SUIT_S)   return TRUMP_S;
    else                        return TRUMP_C;
}

//==============================================================================
//==============================================================================
