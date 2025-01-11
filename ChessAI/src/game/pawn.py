

class Pawn:
    def __init__(self,color,file,row):
        self.color = color #lets code 'w' for whites and 'b' for blacks
        self.piece_type='p'
        self.file = file
        self.row = row
        # coding first move
        if self.color == 'w':
            if self.row!=2:
                self.first_move = 0
            else:
                self.first_move = 1 # 1 if the pawn hasn't moved yet and 0 if it has
        else:
            if self.row!=7:
                self.first_move = 0
            else:
                self.first_move = 1 # 1 if the pawn hasn't moved yet and 0 if it has


    def Move(self,move): #move = [file,row]
        self.file = move[0]
        self.row = move[1]
        if self.first_move == 1:
            self.first_move = 0

    def vision(self,board):

        vision=[] #list of moves for the pown given the board (= {'w':['w' pieces],'b':['b' pieces]} ) wether or not if the
                  #piece is pinned by the king

        white_position = board.get_one_color_position('w')
        black_position = board.get_one_color_position('b')
        all_pieces_position = black_position + white_position


        if self.color=='w':
            #the white pawn can move one up if there is no piece in front of it or if it's not pinned, but the pin rule will be
            #handled during the game play part of the code
            if [self.file,self.row+1] not in all_pieces_position:
                vision.append([self.file,self.row+1])

            #it can takes if there is a black piece one square up on his diagonals
            if [self.file-1,self.row+1] in black_position:
                vision.append([self.file-1,self.row+1])
            if [self.file+1,self.row+1] in black_position:
                vision.append([self.file+1,self.row+1])

            # it can move two squares up if it's his first move
            if self.first_move==1 and [self.file,self.row+1] not in all_pieces_position and [self.file,self.row+2] not in all_pieces_position:
                vision.append([self.file,self.row+2])

            #We still have to take care of en passant
            if self.row == 5 and board.moves!=[]:

                piece_to_take_left = board.get_piece_from_position([self.file-1,5])
                piece_to_take_right = board.get_piece_from_position([self.file+1,5])

                if piece_to_take_left!=0 and piece_to_take_left.color == 'b' and piece_to_take_left.piece_type == 'p' and board.moves[-1]==([self.file-1,7],[self.file-1,5]):
                    vision.append([self.file-1,self.row+1])
                elif piece_to_take_right!=0 and piece_to_take_right.color == 'b' and piece_to_take_right.piece_type == 'p' and board.moves[-1]==([self.file+1,7],[self.file+1,5]):
                    vision.append([self.file+1,self.row+1])
        else:
            #the black pawn can move one down if there is no piece in front of it
            if [self.file,self.row-1] not in all_pieces_position:
                vision.append([self.file,self.row-1])

            #it can takes if there is a white piece one square down on his diagonals
            if [self.file-1,self.row-1] in white_position:
                vision.append([self.file-1,self.row-1])
            if [self.file+1,self.row-1] in white_position:
                vision.append([self.file+1,self.row-1])

            # it can move two squares down if it's his first move
            if self.first_move==1 and [self.file,self.row-1] not in all_pieces_position and [self.file,self.row-2] not in all_pieces_position:
                vision.append([self.file,self.row-2])

            #We still have to take care of en passant
            if self.row == 4 and board.moves!=[]:
                piece_to_take_left = board.get_piece_from_position([self.file-1,4])
                piece_to_take_right = board.get_piece_from_position([self.file+1,4])

                if piece_to_take_left!=0 and piece_to_take_left.color == 'w' and piece_to_take_left.piece_type == 'p' and board.moves[-1]==([self.file-1,2],[self.file-1,4]):
                    vision.append([self.file-1,self.row-1])
                elif piece_to_take_right!=0 and piece_to_take_right.color == 'w' and piece_to_take_right.piece_type == 'p' and board.moves[-1]==([self.file+1,2],[self.file+1,4]):
                    vision.append([self.file+1,self.row-1])

        return vision
