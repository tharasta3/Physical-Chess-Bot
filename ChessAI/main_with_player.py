from src.game import board
from src.graphic_interface import graphic_interface
from src.AI_players.player_Min_Max import Player_Min_Max

game = board.Board(1,0)
# position_to_test = [['w', 'p', 1, 2], ['w', 'p', 2, 2], ['w', 'p', 3, 2], ['w', 'p', 4, 2], ['w', 'p', 5, 3], ['w', 'p', 6, 2], ['w', 'p', 7, 2], ['w', 'p', 8, 2], ['w', 'b', 3, 4], ['w', 'b', 3, 1], ['w', 'k', 7, 1], ['w', 'k', 2, 1], ['w', 'r', 1, 1, 1], ['w', 'r', 8, 1, 1], ['w', 'Q', 6, 7], ['w', 'K', 5, 1, 1], ['b', 'p', 1, 6], ['b', 'p', 2, 5], ['b', 'p', 3, 7], ['b', 'p', 4, 7], ['b', 'p', 5, 7], ['b', 'p', 7, 7], ['b', 'p', 8, 6], ['b', 'b', 6, 8], ['b', 'b', 3, 8], ['b', 'k', 7, 8], ['b', 'k', 2, 8], ['b', 'r', 1, 8, 1], ['b', 'r', 8, 8, 1], ['b', 'Q', 4, 8], ['b', 'K', 6, 7, 1]]
# game = board.Board(0,position_to_test)
player = Player_Min_Max('b',game,2)
gui_game = graphic_interface.App(game.pieces)


while game.result == 0:
    if game.color_to_play == player.color:
        player.self_move()
        gui_game.pieces = game.pieces
        gui_game.get_pieces(game.color_to_play)
    else:
        move = gui_game.on_execute(game.color_to_play)
        start_pos, fin_pos = move[0],move[1]
        piece = game.get_piece_from_position([start_pos[0],start_pos[1]])

        if piece==0:
            print('No piece here')
            pass

        elif piece.color != game.color_to_play:
            print(f"{'white' if game.color_to_play=='w' else 'black'} to play")
            pass
        else:
            if game.Move(piece, [fin_pos[0],fin_pos[1]])==0:
                gui_game.pieces = game.pieces
                gui_game.get_pieces(game.color_to_play)
                print('not a valid move')
                pass

            else:
                player.Update_tree_on_opponent_move(move)
                gui_game.pieces = game.pieces
                gui_game.get_pieces(game.color_to_play)



if game.result == 'w':
    print('white wins')
elif game.result == 'b':
    print('black wins')
else:
    print('draw')

gui_game.on_cleanup()
