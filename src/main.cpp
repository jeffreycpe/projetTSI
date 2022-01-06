/*****************************************************************************\
 * TP CPE, 4ETI, TP synthese d'images
 * --------------
 *
 * Programme principal des appels OpenGL
 \*****************************************************************************/

#include "declaration.h"
#include <math.h>       
#include <cstdlib>
//identifiant des shaders
GLuint shader_program_id;
GLuint gui_program_id;

camera cam;


//nombre objet
const int longueur = 500;
const int carre = 50;
const int nombreArbres = (longueur * longueur) / (carre * carre); //nombres d'arbres + de monstre
const int nb_obj = nombreArbres +3; //objets + magicien + sol + triangle de sortilège 
objet3d obj[nb_obj];

//création du texte
const int nb_text = 2;
text text_to_draw[nb_text];

//activation du bouton de déplacement
bool flecheHaut = false;
bool flecheBas = false;
bool flecheDroite = false;
bool flecheGauche = false;

//gestion du saut
bool doitSauter = false;
bool sautEnCours = false;
float abscisse;
float compteurSaut = 0;

//tableau des objets
int numDetraqueur[100];
int numArbres[100];
//tableaux de coordonnées de déplacement des monstres
int depDetraqueurx[100];
int depDetraqueury[100];
int compteurDetra = 0;

//gestion du sort
int sort;
bool sortEnCours = false;

//autorisation de se deplacer
bool autoriserDeplacement = true;

//verification de la vie
bool enVie = true;

/*****************************************************************************\
* initialisation                                                              *
\*****************************************************************************/
static void init()
{
  shader_program_id = glhelper::create_program_from_file("shaders/shader.vert", "shaders/shader.frag"); CHECK_GL_ERROR();

  cam.projection = matrice_projection(60.0f*M_PI/180.0f,1.0f,0.01f,100.0f);
  // cam.tr.translation = vec3(0.0f, 1.0f, 0.0f);
  //cam.tr.translation = vec3(0.0f, 20.0f, 0.0f);
 // cam.tr.rotation_center = vec3(0.0f, 20.0f, 0.0f);
  //cam.tr.rotation_euler = vec3(M_PI/2, 0.0f, 0.0f);
  cam.tr.translation = obj[0].tr.translation;

  //création du magicien et du sol
  init_model_1();
  init_model_2();
  



  //remplissage d'un tableau de coordonnées réparties uniformément sur le terrain indiquant la postion des objets 
  vec2 coordonneeArbres[nombreArbres];
  vec2 tabCoordArbres[nombreArbres];
  vec2 tabCoordDetraqueurs[nombreArbres];
  int i;
  int j;
  int k;

  //boucle de remplissage du tableau des coordonnées 
  for (i = 0; i < longueur/carre; ++i) {
      for (j = 0; j < longueur/carre; ++j) {
          coordonneeArbres[i * (longueur / carre) + j] = vec2(( - (longueur / 2) + (2 * i + 1) * (carre / 2)) + rand()%20 - 20, ( - (longueur / 2) + (2 * j + 1) * (carre / 2)) + rand() % 20 - 20);
      }
  }


  j = 0;
  k = 0;
  //boucle d'attribution des coordonées à un abre ou à un monstre de manière aléatoire
  for(i=0 ; i < nombreArbres ; i++){
      if (rand() % 101 < 80) {
          tabCoordArbres[k] = coordonneeArbres[i];
          k++ ;    
      }
      else {
          tabCoordDetraqueurs[j] = coordonneeArbres[i];
          j++;   
      }
  }
  int nbrDetraqueurs = j;
  int nbrArbres = k;
  k++;
  j++;
  //coordonnée 500 indique le dernier élement de la liste 
  tabCoordArbres[k] = vec2(-500, -500);
  tabCoordDetraqueurs[j] = vec2(-500, -500);
  
  //création des modèles monstres et arbres 
  init_model_3(2, tabCoordDetraqueurs, nbrDetraqueurs);//création de tous les détraqueurs 
  init_model_4(2+nbrDetraqueurs, tabCoordArbres, nbrArbres);//création des arbres
  init_model_5(nombreArbres + 1); //création du triangle de sortilège 

  //atribution des shaders
  gui_program_id = glhelper::create_program_from_file("shaders/gui.vert", "shaders/gui.frag"); CHECK_GL_ERROR();

  // text_to_draw[0].value = "";
  // text_to_draw[0].bottomLeft = vec2(-0.2, 0.5);
  // text_to_draw[0].topRight = vec2(0.2, 1);
  // init_text(text_to_draw);

//   text_to_draw[1]=text_to_draw[0];
//   text_to_draw[1].value = "IS THE BEST";
//   text_to_draw[1].bottomLeft.y = 0.0f;
//   text_to_draw[1].topRight.y = 0.5f;
  }

