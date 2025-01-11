import pygame
from pygame.locals import *
import numpy as np
import src.graphic_interface.cevent as cevent
import src.graphic_interface.gui_pieces as gui_pieces

class App(cevent.CEvent,gui_pieces.GUI_pieces):
    global move
    def __init__(self, pieces):
        self._running = True
        self._display_surf = None
        self._image_surf = None
        self.pieces = pieces

    def on_init(self):
        pygame.init()
        self._display_surf = pygame.display.set_mode((1000,1000), pygame.HWSURFACE)
        pygame.display.set_caption("Chess game")
        self._running = True
        self.moving = False
        self.moving_piece = None

        'loading board image'
        self._image_surf = pygame.image.load("src/graphic_interface/Chessboard.jpeg").convert()
        self._display_surf.blit(self._image_surf,(0,0))

        'loading pieces'
        self.white_pieces=[]
        for piece in self.pieces['w']:
            self.white_pieces.append(gui_pieces.GUI_pieces(piece.piece_type, 'w', [piece.row,piece.file]))
        self.black_pieces=[]
        for piece in self.pieces['b']:
            self.black_pieces.append(gui_pieces.GUI_pieces(piece.piece_type, 'b', [piece.row,piece.file]))

    def on_loop(self):
        pass

    def on_render(self, color_to_play):
        self._display_surf.blit(self._image_surf,(0,0))
        if color_to_play == 'b':
            for piece in self.white_pieces:
                self._display_surf.blit(piece.image,piece.image_pos)
                # pygame.draw.rect(self._display_surf, (255,0,0), piece.pos_rect, 1)
            for piece in self.black_pieces:
                self._display_surf.blit(piece.image,piece.image_pos)
        else:
            for piece in self.black_pieces:
                self._display_surf.blit(piece.image,piece.image_pos)
            for piece in self.white_pieces:
                self._display_surf.blit(piece.image,piece.image_pos)

        pygame.display.update()

    def on_cleanup(self):
        pygame.quit()

    def on_exit(self):
        self._running = False

    def on_execute(self, color_to_play):
        if self.on_init() == False:
            self._running = False

        while( self._running ):
            for event in pygame.event.get():
                if event.type == QUIT:
                    self.on_exit()
                elif event.type == MOUSEBUTTONDOWN:
                    if event.button == 1:

                        for piece in self.white_pieces:
                            if (piece.pos_rect).collidepoint(event.pos):
                                self.moving = True
                                self.moving_piece = piece
                                self.start_pos = pygame.mouse.get_pos()
                        for piece in self.black_pieces:
                            if (piece.pos_rect).collidepoint(event.pos):
                                self.moving = True
                                self.moving_piece = piece
                                self.start_pos = pygame.mouse.get_pos()

                    if event.button ==3:
                        print(pygame.mouse.get_pos())
                elif event.type == MOUSEBUTTONUP:
                    if event.button == 1 and self.moving:
                        self.moving = False
                        return [image_pos_to_chess_pos(self.start_pos),image_pos_to_chess_pos(pygame.mouse.get_pos())]
                elif event.type == MOUSEMOTION and self.moving:
                    self.moving_piece.image_pos = tuple(map(lambda x, y: x + y, self.moving_piece.image_pos, event.rel))
                    (self.moving_piece.pos_rect).move_ip(event.rel)
            self.on_loop()
            self.on_render(color_to_play)

    def get_pieces(self, color_to_play):
        'loading pieces'
        if color_to_play == 'b':
            self.white_pieces=[]
            for piece in self.pieces['w']:
                self.white_pieces.append(gui_pieces.GUI_pieces(piece.piece_type, 'w', [piece.row,piece.file]))
            self.black_pieces=[]
            for piece in self.pieces['b']:
                self.black_pieces.append(gui_pieces.GUI_pieces(piece.piece_type, 'b', [piece.row,piece.file]))
        else:
            self.black_pieces=[]
            for piece in self.pieces['b']:
                self.white_pieces.append(gui_pieces.GUI_pieces(piece.piece_type, 'w', [piece.row,piece.file]))
            self.white_pieces=[]
            for piece in self.pieces['w']:
                self.black_pieces.append(gui_pieces.GUI_pieces(piece.piece_type, 'b', [piece.row,piece.file]))

def image_pos_to_chess_pos(pos):
    x, y = pos[0],pos[1]
    j = int(np.floor(1-(y+40+112.5/2-949)/112.5))
    i = int(np.floor(1+(x-49-112.5/2+40)/112.5))
    return [i,j+1]


if __name__ == "__main__" :
    theApp = App()
    theApp.on_execute()
