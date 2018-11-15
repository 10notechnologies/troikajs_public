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

Card ai_get_play_cards (int32_t     rule_pass_cards,
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

//==============================================================================
//==============================================================================

TEST(get_play_cards,test_crash_wrong_turn)
{

    Hand    p0 =        {   5, {    card (CARD_J, SUIT_H),
                                    card (CARD_8, SUIT_H),
                                    card (CARD_8, SUIT_C),
                                    card (CARD_Q, SUIT_C),
                                    card (CARD_7, SUIT_C)       }   },
            p1 =        {   4, {    card (CARD_K, SUIT_H),
                                    card (CARD_9, SUIT_H),
                                    card (CARD_3, SUIT_S),
                                    card (CARD_8, SUIT_S)      }   },
            p2 =        {   4, {    card (CARD_8, SUIT_D),
                                    card (CARD_9, SUIT_S),
                                    card (CARD_T, SUIT_S),
                                    card (CARD_T, SUIT_D)   }   },
            p3 =        {   4, {    card (CARD_T, SUIT_H),
                                    card (CARD_Q, SUIT_H),
                                    card (CARD_A, SUIT_H),
                                    card (CARD_5, SUIT_H)      }   },
            my_cards =  {   4, {    card (CARD_Q, SUIT_S),
                                    card (CARD_K, SUIT_C),
                                    card (CARD_9, SUIT_C),
                                    card (CARD_J, SUIT_C)       }   };

    std::cout << std::endl << "---TEST(get_play_cards,test_crash_wrong_turn)---" << std::endl;

    Bid bid;
    bid.bid = 12;
    bid.trump = TRUMP_N;

    try {
        Card c = ai_get_play_cards (0,
                                    52,
                                    7,
                                    0,
                                    bid,
                                    &p0,
                                    &p1,
                                    &p2,
                                    &p3,
                                    &my_cards,
                                    0);
        std::cout << " AI Picked card: " << card_name(c) << std::endl;
        ASSERT_EQUALS(c, card (CARD_T, SUIT_D));
    }
    catch (...) {
    }

}

//==============================================================================
//==============================================================================

TEST(get_play_cards,test_crash_javascript)
{

    Hand    p0 =        {   5, {    card (CARD_A, SUIT_C),
                                    card (CARD_K, SUIT_S),
                                    card (CARD_J, SUIT_D),
                                    card (CARD_K, SUIT_H),
                                    card (CARD_Q, SUIT_H)       }   },
            p1 =        {   4, {    card (CARD_8, SUIT_C),
                                    card (CARD_3, SUIT_S),
                                    card (CARD_7, SUIT_D),
                                    card (CARD_T, SUIT_D)      }   },
            p2 =        {   4, {    card (CARD_9, SUIT_C),
                                    card (CARD_J, SUIT_S),
                                    card (CARD_8, SUIT_D),
                                    card (CARD_8, SUIT_H)   }   },
            p3 =        {   4, {    card (CARD_J, SUIT_C),
                                    card (CARD_A, SUIT_S),
                                    card (CARD_A, SUIT_D),
                                    card (CARD_Q, SUIT_D)      }   },
            my_cards =  {   4, {    card (CARD_A, SUIT_H),
                                    card (CARD_T, SUIT_S),
                                    card (CARD_Q, SUIT_C),
                                    card (CARD_K, SUIT_C)       }   };

    std::cout << std::endl << "---TEST(get_play_cards,test_crash_wrong_turn)---" << std::endl;

    Bid bid;
    bid.bid = 7;
    bid.trump = TRUMP_H;

    try {
        Card c = ai_get_play_cards (0,
                                    62,
                                    7,
                                    0,
                                    bid,
                                    &p0,
                                    &p1,
                                    &p2,
                                    &p3,
                                    &my_cards,
                                    1);
        std::cout << " AI Picked card: " << card_name(c) << std::endl;
        ASSERT_EQUALS(c, card (CARD_A, SUIT_H));
    }
    catch (...) {
    }

}

//==============================================================================
//==============================================================================

TEST(get_play_cards,test_simple_1)
{

    Hand    p0 = { 0 },
            p1 = { 0 },
            p2 = { 0 },
            p3 = { 0 },
            my = { 0 };

    add_card (&my, card (CARD_5, SUIT_H));
    add_card (&my, card (CARD_J, SUIT_D));
    add_card (&my, card (CARD_T, SUIT_C));
    add_card (&my, card (CARD_9, SUIT_H));
    add_card (&my, card (CARD_8, SUIT_C));
    add_card (&my, card (CARD_K, SUIT_H));
    add_card (&my, card (CARD_A, SUIT_H));
    add_card (&my, card (CARD_Q, SUIT_H));

    std::cout << std::endl << "---TEST(get_play_cards,test_simple_1)---" << std::endl;
    std::cout << " Player hand: " << hand_to_string(&my) << std::endl;

    Bid bid;
    bid.bid = 0;
    bid.trump = TRUMP_N;

    Card c = ai_get_play_cards (RULE_PASS_CARDS, RULE_NO_TRUMP_BIDOUT, RULE_MINIMUM_BID, 0, bid, &p0, &p1, &p2, &p3, &my, 0);

    std::cout << " AI Picked card: " << card_name(c) << std::endl;
    ASSERT_EQUALS(c, card (CARD_A, SUIT_H));
}

//==============================================================================
//==============================================================================

TEST(get_play_cards,test_simple_1_trump)
{

    Hand    p0 = { 0 },
            p1 = { 0 },
            p2 = { 0 },
            p3 = { 0 },
            my = { 0 };

    add_card (&my, card (CARD_5, SUIT_H));
    add_card (&my, card (CARD_J, SUIT_D));
    add_card (&my, card (CARD_T, SUIT_C));
    add_card (&my, card (CARD_9, SUIT_H));
    add_card (&my, card (CARD_8, SUIT_C));
    add_card (&my, card (CARD_K, SUIT_H));
    add_card (&my, card (CARD_A, SUIT_H));
    add_card (&my, card (CARD_Q, SUIT_H));

    std::cout << std::endl << "---TEST(get_play_cards,test_simple_1_trump)---" << std::endl;
    std::cout << " Player hand: " << hand_to_string(&my) << std::endl;

    Bid bid;
    bid.bid = 0;
    bid.trump = TRUMP_N;

    Card c = ai_get_play_cards (RULE_PASS_CARDS, RULE_NO_TRUMP_BIDOUT, RULE_MINIMUM_BID, 0, bid, &p0, &p1, &p2, &p3, &my, 0);

    std::cout << " AI Picked card: " << card_name(c) << std::endl;
    //ASSERT_EQUALS(c, card (CARD_Q, SUIT_H));

}

//==============================================================================
//==============================================================================

TEST(get_play_cards,test_simple_partner_doesnt_outplay)
{

    Hand    p0 = { 0 },
            p1 = { 0 },
            p2 = { 0 },
            p3 = { 0 },
            partner = { 0 };

    // Setup hands
    add_card (&p0, card (CARD_K, SUIT_D));
    add_card (&p1, card (CARD_J, SUIT_D));

    // Partner cards
    add_card (&partner, card (CARD_5, SUIT_H));
    add_card (&partner, card (CARD_T, SUIT_D));
    add_card (&partner, card (CARD_A, SUIT_D));
    add_card (&partner, card (CARD_9, SUIT_H));
    add_card (&partner, card (CARD_8, SUIT_C));
    add_card (&partner, card (CARD_K, SUIT_H));
    add_card (&partner, card (CARD_A, SUIT_H));
    add_card (&partner, card (CARD_Q, SUIT_H));

    std::cout << std::endl << "---TEST(get_play_cards,test_simple_partner_doesnt_outplay)---" << std::endl;
    std::cout << " Partner hand: " << hand_to_string(&partner) << std::endl;

    Bid bid;
    bid.bid = 0;
    bid.trump = TRUMP_N;

    Card c = ai_get_play_cards (RULE_PASS_CARDS, RULE_NO_TRUMP_BIDOUT, RULE_MINIMUM_BID, 0, bid, &p0, &p1, &p2, &p3, &partner, 2);

    std::cout << " AI Picked card: " << card_name(c) << std::endl;
    ASSERT_EQUALS(c, card (CARD_T, SUIT_D));
}

//==============================================================================
//==============================================================================

TEST(get_play_cards,test_throws_worst_if_losing)
{

    Hand    p0 = { 0 },
            p1 = { 0 },
            p2 = { 0 },
            p3 = { 0 },
            partner = { 0 };

    // Setup hands
    add_card (&p0, card (CARD_J, SUIT_D));
    add_card (&p1, card (CARD_A, SUIT_D));

    // Partner cards
    add_card (&partner, card (CARD_5, SUIT_H));
    add_card (&partner, card (CARD_T, SUIT_D));
    add_card (&partner, card (CARD_K, SUIT_D));
    add_card (&partner, card (CARD_Q, SUIT_D));
    add_card (&partner, card (CARD_8, SUIT_C));
    add_card (&partner, card (CARD_K, SUIT_H));
    add_card (&partner, card (CARD_A, SUIT_H));
    add_card (&partner, card (CARD_Q, SUIT_H));

    std::cout << std::endl << "---TEST(get_play_cards,test_throws_worst_if_losing)---" << std::endl;
    std::cout << " Partner hand: " << hand_to_string(&partner) << std::endl;

    Bid bid;
    bid.bid = 0;
    bid.trump = TRUMP_N;

    Card c = ai_get_play_cards (RULE_PASS_CARDS, RULE_NO_TRUMP_BIDOUT, RULE_MINIMUM_BID, 0, bid, &p0, &p1, &p2, &p3, &partner, 2);

    std::cout << " AI Picked card: " << card_name(c) << std::endl;
    ASSERT_EQUALS(c, card (CARD_T, SUIT_D));
}

//==============================================================================
//==============================================================================

//TEST(get_play_cards,test_trump_not_with_3)
//{
//
//    Hand    p0 =        {   2,
//                            {
//                                card (CARD_K, SUIT_H),
//                                card (CARD_8, SUIT_H),
//
//                            }
//                        },
//
//            p1 =        {   1,
//                            {
//                                card (CARD_Q, SUIT_H),
//
//                            }
//                        },
//            p2 =        {   1,
//                            {
//                                card (CARD_9, SUIT_H),
//
//                            }
//                        },
//            p3 =        {   1,
//                            {
//                                card (CARD_J, SUIT_H),
//
//                            }
//                        },
//            my_cards =  {   7,
//                            {
//                                card (CARD_8, SUIT_D),
//                                card (CARD_9, SUIT_C),
//                                card (CARD_J, SUIT_C),
//                                card (CARD_8, SUIT_C),
//                                card (CARD_T, SUIT_C),
//                                card (CARD_9, SUIT_D),
//                                card (CARD_3, SUIT_S),
//                            }
//                        };
//
//    std::cout << std::endl << "---TEST(get_play_cards,test_trump_not_with_3)---" << std::endl;
//
//    Bid bid;
//    bid.bid = 7;
//    bid.trump = TRUMP_S;
//
//    Card c = ai_get_play_cards (false,
//                                52,
//                                7,
//                                0,
//                                bid,
//                                &p0,
//                                &p1,
//                                &p2,
//                                &p3,
//                                &my_cards,
//                                1);
//    std::cout << " AI Picked card: " << card_name(c) << std::endl;
//    ASSERT_EQUALS(c, card (CARD_8, SUIT_D));
//}

//==============================================================================
//==============================================================================

TEST(get_play_cards,test_play_ace_first_if_trump)
{

    Hand    p0 =        {   0,
                            {
                            }
                        },

            p1 =        {   0,
                            {
                            }
                        },
            p2 =        {   0,
                            {
                            }
                        },
            p3 =        {   0,
                            {
                            }
                        },
            my_cards =  {   7,
                            {
                                card (CARD_A, SUIT_H),
                                card (CARD_K, SUIT_H),
                                card (CARD_A, SUIT_C),
                                card (CARD_A, SUIT_D),
                                card (CARD_A, SUIT_S),
                                card (CARD_T, SUIT_C),
                                card (CARD_9, SUIT_D),
                                card (CARD_3, SUIT_S),
                            }
                        };

    std::cout << std::endl << "---TEST(get_play_cards,test_play_ace_first_if_trump)---" << std::endl;

    Bid bid;
    bid.bid = 7;
    bid.trump = TRUMP_D;

    Card c = ai_get_play_cards (false,
                                52,
                                7,
                                0,
                                bid,
                                &p0,
                                &p1,
                                &p2,
                                &p3,
                                &my_cards,
                                0);
    std::cout << " AI Picked card: " << card_name(c) << std::endl;
    ASSERT_EQUALS(c, card (CARD_A, SUIT_H));
}

//==============================================================================
//==============================================================================

TEST(get_play_cards,test_play_ace_first_if_trump_hearts)
{

    Hand    p0 =        {   0,
                            {
                            }
                        },

            p1 =        {   0,
                            {
                            }
                        },
            p2 =        {   0,
                            {
                            }
                        },
            p3 =        {   0,
                            {
                            }
                        },
            my_cards =  {   8,
                            {
                                card (CARD_A, SUIT_H),
                                card (CARD_K, SUIT_H),
                                card (CARD_A, SUIT_C),
                                card (CARD_A, SUIT_D),
                                card (CARD_A, SUIT_S),
                                card (CARD_T, SUIT_C),
                                card (CARD_9, SUIT_D),
                                card (CARD_3, SUIT_S),
                            }
                        };

    std::cout << std::endl << "---TEST(get_play_cards,test_play_ace_first_if_trump_hearts)---" << std::endl;

    Bid bid;
    bid.bid = 7;
    bid.trump = TRUMP_H;

    Card c = ai_get_play_cards (false,
                                52,
                                7,
                                0,
                                bid,
                                &p0,
                                &p1,
                                &p2,
                                &p3,
                                &my_cards,
                                0);
    std::cout << " AI Picked card: " << card_name(c) << std::endl;
    ASSERT_EQUALS(c, card (CARD_A, SUIT_H));
}

//==============================================================================
//==============================================================================

TEST(get_play_cards,test_play_single_suit_if_trump_not_hearts)
{

    Hand    p0 =        {   0,
                            {
                            }
                        },

            p1 =        {   0,
                            {
                            }
                        },
            p2 =        {   0,
                            {
                            }
                        },
            p3 =        {   0,
                            {
                            }
                        },
            my_cards =  {   8,
                            {
                                card (CARD_J, SUIT_H),
                                card (CARD_K, SUIT_H),
                                card (CARD_K, SUIT_C),
                                card (CARD_K, SUIT_D),
                                card (CARD_Q, SUIT_H),
                                card (CARD_T, SUIT_C),
                                card (CARD_9, SUIT_D),
                                card (CARD_9, SUIT_S),
                            }
                        };

    std::cout << std::endl << "---TEST(get_play_cards,test_play_single_suit_if_trump_not_hearts)---" << std::endl;

    Bid bid;
    bid.bid = 7;
    bid.trump = TRUMP_D;

    Card c = ai_get_play_cards (false,
                                52,
                                7,
                                0,
                                bid,
                                &p0,
                                &p1,
                                &p2,
                                &p3,
                                &my_cards,
                                0);
    std::cout << " AI Picked card: " << card_name(c) << std::endl;
    ASSERT_EQUALS(c, card (CARD_9, SUIT_S));
}

//==============================================================================
//==============================================================================

TEST(get_play_cards,test_doesnt_play_high_spade)
{

    Hand    p0 =        {   0,
                            {

                            }
                        },

            p1 =        {   0,
                            {

                            }
                        },
            p2 =        {   0,
                            {

                            }
                        },
            p3 =        {   0,
                            {

                            }
                        },
            my_cards =  {   8,
                            {
                                card (CARD_Q, SUIT_C),
                                card (CARD_8, SUIT_D),
                                card (CARD_A, SUIT_D),
                                card (CARD_9, SUIT_D),
                                card (CARD_J, SUIT_D),
                                card (CARD_8, SUIT_C),
                                card (CARD_K, SUIT_H),
                                card (CARD_Q, SUIT_S)
                            }
                        };

    std::cout << std::endl << "---TEST(get_play_cards,test_doesnt_play_high_spade)---" << std::endl;

    Bid bid;
    bid.bid = 7;
    bid.trump = TRUMP_D;

    Card c = ai_get_play_cards (false,
                                62,
                                7,
                                2,
                                bid,
                                &p0,
                                &p1,
                                &p2,
                                &p3,
                                &my_cards,
                                2);
    std::cout << " AI Picked card: " << card_name(c) << std::endl;
    ASSERT_TRUE(c !=  card(CARD_Q, SUIT_S));
}

//==============================================================================
//==============================================================================

TEST(get_play_cards,test_lead_1)
{

    //P2 bid 7S - it must have been a force bid because the cards were not good
    //Lead out with 10S and also had AD and AC.
    //Its not a horrible move but it gave away a trick plus potentially more later in the hand.
    //Should have lead out with AD or AC first to win the tricks of that suit as soon as possible.
    //Later in the hand if still has its AD or AC by then other players might out of that suit and can trump in to take the trick.
    //Can somehow make it play its Ace if has in another suits first?

    Hand    p0 =        {   0,
                            {

                            }
                        },

            p1 =        {   0,
                            {

                            }
                        },
            p2 =        {   0,
                            {

                            }
                        },
            p3 =        {   0,
                            {

                            }
                        },
            my_cards =  {   8,
                            {
                                card (CARD_T, SUIT_S),
                                card (CARD_3, SUIT_S),
                                card (CARD_Q, SUIT_H),
                                card (CARD_A, SUIT_D),
                                card (CARD_T, SUIT_C),
                                card (CARD_A, SUIT_C),
                                card (CARD_8, SUIT_S),
                                card (CARD_K, SUIT_D),

                            }
                        };

    std::cout << std::endl << "---TEST(get_play_cards,test_lead_1)---" << std::endl;

    Bid bid;
    bid.bid = 7;
    bid.trump = TRUMP_S;

    Card c = ai_get_play_cards (false,
                                62,
                                7,
                                2,
                                bid,
                                &p0,
                                &p1,
                                &p2,
                                &p3,
                                &my_cards,
                                2);
    std::cout << " AI Picked card: " << card_name(c) << std::endl;
    ASSERT_TRUE(c == card (CARD_A, SUIT_D) || c == card (CARD_A, SUIT_C));
}

//==============================================================================
//==============================================================================

//TEST(get_play_cards,play_1)
//{
//
//    //P2 bid 7H - had AH but only one other heart (10H)
//    //
//    //Lead out with AH which is perfect but unfortunately partner did not have the 5.
//    //next hand lead out with TH - P3 (other team) played JH and P1 (other team) played 5H
//    //P0 (me its parnter) had no hearts to help so we lost the 5
//    //
//    //Is ok to play AH but 5 was not played so from there should have played any other suit
//    //and hope for the best and if you look at P0 (me) i had lots of good cards if it would
//    //have given me the lead we may have been ok.
//    //
//    //was a week hand to bid 7 not sure if was forced.
//
//    Hand    p0 =        {   1,
//                            {
//                                card (CARD_K, SUIT_H),
//                            }
//                        },
//
//            p1 =        {   1,
//                            {
//                                card (CARD_9, SUIT_H),
//
//                            }
//                        },
//            p2 =        {   1,
//                            {
//                                card (CARD_A, SUIT_H)
//                            }
//                        },
//            p3 =        {   1,
//                            {
//                                card (CARD_8, SUIT_H),
//
//                            }
//                        },
//            my_cards =  {   7,
//                            {
//                                card (CARD_T, SUIT_H),
//                                card (CARD_J, SUIT_D),
//                                card (CARD_9, SUIT_S),
//                                card (CARD_T, SUIT_D),
//                                card (CARD_3, SUIT_S),
//                                card (CARD_K, SUIT_C),
//                                card (CARD_8, SUIT_C),
//
//                            }
//                        };
//
//    std::cout << std::endl << "---TEST(get_play_cards,play_1)---" << std::endl;
//
//    Bid bid;
//    bid.bid = 7;
//    bid.trump = TRUMP_H;
//
//    Card c = ai_get_play_cards (false,
//                                62,
//                                7,
//                                2,
//                                bid,
//                                &p0,
//                                &p1,
//                                &p2,
//                                &p3,
//                                &my_cards,
//                                2);
//    std::cout << " AI Picked card: " << card_name(c) << std::endl;
//    ASSERT_TRUE(c != card (CARD_T, SUIT_H));
//}

//==============================================================================
//==============================================================================

TEST(get_play_cards,test_lead_2)
{

    Hand    p0 =        {   0,
                            {

                            }
                        },

            p1 =        {   0,
                            {

                            }
                        },
            p2 =        {   0,
                            {

                            }
                        },
            p3 =        {   0,
                            {

                            }
                        },
            my_cards =  {   8,
                            {
                                card (CARD_A, SUIT_D),
                                card (CARD_8, SUIT_C),
                                card (CARD_J, SUIT_D),
                                card (CARD_9, SUIT_D),
                                card (CARD_K, SUIT_D),
                                card (CARD_A, SUIT_S),
                                card (CARD_T, SUIT_D),
                                card (CARD_T, SUIT_S),

                            }
                        };

    std::cout << std::endl << "---TEST(get_play_cards,test_lead_2)---" << std::endl;

    Bid bid;
    bid.bid = 7;
    bid.trump = TRUMP_N;

    Card c = ai_get_play_cards (false,
                                62,
                                7,
                                2,
                                bid,
                                &p0,
                                &p1,
                                &p2,
                                &p3,
                                &my_cards,
                                2);

    std::cout << " AI Picked card: " << card_name(c) << std::endl;
    ASSERT_EQUALS(c, card (CARD_A, SUIT_D));
}

//==============================================================================
//==============================================================================

TEST(get_play_cards,dont_waste_ace)
{

    Hand    p0 =        {   2,
                            {
                                card (CARD_A, SUIT_C),
                                card (CARD_8, SUIT_D),

                            }
                        },

            p1 =        {   3,
                            {
                                card (CARD_8, SUIT_C),
                                card (CARD_A, SUIT_D),
                                card (CARD_Q, SUIT_D),

                            }
                        },
            p2 =        {   2,
                            {
                                card (CARD_9, SUIT_C),
                                card (CARD_J, SUIT_D),

                            }
                        },
            p3 =        {   2,
                            {
                                card (CARD_7, SUIT_C),
                                card (CARD_9, SUIT_D),

                            }
                        },
            my_cards =  {   6,
                            {
                                card (CARD_T, SUIT_S),
                                card (CARD_Q, SUIT_S),
                                card (CARD_9, SUIT_S),
                                card (CARD_T, SUIT_C),
                                card (CARD_K, SUIT_S),
                                card (CARD_A, SUIT_H),
                            }
                        };

    std::cout << std::endl << "---TEST(get_play_cards,dont_waste_ace)---" << std::endl;

    Bid bid;
    bid.bid = 7;
    bid.trump = TRUMP_D;

    Card c = ai_get_play_cards (false,
                                62,
                                7,
                                1,
                                bid,
                                &p0,
                                &p1,
                                &p2,
                                &p3,
                                &my_cards,
                                2);
    std::cout << " AI Picked card: " << card_name(c) << std::endl;
    ASSERT_EQUALS(c, card (CARD_9, SUIT_S));
}

//==============================================================================
//==============================================================================

TEST(get_play_cards,dont_waste_ace_2)
{

    Hand    p0 =        {   1,
                            {
                                card (CARD_8, SUIT_S),

                            }
                        },

            p1 =        {   0,
                            {

                            }
                        },
            p2 =        {   0,
                            {

                            }
                        },
            p3 =        {   0,
                            {

                            }
                        },
            my_cards =  {   8,
                            {
                                card (CARD_A, SUIT_H),
                                card (CARD_Q, SUIT_C),
                                card (CARD_8, SUIT_C),
                                card (CARD_Q, SUIT_D),
                                card (CARD_A, SUIT_C),
                                card (CARD_K, SUIT_H),
                                card (CARD_9, SUIT_H),
                                card (CARD_T, SUIT_C),

                            }
                        };

    std::cout << std::endl << "---TEST(get_play_cards,dont_waste_ace_2)---" << std::endl;

    Bid bid;
    bid.bid = 7;
    bid.trump = TRUMP_S;

    Card c = ai_get_play_cards (false,
                                62,
                                7,
                                0,
                                bid,
                                &p0,
                                &p1,
                                &p2,
                                &p3,
                                &my_cards,
                                1);
    std::cout << " AI Picked card: " << card_name(c) << std::endl;
    ASSERT_TRUE(c != card (CARD_A, SUIT_H));
}

//==============================================================================
//==============================================================================

TEST(get_play_cards,test_lead_not_hearts)
{

    // id=191 card=8H
    // Player P3 bid 7C
    // Second hand lead out with 8H, I played QH and my partner played 5H.. wasn’t lucky this time

    Hand    p0 =        {   1,
                            {
                                card (CARD_8, SUIT_C),

                            }
                        },

            p1 =        {   1,
                            {
                                card (CARD_7, SUIT_C),

                            }
                        },
            p2 =        {   1,
                            {
                                card (CARD_9, SUIT_D),

                            }
                        },
            p3 =        {   1,
                            {
                                card (CARD_A, SUIT_C)

                            }
                        },
            my_cards =  {   7,
                            {
                                card (CARD_8, SUIT_H),
                                card (CARD_T, SUIT_C),
                                card (CARD_Q, SUIT_S),
                                card (CARD_7, SUIT_D),
                                card (CARD_9, SUIT_C),
                                card (CARD_Q, SUIT_D),
                                card (CARD_8, SUIT_D),

                            }
                        };

    std::cout << std::endl << "---TEST(get_play_cards,test_lead_not_hearts)---" << std::endl;

    Bid bid;
    bid.bid = 7;
    bid.trump = TRUMP_C;

    Card c = ai_get_play_cards (false,
                                62,
                                7,
                                3,
                                bid,
                                &p0,
                                &p1,
                                &p2,
                                &p3,
                                &my_cards,
                                3);
    std::cout << " AI Picked card: " << card_name(c) << std::endl;
    ASSERT_TRUE(c != card (CARD_8, SUIT_H));
}

//==============================================================================
//==============================================================================

TEST(get_play_cards,test_lead_not_hearts_2)
{

    Hand    p0 =        {   2,
                            {
                                card (CARD_8, SUIT_H),
                                card (CARD_7, SUIT_D),

                            }
                        },

            p1 =        {   2,
                            {
                                card (CARD_9, SUIT_H),
                                card (CARD_8, SUIT_D),

                            }
                        },
            p2 =        {   2,
                            {
                                card (CARD_J, SUIT_H),
                                card (CARD_9, SUIT_D),

                            }
                        },
            p3 =        {   2,
                            {
                                card (CARD_A, SUIT_H),
                                card (CARD_A, SUIT_D),

                            }
                        },
            my_cards =  {   6,
                            {
                                card (CARD_Q, SUIT_H),
                                card (CARD_A, SUIT_S),
                                card (CARD_T, SUIT_S),
                                card (CARD_Q, SUIT_C),
                                card (CARD_9, SUIT_S),
                                card (CARD_J, SUIT_C),

                            }
                        };

    std::cout << std::endl << "---TEST(get_play_cards,test_lead_not_hearts_2)---" << std::endl;

    Bid bid;
    bid.bid = 7;
    bid.trump = TRUMP_S;

    Card c = ai_get_play_cards (false,
                                62,
                                7,
                                3,
                                bid,
                                &p0,
                                &p1,
                                &p2,
                                &p3,
                                &my_cards,
                                3);
    std::cout << " AI Picked card: " << card_name(c) << std::endl;
    ASSERT_TRUE(c != card (CARD_Q, SUIT_H));
}

//==============================================================================
//==============================================================================

TEST(get_play_cards,lead_not_with_5)
{

    Hand    p0 =        {   1,
                            {
                                card (CARD_8, SUIT_H),

                            }
                        },

            p1 =        {   1,
                            {
                                card (CARD_9, SUIT_H),

                            }
                        },
            p2 =        {   1,
                            {
                                card (CARD_A, SUIT_H)

                            }
                        },
            p3 =        {   1,
                            {
                                card (CARD_T, SUIT_H),

                            }
                        },
            my_cards =  {   7,
                            {
                                card (CARD_5, SUIT_H),
                                card (CARD_Q, SUIT_C),
                                card (CARD_8, SUIT_S),
                                card (CARD_3, SUIT_S),
                                card (CARD_Q, SUIT_H),
                                card (CARD_J, SUIT_D),
                                card (CARD_9, SUIT_D),

                            }
                        };

    std::cout << std::endl << "---TEST(get_play_cards,lead_not_with_5)---" << std::endl;

    Bid bid;
    bid.bid = 7;
    bid.trump = TRUMP_H;

    Card c = ai_get_play_cards (false,
                                62,
                                7,
                                2,
                                bid,
                                &p0,
                                &p1,
                                &p2,
                                &p3,
                                &my_cards,
                                2);
    std::cout << " AI Picked card: " << card_name(c) << std::endl;
    ASSERT_TRUE(c != card (CARD_5, SUIT_H));
}

//==============================================================================
//==============================================================================

TEST(get_play_cards,lead_not_with_5_2)
{

    Hand    p0 =        {   1,
                            {
                                card (CARD_J, SUIT_H),

                            }
                        },

            p1 =        {   1,
                            {
                                card (CARD_A, SUIT_H),

                            }
                        },
            p2 =        {   1,
                            {
                                card (CARD_T, SUIT_H),

                            }
                        },
            p3 =        {   1,
                            {
                                card (CARD_9, SUIT_H),

                            }
                        },
            my_cards =  {   7,
                            {
                                card (CARD_5, SUIT_H),
                                card (CARD_K, SUIT_H),
                                card (CARD_T, SUIT_C),
                                card (CARD_Q, SUIT_C),
                                card (CARD_9, SUIT_S),
                                card (CARD_8, SUIT_H),
                                card (CARD_J, SUIT_S),

                            }
                        };

    std::cout << std::endl << "---TEST(get_play_cards,lead_not_with_5_2)---" << std::endl;

    Bid bid;
    bid.bid = 7;
    bid.trump = TRUMP_H;

    Card c = ai_get_play_cards (false,
                                62,
                                7,
                                1,
                                bid,
                                &p0,
                                &p1,
                                &p2,
                                &p3,
                                &my_cards,
                                1);
    std::cout << " AI Picked card: " << card_name(c) << std::endl;
    ASSERT_TRUE(c != card (CARD_5, SUIT_H));
}

//==============================================================================
//==============================================================================

TEST(get_play_cards,lead_not_with_5_3)
{

    Hand    p0 =        {   2,
                            {
                                card (CARD_9, SUIT_H),
                                card (CARD_7, SUIT_D),

                            }
                        },

            p1 =        {   2,
                            {
                                card (CARD_3, SUIT_S),
                                card (CARD_7, SUIT_C),

                            }
                        },
            p2 =        {   2,
                            {
                                card (CARD_A, SUIT_H),
                                card (CARD_J, SUIT_H),

                            }
                        },
            p3 =        {   2,
                            {
                                card (CARD_8, SUIT_H),
                                card (CARD_K, SUIT_H),

                            }
                        },
            my_cards =  {   6,
                            {
                                card (CARD_5, SUIT_H),
                                card (CARD_9, SUIT_S),
                                card (CARD_Q, SUIT_D),
                                card (CARD_Q, SUIT_H),
                                card (CARD_T, SUIT_C),
                                card (CARD_J, SUIT_S),

                            }
                        };

    std::cout << std::endl << "---TEST(get_play_cards,lead_not_with_5_3)---" << std::endl;

    Bid bid;
    bid.bid = 8;
    bid.trump = TRUMP_H;

    Card c = ai_get_play_cards (false,
                                62,
                                7,
                                3,
                                bid,
                                &p0,
                                &p1,
                                &p2,
                                &p3,
                                &my_cards,
                                3);
    std::cout << " AI Picked card: " << card_name(c) << std::endl;
    ASSERT_TRUE(c != card (CARD_5, SUIT_H));
}

//==============================================================================
//==============================================================================

TEST(get_play_cards,test_lead_hearts)
{

    Hand    p0 =        {   0,
                            {

                            }
                        },

            p1 =        {   0,
                            {

                            }
                        },
            p2 =        {   0,
                            {

                            }
                        },
            p3 =        {   0,
                            {

                            }
                        },
            my_cards =  {   8,
                            {
                                card (CARD_Q, SUIT_H),
                                card (CARD_A, SUIT_S),
                                card (CARD_J, SUIT_C),
                                card (CARD_Q, SUIT_D),
                                card (CARD_9, SUIT_S),
                                card (CARD_K, SUIT_S),
                                card (CARD_Q, SUIT_C),
                                card (CARD_T, SUIT_D),

                            }
                        };

    std::cout << std::endl << "---TEST(get_play_cards,test_lead_hearts)---" << std::endl;

    Bid bid;
    bid.bid = 7;
    bid.trump = TRUMP_S;

    Card c = ai_get_play_cards (false,
                                62,
                                7,
                                2,
                                bid,
                                &p0,
                                &p1,
                                &p2,
                                &p3,
                                &my_cards,
                                2);
    std::cout << " AI Picked card: " << card_name(c) << std::endl;
    ASSERT_TRUE(c != card (CARD_Q, SUIT_H));
}

//==============================================================================
//==============================================================================

TEST(get_play_cards,test_lead_ace_spades)
{

    Hand    p0 =        {   0,
                            {

                            }
                        },

            p1 =        {   0,
                            {

                            }
                        },
            p2 =        {   0,
                            {

                            }
                        },
            p3 =        {   0,
                            {

                            }
                        },
            my_cards =  {   8,
                            {
                                card (CARD_A, SUIT_S),
                                card (CARD_Q, SUIT_H),
                                card (CARD_J, SUIT_C),
                                card (CARD_Q, SUIT_D),
                                card (CARD_9, SUIT_S),
                                card (CARD_K, SUIT_S),
                                card (CARD_Q, SUIT_C),
                                card (CARD_T, SUIT_D),
                            }
                        };

    std::cout << std::endl << "---TEST(get_play_cards,test_lead_ace_spades)---" << std::endl;

    Bid bid;
    bid.bid = 7;
    bid.trump = TRUMP_S;

    Card c = ai_get_play_cards (false,
                                62,
                                7,
                                2,
                                bid,
                                &p0,
                                &p1,
                                &p2,
                                &p3,
                                &my_cards,
                                2);
    std::cout << " AI Picked card: " << card_name(c) << std::endl;
    ASSERT_TRUE(c != card (CARD_A, SUIT_S));
    ASSERT_TRUE(c != card (CARD_K, SUIT_S));
}

//==============================================================================
//==============================================================================

TEST(get_play_cards,test_lead_hearts_3)
{

    Hand    p0 =        {   1,
                            {
                                card (CARD_9, SUIT_D),

                            }
                        },

            p1 =        {   1,
                            {
                                card (CARD_A, SUIT_D),

                            }
                        },
            p2 =        {   1,
                            {
                                card (CARD_J, SUIT_D),

                            }
                        },
            p3 =        {   1,
                            {
                                card (CARD_K, SUIT_D),

                            }
                        },
            my_cards =  {   7,
                            {
                                card (CARD_Q, SUIT_H),
                                card (CARD_J, SUIT_C),
                                card (CARD_T, SUIT_D),
                                card (CARD_J, SUIT_H),
                                card (CARD_5, SUIT_H),
                                card (CARD_7, SUIT_D),
                                card (CARD_8, SUIT_D),

                            }
                        };

    std::cout << std::endl << "---TEST(get_play_cards,test_lead_hearts_3)---" << std::endl;

    Bid bid;
    bid.bid = 7;
    bid.trump = TRUMP_C;

    Card c = ai_get_play_cards (false,
                                62,
                                7,
                                1,
                                bid,
                                &p0,
                                &p1,
                                &p2,
                                &p3,
                                &my_cards,
                                1);
    std::cout << " AI Picked card: " << card_name(c) << std::endl;
    ASSERT_TRUE(c != card (CARD_Q, SUIT_H));
}

//==============================================================================
//==============================================================================

TEST(get_play_cards,test_waste_card)
{

    Hand    p0 =        {   1,
                            {
                                card (CARD_T, SUIT_C),
                            }
                        },

            p1 =        {   1,
                            {
                                card (CARD_Q, SUIT_C),
                            }
                        },
            p2 =        {   1,
                            {
                                card (CARD_A, SUIT_C),
                            }
                        },
            p3 =        {   0,
                            {

                            }
                        },
            my_cards =  {   7,
                            {
                                card (CARD_K, SUIT_D),
                                card (CARD_J, SUIT_D),
                                card (CARD_T, SUIT_D),
                                card (CARD_9, SUIT_D),
                                card (CARD_8, SUIT_D),
                                card (CARD_7, SUIT_D),
                                card (CARD_5, SUIT_H),

                            }
                        };

    std::cout << std::endl << "---TEST(get_play_cards,test_waste_card)---" << std::endl;

    Bid bid;
    bid.bid = 7;
    bid.trump = TRUMP_D;

    Card c = ai_get_play_cards (false,
                                62,
                                7,
                                0,
                                bid,
                                &p0,
                                &p1,
                                &p2,
                                &p3,
                                &my_cards,
                                3);
    std::cout << " AI Picked card: " << card_name(c) << std::endl;
    ASSERT_TRUE(c != card (CARD_K, SUIT_D));
}

//==============================================================================
//==============================================================================