/*****************************************************************************\
* display_callback                                                           *
\*****************************************************************************/
 static void display_callback()
{
  glClearColor(0.5f, 0.6f, 0.9f, 1.0f); CHECK_GL_ERROR();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); CHECK_GL_ERROR();

  //dsssiner objets 
  for (int i = 0; i < nb_obj; ++i) {
      draw_obj3d(obj + i, cam);
  }
    

  //for(int i = 0; i < nb_text; ++i)
    //draw_text(text_to_draw + i);

  glutSwapBuffers();
}

/*****************************************************************************\
* keyboard_callback                                                           *
\*****************************************************************************/
static void keyboard_callback(unsigned char key, int, int)
{
   
  switch (key)
  {
      
    case 'p':
      glhelper::print_screen();
      break;
    case 'a':
        break;
    case 'A':
        break;
    case 'q': //rotation caméra à gauche
        cam.tr.rotation_euler.y += -0.05 * M_PI;
        break;
    case 'd'://rotation caméra à droite
        cam.tr.rotation_euler.y += 0.05 * M_PI;
        break;
    case 'r'://bouton ressuciter 
        if (enVie == false) {
            obj[0].tr.translation = vec3(-2.0, 0.0f, -10.0);//on se remet à la coordonnée d'origine
            obj[0].tr.rotation_euler = vec3(0.0f, 0.0f, 0.0f); //on se remet droit
            enVie = true;
        }
        break;
    case ' '://bouton de saut
        
        if (!sautEnCours) { //si pas de saut en cours on commence l'algo
            compteurSaut = 0;
            sautEnCours = true;
            doitSauter = true;
        }
        break;
    case 27:
      exit(0);
      break;
  }
}


/*
Faire bouger la sourie
*/
static void mouse_motion(int x, int y) {

    int viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    //abscisse = (static_cast<float>(x) / viewport[2]);
    //abscisse -= 0.5;
    //abscisse *= 2;
    //std::cout <<"x "<< abscisse  << std::endl;
    
    //si on clique sur la souris alors il saute
    if (sortEnCours == false && enVie) {
        sortEnCours = true;
        obj[sort].tr.translation = obj[0].tr.translation;
    }
    
    //std::cout << viewport[2] << std::endl;
}


/*****************************************************************************\
* special_callback                                                            *
\*****************************************************************************/
static void special_callback(int key, int, int)
{
switch (key)
  {
    case GLUT_KEY_UP:
      flecheHaut = true; //rotation avec la touche du haut
      break;
    case GLUT_KEY_DOWN:
      flecheBas = true; //rotation avec la touche du bas
      break;
    case GLUT_KEY_LEFT:
      flecheGauche = true; //rotation avec la touche de gauche
      break;
    case GLUT_KEY_RIGHT:
      flecheDroite = true; //rotation avec la touche de droite
      break;
  }
}

static void special_up_callback(int key, int,int)
{
    



  switch (key)
  {
    case GLUT_KEY_UP:
      flecheHaut = false; //rotation avec la touche du haut
      break;
    case GLUT_KEY_DOWN:
      flecheBas = false; //rotation avec la touche du bas
      break;
    case GLUT_KEY_LEFT:
      flecheGauche = false; //rotation avec la touche de gauche
      break;
    case GLUT_KEY_RIGHT:
      flecheDroite = false; //rotation avec la touche de droite
      break;
  }
}

