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

TrumpSuit ai_get_trump  (int32_t    rule_pass_cards,
                        int32_t     rule_no_trump_bidout,
                        int32_t     rule_minimum_bid,
                        Hand        *my_cards);

//==============================================================================
//==============================================================================

TEST(chars_to_bid,test_cases)
{
    std::cout << std::endl << "---TEST(chars_to_bid,test_cases)---" << std::endl;

    
    ASSERT_EQUALS(chars_to_bid("pass"), Bid(0,TRUMP_UNKNOWN));
    ASSERT_EQUALS(chars_to_bid("5"), Bid(5,TRUMP_UNKNOWN));
    ASSERT_EQUALS(chars_to_bid("5N"), Bid(5,TRUMP_N));
    ASSERT_EQUALS(chars_to_bid("5D"), Bid(5,TRUMP_D));
    ASSERT_EQUALS(chars_to_bid("5C"), Bid(5,TRUMP_C));
    ASSERT_EQUALS(chars_to_bid("5H"), Bid(5,TRUMP_H));
    ASSERT_EQUALS(chars_to_bid("5S"), Bid(5,TRUMP_S));

    ASSERT_EQUALS(chars_to_bid("pass"), Bid(0,TRUMP_UNKNOWN));
    ASSERT_EQUALS(chars_to_bid("6"), Bid(6,TRUMP_UNKNOWN));
    ASSERT_EQUALS(chars_to_bid("6N"), Bid(6,TRUMP_N));
    ASSERT_EQUALS(chars_to_bid("6D"), Bid(6,TRUMP_D));
    ASSERT_EQUALS(chars_to_bid("6C"), Bid(6,TRUMP_C));
    ASSERT_EQUALS(chars_to_bid("6H"), Bid(6,TRUMP_H));
    ASSERT_EQUALS(chars_to_bid("6S"), Bid(6,TRUMP_S));

    ASSERT_EQUALS(chars_to_bid("pass"), Bid(0,TRUMP_UNKNOWN));
    ASSERT_EQUALS(chars_to_bid("7"), Bid(7,TRUMP_UNKNOWN));
    ASSERT_EQUALS(chars_to_bid("7N"), Bid(7,TRUMP_N));
    ASSERT_EQUALS(chars_to_bid("7D"), Bid(7,TRUMP_D));
    ASSERT_EQUALS(chars_to_bid("7C"), Bid(7,TRUMP_C));
    ASSERT_EQUALS(chars_to_bid("7H"), Bid(7,TRUMP_H));
    ASSERT_EQUALS(chars_to_bid("7S"), Bid(7,TRUMP_S));

    ASSERT_EQUALS(chars_to_bid("pass"), Bid(0,TRUMP_UNKNOWN));
    ASSERT_EQUALS(chars_to_bid("8"), Bid(8,TRUMP_UNKNOWN));
    ASSERT_EQUALS(chars_to_bid("8N"), Bid(8,TRUMP_N));
    ASSERT_EQUALS(chars_to_bid("8D"), Bid(8,TRUMP_D));
    ASSERT_EQUALS(chars_to_bid("8C"), Bid(8,TRUMP_C));
    ASSERT_EQUALS(chars_to_bid("8H"), Bid(8,TRUMP_H));
    ASSERT_EQUALS(chars_to_bid("8S"), Bid(8,TRUMP_S));

    ASSERT_EQUALS(chars_to_bid("pass"), Bid(0,TRUMP_UNKNOWN));
    ASSERT_EQUALS(chars_to_bid("9"), Bid(9,TRUMP_UNKNOWN));
    ASSERT_EQUALS(chars_to_bid("9N"), Bid(9,TRUMP_N));
    ASSERT_EQUALS(chars_to_bid("9D"), Bid(9,TRUMP_D));
    ASSERT_EQUALS(chars_to_bid("9C"), Bid(9,TRUMP_C));
    ASSERT_EQUALS(chars_to_bid("9H"), Bid(9,TRUMP_H));
    ASSERT_EQUALS(chars_to_bid("9S"), Bid(9,TRUMP_S));

    ASSERT_EQUALS(chars_to_bid("pass"), Bid(0,TRUMP_UNKNOWN));
    ASSERT_EQUALS(chars_to_bid("10"), Bid(10,TRUMP_UNKNOWN));
    ASSERT_EQUALS(chars_to_bid("10N"), Bid(10,TRUMP_N));
    ASSERT_EQUALS(chars_to_bid("10D"), Bid(10,TRUMP_D));
    ASSERT_EQUALS(chars_to_bid("10C"), Bid(10,TRUMP_C));
    ASSERT_EQUALS(chars_to_bid("10H"), Bid(10,TRUMP_H));
    ASSERT_EQUALS(chars_to_bid("10S"), Bid(10,TRUMP_S));
    
    ASSERT_EQUALS(chars_to_bid("pass"), Bid(0,TRUMP_UNKNOWN));
    ASSERT_EQUALS(chars_to_bid("11"), Bid(11,TRUMP_UNKNOWN));
    ASSERT_EQUALS(chars_to_bid("11N"), Bid(11,TRUMP_N));
    ASSERT_EQUALS(chars_to_bid("11D"), Bid(11,TRUMP_D));
    ASSERT_EQUALS(chars_to_bid("11C"), Bid(11,TRUMP_C));
    ASSERT_EQUALS(chars_to_bid("11H"), Bid(11,TRUMP_H));
    ASSERT_EQUALS(chars_to_bid("11S"), Bid(11,TRUMP_S));
    
    ASSERT_EQUALS(chars_to_bid("pass"), Bid(0,TRUMP_UNKNOWN));
    ASSERT_EQUALS(chars_to_bid("12"), Bid(12,TRUMP_UNKNOWN));
    ASSERT_EQUALS(chars_to_bid("12N"), Bid(12,TRUMP_N));
    ASSERT_EQUALS(chars_to_bid("12D"), Bid(12,TRUMP_D));
    ASSERT_EQUALS(chars_to_bid("12C"), Bid(12,TRUMP_C));
    ASSERT_EQUALS(chars_to_bid("12H"), Bid(12,TRUMP_H));
    ASSERT_EQUALS(chars_to_bid("12S"), Bid(12,TRUMP_S));

}

//==============================================================================
//==============================================================================
