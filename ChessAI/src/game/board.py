from .pawn import Pawn
from .king import King
from .queen import Queen
from .bishop import Bishop
from .knight import Knight
from .rook import Rook



class Board:
    def __init__(self,newgame,position,moves=[],color_to_play='w'):
        self.moves = moves
        self.color_to_play = color_to_play
        self.result = 0
        if newgame:

            #create the white pieces
            #wAp = white A pawn ...
            wAp,wBp,wCp,wDp,wEp,wFp,wGp,wHp=Pawn('w',1,2),Pawn('w',2,2),Pawn('w',3,2),Pawn('w',4,2),Pawn('w',5,2),Pawn('w',6,2),Pawn('w',7,2),Pawn('w',8,2)
            #wwb = white bishop on the white square
            wwb,wbb=Bishop('w',6,1),Bishop('w',3,1)
            #wk1 = white knight 1
            wk1,wk2=Knight('w',7,1),Knight('w',2,1)
            #wr1 = white rook 1
            wr1,wr2=Rook('w',1,1),Rook('w',8,1)
            wQ=Queen('w',4,1)
            wK=King('w',5,1)

            #create the black pieces
            bAp,bBp,bCp,bDp,bEp,bFp,bGp,bHp=Pawn('b',1,7),Pawn('b',2,7),Pawn('b',3,7),Pawn('b',4,7),Pawn('b',5,7),Pawn('b',6,7),Pawn('b',7,7),Pawn('b',8,7)
            bwb,bbb=Bishop('b',6,8),Bishop('b',3,8)
            bk1,bk2=Knight('b',7,8),Knight('b',2,8)
            br1,br2=Rook('b',1,8),Rook('b',8,8)
            bQ=Queen('b',4,8)
            bK=King('b',5,8)

            self.pieces = {'w':[wAp,wBp,wCp,wDp,wEp,wFp,wGp,wHp,wwb,wbb,wk1,wk2,wr1,wr2,wQ,wK],
                           'b':[bAp,bBp,bCp,bDp,bEp,bFp,bGp,bHp,bwb,bbb,bk1,bk2,br1,br2,bQ,bK]}


        else:
            self.pieces = {'w':[],
                           'b':[]}
            #position=[[color,type of piece, file, row, first_move (if r or K)]... ]
            for pos in position:
                if pos[1]=='p':
                    self.pieces[pos[0]].append(Pawn(pos[0],pos[2],pos[3]))
                elif pos[1]=='b':
                    self.pieces[pos[0]].append(Bishop(pos[0],pos[2],pos[3]))
                elif pos[1]=='k':
                    self.pieces[pos[0]].append(Knight(pos[0],pos[2],pos[3]))
                elif pos[1]=='r':
                    self.pieces[pos[0]].append(Rook(pos[0],pos[2],pos[3],first_move=pos[4]))
                elif pos[1]=='Q':
                    self.pieces[pos[0]].append(Queen(pos[0],pos[2],pos[3]))
                else:
                    self.pieces[pos[0]].append(King(pos[0],pos[2],pos[3],first_move=pos[4]))

    # def __init__(self, board_state=None):
    #     # Default chessboard starting position
    #     self.default_board = [
    #         ['r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'],
    #         ['p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'],
    #         ['.', '.', '.', '.', '.', '.', '.', '.'],
    #         ['.', '.', '.', '.', '.', '.', '.', '.'],
    #         ['.', '.', '.', '.', '.', '.', '.', '.'],
    #         ['.', '.', '.', '.', '.', '.', '.', '.'],
    #         ['P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'],
    #         ['R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R']
    #     ]
        
    #     # If no board state is passed, initialize with default board
    #     if board_state is None:
    #         self.board = [row[:] for row in self.default_board]
    #     else:
    #         # Initialize board from the provided board state string
    #         self.board = self.parse_board_state(board_state)
    
    # def parse_board_state(self, board_state):
    #     """Converts a board state string into a 2D list."""
    #     rows = board_state.strip().split('\n')
    #     board = []
    #     for row in rows:
    #         board.append(row.split())
    #     return board
    
    # def print_board(self):
    #     """Prints the board in a readable format."""
    #     for row in self.board:
    #         print(" ".join(row))


    def parse_board_from_string(self, board_string):
        piece_map = {
            'p': 'p', 'r': 'r', 'n': 'k', 'b': 'b', 'q': 'Q', 'k': 'K',  # lowercase for black
            'P': 'p', 'R': 'r', 'N': 'k', 'B': 'b', 'Q': 'Q', 'K': 'K'   # uppercase for white
        }
        position = []
        rows = board_string.strip().split('\n')  # Split string into rows
        for rank, row in enumerate(reversed(rows), start=1):  # Iterate rows from bottom to top
            for file, char in enumerate(row.split(), start=1):  # Iterate columns
                if char != '.':
                    color = 'w' if char.isupper() else 'b'
                    piece_type = piece_map[char]
                    if piece_type in {'r', 'K'}:
                        # For rooks and kings, add the `first_move` flag (default to True)
                        position.append([color, piece_type, file, rank, True])
                    else:
                        position.append([color, piece_type, file, rank])
        return position

    def get_entire_position(self):
            position=[]

            for piece in self.pieces['w']:
                if piece.piece_type=='r' or piece.piece_type=='K':
                    position.append([piece.color,piece.piece_type,piece.file,piece.row,piece.first_move])
                else:
                    position.append([piece.color,piece.piece_type,piece.file,piece.row])

            for piece in self.pieces['b']:
                if piece.piece_type=='r' or piece.piece_type=='K':
                    position.append([piece.color,piece.piece_type,piece.file,piece.row,piece.first_move])
                else:
                    position.append([piece.color,piece.piece_type,piece.file,piece.row])
            return position

    def get_one_color_position(self,color):
            position=[]
            for piece in self.pieces[color]:
                position.append([piece.file,piece.row])
            return position

    def get_piece_from_position(self,pos):
        #pos = [piece.file, piece.row]

        for piece in self.pieces['w']:
            if piece.file==pos[0] and piece.row==pos[1]:
                return piece
        for piece in self.pieces['b']:
            if piece.file==pos[0] and piece.row==pos[1]:
                return piece

        return 0 #if no piece found in this position it returns 0

    def is_checked(self):
        """ returns 'w' if white is checked, 'b' if 'black' is checked and 0 otherwise """

        #getting the white king position
        for piece in self.pieces['w']:
            if piece.piece_type=='K':
                break
        white_king_pos = [piece.file,piece.row]

        #looking for checks
        for piece in self.pieces['b']:
            if piece.piece_type!='K':
                vision = piece.vision(self)
                if white_king_pos in vision:

                    return 'w' #returns 'w' if the white king is checked

        #getting the black king position
        for piece in self.pieces['b']:
            if piece.piece_type=='K':
                break
        black_king_pos = [piece.file,piece.row]

        #looking for checks
        for piece in self.pieces['w']:
            if piece.piece_type!='K':
                vision = piece.vision(self)
                if black_king_pos in vision:
                    return 'b' #returns 'b' if the black king is checked

        return 0 # if no one is checked

    def possible_moves(self,color):
        possible_moves = [] #[[piece,move],...]

        for piece in self.pieces[color]:
            for move in piece.vision(self):
                position_to_test = self.get_entire_position()

                if piece.piece_type=='K' or piece.piece_type=='r':
                    i = position_to_test.index([piece.color,piece.piece_type,piece.file,piece.row,piece.first_move])
                    position_to_test[i]=[piece.color,piece.piece_type,move[0],move[1],piece.first_move]
                else:
                    i = position_to_test.index([piece.color,piece.piece_type,piece.file,piece.row])
                    position_to_test[i]=[piece.color,piece.piece_type,move[0],move[1]]
                position_to_test_taken_piece = self.get_piece_from_position(move)

                if position_to_test_taken_piece != 0 and position_to_test_taken_piece.piece_type !='K':
                    if position_to_test_taken_piece.piece_type == 'r':
                        to_remove = [position_to_test_taken_piece.color,position_to_test_taken_piece.piece_type,position_to_test_taken_piece.file,position_to_test_taken_piece.row,position_to_test_taken_piece.first_move]
                    else:
                        to_remove = [position_to_test_taken_piece.color,position_to_test_taken_piece.piece_type,position_to_test_taken_piece.file,position_to_test_taken_piece.row]
                    position_to_test.remove(to_remove)
                Position_to_test = Board(0,position_to_test,moves=self.moves,color_to_play=color)
                if color == 'w':
                    if Position_to_test.is_checked()!='w':
                        possible_moves.append([piece,move])
                else:
                    if Position_to_test.is_checked()!='b':
                        possible_moves.append([piece,move])

                Position_to_test.delete_all_pieces()
                del Position_to_test

        return possible_moves

    def delete_all_pieces(self):
        del self.pieces

    def Move(self,piece,move):
        if move not in piece.vision(self):
            return 0 #not a legal move
        else:
            position_to_test = self.get_entire_position()

            if piece.piece_type=='K' or piece.piece_type=='r':
                i = position_to_test.index([piece.color,piece.piece_type,piece.file,piece.row,piece.first_move])
                position_to_test[i]=[piece.color,piece.piece_type,move[0],move[1],piece.first_move]
            else:
                i = position_to_test.index([piece.color,piece.piece_type,piece.file,piece.row])
                position_to_test[i]=[piece.color,piece.piece_type,move[0],move[1]]

            position_to_test_taken_piece = self.get_piece_from_position(move)

            if position_to_test_taken_piece != 0:
                if position_to_test_taken_piece.piece_type == 'r':
                    to_remove = [position_to_test_taken_piece.color,position_to_test_taken_piece.piece_type,position_to_test_taken_piece.file,position_to_test_taken_piece.row,position_to_test_taken_piece.first_move]
                else:
                    to_remove = [position_to_test_taken_piece.color,position_to_test_taken_piece.piece_type,position_to_test_taken_piece.file,position_to_test_taken_piece.row]

                position_to_test.remove(to_remove)

            Position_to_test = Board(0,position_to_test,moves=self.moves,color_to_play=self.color_to_play)
            if piece.color == 'w':
                if Position_to_test.is_checked()=='w':
                    return 0
            else:
                if Position_to_test.is_checked()=='b':
                    return 0

            Position_to_test.delete_all_pieces()
            del Position_to_test

            # we have handled all cases of illegal moves
            # Now we have to handle the case of capture (with en passant) and castling

            if piece.color =='w':
                if move in self.get_one_color_position('b'):
                    taken_piece = self.get_piece_from_position(move)
                    self.pieces['b'].remove(taken_piece)
                    del taken_piece
                    self.moves.append(([piece.file,piece.row],move))
                    piece.Move(move)
                    self.color_to_play = self.inverse_color(self.color_to_play)
                    self.game_end()

                    # print('take')
                # en passant
                elif piece.piece_type == 'p' and move[0]!= piece.file:
                    taken_piece = self.get_piece_from_position([move[0],move[1]-1])
                    self.pieces['b'].remove(taken_piece)
                    del taken_piece
                    self.moves.append(([piece.file,piece.row],move))
                    piece.Move(move)
                    self.color_to_play = self.inverse_color(self.color_to_play)
                    self.game_end()
                    # print('en passant !')

                else:
                    if piece.piece_type=='K' and abs(move[0]-piece.file)>1:
                        #castling
                        position_to_test = self.get_entire_position()

                        i = position_to_test.index([piece.color,piece.piece_type,piece.file,piece.row,piece.first_move])
                        if move[0]==7: #right castling
                            position_to_test[i]=[piece.color,piece.piece_type,6,1,1]
                            Position_to_test = Board(0,position_to_test,moves=self.moves,color_to_play=self.color_to_play)
                            if Position_to_test.is_checked()=='w':
                                Position_to_test.delete_all_pieces()
                                del Position_to_test
                                return 0
                            rook = self.get_piece_from_position([8,1])
                            rook.Move([6,1])
                            self.moves.append(([piece.file,piece.row],move))
                            piece.Move(move)
                            self.color_to_play = self.inverse_color(self.color_to_play)
                            self.game_end()
                        else: #left castling
                            position_to_test[i]=[piece.color,piece.piece_type,4,1,1]
                            Position_to_test = Board(0,position_to_test,moves=self.moves,color_to_play=self.color_to_play)
                            if Position_to_test.is_checked()=='w':
                                Position_to_test.delete_all_pieces()
                                del Position_to_test
                                return 0
                            rook = self.get_piece_from_position([1,1])
                            rook.Move([4,1])
                            self.moves.append(([piece.file,piece.row],move))
                            piece.Move(move)
                            self.color_to_play = self.inverse_color(self.color_to_play)
                            self.game_end()
                    else:
                        self.moves.append(([piece.file,piece.row],move))
                        piece.Move(move)
                        self.color_to_play = self.inverse_color(self.color_to_play)
                        self.game_end()
            else:
                if move in self.get_one_color_position('w'):
                    taken_piece = self.get_piece_from_position(move)
                    self.pieces['w'].remove(taken_piece)
                    del taken_piece
                    self.moves.append(([piece.file,piece.row],move))
                    piece.Move(move)
                    self.color_to_play = self.inverse_color(self.color_to_play)
                    self.game_end()
                    # print('take')
                elif piece.piece_type == 'p' and move[0]!= piece.file:
                    taken_piece = self.get_piece_from_position([move[0],move[1]+1])
                    self.pieces['w'].remove(taken_piece)
                    del taken_piece
                    self.moves.append(([piece.file,piece.row],move))
                    piece.Move(move)
                    self.color_to_play = self.inverse_color(self.color_to_play)
                    self.game_end()
                    # print('en passant !')
                else:
                    if piece.piece_type=='K' and (move[0]-piece.file<-1 or move[0]-piece.file>1):
                        position_to_test = self.get_entire_position()

                        i = position_to_test.index([piece.color,piece.piece_type,piece.file,piece.row,piece.first_move])
                        if move[0]==7: #right castling
                            position_to_test[i]=[piece.color,piece.piece_type,6,8,1]
                            Position_to_test = Board(0,position_to_test,moves=self.moves,color_to_play=self.color_to_play)
                            if Position_to_test.is_checked()=='b':
                                Position_to_test.delete_all_pieces()
                                del Position_to_test
                                return 0
                            rook = self.get_piece_from_position([8,8])
                            rook.Move([6,8])
                            self.moves.append(([piece.file,piece.row],move))
                            piece.Move(move)
                            self.color_to_play = self.inverse_color(self.color_to_play)
                            self.game_end()
                        else: #left castling
                            position_to_test[i]=[piece.color,piece.piece_type,4,8,1]
                            Position_to_test = Board(0,position_to_test,moves=self.moves,color_to_play=self.color_to_play)
                            if Position_to_test.is_checked()=='b':
                                Position_to_test.delete_all_pieces()
                                del Position_to_test
                                return 0
                            rook = self.get_piece_from_position([1,8])
                            rook.Move([4,8])
                            self.moves.append(([piece.file,piece.row],move))
                            piece.Move(move)
                            self.color_to_play = self.inverse_color(self.color_to_play)
                            self.game_end()
                    else:
                        self.moves.append(([piece.file,piece.row],move))
                        piece.Move(move)
                        self.color_to_play = self.inverse_color(self.color_to_play)
                        self.game_end()

            # Then the case of promotion for now only in queen
            if piece.color =='w':
                if piece.piece_type=='p' and piece.row==8:
                    self.pieces['w'].remove(piece)
                    self.pieces['w'].append(Queen('w',piece.file,piece.row))
                    del piece
                    self.game_end()

            else:
                if piece.piece_type=='p' and piece.row==1:
                    self.pieces['b'].remove(piece)
                    self.pieces['b'].append(Queen('b',piece.file,piece.row))
                    del piece
                    self.game_end()

    def game_end(self):
        if len(self.possible_moves(self.color_to_play))==0:
                            if self.is_checked() == self.color_to_play:
                                self.result = self.inverse_color(self.color_to_play)
                            else:
                                self.result = 'd'

    # custom function to print board state
    def __str__(self):
        # Create an 8x8 grid initialized with empty spaces
        board = [["." for _ in range(8)] for _ in range(8)]
        
        # Place pieces on the board
        for color, pieces in self.pieces.items():
            for piece in pieces:
                symbol = piece.piece_type.upper() if color == "w" else piece.piece_type.lower()
                board[8 - piece.row][piece.file - 1] = symbol
        
        # Convert the board into a string
        board_str = "\n".join([" ".join(row) for row in board])
        return board_str


    @staticmethod
    def inverse_color(color):
        if color == 'w':
            return 'b'
        else:
            return 'w'