static void deplacementPerso() {
    cam.tr.rotation_center = obj[0].tr.translation;
    float dL = 1.0f;

    vec3 t = obj[0].tr.translation;
    vec3 suiviCamera = obj[0].tr.translation;
    vec2 vectDeplacement = vec2(dL * cos(cam.tr.rotation_euler.y), dL * sin(cam.tr.rotation_euler.y));
    //Mouvement du personnage en fonction de la caméra

    if (flecheHaut == true) {
        t.z -= vectDeplacement.x;
        t.x += vectDeplacement.y;
        if (flecheDroite == true) {
            obj[0].tr.rotation_euler.y = -cam.tr.rotation_euler.y + 3 * M_PI / 4;
            t.z += vectDeplacement.y / sqrt(2);
            t.x += vectDeplacement.x / sqrt(2);
        }
        else if (flecheGauche == true) {
            obj[0].tr.rotation_euler.y = -cam.tr.rotation_euler.y - 3 * M_PI / 4;
            t.z -= vectDeplacement.y / sqrt(2);
            t.x -= vectDeplacement.x / sqrt(2);
        }
        else {
            obj[0].tr.rotation_euler.y = -cam.tr.rotation_euler.y + M_PI;
        }

    }
    if (flecheBas == true) {
        t.z += vectDeplacement.x;
        t.x -= vectDeplacement.y;
        if (flecheDroite == true) {
            obj[0].tr.rotation_euler.y = -cam.tr.rotation_euler.y + M_PI / 4;
            t.z += vectDeplacement.y / sqrt(2);
            t.x += vectDeplacement.x / sqrt(2);
        }
        else if (flecheGauche == true) {
            obj[0].tr.rotation_euler.y = -cam.tr.rotation_euler.y - M_PI / 4;
            t.z -= vectDeplacement.y / sqrt(2);
            t.x -= vectDeplacement.x / sqrt(2);
        }
        else {
            obj[0].tr.rotation_euler.y = -cam.tr.rotation_euler.y;
        }
    }
    if (flecheGauche == true && flecheHaut == false && flecheBas == false) {

        t.z -= vectDeplacement.y;
        t.x -= vectDeplacement.x;
        obj[0].tr.rotation_euler.y = -cam.tr.rotation_euler.y - M_PI / 2;
    }


    if (flecheDroite == true && flecheHaut == false && flecheBas == false) {
        t.z += vectDeplacement.y;
        t.x += vectDeplacement.x;
        obj[0].tr.rotation_euler.y = -cam.tr.rotation_euler.y + M_PI / 2;
    }

    int i = 0;
    //vérifier s'il rencontre un arbre 
    while (numArbres[i] != -500) {
        if (norm(t - obj[numArbres[i]].tr.translation) < 1.8) {
            autoriserDeplacement = false;
        }
        i++;
    }

    i = 0;
    //vérifier s'il sort du terrain
    if (abs(t.x) > 250 || abs(t.z) > 250) {
        autoriserDeplacement = false;
    }

    
    if (autoriserDeplacement == true) {
        obj[0].tr.translation = t;
    }

    
    //offset de la caméra pur le voir par derrière 
    suiviCamera.z += 6;
    suiviCamera.y += 1;

   
    cam.tr.translation = suiviCamera;
   
    /////////////////////////////////////////////
    //SAUT
    ////////////////////////////////////////

    if (doitSauter && autoriserDeplacement) {
        obj[0].tr.translation.y = -0.5 * 3* 9.8 * compteurSaut * compteurSaut + 10 * compteurSaut; //équation du saut
        if (obj[0].tr.translation.y - obj[1].tr.translation.y < 0 ) {
            obj[0].tr.translation.y = 0;
            sautEnCours = false;
            doitSauter = false;
        }
    }

    

    autoriserDeplacement = true;
    
}
/*******************************************************/
//detraqueur
/******************************************************/
static void deplacementMonstres() {
    int i = 0;
    compteurDetra = 0;
    while (numDetraqueur[i] != -500) {
        
        if (obj[numDetraqueur[i]].tr.translation.x != -700) { //si le détraqueur n'est pas mort 
            compteurDetra++;
            testDetraqueur(i);//on test s'il meurt ou tue 
            //coordonnée du monstre
            float x = obj[numDetraqueur[i]].tr.translation.x;
            float y = obj[numDetraqueur[i]].tr.translation.z;
            //distance qu'il lui reste à parcourir pour atteindre un objectif
            float varx = depDetraqueurx[i] - x;
            float vary = depDetraqueury[i] - y; 


            //detection de s'il touche un autre monste => n'a pas abouti
            bool pasBouger = false;
            int j = 0;
            while (numDetraqueur[j] != -500) {
                if ((norm(obj[numDetraqueur[j]].tr.translation - obj[numDetraqueur[i]].tr.translation) < 2) && (i != j)) {
                    pasBouger = true;
                }
                j++;
            }
            j = 0;

            //création d'un comportement si le jouer est dans un rayon de détection 
            if (norm(obj[0].tr.translation - obj[numDetraqueur[i]].tr.translation) < 20) {
                //Le monstre se dirige vers nous
                depDetraqueurx[i] = obj[0].tr.translation.x;
                depDetraqueury[i] = obj[0].tr.translation.z;
                varx = depDetraqueurx[i] - x;
                vary = depDetraqueury[i] - y; //rayon

                //déplacement
                obj[numDetraqueur[i]].tr.translation.x += 0.3 * varx / abs(varx);
                obj[numDetraqueur[i]].tr.translation.z += 0.3 * vary / abs(vary);
            }
            else { //déplacement aléatoire

                if (abs(vary) > 0.3 && abs(varx) > 0.3) { //s'il n'a pas atteint son objectif, il bouge

                    obj[numDetraqueur[i]].tr.translation.x += 0.3 * varx / abs(varx);
                    obj[numDetraqueur[i]].tr.translation.z += 0.3 * vary / abs(vary);

                }
                else {
                    //création de coordonnée aléatoire dans un certain rayon 
                    float randx = (rand() % 21) - 10;
                    float randy = (rand() % 21) - 10;

                    //ne pas autoriser les déplacements en deshors de la zone
                    if (abs(x + randx) < 250 || abs(y + randy) < 250) {
                        depDetraqueurx[i] = x + randx;
                        depDetraqueury[i] = y + randy;
                        
                    }

                }

                
            }





            pasBouger = false;
            i++;
        }
        else {
            i++;
        }
        
        
        //
    }
    //création d'une victoire (bug)
    std::cout << "Il reste "<<compteurDetra<<"à tuer" << std::endl;
    if (compteurDetra == 0) {
        std::cout << "Vous avez gagné" << std::endl;
        exit(1);
   }
}

