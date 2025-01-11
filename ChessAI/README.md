# Todo:

- get a way to choose promotion (another window or comand line)
- code a Monte Carlo player
- code a reinforcement learning player
- code an Min-Max player :white_check_mark:
- code a me looking AI
- code en passant :white_check_mark:
- code draw by repetition
- code board.result = 0 (if not finished), 'w' (if white won), 'b' (if black won), 'd' (if draw) :white_check_mark:

# Debbug:
- Player_Min_Max cannot play with 'w'
- Debbug the Min-Max player as it sacrifices valuable pieces when it shouldn't
- Optimize the Min-Max player (it is way too slow)
- code first move for king and rook and make sure the engine respect the rule :white_check_mark:
- allow black to promote :white_check_mark:
- change the way the game is rendered so it don't flicker
- block castling if the intermediary square is in the opposite color vision :white_check_mark:
- 'not a valid move' on black king check by taking :white_check_mark:
- allow taking a piece that is checking the opposite king (blocking works) (the engine probably only allow a king move if is_checked) :white_check_mark:
