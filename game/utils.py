#==============================================================================
#
# Copyright (c) 2019 Smells Like Donkey Software Inc.  All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS `AS IS'
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#
#==============================================================================

def decode_bid (bid):
    suit = bid[-1:]

    if suit in ('C','H','D','S','N'):
        bid = bid[:-1]
    else:
        suit = None

    return bid, suit

def decode_card (card):
    if card[0] == "3":
        return 3, card[1]
    if card[0] == "5":
        return 5, card[1]
    if card[0] == "7":
        return 7, card[1]
    if card[0] == "8":
        return 8, card[1]
    if card[0] == "9":
        return 9, card[1]
    if card[0] == "T":
        return 10, card[1]
    if card[0] == "J":
        return 11, card[1]
    if card[0] == "Q":
        return 12, card[1]
    if card[0] == "K":
        return 13, card[1]
    if card[0] == "A":
        return 14, card[1]
    return 0, ""

def get_better_bid (a,b):
    value_a, suit_a = decode_bid(a)
    value_b, suit_b = decode_bid(b)

    # Convert "pass" to a value of 0
    if value_a == "pass":
        value_a = 0

    if value_b == "pass":
        value_b = 0

    no_a = 0
    if suit_a == 'N':
        no_a = 1

    no_b = 0
    if suit_b == 'N':
        no_b = 1

    if int(value_a)*2+no_a > int(value_b)*2+no_b:
        return a
    else:
        return b

def get_winner (first_suit, trump_suit, played_cards):
    winner = 0
    winner_score = 0

    had_5 = False
    had_3 = False

    for i in range(0, 4):
        i_score, i_suit = decode_card(played_cards[i])

        if i_score == 3:
            had_3 = True
        if i_score == 5:
            had_5 = True

        if trump_suit == i_suit:
            i_score += 200
        elif first_suit == i_suit:
            i_score += 100

        if i_score >  winner_score:
            winner = i
            winner_score = i_score

    return winner