/*GESTION SORTILEGE*/

static void deplacementSort() {
    //le sortilège suit la caméra
    float dL = 0.8;
    float posY = obj[sort].tr.translation.z;
    float posX = obj[sort].tr.translation.x;
    float rotSort = obj[sort].tr.rotation_euler.y;

    vec2 vectDeplacement = vec2(dL * cos(cam.tr.rotation_euler.y), dL * sin(cam.tr.rotation_euler.y));

   

    posY -= vectDeplacement.x;
    posX += vectDeplacement.y;
    rotSort = -cam.tr.rotation_euler.y + M_PI;

    

    obj[sort].tr.translation.z = posY;
    obj[sort].tr.translation.x = posX;
    obj[sort].tr.rotation_euler.y = rotSort;
    

}

/*Mort du personnage*/

static void mourrir() {
    obj[0].tr.rotation_euler.z = M_PI / 2;
    enVie = false;
}

/*Test mortalité du détraqueur*/
static void testDetraqueur(int i) {
    //détraqueur meurt
    if (norm(obj[numDetraqueur[i]].tr.translation - obj[sort].tr.translation) < 10) {
        obj[numDetraqueur[i]].tr.translation.x = -700;

    }
    //détraqueur tue

    if (norm(obj[numDetraqueur[i]].tr.translation - obj[0].tr.translation) < 2) {
        mourrir();

    }
}

/*****************************************************************************\
* timer_callback                                                              *
\*****************************************************************************/
static void timer_callback(int)
{
    if (enVie) { //si on est en vie, on peut se déplacer 
        deplacementPerso();
  }
  
  deplacementMonstres();
  if (sortEnCours == true) {
      if (norm(obj[0].tr.translation - obj[sort].tr.translation) < 20) { // si le sort s'éloigne d'un certain rayon du magicien, il disparaît
          deplacementSort();
      }
      else {
          obj[sort].tr.translation = vec3(-1000, -1000, -1000);
          sortEnCours = false;
      }
      
  }
 
  //demande de rappel de cette fonction dans 25ms
  glutTimerFunc(25, timer_callback, 0);
  glutSpecialFunc(special_callback);
  glutPostRedisplay();

  compteurSaut += 0.025;
}

/*****************************************************************************\
* main                                                                         *
\*****************************************************************************/
int main(int argc, char** argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | MACOSX_COMPATIBILITY);
  glutInitWindowSize(600, 600);
  glutCreateWindow("OpenGL");

  glutDisplayFunc(display_callback);
  glutSpecialUpFunc(special_up_callback);



  glutMotionFunc(mouse_motion);
  glutKeyboardFunc(keyboard_callback);
  glutSpecialFunc(special_callback);
  glutTimerFunc(25, timer_callback, 0);

  glewExperimental = true;
  glewInit();

  std::cout << "OpenGL: " << (GLchar *)(glGetString(GL_VERSION)) << std::endl;

  init();
  glutMainLoop();

  return 0;
}

