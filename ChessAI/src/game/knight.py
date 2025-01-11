import itertools

class Knight:
    def __init__(self,color,file,row):
        self.color = color #lets code 'w' for whites and 'b' for blacks
        self.piece_type='k'
        self.file = file
        self.row = row

    def Move(self,move): #move = [file,row]
        self.file = move[0]
        self.row = move[1]

    def vision(self,board):
        vision=[]

        white_position = board.get_one_color_position('w')
        black_position = board.get_one_color_position('b')

        if self.color=='w':
            for i,j in itertools.product([-1,1],[-2,2]):
                if self.file+i<9 and self.file+i>0 and self.row+j<9 and self.row+j>0 and [self.file+i,self.row+j] not in white_position:
                    vision.append([self.file+i,self.row+j])

            for i,j in itertools.product([-2,2],[-1,1]):
                if self.file+i<9 and self.file+i>0 and self.row+j<9 and self.row+j>0 and [self.file+i,self.row+j] not in white_position:
                    vision.append([self.file+i,self.row+j])

        else:
            for i,j in itertools.product([-1,1],[-2,2]):
                if self.file+i<9 and self.file+i>0 and self.row+j<9 and self.row+j>0 and [self.file+i,self.row+j] not in black_position:
                    vision.append([self.file+i,self.row+j])

            for i,j in itertools.product([-2,2],[-1,1]):
                if self.file+i<9 and self.file+i>0 and self.row+j<9 and self.row+j>0 and [self.file+i,self.row+j] not in black_position:
                    vision.append([self.file+i,self.row+j])
        return vision
