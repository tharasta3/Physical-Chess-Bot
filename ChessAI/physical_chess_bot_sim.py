import serial
import time
from src.game import board
from src.AI_players.player_Min_Max import Player_Min_Max

# def initialize_board():
#     game = board.Board(1, 0)
#     return game

# r n b q k b n r
# p p p p p p p p
# . . . . . . . .
# . . . . . . . .
# . . . . . . . .
# . . . . . . . .
# P P P P P P P P
# R N B Q K B N R

def initialize_board2(board_state=None):
    board_string = """
r n b q k b n r
p p p p p p p p
. . . . . . . .
. . . . . . . .
. . . . . . . .
. . . . . . . .
P P P P P P P P
R N B Q K B N R
    """
    game = board.Board(1, 0)
    print(board_string)
    parsed_position = game.parse_board_from_string(board_string)

    return board.Board(newgame=False, position=parsed_position)


def get_best_move(game, player_color='b', depth=5):
    player = Player_Min_Max(player_color, game, depth)
    player.self_move()
    if player.tree.root.move:
        return player.tree.root.move
    else: 
        raise ValueError("no valid move available to chess bot")

# def connect_to_stm32(port="COM3", baud_rate=115200):
#     stm32 = serial.Serial(port, baud_rate, timeout=1)
#     return stm32

def connect_to_stm32(port="COM3", baud_rate=115200):
    stm32 = serial.Serial(
        port=port,
        baudrate=baud_rate,   # Set baud rate
        bytesize=serial.EIGHTBITS,  # Set 8 data bits
        parity=serial.PARITY_NONE,  # No parity
        stopbits=serial.STOPBITS_ONE,  # 1 stop bit
        timeout=2  # Timeout in seconds
    )
    return stm32

def send_move_to_stm32(stm32, move):
    move_str = f"{move[0][0]}{move[0][1]}{move[1][0]}{move[1][1]}\n"
    print(move_str)
    stm32.write(move_str.encode())

def receive_move_from_stm32(stm32):
    while True:
        move = stm32.readline()
        if len(move) != 0:
            start_pos = [int(move[0]), int(move[1])]
            end_pos = [int(move[2]), int(move[3])]
            return [start_pos, end_pos]
        # else:
        #     print("Invalid move received, waiting for valid input...")

def play_chess_bot():
    game = initialize_board2()
    stm32 = connect_to_stm32(port="/dev/tty.usbserial-00000000", baud_rate=115200)

    while game.result == 0:
        move = receive_move_from_stm32(stm32)
        print("read completed: ", move[0], move[1])
        print("Game Board after player move: ")
        print(game)
        start_pos, end_pos = move[0], move[1]
        piece = game.get_piece_from_position([start_pos[0], start_pos[1]])
        #breakpoint()
        if piece == 0 or piece.color != game.color_to_play:
            print("error: no piece at that position or that is not your piece!!!")
            continue

        game.Move(piece, [end_pos[0], end_pos[1]])

        if game.result == 'w':
            print("print", game.result)
            print('White wins!')
            break
        elif game.result == 'b':
            print('Black wins!')
            break
        elif game.result == 'd':
            print('Draw!')
            break
        print("Game Board after chess bot move: ")
        print(game)
        ai_move = get_best_move(game, player_color='b', depth=2)
        send_move_to_stm32(stm32, ai_move)

        if game.result == 'w':
            print("print", game.result)
            print('White wins!')
            break
        elif game.result == 'b':
            print('Black wins!')
            break
        elif game.result == 'd':
            print('Draw!')
            break

        time.sleep(1)

    
    

    stm32.close()

play_chess_bot()