/*****************************************************************************\
* draw_text                                                                   *
\*****************************************************************************/
void draw_text(const text * const t)
{
  if(!t->visible) return;
  
  glDisable(GL_DEPTH_TEST);
  glUseProgram(t->prog);

  vec2 size = (t->topRight - t->bottomLeft) / float(t->value.size());
  
  GLint loc_size = glGetUniformLocation(gui_program_id, "size"); CHECK_GL_ERROR();
  if (loc_size == -1) std::cerr << "Pas de variable uniforme : size" << std::endl;
  glUniform2f(loc_size,size.x, size.y);     CHECK_GL_ERROR();

  glBindVertexArray(t->vao);                CHECK_GL_ERROR();
  
  for(unsigned i = 0; i < t->value.size(); ++i)
  {
    GLint loc_start = glGetUniformLocation(gui_program_id, "start"); CHECK_GL_ERROR();
    if (loc_start == -1) std::cerr << "Pas de variable uniforme : start" << std::endl;
    glUniform2f(loc_start,t->bottomLeft.x+i*size.x, t->bottomLeft.y);    CHECK_GL_ERROR();

    GLint loc_char = glGetUniformLocation(gui_program_id, "c"); CHECK_GL_ERROR();
    if (loc_char == -1) std::cerr << "Pas de variable uniforme : c" << std::endl;
    glUniform1i(loc_char, (int)t->value[i]);    CHECK_GL_ERROR();
    glBindTexture(GL_TEXTURE_2D, t->texture_id);                            CHECK_GL_ERROR();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);                    CHECK_GL_ERROR();
  }
}

/*****************************************************************************\
* draw_obj3d                                                                  *
\*****************************************************************************/
void draw_obj3d(const objet3d* const obj, camera cam)
{
  if(!obj->visible) return;

  glEnable(GL_DEPTH_TEST);
  glUseProgram(obj->prog);
  
  {
    GLint loc_projection = glGetUniformLocation(shader_program_id, "projection"); CHECK_GL_ERROR();
    if (loc_projection == -1) std::cerr << "Pas de variable uniforme : projection" << std::endl;
    glUniformMatrix4fv(loc_projection,1,false,pointeur(cam.projection));    CHECK_GL_ERROR();

    GLint loc_rotation_view = glGetUniformLocation(shader_program_id, "rotation_view"); CHECK_GL_ERROR();
    if (loc_rotation_view == -1) std::cerr << "Pas de variable uniforme : rotation_view" << std::endl;
    mat4 rotation_x = matrice_rotation(cam.tr.rotation_euler.x, 1.0f, 0.0f, 0.0f);
    mat4 rotation_y = matrice_rotation(cam.tr.rotation_euler.y, 0.0f, 1.0f, 0.0f);
    mat4 rotation_z = matrice_rotation(cam.tr.rotation_euler.z, 0.0f, 0.0f, 1.0f);
    glUniformMatrix4fv(loc_rotation_view,1,false,pointeur(rotation_x*rotation_y*rotation_z));    CHECK_GL_ERROR();

    vec3 cv = cam.tr.rotation_center;
    GLint loc_rotation_center_view = glGetUniformLocation(shader_program_id, "rotation_center_view"); CHECK_GL_ERROR();
    if (loc_rotation_center_view == -1) std::cerr << "Pas de variable uniforme : rotation_center_view" << std::endl;
    glUniform4f(loc_rotation_center_view , cv.x,cv.y,cv.z , 0.0f); CHECK_GL_ERROR();

    vec3 tv = cam.tr.translation;
    GLint loc_translation_view = glGetUniformLocation(shader_program_id, "translation_view"); CHECK_GL_ERROR();
    if (loc_translation_view == -1) std::cerr << "Pas de variable uniforme : translation_view" << std::endl;
    glUniform4f(loc_translation_view , tv.x,tv.y,tv.z , 0.0f); CHECK_GL_ERROR();
  }
  {
    GLint loc_rotation_model = glGetUniformLocation(obj->prog, "rotation_model"); CHECK_GL_ERROR();
    if (loc_rotation_model == -1) std::cerr << "Pas de variable uniforme : rotation_model" << std::endl;
    mat4 rotation_x = matrice_rotation(obj->tr.rotation_euler.x, 1.0f, 0.0f, 0.0f);
    mat4 rotation_y = matrice_rotation(obj->tr.rotation_euler.y, 0.0f, 1.0f, 0.0f);
    mat4 rotation_z = matrice_rotation(obj->tr.rotation_euler.z, 0.0f, 0.0f, 1.0f);
    glUniformMatrix4fv(loc_rotation_model,1,false,pointeur(rotation_x*rotation_y*rotation_z));    CHECK_GL_ERROR();

    vec3 c = obj->tr.rotation_center;
    GLint loc_rotation_center_model = glGetUniformLocation(obj->prog, "rotation_center_model");   CHECK_GL_ERROR();
    if (loc_rotation_center_model == -1) std::cerr << "Pas de variable uniforme : rotation_center_model" << std::endl;
    glUniform4f(loc_rotation_center_model , c.x,c.y,c.z , 0.0f);                                  CHECK_GL_ERROR();

    vec3 t = obj->tr.translation;
    GLint loc_translation_model = glGetUniformLocation(obj->prog, "translation_model"); CHECK_GL_ERROR();
    if (loc_translation_model == -1) std::cerr << "Pas de variable uniforme : translation_model" << std::endl;
    glUniform4f(loc_translation_model , t.x,t.y,t.z , 0.0f);                                     CHECK_GL_ERROR();
  }
  glBindVertexArray(obj->vao);                                              CHECK_GL_ERROR();

  glBindTexture(GL_TEXTURE_2D, obj->texture_id);                            CHECK_GL_ERROR();
  glDrawElements(GL_TRIANGLES, 3*obj->nb_triangle, GL_UNSIGNED_INT, 0);     CHECK_GL_ERROR();
}

