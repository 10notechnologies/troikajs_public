# Machine Learning Dataset

This directory contains all of the played hands for every single multiplayer game.

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
