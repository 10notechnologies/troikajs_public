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
#include <cmath>

TrumpSuit ai_get_trump  (int32_t    rule_pass_cards,
                        int32_t     rule_no_trump_bidout,
                        int32_t     rule_minimum_bid,
                        Hand        *my_cards);

//==============================================================================
// Returns the best cards to pass. out_cards must point to array of at least
// rule_pass_cards in size
//==============================================================================

Bid ai_get_bid (int32_t     rule_pass_cards,
                int32_t     rule_no_trump_bidout,
                int32_t     rule_minimum_bid,
                Bid         bids[4],
                Hand        *my_cards,
                int32_t     my_index,
                int32_t     dealer_index)
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


    if (my_cards->num_cards != 8) {
        throw AIException("Number of cards in hand is wrong");
    }
    
    bool forced_bid = (my_index == dealer_index) && (bids[0].bid == 0) && (bids[1].bid == 0) && (bids[2].bid == 0) && (bids[3].bid == 0);
    
    // Top bid
    Bid top_bid = bids[0];
    for (uint8_t i = 1; i < 4; ++i) {
        uint32_t top_bid_val = top_bid.bid * 2 + (top_bid.trump == TRUMP_N ? 1 : 0);
        uint32_t bid_val = bids[i].bid * 2 + (bids[i].trump == TRUMP_N ? 1 : 0);

        if (bid_val > top_bid_val) {
            top_bid = bids[i];
        }
    }
    
    
    // Kevin's trick: Player across from dealer can signal dealer
    if ((dealer_index == 0 && my_index == 2) ||
        (dealer_index == 1 && my_index == 3) ||
        (dealer_index == 2 && my_index == 0) ||
        (dealer_index == 3 && my_index == 1) ) {

        // Special case having Ace or 5 of hearts
        for (uint8_t i = 0; i < my_cards->num_cards; ++i) {
            if (my_cards->cards[i] == card (CARD_5, SUIT_H) || my_cards->cards[i] == card (CARD_A, SUIT_H)) {
                Bid bid;
                bid.bid = forced_bid ? rule_minimum_bid : 0;
                bid.trump = TRUMP_UNKNOWN;
                return bid;
            }
        }
    }
    
    // Sum up different suits
    float suits[4];
    suits[SUIT_C] = weighted_sum_for_suit(SUIT_C, my_cards);
    suits[SUIT_D] = weighted_sum_for_suit(SUIT_D, my_cards);
    suits[SUIT_H] = weighted_sum_for_suit(SUIT_H, my_cards);
    suits[SUIT_S] = weighted_sum_for_suit(SUIT_S, my_cards);

    float bid_num;
    bid_num = std::floor(suits[SUIT_C] + suits[SUIT_D] + suits[SUIT_H] + suits[SUIT_S] + 0.5);

    if (bid_num > 12)
        bid_num = 12;
    else if (bid_num < rule_minimum_bid) {
        Bid bid;
        bid.bid = forced_bid ? rule_minimum_bid : 0;
        bid.trump = TRUMP_UNKNOWN;
        return bid;
    }
    
    TrumpSuit max_suit = ai_get_trump  (rule_pass_cards, rule_no_trump_bidout, rule_minimum_bid, my_cards);
    
    int32_t nums[4];
    count_suits_in_hand (my_cards, nums);

    // Kevin: If not bidding hearts and more than two hearts, pass
    if (nums[SUIT_H] >= 3 && max_suit != TRUMP_H) {
        Bid bid;
        bid.bid = forced_bid ? rule_minimum_bid : 0;
        bid.trump = TRUMP_UNKNOWN;
        return bid;
    }
    
    // Kevin: If has 5 hearts and any other hearts, pass
    if (nums[SUIT_H] >= 2 && has_card(my_cards, card(CARD_5, SUIT_H))) {
        Bid bid;
        bid.bid = forced_bid ? rule_minimum_bid : 0;
        bid.trump = TRUMP_UNKNOWN;
        return bid;
    }
    

    bool no = false;

// Removed as per kevin
//    if ((nums[SUIT_C] >= 4 && has_card(my_cards, card(CARD_A, SUIT_C)) && has_card(my_cards, card(CARD_K, SUIT_C))) ||
//        (nums[SUIT_C] >= 5 && has_card(my_cards, card(CARD_A, SUIT_C)) && has_card(my_cards, card(CARD_Q, SUIT_C))) ) {
//        no = true;
//    }
//
//    if ((nums[SUIT_D] >= 4 && has_card(my_cards, card(CARD_A, SUIT_D)) && has_card(my_cards, card(CARD_K, SUIT_D))) ||
//        (nums[SUIT_D] >= 5 && has_card(my_cards, card(CARD_A, SUIT_D)) && has_card(my_cards, card(CARD_Q, SUIT_D))) ) {
//        no = true;
//    }
//
//    if ((nums[SUIT_S] >= 4 && has_card(my_cards, card(CARD_A, SUIT_S)) && has_card(my_cards, card(CARD_K, SUIT_S))) ||
//        (nums[SUIT_S] >= 5 && has_card(my_cards, card(CARD_A, SUIT_S)) && has_card(my_cards, card(CARD_Q, SUIT_S))) ) {
//        no = true;
//    }

    if ((has_card(my_cards, card(CARD_A, SUIT_H)) && has_card(my_cards, card(CARD_K, SUIT_H))) ||
        (nums[SUIT_H] >= 4 && has_card(my_cards, card(CARD_A, SUIT_H)) && has_card(my_cards, card(CARD_Q, SUIT_H))) ) {
        no = true;
    }

    uint8_t bid_val = (uint8_t) (bid_num + 0.5F);

    // Kevin: Clamp to minimum_bid if hand not good enough
    if (!no && bid_val > rule_minimum_bid && nums[max_suit] < 3) {
        bid_val = rule_minimum_bid;
    }
    
    Bid bid;
    bid.bid = bid_val;
    bid.trump = no ? TRUMP_N : TRUMP_UNKNOWN;

    if (!forced_bid) {
        if ( (my_index == dealer_index) && (bid_val > top_bid.bid))
            bid = top_bid; // Take the bid
    } else {
        bid.bid = rule_minimum_bid;
    }

    return bid;
}

//==============================================================================
//==============================================================================
