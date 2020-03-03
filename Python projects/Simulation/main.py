from pygame.constants import *
from OpenGL.GLU import *
from update_camera import*

# IMPORT OBJECT LOADER
from objloader import *
PATH = '/home/samuel/Downloads/Kuka_CAD_IIMAS/Partes/'

pygame.init()
viewport = (1700, 1500)
hx = viewport[0]
hy = viewport[1]

srf = pygame.display.set_mode(viewport, OPENGL | DOUBLEBUF)

angle = np.array([0.0]*5)

rx, ry = (0,0)
tx, ty = (0,0)
zpos = 90
rotate = move = False

def init():
    global part1, part2, part3, part4, part5, part6, clock
    glClearColor(0, 0, 0, 1)  # Gray Background
    # light 0
    glLightfv(GL_LIGHT0, GL_POSITION, (-100, 500, 0, 1.0))
    glLightfv(GL_LIGHT0, GL_AMBIENT, (0.2, 0.2, 0.2, 1.0))
    glLightfv(GL_LIGHT0, GL_DIFFUSE, (.1, .1, .1, .1))
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, [0.2, 0.2, 0.2, 1.0])
    glEnable(GL_LIGHT0)

    # Light 1
    glLightfv(GL_LIGHT1, GL_AMBIENT, [0.0, 0.0, 0.0, 1.0])  # R G B A
    glLightfv(GL_LIGHT1, GL_DIFFUSE, [.85, .85, .85, 1])  # R G B A
    glLightfv(GL_LIGHT1, GL_POSITION, [300, 550, 300, 0.0])  # x y z w y = height, z = distance
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, [0.2, 0.2, 0.2, 1.0])
    glEnable(GL_LIGHT1)

    glEnable(GL_LIGHTING)
    glEnable(GL_DEPTH_TEST)
    glShadeModel(GL_SMOOTH)

    glEnable(GL_COLOR_MATERIAL)

    #part1 = OBJ("part_01b.obj", swapyz=False)
    part2 = OBJ(PATH + "part_02b.obj", swapyz=False, my_texture=False, use_mat=False)
    part3 = OBJ(PATH + "part_03b.obj", swapyz=False, my_texture=False, use_mat=False)
    #part4 = OBJ(PATH + "part_04b.obj", swapyz=False, my_texture=False, use_mat=False)
    # part5 = OBJ("part_05b.obj", swapyz=False)
    # part6 = OBJ("part_06b.obj", swapyz=False)

    clock = pygame.time.Clock()

    glMatrixMode(GL_PROJECTION)
    glLoadIdentity()
    width, height = viewport
    gluPerspective(20.0, width / float(height), 1, 1000)
    glEnable(GL_DEPTH_TEST)
    glMatrixMode(GL_MODELVIEW)

def calc_angle(a, speed=1):
    a = a*speed
    if a > 360:
        a -= 360
    return a

def display():
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
    glLoadIdentity()

    control(0.05, 0.05, mouse_down)

    update_camera()

    render_robot()

def render_robot():
    glTranslate(0, 0, - zpos)
    glRotate(90, 0, 1, 0)
    #glCallList(part1.gl_list)

    part2_rot(angle[0])
    part3_rot(angle[1])


def part2_rot(angle_rot):
    glTranslate(0, 0, 0)
    glRotatef(angle_rot, 0, 1, 0)
    glTranslate(0, 0, 0)
    glCallList(part2.gl_list)

def part3_rot(angle_rot):
    glTranslate(0, 6, 1)
    glRotatef(angle_rot, 1, 0, 0)
    glTranslate(0, -6, -1)
    glCallList(part3.gl_list)

class main():
    global mouse_down
    init()
    while 1:
        clock.tick(60)  # modificar los frames
        for e in pygame.event.get():
            if e.type == QUIT:
                sys.exit()
            elif e.type == KEYDOWN:
                if e.key == K_ESCAPE:
                    sys.exit()
                elif e.key == K_q:
                    button[0] = 1
                elif e.key == K_w:
                    button[1] = 1
                elif e.key == K_e:
                    button[2] = 1
                elif e.key == K_s:
                    button[3] = 1

            elif e.type == KEYUP:
                if e.key == K_ESCAPE:
                    sys.exit()
                elif e.key == K_q:
                    button[0] = 0
                elif e.key == K_w:
                    button[1] = 0
                elif e.key == K_e:
                    button[2] = 0
                elif e.key == K_s:
                    button[3] = 0

            elif e.type == MOUSEBUTTONUP:
                pygame.mouse.set_visible(True)
                mouse_down = False
            elif e.type == MOUSEBUTTONDOWN:
                mouse_down = True


        # get keypress
        a_s = a_b = a_b001 = a_w = a_g = 0
        kp = pygame.key.get_pressed()
        if kp[pygame.K_F1]:
            a_s = 1
        if kp[pygame.K_1]:
            a_s = -1
        if kp[pygame.K_F2]:
            a_b = 1
        if kp[pygame.K_2]:
            a_b = -1
        if kp[pygame.K_F3]:
            a_b001 = 1
        if kp[pygame.K_3]:
            a_b001 = -1
        if kp[pygame.K_F4]:
            a_w = 1
        if kp[pygame.K_4]:
            a_w = -1
        if kp[pygame.K_a]:
            a_g = 1
        if kp[pygame.K_d]:
            a_g = -1

            # get angle rotation
        angle[0] += calc_angle(a_s)
        angle[1] += calc_angle(a_b)
        angle[2] += calc_angle(a_b001)
        angle[3] += calc_angle(a_w)
        angle[4] += calc_angle(a_g)
        if angle[4] > 17:
            angle[4] = 17
        elif angle[4] < 1:
            angle[4] = 1

        display()

        pygame.display.flip()

if __name__ == "__main__":
    mane = main()

'''
for e in pygame.event.get():
    if e.type == QUIT:
        sys.exit()
    elif e.type == KEYDOWN and e.key == K_ESCAPE:
        sys.exit()

    elif e.type == MOUSEBUTTONDOWN:
        if e.button == 4:
            zpos = max(1, zpos - 1)
        elif e.button == 5:
            zpos += 1
        elif e.button == 1:
            rotate = True
        elif e.button == 3:
            move = True
    elif e.type == MOUSEBUTTONUP:
        if e.button == 1:
            rotate = False
        elif e.button == 3:
            move = False
    elif e.type == MOUSEMOTION:
        i, j = e.rel
        if rotate:
            rx += i
            ry += j
        if move:
            tx += i
            ty -= j

glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
glLoadIdentity()

# RENDER OBJECT
glTranslate(tx / 20.0, ty / 20.0, - zpos)
glRotate(ry, 1, 0, 0)
glRotate(rx, 0, 1, 0)

glCallList(part1.gl_list)
glCallList(part2.gl_list)
glCallList(part3.gl_list)
#glCallList(part4.gl_list)
#glCallList(part5.gl_list)
#glCallList(part6.gl_list)
'''''