void init_text(text *t){
  vec3 p0=vec3( 0.0f, 0.0f, 0.0f);
  vec3 p1=vec3( 0.0f, 1.0f, 0.0f);
  vec3 p2=vec3( 1.0f, 1.0f, 0.0f);
  vec3 p3=vec3( 1.0f, 0.0f, 0.0f);

  vec3 geometrie[4] = {p0, p1, p2, p3}; 
  triangle_index index[2] = { triangle_index(0, 1, 2), triangle_index(0, 2, 3)};

  glGenVertexArrays(1, &(t->vao));                                              CHECK_GL_ERROR();
  glBindVertexArray(t->vao);                                                  CHECK_GL_ERROR();

  GLuint vbo;
  glGenBuffers(1, &vbo);                                                       CHECK_GL_ERROR();
  glBindBuffer(GL_ARRAY_BUFFER,vbo);                                          CHECK_GL_ERROR();
  glBufferData(GL_ARRAY_BUFFER,sizeof(geometrie),geometrie,GL_STATIC_DRAW);   CHECK_GL_ERROR();

  glEnableVertexAttribArray(0); CHECK_GL_ERROR();
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); CHECK_GL_ERROR();

  GLuint vboi;
  glGenBuffers(1,&vboi);                                                      CHECK_GL_ERROR();
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboi);                                 CHECK_GL_ERROR();
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(index),index,GL_STATIC_DRAW);   CHECK_GL_ERROR();

  t->texture_id = glhelper::load_texture("data/fontB.tga");

  t->visible = false;
  t->prog = gui_program_id;
}

GLuint upload_mesh_to_gpu(const mesh& m)
{
  GLuint vao, vbo, vboi;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glGenBuffers(1,&vbo);                                 CHECK_GL_ERROR();
  glBindBuffer(GL_ARRAY_BUFFER,vbo); CHECK_GL_ERROR();
  glBufferData(GL_ARRAY_BUFFER,m.vertex.size()*sizeof(vertex_opengl),&m.vertex[0],GL_STATIC_DRAW); CHECK_GL_ERROR();

  glEnableVertexAttribArray(0); CHECK_GL_ERROR();
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_opengl), 0); CHECK_GL_ERROR();

  glEnableVertexAttribArray(1); CHECK_GL_ERROR();
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(vertex_opengl), (void*)sizeof(vec3)); CHECK_GL_ERROR();

  glEnableVertexAttribArray(2); CHECK_GL_ERROR();
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_opengl), (void*)(2*sizeof(vec3))); CHECK_GL_ERROR();

  glEnableVertexAttribArray(3); CHECK_GL_ERROR();
  glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_opengl), (void*)(3*sizeof(vec3))); CHECK_GL_ERROR();

  glGenBuffers(1,&vboi); CHECK_GL_ERROR();
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboi); CHECK_GL_ERROR();
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,m.connectivity.size()*sizeof(triangle_index),&m.connectivity[0],GL_STATIC_DRAW); CHECK_GL_ERROR();

  return vao;
}

