

class Queen:
    def __init__(self,color,file,row):
        self.color = color #lets code 'w' for whites and 'b' for blacks
        self.piece_type='Q'
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
            i=1
            while self.file+i<9 and self.row+i<9:
                if [self.file+i,self.row+i] in white_position:
                    break
                elif [self.file+i,self.row+i] in black_position:
                    vision.append([self.file+i,self.row+i])
                    break
                else:
                    vision.append([self.file+i,self.row+i])
                    i+=1
            i=1
            while self.file-i>0 and self.row-i>0:
                if [self.file-i,self.row-i] in white_position:
                    break
                elif [self.file-i,self.row-i] in black_position:
                    vision.append([self.file-i,self.row-i])
                    break
                else:
                    vision.append([self.file-i,self.row-i])
                    i+=1

            j=1
            while self.file+j<9 and self.row-j>0:
                if [self.file+j,self.row-j] in white_position:
                    break
                elif [self.file+j,self.row-j] in black_position:
                    vision.append([self.file+j,self.row-j])
                    break
                else:
                    vision.append([self.file+j,self.row-j])
                    j+=1
            j=1
            while self.file-j>0 and self.row+j<9:
                if [self.file-j,self.row+j] in white_position:
                    break
                elif [self.file-j,self.row+j] in black_position:
                    vision.append([self.file-j,self.row+j])
                    break
                else:
                    vision.append([self.file-j,self.row+j])
                    j+=1

            i=1
            while self.file+i<9:
                if [self.file+i,self.row] in white_position:
                    break
                elif [self.file+i,self.row] in black_position:
                    vision.append([self.file+i,self.row])
                    break
                else:
                    vision.append([self.file+i,self.row])
                    i+=1
            i=1
            while self.file-i>0:
                if [self.file-i,self.row] in white_position:
                    break
                elif [self.file-i,self.row] in black_position:
                    vision.append([self.file-i,self.row])
                    break
                else:
                    vision.append([self.file-i,self.row])
                    i+=1

            j=1
            while self.row+j<9:
                if [self.file,self.row+j] in white_position:
                    break
                elif [self.file,self.row+j] in black_position:
                    vision.append([self.file,self.row+j])
                    break
                else:
                    vision.append([self.file,self.row+j])
                    j+=1
            j=1
            while self.row-j>0:
                if [self.file,self.row-j] in white_position:
                    break
                elif [self.file,self.row-j] in black_position:
                    vision.append([self.file,self.row-j])
                    break
                else:
                    vision.append([self.file,self.row-j])
                    j+=1
        else:

            i=1
            while self.file+i<9 and self.row+i<9:
                if [self.file+i,self.row+i] in black_position:
                    break
                elif [self.file+i,self.row+i] in white_position:
                    vision.append([self.file+i,self.row+i])
                    break
                else:
                    vision.append([self.file+i,self.row+i])
                    i+=1
            i=1
            while self.file-i>0 and self.row-i>0:
                if [self.file-i,self.row-i] in black_position:
                    break
                elif [self.file-i,self.row-i] in white_position:
                    vision.append([self.file-i,self.row-i])
                    break
                else:
                    vision.append([self.file-i,self.row-i])
                    i+=1

            j=1
            while self.file+j<9 and self.row-j>0:
                if [self.file+j,self.row-j] in black_position:
                    break
                elif [self.file+j,self.row-j] in white_position:
                    vision.append([self.file+j,self.row-j])
                    break
                else:
                    vision.append([self.file+j,self.row-j])
                    j+=1
            j=1
            while self.file-j>0 and self.row+j<9:
                if [self.file-j,self.row+j] in black_position:
                    break
                elif [self.file-j,self.row+j] in white_position:
                    vision.append([self.file-j,self.row+j])
                    break
                else:
                    vision.append([self.file-j,self.row+j])
                    j+=1

            i=1
            while self.file+i<9:
                if [self.file+i,self.row] in black_position:
                    break
                elif [self.file+i,self.row] in white_position:
                    vision.append([self.file+i,self.row])
                    break
                else:
                    vision.append([self.file+i,self.row])
                    i+=1
            i=1
            while self.file-i>0:
                if [self.file-i,self.row] in black_position:
                    break
                elif [self.file-i,self.row] in white_position:
                    vision.append([self.file-i,self.row])
                    break
                else:
                    vision.append([self.file-i,self.row])
                    i+=1

            j=1
            while self.row+j<9:
                if [self.file,self.row+j] in black_position:
                    break
                elif [self.file,self.row+j] in white_position:
                    vision.append([self.file,self.row+j])
                    break
                else:
                    vision.append([self.file,self.row+j])
                    j+=1
            j=1
            while self.row-j>0:
                if [self.file,self.row-j] in black_position:
                    break
                elif [self.file,self.row-j] in white_position:
                    vision.append([self.file,self.row-j])
                    break
                else:
                    vision.append([self.file,self.row-j])
                    j+=1

        return vision
