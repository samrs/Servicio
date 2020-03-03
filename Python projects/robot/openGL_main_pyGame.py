from OpenGL.GLU import *#@UnusedWildImport

# Local class imports
from mat_obj_loader import * #@UnusedWildImport
from camera import * #@UnusedWildImport
from rayTrace import * #@UnusedWildImport

pygame.init()
'''-----------------------------Variables----------------------------------------------------'''
#global object
game = game_screen(1500, 1200) #window size

#global variables
angle = np.array([0.0]*5)#s_a,b_a,b001_a,w_a,g_a
gripper_angles = np.sort(np.arange(-90, 90, 5, dtype=int)) # a list of gripper angles
list_count = 0 #a counter for the gripper angle

'''------------------------------ functions -----------------------------------------------------------'''
#initializes screen and loads obj files
def init():
    global clock, room_T_B, room_F_B_B, room_L_R_B, room_FB_T, room_LR_T,room_sill, base, shoulder, bicep, bicep001, wrist, gRail, gripper, gripper001
    glClearColor(0.5, 0.5, 0.5, 1.0) #Gray Background
    
    #light 0
    glLightfv(GL_LIGHT0, GL_POSITION,  (-100, 500,  0, 1.0))
    glLightfv(GL_LIGHT0, GL_AMBIENT, (0.2, 0.2, 0.2, 1.0))
    glLightfv(GL_LIGHT0, GL_DIFFUSE, (.1, .1, .1, .1))
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, [0.2, 0.2, 0.2, 1.0])
    glEnable(GL_LIGHT0)
   
    #Light 1
    glLightfv(GL_LIGHT1, GL_AMBIENT,  [0.0, 0.0, 0.0, 1.0]) # R G B A
    glLightfv(GL_LIGHT1, GL_DIFFUSE,  [.85, .85, .85, 1]) # R G B A
    glLightfv(GL_LIGHT1, GL_POSITION, [300, 550, 300, 0.0]) # x y z w y = height, z = distance
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, [0.2, 0.2, 0.2, 1.0])
    glEnable(GL_LIGHT1)
    
    glEnable(GL_LIGHTING)
    glEnable(GL_DEPTH_TEST)
    glShadeModel(GL_SMOOTH)
         
    # LOAD OBJECT AFTER PYGAME INIT
    room_T_B = OBJ(PATH+'room_T_B.obj', swapyz=False, my_texture = True, use_mat = False)
    room_F_B_B = OBJ(PATH+'room_F_B_B.obj', swapyz=False, my_texture = True, use_mat = False)
    room_L_R_B = OBJ(PATH+'room_LR_B.obj', swapyz=False, my_texture = True, use_mat = False)
    room_FB_T = OBJ(PATH+'room_FB_T.obj', swapyz=False, my_texture = True, use_mat = False)
    room_LR_T = OBJ(PATH+'room_LR_T.obj', swapyz=False, my_texture = True, use_mat = False)
    room_sill = OBJ(PATH+'room_sill.obj', swapyz=False, my_texture = True, use_mat = False)
    base = OBJ(PATH+'base_bm.obj', swapyz=False, my_texture=False, use_mat = False)
    shoulder = OBJ(PATH+'shoulder_b.obj', swapyz=False, my_texture=False, use_mat = False)
    bicep = OBJ(PATH+'bicep_b.obj', swapyz=False, my_texture=False, use_mat = False)
    bicep001 = OBJ(PATH+'bicep001_b.obj', swapyz=False, my_texture=False, use_mat = False)
    wrist = OBJ(PATH+'wrist_b.obj', swapyz=False, my_texture=False, use_mat = False)
    gRail = OBJ(PATH+'grail_b.obj', swapyz=False, my_texture=False, use_mat = False)
    gripper = OBJ(PATH+'gripper_b.obj', swapyz=False, my_texture=False, use_mat = False)
    gripper001 = OBJ(PATH+'gripper001_b.obj', swapyz=False, my_texture=False, use_mat = False)

    clock = pygame.time.Clock()
    
    glMatrixMode(GL_PROJECTION)
    glLoadIdentity()
    gluPerspective(45.0, game.width/float(game.height), 1, 2000.0)
    glEnable(GL_DEPTH_TEST)
    glMatrixMode(GL_MODELVIEW)
    glEnable(GL_COLOR_MATERIAL)