void init_model_1()
{
  // Chargement d'un maillage a partir d'un fichier
  mesh m = load_obj_file("data/source/sorcier/magicien.obj");

  // Affecte une transformation sur les sommets du maillage
  float s = 0.8f;
  mat4 transform = mat4(   s, 0.0f, 0.0f, 0.0f,
      0.0f,    s, 0.0f, 0.0f,
      0.0f, 0.0f,   s , 0.0f,
      0.0f, 0.0f, 0.0f, 1.0f);
  apply_deformation(&m,transform);

  // Centre la rotation du modele 1 autour de son centre de gravite approximatif
  obj[0].tr.rotation_center = vec3(0.0f,0.0f,0.0f);

  update_normals(&m);
  fill_color(&m,vec3(1.0f,1.0f,1.0f));

  obj[0].vao = upload_mesh_to_gpu(m);

  obj[0].nb_triangle = m.connectivity.size();
  obj[0].texture_id = glhelper::load_texture("data/source/sorcier/color_baseColor.tga");
  obj[0].visible = true;
  obj[0].prog = shader_program_id;

  obj[0].tr.translation = vec3(-2.0, 0.0f, -10.0);

  obj[0].tr.rotation_euler = vec3(0.0f, 0.0f , 0.0f);
}

void init_model_2()
{

  mesh m;
  int s = 2;
  //coordonnees geometriques des sommets
  vec3 p0=vec3(-s*250.0f,0.0f,-s*250.0f);
  vec3 p1=vec3( s*250.0f,0.0f,-s*250.0f);
  vec3 p2=vec3( s*250.00f,0.0f, s*250.0f);
  vec3 p3=vec3(-s*250.0f,0.0f, s*250.0f);

 

  //normales pour chaque sommet
  vec3 n0=vec3(0.0f,1.0f,0.0f);
  vec3 n1=n0;
  vec3 n2=n0;
  vec3 n3=n0;

  //couleur pour chaque sommet
  vec3 c0=vec3(1.0f,1.0f,1.0f);
  vec3 c1=c0;
  vec3 c2=c0;
  vec3 c3=c0;


  //texture du sommet
  vec2 t0=vec2(0.0f,0.0f);
  vec2 t1=vec2(1.0f,0.0f);
  vec2 t2=vec2(1.0f,1.0f);
  vec2 t3=vec2(0.0f,1.0f);



  vertex_opengl v0=vertex_opengl(p0,n0,c0,t0);
  vertex_opengl v1=vertex_opengl(p1,n1,c1,t1);
  vertex_opengl v2=vertex_opengl(p2,n2,c2,t2);
  vertex_opengl v3=vertex_opengl(p3,n3,c3,t3);

  
  m.vertex = {v0, v1, v2, v3};

  //indice des triangles
  triangle_index tri0=triangle_index(0,1,2);
  triangle_index tri1=triangle_index(0,2,3);  

  //mur
  triangle_index tri2 = triangle_index(0, 4, 1);
  triangle_index tri3 = triangle_index(1, 5, 4);

  triangle_index tri4 = triangle_index(1, 5, 2);
  triangle_index tri5 = triangle_index(2, 6, 5);

  m.connectivity = {tri0, tri1, tri2, tri3, tri4, tri5};

  obj[1].nb_triangle = 2;
  obj[1].vao = upload_mesh_to_gpu(m);

  obj[1].texture_id = glhelper::load_texture("data/green.png");

  obj[1].visible = true;
  obj[1].prog = shader_program_id;
  
}


