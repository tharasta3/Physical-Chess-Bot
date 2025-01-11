import pygame.sprite
import numpy as np

pos_dict = {}
img_pos_list = [(108, 892),(213, 890),(326, 889),(437, 893),(554, 891),(659, 890),(780, 888),(892, 884),(108, 767),(212, 774),(323, 779),(443, 774),(557, 768),(661, 772),(775, 770),(890, 774),(104, 667),(215, 660),(331, 664),(443, 661),(555, 660),(667, 659),(773, 658),(893, 658),(94, 549),(216, 549),(328, 546),(443, 546),(550, 547),(666, 548),(777, 545),(900, 547),(100, 435),(217, 437),(328, 438),(436, 435),(553, 439),(661, 436),(783, 439),(890, 437),(98, 322),(219, 327),(333, 325),(439, 326),(551, 327),(667, 327),(776, 328),(890, 327),(98, 208),(216, 211),(330, 212),(439, 214),(557, 210),(667, 212),(780, 210),(893, 217),(103, 100),(214, 97),(326, 100),(437, 102),(557, 101),(670, 100),(776, 99),(893, 105)]

k=0
for i in range(1,9):
    for j in range(1,9):
        pos_dict[str([i,j])] = img_pos_list[k]
        k+=1

def chess_pos_to_image_pos(pos):
    j,i = pos[0],pos[1]
    x = np.floor(49+112.5*(i-1)+112.5/2-40)
    y = np.floor(949-112.5*(j-1)-112.5/2-40)
    return (int(x),int(y))



class GUI_pieces(pygame.sprite.Sprite):
    def __init__(self,type,color,chess_pos):
        super().__init__()
        self.type = type
        self.color = color
        self.chess_pos = chess_pos
        self.image_pos = chess_pos_to_image_pos(chess_pos)
        self.pos_rect = pygame.Rect(self.image_pos[0], self.image_pos[1], 80, 80)

        if self.type == 'p':
            if self.color == 'b':
                self.image = pygame.transform.scale(pygame.image.load("src/graphic_interface/b_pawn_png_128px.png").convert_alpha(),(80,80))
            elif self.color=='w':
                self.image = pygame.transform.scale(pygame.image.load("src/graphic_interface/w_pawn_png_128px.png").convert_alpha(),(80,80))

        if self.type == 'K':
            if self.color == 'b':
                self.image = pygame.transform.scale(pygame.image.load("src/graphic_interface/b_king_png_128px.png").convert_alpha(),(80,80))
            elif self.color=='w':
                self.image = pygame.transform.scale(pygame.image.load("src/graphic_interface/w_king_png_128px.png").convert_alpha(),(80,80))

        if self.type == 'Q':
            if self.color == 'b':
                self.image = pygame.transform.scale(pygame.image.load("src/graphic_interface/b_queen_png_128px.png").convert_alpha(),(80,80))
            elif self.color=='w':
                self.image = pygame.transform.scale(pygame.image.load("src/graphic_interface/w_queen_png_128px.png").convert_alpha(),(80,80))

        if self.type == 'b':
            if self.color == 'b':
                self.image = pygame.transform.scale(pygame.image.load("src/graphic_interface/b_bishop_png_128px.png").convert_alpha(),(80,80))
            elif self.color=='w':
                self.image = pygame.transform.scale(pygame.image.load("src/graphic_interface/w_bishop_png_128px.png").convert_alpha(),(80,80))

        if self.type == 'k':
            if self.color == 'b':
                self.image = pygame.transform.scale(pygame.image.load("src/graphic_interface/b_knight_png_128px.png").convert_alpha(),(80,80))
            elif self.color=='w':
                self.image = pygame.transform.scale(pygame.image.load("src/graphic_interface/w_knight_png_128px.png").convert_alpha(),(80,80))

        if self.type == 'r':
            if self.color == 'b':
                self.image = pygame.transform.scale(pygame.image.load("src/graphic_interface/b_rook_png_128px.png").convert_alpha(),(80,80))
            elif self.color=='w':
                self.image = pygame.transform.scale(pygame.image.load("src/graphic_interface/w_rook_png_128px.png").convert_alpha(),(80,80))