#sets lighting for focul objects
def set_specular():
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT)
    glColor(.2, .2, .2)
    glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE)
    glColor(.3,1,.1) 
    glColorMaterial(GL_FRONT_AND_BACK, GL_SPECULAR)
    glColor(.75,.75,.75)
    glColorMaterial(GL_FRONT_AND_BACK, GL_EMISSION)
    glColor(.1,.1,.1)
    glMaterial(GL_FRONT_AND_BACK, GL_SHININESS, 128 )

#draw the robot and update the angles    
def render_robot():    
    #begin render robot
    glPushMatrix()
    set_specular()
    glRotate(-90, 1, 0, 0)
    glTranslate(-99.5, -131, 0)
    glCallList(base.gl_list)

    glPushMatrix()
    shoulder_rot(angle[0])#this rotates backwards
    bicep_rot(angle[1])
    bicep001_rot(angle[2])
    wrist_rot(angle[3])
    #check to see if captured if it is draw the captured ball
    gripper_mov(angle[4])
    
    glPopMatrix()
    glPopMatrix()

def calc_angle(a, speed=1):
    a = a*speed
    if a > 360:
        a -= 360
    return a  

'''functions to rotate robot joints'''
def shoulder_rot(a=0):
    glTranslate(99, 131.75, 87.4)
    glRotatef(a, 0.0, 0, 1)
    glTranslate(-99, -131.75, -87.4)
    glCallList(shoulder.gl_list)
    
def bicep_rot(a=0):
    glTranslate(99, 131.75, 128.9)
    glRotatef(a, .98052, -0.1964, 0)
    glTranslate(-99, -131.75, -128.9)
    glCallList(bicep.gl_list)
    
def bicep001_rot(a=0):
    glTranslate(105.11, 162.26, 230.96)
    glRotatef(a, .98052,-0.1964, 0)
    glTranslate(-105.11, -162.26, -230.96) 
    glCallList(bicep001.gl_list)
    
def wrist_rot(a=0):#86.6, 180.29, 70.3
    glTranslate(86.6, 70.3, 180.29)
    glRotatef(a, .98052,  -0.1964, 0)
    glTranslate(-86.6, -70.3, -180.29) 
    
    glCallList(wrist.gl_list)
    glCallList(gRail.gl_list)
    
def gripper_mov(a=0):
    glTranslate(a*.9805, a*-0.1964, 0)
    glCallList(gripper.gl_list)
    glTranslate(-a*1.961,  a*0.3928, 0)
    glCallList(gripper001.gl_list)

#the main display function calls methods to move robot
#@todo: put this in a class and get rid of globals
def display():
    global  space_bar
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT)
    glLoadIdentity()
    
    #set the camera position
    control(0.1, 0.1, mouse_down)

    #render new camera position
    update_camera()

    #render room
    #@todo:fix the room walls need own material
    glCallList(room_T_B.gl_list)
    glCallList(room_F_B_B.gl_list)
    glCallList(room_L_R_B.gl_list)
    glCallList(room_FB_T.gl_list)
    glCallList(room_LR_T.gl_list)
    glCallList(room_sill.gl_list)

    # save start up atributes
    glPushAttrib(GL_CURRENT_BIT)
    glPushAttrib(GL_LIGHTING_BIT)
     
    #draw robot and ball if ball is grabbed 
    render_robot()

    #pop start up atributes
    glPopAttrib()
    glPopAttrib()

#main function to handle loop and keyboard the camera also uses keyboard
class main():
    def __init__(self):
        self.run_game()
    
    def run_game(self):
        global mouse_down
        init()
        while 1:
            #clock.tick(60)#modificar los frames
            for e in pygame.event.get():
                if e.type == QUIT:
                    sys.exit()
        
                elif e.type == KEYDOWN:
                    #keypress = True # this may be used for camera collision detection
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
                    #keypress = False #this will be used for camera collision detection
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
            #get keypress
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
    
            #get angle rotation
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