void init_model_3(int premier, vec2* coord, int nbr)
{
  // Chargement d'un maillage a partir d'un fichier
    mesh m = load_obj_file("data/Dementor.obj");

  // Affecte une transformation sur les sommets du maillage
  float s = 0.015f;
  mat4 transform = mat4(   s, 0.0f, 0.0f, 0.0f,
      0.0f,    s, 0.0f, 0.50f,
      0.0f, 0.0f,   s , 0.0f,
      0.0f, 0.0f, 0.0f, 1.0f);
  //apply_deformation(&m,matrice_rotation(M_PI/2.0f,1.0f,0.0f,0.0f));
  //apply_deformation(&m,matrice_rotation(M_PI,0.0f,1.0f,0.0f));
  apply_deformation(&m,transform);

  int num;
  int i = 0;
  for (num = premier; num < nbr + premier; num++) {
      obj[num].tr.rotation_center = vec3(0.0f, 0.0f, 0.0f);

      update_normals(&m);
      fill_color(&m, vec3(1.0f, 1.0f, 1.0f));
      
      obj[num].vao = upload_mesh_to_gpu(m);

      obj[num].nb_triangle = m.connectivity.size();
      obj[num].texture_id = glhelper::load_texture("data/textures/Tex_0075_0.png");

      obj[num].visible = true;
      obj[num].prog = shader_program_id;
      
      obj[num].tr.translation = vec3(coord[num - premier].x, 1.5, coord[num - premier].y);

      numDetraqueur[i] = num;

      depDetraqueurx[i] = obj[num].tr.translation.x;
      depDetraqueury[i] = obj[num].tr.translation.z;
      i++;
      
  }
  numDetraqueur[i] = -500;
  
}


void init_model_4(int premier, vec2 *coord, int nbr)
{
    // Chargement d'un maillage a partir d'un fichier
    mesh m = load_obj_file("data/arbre.obj");

    // Affecte une transformation sur les sommets du maillage
    float s = 0.20f;
    mat4 transform = mat4(s, 0.0f, 0.0f, 0.0f,
        0.0f, s, 0.0f, 0.50f,
        0.0f, 0.0f, s, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);
    //apply_deformation(&m,matrice_rotation(M_PI/2.0f,1.0f,0.0f,0.0f));
    //apply_deformation(&m,matrice_rotation(M_PI,0.0f,1.0f,0.0f));
    apply_deformation(&m, transform);

    int num; 
    int i = 0;
    for (num = premier; num < nbr + premier-1; num++) {
    
        obj[num].tr.rotation_center = vec3(0.0f, 0.0f, 0.0f);

        update_normals(&m);
        fill_color(&m, vec3(1.0f, 1.0f, 1.0f));

        obj[num].vao = upload_mesh_to_gpu(m);

        obj[num].nb_triangle = m.connectivity.size();
        obj[num].texture_id = glhelper::load_texture("data/source/maple_tree/textures/sugar_maple_leaf_baseColor.png");

        obj[num].visible = true;
        obj[num].prog = shader_program_id;
        //std::cout << coord[num - premier].x << std::endl;
        obj[num].tr.translation = vec3(coord[num - premier].x, -1.15f, coord[num - premier].y);
        
        numArbres[i] = num;
        i++;

    }
    
    numArbres[i] = -500;

}


void init_model_5(int nombre)
{

    mesh m;

    //coordonnees geometriques des sommets
    vec3 p0 = vec3(-1.0f, 0.2f, 0.0f);
    vec3 p1 = vec3(1.0f, 0.2f, 0.0f);
    vec3 p2 = vec3(0.00f, 2.2f, 0.0f);
    

    //normales pour chaque sommet
    vec3 n0 = vec3(0.0f, 1.0f, 0.0f);
    vec3 n1 = n0;
    vec3 n2 = n0;
   

    //couleur pour chaque sommet
    vec3 c0 = vec3(1.0f, 1.0f, 1.0f);
    vec3 c1 = c0;
    vec3 c2 = c0;
    

    //texture du sommet
    vec2 t0 = vec2(0.0f, 0.0f);
    vec2 t1 = vec2(1.0f, 0.0f);
    vec2 t2 = vec2(1.0f, 1.0f);
    

    vertex_opengl v0 = vertex_opengl(p0, n0, c0, t0);
    vertex_opengl v1 = vertex_opengl(p1, n1, c1, t1);
    vertex_opengl v2 = vertex_opengl(p2, n2, c2, t2);
    //vertex_opengl v3 = vertex_opengl(p3, n3, c3, t3);

    m.vertex = { v0, v1, v2 };

    //indice des triangles
    triangle_index tri0 = triangle_index(0, 1, 2);
    //triangle_index tri1 = triangle_index(0, 2, 3);
    m.connectivity = { tri0 };

    obj[nombre].nb_triangle = 2;
    obj[nombre].vao = upload_mesh_to_gpu(m);

    obj[nombre].texture_id = glhelper::load_texture("data/white.png");

    obj[nombre].visible = true;
    obj[nombre].prog = shader_program_id;
    sort = nombre;
    obj[nombre].tr.translation = vec3(-1000, -1000, -1000);
    //std::cout << "alleeed quand meme"  << std::endl;
}