import pygame
import time
import sys
import numpy as np
import math
try:
    import vrep
except:
    print ('--------------------------------------------------------------')
    print ('"vrep.py" could not be imported.')
    print ('--------------------------------------------------------------')

h = np.array([0]*6)
pos_1 = [0, 0, 0]
ori_1 = [0, 0, 0]

BLACK = (0, 0, 0)
WHITE = (255, 255, 255)

pygame.init()
size = [1000, 1000]
screen = pygame.display.set_mode(size)
pygame.display.set_caption("Simulation")
clock = pygame.time.Clock()
pygame.joystick.init()
speed = 5
name_object = "target"


class TextPrint:
    def __init__(self):
        self.reset()
        self.font = pygame.font.Font(None, 40)

    def print(self, screen, textString):
        textBitmap = self.font.render(textString, True, BLACK)
        screen.blit(textBitmap, [self.x, self.y])
        self.y += self.line_height

    def reset(self):
        self.x = 10
        self.y = 10
        self.line_height = 30

    def indent(self):
        self.x += 10

    def unindent(self):
        self.x -= 10


textPrint = TextPrint()


def connect_vrep():
    print('Program started')
    vrep.simxFinish(-1)
    global clientID
    clientID = vrep.simxStart('127.0.0.1', 19999, True, True, 5000, 5)  # Connect to V-REP
    if clientID != -1:
        print('Connected to remote API server')

        number_returned_0, h[0] = vrep.simxGetObjectHandle(clientID, name_object, vrep.simx_opmode_blocking)
        res, pos = vrep.simxGetObjectPosition(clientID, h[0], vrep.sim_handle_parent, vrep.simx_opmode_oneshot_wait)

    else:
        print('Failed connecting to remote API server')


def close_program():
    vrep.simxStopSimulation(clientID, vrep.simx_opmode_oneshot)
    vrep.simxGetPingTime(clientID)
    vrep.simxFinish(clientID)
    pygame.quit()
    sys.exit()


def main_start():
    connect_vrep()
    while 1:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                close_program()

        screen.fill(WHITE)
        textPrint.reset()

        joystick_count = pygame.joystick.get_count()

        textPrint.print(screen, "Number of joysticks: {}".format(joystick_count))
        textPrint.indent()

        for i in range(joystick_count):
            joystick = pygame.joystick.Joystick(i)
            joystick.init()

            textPrint.print(screen, "Joystick {}".format(i))
            textPrint.indent()

            name = joystick.get_name()
            textPrint.print(screen, "Joystick name: {}".format(name))

            axes = joystick.get_numaxes()
            textPrint.print(screen, "Number of axes: {}".format(axes))
            textPrint.indent()

            for i in range(axes):
                axis = round(joystick.get_axis(i)*10)/10
                # textPrint.print(screen, "Axis {} value: {:>6.3f}".format(i, axis))
                textPrint.print(screen, "Axis {} value: {}".format(i, axis))
                if axis != 0:
                    pos_1[i] += speed * axis

                    if pos_1[2] > 90:
                        pos_1[2] = 90
                    if pos_1[2] < -15:
                        pos_1[2] = -15

                    if pos_1[1] > 40:
                        pos_1[1] = 40
                    if pos_1[1] < -90:
                        pos_1[1] = -90

                    vrep.simxSetObjectPosition(clientID, h[0], -1, pos_1, vrep.simx_opmode_streaming)
            textPrint.unindent()

            buttons = joystick.get_numbuttons()
            textPrint.print(screen, "Number of buttons: {}".format(buttons))
            textPrint.indent()

            for i in range(buttons):
                button = joystick.get_button(i)
                textPrint.print(screen, "Button {:>2} value: {}".format(i, button))
            textPrint.unindent()

            hats = joystick.get_numhats()
            textPrint.print(screen, "Number of hats: {}".format(hats))
            textPrint.indent()

            for i in range(hats):
                hat = joystick.get_hat(i)
                textPrint.print(screen, "Hat {} value: {}".format(i, str(hat)))
                if hat[0] == 1:
                    ori_1[0] += 1
                elif hat[0] == -1:
                    ori_1[0] -= 1
                if hat[1] == 1:
                    ori_1[1] += 1
                elif hat[1] == -1:
                    ori_1[1] -= 1

            #vrep.simxSetJointPosition(clientID, h[4], pos_1[4] * math.pi / 180, vrep.simx_opmode_streaming)
            vrep.simxSetObjectPosition(clientID, h[0], -1, ori_1, vrep.simx_opmode_streaming)
            #vrep.simxSetJointPosition(clientID, h[5], pos_1[5] * math.pi / 180, vrep.simx_opmode_streaming)
            textPrint.unindent()

            textPrint.unindent()

        pygame.display.flip()
        clock.tick(30)


if __name__ == "__main__":
    mane = main_start()
    #0. 360
    #1. 40 a -90
    #2. 90 a -15
    #3. 360
    #4. 120 a -90
    #5. 360