# Machine Learning Dataset

This directory contains all of the played hands for every single multiplayer game. Cards are identified by a two character sequence. For example the 8 of spades would be 8S and the 10 of hearts would be TH.

## playedhand.csv

This is the schema for the table.
```
                                                                 Table "public.playedhand"
            Column             |           Type           |                        Modifiers                        | Storage  | Stats target | Description 
-------------------------------+--------------------------+---------------------------------------------------------+----------+--------------+-------------
 id                            | integer                  | not null default nextval('playedhand_id_seq'::regclass) | plain    |              | 
 added_date                    | timestamp with time zone |                                                         | plain    |              | 
 setting_min_bid               | integer                  |                                                         | plain    |              | 
 setting_pass_cards            | boolean                  |                                                         | plain    |              | 
 setting_no_trump_bid_out      | integer                  |                                                         | plain    |              | 
 setting_game_over_at_minus_52 | boolean                  |                                                         | plain    |              | 
 dealer                        | integer                  |                                                         | plain    |              | 
 player_0_bid                  | character varying(4)     |                                                         | extended |              | 
 player_1_bid                  | character varying(4)     |                                                         | extended |              | 
 player_2_bid                  | character varying(4)     |                                                         | extended |              | 
 player_3_bid                  | character varying(4)     |                                                         | extended |              | 
 player_0_hand                 | character varying(16)    |                                                         | extended |              | 
 player_1_hand                 | character varying(16)    |                                                         | extended |              | 
 player_2_hand                 | character varying(16)    |                                                         | extended |              | 
 player_3_hand                 | character varying(16)    |                                                         | extended |              | 
 played_cards                  | character varying(64)    |                                                         | extended |              | 
 player_0_ai                   | boolean                  |                                                         | plain    |              | 
 player_1_ai                   | boolean                  |                                                         | plain    |              | 
 player_2_ai                   | boolean                  |                                                         | plain    |              | 
 player_3_ai                   | boolean                  |                                                         | plain    |              | 
Indexes:
    "playedhand_pkey" PRIMARY KEY, btree (id)

```

An example of a program that can play the entire hand is as follows:

```

def decode_bid (bid):
    suit = bid[-1:]

    if suit in ('C','H','D','S','N'):
        bid = bid[:-1]
    else:
        suit = None

    return bid, suit

def get_better_bid (a,b):
    value_a, suit_a = cls.decode_bid(a)
    value_b, suit_b = cls.decode_bid(b)

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
        i_score, i_suit = cls.decode_card(played_cards[i])

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



h = ...loaded row from csv file...

# Play hands forward
winner = 0
bids = [h.player_0_bid, h.player_1_bid, h.player_2_bid, h.player_3_bid]

for b in range(1,4):
    bi = (h.dealer + 1 + b) % 4
    if (get_better_bid(bids[winner], bids[bi]) == bids[bi]):
        winner = bi
highest_value, highest_suit = decode_bid(bids[winner])

player_0_hand = []
player_1_hand = []
player_2_hand = []
player_3_hand = []

for j in range(0,8):
    player_0_hand.append(h.player_0_hand[j*2+0] + h.player_0_hand[j*2+1])
    player_1_hand.append(h.player_1_hand[j*2+0] + h.player_1_hand[j*2+1])
    player_2_hand.append(h.player_2_hand[j*2+0] + h.player_2_hand[j*2+1])
    player_3_hand.append(h.player_3_hand[j*2+0] + h.player_3_hand[j*2+1])

for i in range(len(h.played_cards)/8):  # 4 pairs (8) per hand
    played_cards = ["","","",""]
    first_suit = None

    for j in range(0,4):
        card = h.played_cards[i*8+j*2+0]
        suit = h.played_cards[i*8+j*2+1]

        if j==0:
            first_suit = suit

        my_index = (j+winner)%4
        played_cards[my_index] = card + suit

        if my_index==0:
            player_0_hand.remove(card+suit)
        elif my_index==1:
            player_1_hand.remove(card+suit)
        elif my_index==2:
            player_2_hand.remove(card+suit)
        elif my_index==3:
            player_3_hand.remove(card+suit)

    winner = get_winner (first_suit, highest_suit, played_cards)

```