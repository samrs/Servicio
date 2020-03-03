import pygame #@UnusedImport
from pygame.constants import * #@UnusedWildImport

class game_screen():
    def __init__(self, width, height):
        self.viewport = (width, height)
        self.width = width
        self.height = height
        self.srf = pygame.display.set_mode(self.viewport, OPENGL | DOUBLEBUF )#FULLSCREEN | HWSURFACE )
