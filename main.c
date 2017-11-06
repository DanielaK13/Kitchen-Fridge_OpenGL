/*
********************************************************
            TRABALHO 1 DE COMPUTAÇÃO GRÁFICA

                      GELADEIRA

            CHRYSTIAN MUNZ,
            DANIELA KUINCHTNER, 152064
            GABRIEL CONSALTER,
            CIÊNCIA DA COMPUTAÇÃO - UPF
            PROF. EVANDRO VIAPIANA
********************************************************
*/
#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
#include <locale.h>

#define PASSO    5

#define NUM_OBJETOS 2
#define GELADEIRA   1
#define CENA        0


#define NUM_TEX   1
#define TEXTURA1  1000
#define TEXTURA2  1001


struct tipo_camera {
   GLfloat posx;               // posição x da camera
   GLfloat posy;               // posição y da camera
   GLfloat posz;               // posição z da camera
   GLfloat alvox;              // alvo x da visualização
   GLfloat alvoy;              // alvo y da visualização
   GLfloat alvoz;              // alvo z da visualização
   GLfloat inicio;             // início da área de visualização em profundidade
   GLfloat fim;                // fim da área de visualização em profundidade
   GLfloat ang;                // abertura da 'lente' - efeito de zoom
};

struct tipo_luz {
   GLfloat posicao[ 4 ];
   GLfloat ambiente[ 4 ];
   GLfloat difusa[ 4  ];
   GLfloat especular[ 4 ];
   GLfloat especularidade[ 4 ];
};

typedef struct tipo_transformacao_{
    GLfloat dx, dy, dz;
    GLfloat sx, sy, sz;
    GLfloat angx, angy, angz, ang;
} tipo_transformacao;

tipo_transformacao transf[ NUM_OBJETOS ];

// camera vai conter as definições da camera sintética
struct tipo_camera camera;
// luz vai conter as informações da iluminação
struct tipo_luz luz;

GLfloat aspecto;
GLfloat rotacao = 0;
GLuint  texture_id[ NUM_TEX ];

enum boolean {
    true = 1, false = 0
};
typedef enum boolean bool;

bool visual_eixo;
char transformacao, eixo;
GLint  objeto;
GLfloat cab;

int  LoadBMP(char *filename){
    #define SAIR        {fclose(fp_arquivo); return -1;}
    #define CTOI(C)     (*(int*)&C)

    GLubyte     *image;
    GLubyte     Header[0x54];
    GLuint      DataPos, imageSize;
    GLsizei     Width,Height;

    int nb = 0;

    // Abre o arquivo e efetua a leitura do Header do arquivo BMP
    FILE * fp_arquivo = fopen(filename,"rb");
    if (!fp_arquivo)
        return -1;
    if (fread(Header,1,0x36,fp_arquivo)!=0x36)
        SAIR;
    if (Header[0]!='B' || Header[1]!='M')
        SAIR;
    if (CTOI(Header[0x1E])!=0)
        SAIR;
    if (CTOI(Header[0x1C])!=24)
        SAIR;

    // Recupera a informação dos atributos de
    // altura e largura da imagem

    Width   = CTOI(Header[0x12]);
    Height  = CTOI(Header[0x16]);
    (CTOI(Header[0x0A]) == 0 ) ? ( DataPos=0x36 ) : ( DataPos = CTOI(Header[0x0A]));

    imageSize=Width*Height*3;

    // Efetura a Carga da Imagem
    image = (GLubyte *) malloc ( imageSize );
    int retorno;
    retorno = fread(image,1,imageSize,fp_arquivo);

    if (retorno !=imageSize){
        free (image);
        SAIR;
    }

    // Inverte os valores de R e B
    int t, i;

    for ( i = 0; i < imageSize; i += 3 ){
        t = image[i];
        image[i] = image[i+2];
        image[i+2] = t;
    }

    // Tratamento da textura para o OpenGL
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S    ,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T    ,GL_REPEAT);

    glTexEnvf ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

    // Faz a geraçao da textura na memória
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

    fclose (fp_arquivo);
    free (image);
    return 1;
}

void Texturizacao(){
   glEnable(GL_TEXTURE_2D);
   glPixelStorei ( GL_UNPACK_ALIGNMENT, 1 );//Como armazena o pixel
   glGenTextures ( NUM_TEX , texture_id );//armazena q qtidade de textura

   texture_id[ 0 ] = TEXTURA1; // define um numero (identificacao) para a textura
   glBindTexture ( GL_TEXTURE_2D, texture_id[0] );//armazena na posição 0 do vetor
   LoadBMP ( "textPorta.bmp" ); // lê a textura

   texture_id[ 1 ] = TEXTURA2; // define um numero (identificacao) para a textura
   glBindTexture ( GL_TEXTURE_2D, texture_id[1] );//armazena na posição 0 do vetor
   LoadBMP ( "laranja.bmp" ); // lê a textura

   glTexGeni( GL_S , GL_TEXTURE_GEN_MODE , GL_SPHERE_MAP );
   glTexGeni( GL_T , GL_TEXTURE_GEN_MODE , GL_SPHERE_MAP );
}

// Funcão que define a iluminação da cena
void Define_Iluminacao( void ){
    // modelo de preenchimento dos objetos
    glShadeModel( GL_SMOOTH );
    //glShadeModel( GL_FLAT );

    // habilita iluminação
    glEnable( GL_LIGHTING );

    // Ativa o uso da luz ambiente
    glLightModelfv( GL_LIGHT_MODEL_AMBIENT , luz.ambiente );

    // poisção da luz no universo
    glLightfv( GL_LIGHT0 , GL_POSITION , luz.posicao );

    // configura a luz ambiente
    glLightfv( GL_LIGHT0 , GL_AMBIENT  , luz.ambiente  );
    // configura a luz difusa
    glLightfv( GL_LIGHT0 , GL_DIFFUSE  , luz.difusa );
    // configura a luz especular
    glLightfv( GL_LIGHT0 , GL_SPECULAR , luz.especular );

    //habilita a luz 0
    glEnable ( GL_LIGHT0 );

    // Define a refletância do material
    glMaterialfv( GL_FRONT , GL_SPECULAR  , luz.especularidade );

    // define o brilho do material
    glMateriali ( GL_FRONT , GL_SHININESS , 10 );


    glEnable(GL_COLOR_MATERIAL);
}

// Inicializa parâmetros de rendering
void Inicializa (void){
    // cor de fundo da janela (RGBA)
    glClearColor( 0.0 , 0.0 , 0.0 , 1.0 );
    int i;
    for( i = 0 ; i < NUM_OBJETOS ; i++ ){
        transf[ i ].dx  = 0.0;
        transf[ i ].dy  = 0.0;
        transf[ i ].dz  = 0.0;
        transf[ i ].sx  = 1.0;
        transf[ i ].sy  = 1.0;
        transf[ i ].sz  = 1.0;
        transf[i].angx  = 0.0;
        transf[i].angy  = 0.0;
        transf[i].angz  = 0.0;
        transf[i].ang   = 0.0;
    }

    //inicialização
    objeto = CENA;
    transformacao = 'R';
    eixo = 'Y';

    // posição x da câmera no universo
    camera.posx   = 0;
    // posição y da câmera no universo
    camera.posy   = 0;
    // posição z da câmera no universo
    camera.posz   = 400;
    // posição x do alvo da câmera no universo - para onde a cÂmera 'olha'
    camera.alvox  = 0;
    // posição y do alvo da câmera no universo - para onde a cÂmera 'olha'
    camera.alvoy  = 0;
    // posição z do alvo da câmera no universo - para onde a cÂmera 'olha'
    camera.alvoz  = 0;
    // a partir de que distância da câmera ela começa a 'enxergar' os objetos
    camera.inicio = 0.1;
    // até que distância da câmera ela consegue 'enxergar' os objetos
    camera.fim    = 5000.0;
    // ângulo da câmera - define o zoom
    camera.ang    = 45;

    // posição da fonte de luz
    luz.posicao[ 0 ] = 100.0;
    luz.posicao[ 1 ] = 100.0;
    luz.posicao[ 2 ] = 100.0;
    luz.posicao[ 3 ] = 1.0;

    // cor e intensidade da luz ambiente
    luz.ambiente[ 0 ] = 0.2;
    luz.ambiente[ 1 ] = 0.2;
    luz.ambiente[ 2 ] = 0.2;
    luz.ambiente[ 3 ] = 1.0;

    // cor e intensidade da luz difusa
    luz.difusa[ 0 ] = 0.5;
    luz.difusa[ 1 ] = 0.5;
    luz.difusa[ 2 ] = 0.5;
    luz.difusa[ 3 ] = 1.0;

    // cor e intensidade da luz especular
    luz.especular[ 0 ] = 0.8;
    luz.especular[ 1 ] = 0.8;
    luz.especular[ 2 ] = 0.8;
    luz.especular[ 3 ] = 1.0;

    // cor e intensidade da especularidade
    luz.especularidade[ 0 ] = 0.8;
    luz.especularidade[ 1 ] = 0.8;
    luz.especularidade[ 2 ] = 0.8;
    luz.especularidade[ 3 ] = 1.0;

    // ativa a possibilidade de transparência dos objetos - canal alfa
    glEnable( GL_BLEND );

    // define a forma de cálculo da transparência
    glBlendFunc( GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA );

    // ativa a remoçào das faces ocultas
    glEnable( GL_CULL_FACE );

    // ativa o cálculo de profundidade z-buffer
    glEnable( GL_DEPTH_TEST );

    Define_Iluminacao();

    Texturizacao();
}

void desenhacubo(int x, int y, int z){

   glPushMatrix(); // face frontal
        //glColor3ub(255,0,0);
        glBegin(GL_QUADS);
        glNormal3f(   0.0 ,   0.0 ,  1.0 );	// normal da face
           //glTexCoord2f( 1.0 , 1.0 );
           glVertex3d(-x,-y,z);
           //glTexCoord2f( 0.0 , 1.0 );
           glVertex3d(x,-y,z);
           //glTexCoord2f( 0.0 , 0.0 );
           glVertex3d(x,y,z);
           //glTexCoord2f( 1.0 , 0.0 );
           glVertex3d(-x,y,z);
        glEnd();
    glPopMatrix();

    glPushMatrix(); // face traseira
        glColor3ub(255,255,255);
        //glColor3ub(0,255,255);
        glTranslated(0,0,-4);
        glRotated(180,0,1,0);
        glBegin(GL_QUADS);
        glNormal3f(   0.0 ,   0.0 ,  -1.0 );
           glTexCoord2f(  0.0 , 0.0 );  glVertex3d(-x,-y,-z);
           glTexCoord2f(  1.0 , 0.0 );  glVertex3d(x,-y,-z);
           glTexCoord2f(  1.0 , 1.0 ); glVertex3d(x,y,-z);
           glTexCoord2f(  0.0 , 1.0 ); glVertex3d(-x,y,-z);
        glEnd();
    glPopMatrix();

    glPushMatrix(); // face direita
        //glColor3ub(255,0,255);
        glColor3ub(255,255,255);
        glBegin(GL_QUADS);
        glNormal3f(   1.0 ,   0.0 ,  0.0 );
           //glTexCoord2f( 1.0 , 1.0 );
           glVertex3d(x,-y,z);
           //glTexCoord2f( 0.0 , 1.0 );
           glVertex3d(x,-y,-z);
           //glTexCoord2f( 0.0 , 0.0 );
           glVertex3d(x,y,-z);
           //glTexCoord2f( 1.0 , 0.0 );
           glVertex3d(x,y,z);
        glEnd();
    glPopMatrix();

    glPushMatrix(); // face esquerda
        //glColor3ub(255,180,50);
        glColor3ub(255,255,255);
        glTranslatef(-100,0,0);
        glRotated(180,0,1,0);
        glBegin(GL_QUADS);
        glNormal3f(   -1.0 ,   0.0 ,  0.0 );
           //glTexCoord2f( 0.0 , 1.0 );
           glVertex3d(-x,-y,z);
           //glTexCoord2f( 0.0 , 0.0 );
           glVertex3d(-x,-y,-z);
           //glTexCoord2f( 1.0 , 0.0 );
           glVertex3d(-x,y,-z);
          // glTexCoord2f( 1.0 , 1.0 );
           glVertex3d(-x,y,z);
        glEnd();
    glPopMatrix();

    glPushMatrix(); // face superior
        //glColor3ub(180,10,255);
        glColor3ub(255,255,255);
        glBegin(GL_QUADS);
        glNormal3f(   0.0 ,   1.0 ,  0.0 );
           //glTexCoord2f( 0.0 , 1.0 );
           glVertex3d(-x,y,z);
           //glTexCoord2f( 1.0 , 1.0 );
           glVertex3d(x,y,z);
           //glTexCoord2f( 1.0 , 0.0 );
           glVertex3d(x,y,-z);
           //glTexCoord2f( 0.0 , 0.0 );
           glVertex3d(-x,y,-z);
        glEnd();
    glPopMatrix();

    glPushMatrix(); // face inferior
        //glColor3ub(0,255,0);
        glColor3ub(255,255,255);
        glTranslatef(0,-100,0);
        glRotated(180,1,0,0);
        glBegin(GL_QUADS);
        glNormal3f(   0.0 ,   -1.0 ,  0.0 );
            //glTexCoord2f( 1.0 , 1.0 );
            glVertex3d(-x,-y,z);
            //glTexCoord2f( 0.0 , 1.0 );
            glVertex3d(x,-y,z);
            //glTexCoord2f( 0.0 , 0.0 );
            glVertex3d(x,-y,-z);
            //glTexCoord2f( 1.0 , 0.0 );
            glVertex3d(-x,-y,-z);
        glEnd();
    glPopMatrix();
}

void desenhacubo2(int x, int y, int z){ // pés

   glPushMatrix(); // face frontal
        //glColor3ub(255,0,0);
        glColor3ub(255,255,255);
        glBegin(GL_QUADS);
            glVertex3d(-x,-y,z);
            glVertex3d(x,-y,z);
            glVertex3d(x,y,z);
            glVertex3d(-x,y,z);
        glEnd();
    glPopMatrix();

    glPushMatrix(); // face traseira
        //glColor3ub(0,255,255);
        glColor3ub(255,255,255);
        glTranslated(0,0,-8);
        glRotated(180,0,1,0);
        glBegin(GL_QUADS);
            glVertex3d(-x,-y,-z);
            glVertex3d(x,-y,-z);
            glVertex3d(x,y,-z);
            glVertex3d(-x,y,-z);
        glEnd();
    glPopMatrix();

    glPushMatrix(); // face direita
        //glColor3ub(255,0,255);
        glColor3ub(255,255,255);
        glBegin(GL_QUADS);
            glVertex3d(x,-y,z);
            glVertex3d(x,-y,-z);
            glVertex3d(x,y,-z);
            glVertex3d(x,y,z);
        glEnd();
    glPopMatrix();

    glPushMatrix(); // face esquerda
        //glColor3ub(255,180,50);
        glColor3ub(255,255,255);
        glTranslatef(-12,0,0); // SE AUMENTAR AQUI, AUMENTA TODOS
        glRotated(180,0,1,0);
        glBegin(GL_QUADS);
            glVertex3d(-x,-y,z);
            glVertex3d(-x,-y,-z);
            glVertex3d(-x,y,-z);
            glVertex3d(-x,y,z);
        glEnd();
    glPopMatrix();

    glPushMatrix(); // face superior
        //glColor3ub(180,10,255);
        glColor3ub(255,255,255);
        glBegin(GL_QUADS);
            glVertex3d(-x,y,z);
            glVertex3d(x,y,z);
            glVertex3d(x,y,-z);
            glVertex3d(-x,y,-z);
        glEnd();
    glPopMatrix();

    glPushMatrix(); // face inferior
        //glColor3ub(0,255,0);
        glColor3ub(255,255,255);
        glTranslatef(0,-8,0);
        glRotated(180,1,0,0);
        glBegin(GL_QUADS);
            glVertex3d(-x,-y,z);
            glVertex3d(x,-y,z);
            glVertex3d(x,-y,-z);
            glVertex3d(-x,-y,-z);
        glEnd();
    glPopMatrix();
}

void desenhacubo3(int x, int y, int z){ // prateleira

   glPushMatrix(); // face frontal
        //glColor3ub(255,0,0);
        glBegin(GL_QUADS);
        glNormal3f(   0.0 ,   0.0 ,  1.0 );	// normal da face
           //glTexCoord2f( 1.0 , 1.0 );
           glVertex3d(-x,-y,z);
           //glTexCoord2f( 0.0 , 1.0 );
           glVertex3d(x,-y,z);
           //glTexCoord2f( 0.0 , 0.0 );
           glVertex3d(x,y,z);
           //glTexCoord2f( 1.0 , 0.0 );
           glVertex3d(-x,y,z);
        glEnd();
    glPopMatrix();

    glPushMatrix(); // face traseira
        glColor3ub(255,255,255);
        //glColor3ub(0,255,255);
        glTranslated(0,0,-2);
        glRotated(180,0,1,0);
        glBegin(GL_QUADS);
        glNormal3f(   0.0 ,   0.0 ,  -1.0 );
           //glTexCoord2f(  0.0 , 0.0 );
           glVertex3d(-x,-y,-z);
           //glTexCoord2f(  1.0 , 0.0 );
           glVertex3d(x,-y,-z);
           //glTexCoord2f(  1.0 , 1.0 );
           glVertex3d(x,y,-z);
           //glTexCoord2f(  0.0 , 1.0 );
           glVertex3d(-x,y,-z);
        glEnd();
    glPopMatrix();

    glPushMatrix(); // face direita
        //glColor3ub(255,0,255);
        glColor3ub(255,255,255);
        glBegin(GL_QUADS);
        glNormal3f(   1.0 ,   0.0 ,  0.0 );
           //glTexCoord2f( 1.0 , 1.0 );
           glVertex3d(x,-y,z);
           //glTexCoord2f( 0.0 , 1.0 );
           glVertex3d(x,-y,-z);
           //glTexCoord2f( 0.0 , 0.0 );
           glVertex3d(x,y,-z);
           //glTexCoord2f( 1.0 , 0.0 );
           glVertex3d(x,y,z);
        glEnd();
    glPopMatrix();

    glPushMatrix(); // face esquerda
        //glColor3ub(255,180,50);
        glColor3ub(255,255,255);
        glTranslatef(-0,0,0);
        glRotated(180,0,1,0);
        glBegin(GL_QUADS);
        glNormal3f(   -1.0 ,   0.0 ,  0.0 );
           //glTexCoord2f( 0.0 , 1.0 );
           glVertex3d(-x,-y,z);
           //glTexCoord2f( 0.0 , 0.0 );
           glVertex3d(-x,-y,-z);
           //glTexCoord2f( 1.0 , 0.0 );
           glVertex3d(-x,y,-z);
          // glTexCoord2f( 1.0 , 1.0 );
           glVertex3d(-x,y,z);
        glEnd();
    glPopMatrix();

    glPushMatrix(); // face superior
        //glColor3ub(180,10,255);
        glColor3ub(255,255,255);
        glBegin(GL_QUADS);
        glNormal3f(   0.0 ,   1.0 ,  0.0 );
           //glTexCoord2f( 0.0 , 1.0 );
           glVertex3d(-x,y,z);
           //glTexCoord2f( 1.0 , 1.0 );
           glVertex3d(x,y,z);
           //glTexCoord2f( 1.0 , 0.0 );
           glVertex3d(x,y,-z);
           //glTexCoord2f( 0.0 , 0.0 );
           glVertex3d(-x,y,-z);
        glEnd();
    glPopMatrix();

    glPushMatrix(); // face inferior
        //glColor3ub(0,255,0);
        glColor3ub(255,255,255);
        glTranslatef(0,-0,0);
        glRotated(180,1,0,0);
        glBegin(GL_QUADS);
        glNormal3f(   0.0 ,   -1.0 ,  0.0 );
            //glTexCoord2f( 1.0 , 1.0 );
            glVertex3d(-x,-y,z);
            //glTexCoord2f( 0.0 , 1.0 );
            glVertex3d(x,-y,z);
            //glTexCoord2f( 0.0 , 0.0 );
            glVertex3d(x,-y,-z);
            //glTexCoord2f( 1.0 , 0.0 );
            glVertex3d(-x,-y,-z);
        glEnd();
    glPopMatrix();



}
// Função callback chamada para fazer o desenho
void Desenha(void){

    GLUquadricObj *quadObj; // um objeto é criado
    quadObj = gluNewQuadric();
    gluQuadricTexture(quadObj, GL_TRUE);
    gluQuadricDrawStyle(quadObj, GLU_FILL);

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glLineWidth( 1 );

    GLint raio, segmentos;
    GLfloat ang;


    glPushMatrix(); //CENA
        glTranslatef( 0 , 0 , 0 );
        glRotatef( transf[CENA].angx, 1,0,0);
        glRotatef( transf[CENA].angy, 0,1,0);
        glRotatef( transf[CENA].angz, 0,0,1);

        glEnable(GL_TEXTURE_2D);
        glBindTexture ( GL_TEXTURE_2D, TEXTURA1 );

        glPushMatrix(); //CUBO FRONTAL
            glTranslatef(0,0,50);
            glTranslatef(50, 0, 0);
            glRotatef( transf[GELADEIRA].angx, 1,0,0);
            glRotatef( transf[GELADEIRA].angy, 0,1,0);
            glRotatef( transf[GELADEIRA].angz, 0,0,1);
            glTranslatef(50, 0, 0);
            //glRotatef(180,0,1,0);
            desenhacubo(50,50,2);
        glPopMatrix();

        glDisable(GL_TEXTURE_2D);

        glPushMatrix(); //CUBO TRASEIRO
            glTranslatef( 0 , 0 , -50 );
            desenhacubo(50,50,2);
        glPopMatrix();

        glPushMatrix(); //CUBO ESQUERDO
            glTranslatef( -50 , 0 , 0 );
            glRotated(90, 0, 1, 0);
            desenhacubo(50,50,2);

        glPopMatrix();

        glPushMatrix(); //CUBO DIREITO
            glTranslatef( 50 , 0 , 0 );
            glRotatef( 90, 0, 1, 0);
            desenhacubo(50,50,2);
        glPopMatrix();

        glPushMatrix(); //CUBO SUPERIOR
            glTranslatef( 0 , 50 , 0 );
            glRotatef( 90, 1, 0, 0);
            desenhacubo(50,50,2);
        glPopMatrix();

        glPushMatrix(); //CUBO INFERIOR
            glTranslatef( 0 , -50 , 0 );
            glRotatef( 90, 1, 0, 0);
            desenhacubo(50,50,2);
        glPopMatrix();

        glPushMatrix(); // pé direito frente
            glTranslatef(30,-54,30);
            desenhacubo2(6,4,4);
        glPopMatrix();

        glPushMatrix(); // pé direito trás
            glTranslatef(30,-54,-30);
            desenhacubo2(6,4,4);
        glPopMatrix();

        glPushMatrix(); // pé esquerdo frente
            glTranslatef(-30,-54,30);
            desenhacubo2(6,4,4);
        glPopMatrix();

        glPushMatrix(); // pé esquerdo trás
            glTranslatef(-30,-54,-30);
            desenhacubo2(6,4,4);
        glPopMatrix();

        glPushMatrix(); // prateleira
            glRotatef(90,1,0,0);
            glTranslatef(0,0,-20);
            desenhacubo3(50,30,1);
        glPopMatrix();

        glEnable(GL_TEXTURE_2D);
        glBindTexture ( GL_TEXTURE_2D, TEXTURA2 );
        glPushMatrix(); // laranja
            glTranslatef(-10,-10,-10);
            glRotatef(180,1,0,0);
            glutSolidSphere(10,20,20);
        glPopMatrix();

        glDisable(GL_TEXTURE_2D);

        glPushMatrix(); // prateleira
            glRotatef(90,1,0,0);
            glTranslatef(0,0,20);
            desenhacubo3(50,30,1);
        glPopMatrix();


    glPopMatrix(); //fecha cena

    glDisable( GL_TEXTURE_GEN_S );
    glDisable( GL_TEXTURE_GEN_T );

    glutSwapBuffers();

    gluDeleteQuadric( quadObj );
 }

void maxTrans(){
    if(transf[GELADEIRA].angx > 0) transf[GELADEIRA].angx = 0;
    if(transf[GELADEIRA].angy > 0) transf[GELADEIRA].angy = 0;
    if(transf[GELADEIRA].angz > 0) transf[GELADEIRA].angz = 0;
}

// Função usada para especificar o volume de visualização
void EspecificaParametrosVisualizacao( void ){
    // seleciona o tipo de matriz para a projeção
    glMatrixMode( GL_PROJECTION );

    // limpa (zera) as matrizes
    glLoadIdentity();

    // Especifica e configura a projeção perspectiva
    gluPerspective( camera.ang , aspecto , camera.inicio , camera.fim );

    // Especifica sistema de coordenadas do modelo
    glMatrixMode( GL_MODELVIEW );

    // Inicializa sistema de coordenadas do modelo
    glLoadIdentity();

    // rotaciona a camera
    glRotatef( rotacao , 0 , 1 , 0 );

    // Especifica posição da camera (o observador) e do alvo
    gluLookAt( camera.posx , camera.posy , camera.posz , camera.alvox , camera.alvoy , camera.alvoz , 0 , 1 , 0 );
}

// Função callback chamada quando o tamanho da janela é alterado
void AlteraTamanhoJanela( GLsizei largura , GLsizei altura ){
    // Para previnir uma divisão por zero
    if ( altura == 0 ) altura = 1;

    // Especifica as dimensões da viewport
    glViewport( 0 , 0 , largura , altura );

    // Calcula a correção de aspecto
    aspecto = ( GLfloat )largura / ( GLfloat )altura;

    printf("\nAspecto (relação entre a altura e a largura) atual : %f", aspecto );

    EspecificaParametrosVisualizacao();
}

// Função callback chamada para gerenciar eventos do mouse
void GerenciaMouse( GLint button , GLint state , GLint x , GLint y ){
   // botão esquerdo do mouse
    if ( button == GLUT_LEFT_BUTTON )
        if ( state == GLUT_DOWN ) // Zoom-in
            if ( camera.ang >= 6 ) camera.ang -= 5;

    // botão direito do mouse
    if ( button == GLUT_RIGHT_BUTTON )
        if ( state == GLUT_DOWN ) // Zoom-out
            if ( camera.ang <= 174 ) camera.ang += 5;

    EspecificaParametrosVisualizacao();

    // obriga redesenhar
    glutPostRedisplay();
}

// tratamento do teclado
void GerenciaTeclado( GLubyte key , GLint x , GLint y ){
    if ( key-48 >= 0 && key-48 <= NUM_OBJETOS-1 )
        objeto = key - 48;

    if ( key == 27 )
        exit( 0 );

    if ( key == 'v' || key == 'V' )
        visual_eixo =! visual_eixo;

    if ( toupper( key ) == 'R' || toupper( key ) == 'T' )
        transformacao = toupper( key );

    if ( toupper( key ) == 'X' || toupper( key ) == 'Y' || toupper(key)=='Z')
        eixo = toupper( key );

    if ( key == '+' || key == '=' ){
        switch( transformacao ){
            case 'R':
                if ( eixo == 'X'){
                    transf[ objeto ].angx += PASSO;
                    maxTrans();
                }
                else if ( eixo == 'Y'){
                    transf[ objeto ].angy += PASSO;
                    maxTrans();
                }
                else{
                    transf[ objeto ].angz += PASSO;
                    maxTrans();
                }
                break;

            case 'T':
                if ( eixo == 'X'){
                    transf[ objeto ].dx += PASSO;
                    maxTrans();
                }
                else if ( eixo == 'Y'){
                    transf[ objeto ].dy += PASSO;
                    maxTrans();
                }
                else{
                    transf[ objeto ].dz += PASSO;
                    maxTrans();
                }
                break;

        }
    }
    if ( key == '-' || key == '_' ){
        switch( transformacao ){
            case 'R':
                if ( eixo == 'X'){
                    transf[ objeto ].angx -= PASSO;
                    maxTrans();
                }

                else if ( eixo == 'Y'){
                    transf[ objeto ].angy -= PASSO;
                    maxTrans();
                }
                else{
                    transf[ objeto ].angz -= PASSO;
                    maxTrans();
                }

                break;

            case 'T':
                if ( eixo == 'X'){
                    transf[ objeto ].dx -= PASSO;
                    maxTrans();
                }
                else if(eixo=='Y'){
                    transf[ objeto ].dy -= PASSO;
                    maxTrans();
                }
                else{
                    transf[ objeto ].dz -= PASSO;
                    maxTrans();
                }

                break;
        }
    }

    // rotaciona a camera
   if ( key == 'G' )
       rotacao += PASSO;

   if ( key == 'g' )
       rotacao -= PASSO;

   // leva a câmera para a direita
   if ( key == 'D' ){
       camera.posx += PASSO;
       camera.alvox += PASSO; //leva o alvo junto
   }

   // leva a câmera para a esquerda
   if ( key == 'd' ){
       camera.posx -= PASSO;
       camera.alvox -= PASSO;//leva o alvo junto
   }

   // leva a câmera para cima
   if ( key == 'C' ){
       camera.posy += PASSO;
       camera.alvoy += PASSO;//leva o alvo junto
   }

   // leva a câmera para baixo
   if ( key == 'c' ){
       camera.posy -= PASSO;
       camera.alvoy -= PASSO;//leva o alvo junto
   }

    //leva a câmera para trás
   if ( key == 'P' ){
       camera.posz += PASSO;
       camera.alvoz += PASSO;//leva o alvo junto
   }

   // leva a câmera para frente
   if ( key == 'p' ){
       camera.posz -= PASSO;
       camera.alvoz -= PASSO;//leva o alvo junto
   }

   // leva o ponto para onde a câmera 'olha' para a direita
   if ( key == 'E' )
       camera.alvox += PASSO;

   // leva o ponto para onde a câmera 'olha' para a esquerda
   if ( key == 'e' )
       camera.alvox -= PASSO;

   // leva o ponto para onde a câmera 'olha' para cima
   if ( key == 'B' )
       camera.alvoy += PASSO;

   // leva o ponto para onde a câmera 'olha' para baixo
   if ( key == 'b' )
       camera.alvoy -= PASSO;

   // leva o ponto para onde a câmera 'olha' para a frente
   if ( key == 'f' )
       camera.alvoz += PASSO;

   // leva o ponto para onde a câmera 'olha' para trás
   if ( key == 'F' )
       camera.alvoz -= PASSO;

   // aumenta o ponto inicial de visualização do objetos pela câmetra
   if ( key == 'K' && camera.inicio + PASSO < camera.fim )
       camera.inicio += PASSO;

   // diminui o ponto inicial de visualização do objetos pela câmetra
   if ( key == 'k' && camera.inicio - PASSO > 0 )
       camera.inicio -= PASSO;

   // aumenta o ponto final de visualização do objetos pela câmetra
   if ( key == 'L' )
       camera.fim += PASSO;

   // diminiu o ponto final de visualização do objetos pela câmetra
   if ( key == 'l' && camera.fim - PASSO > camera.inicio )
       camera.fim -= PASSO;

   // aumenta o angulo de visão da câmera (zoom-out)
   if ( key == 'A' && camera.ang + PASSO < 180 )
       camera.ang += PASSO;

   // diminui o angulo de visão da câmera (zoom-in)
   if ( key == 'a' && camera.ang - PASSO > 0 )
       camera.ang -= PASSO;

   // inicializa tudo
   if ( key == 'I'  || key == 'i' )
      Inicializa();

   EspecificaParametrosVisualizacao();

   // obriga redesenhar
   glutPostRedisplay();

}

// tratamento das teclas especiais (teclas de função e de navegação). os parâmetros que recebe são a tecla pressionada e a posição x e y
void TeclasEspeciais( GLint key , GLint x , GLint y ){
}

// Programa Principal
int main( int argc , char *argv[] ){
   setlocale(LC_ALL, "Portuguese");

   glutInit( &argc , argv );

   // especifica a utilização dos buffers duplos e indica o sistema de cores
   glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH  );

   // especifica as dimensões da janela na tela, em pixels
   glutInitWindowSize( 500 , 500 );

   // especifica a coordenada superior esquerda da janela na tela de vídeo
   glutInitWindowPosition( 0 , 0 );

   // cria a janela com o titulo especificado
   glutCreateWindow( "Visualizacao 3D com Textura" );

   // determina qual a função do programa que desenhará os objetos
   glutDisplayFunc( Desenha );

   // determina qual a função do programa que será executada a cada alteração do tamanho da janela no video
   glutReshapeFunc( AlteraTamanhoJanela );

   // determina qual a função do programa tratará o uso do mouse
   glutMouseFunc( GerenciaMouse );

   // determina qual a função do programa tratará o teclado
   glutKeyboardFunc( GerenciaTeclado );

   // Determina qual a função do programa que tratará as teclas especiais digitadas ( teclas de função e de navegação)
   glutSpecialFunc( TeclasEspeciais );

   // mostra na console um help sobre as teclas que o programa usa e o que cada uma faz

   printf("\n----------------------COMANDOS BÁSICOS----------------------");
   printf("\nI i => Inicialização");
   printf("\nR r => seleciona transformação ROTAÇÃO");
   printf("\nT t => seleciona transformação TRANSLAÇÃO");
   printf("\nX x => seleciona eixo X");
   printf("\nY y => seleciona eixo Y");
   printf("\nZ z => seleciona eixo Z");
   printf("\n+ - => direção da transformação");
   printf("\n\n----------------------OUTROS COMANDOS-----------------------");
   printf("\nG g => rotaciona a câmera");
   printf("\nD d => muda a posição da câmera no eixo X");
   printf("\nC c => muda a posição da câmera no eixo Y");
   printf("\nP p => muda a posição da câmera no eixo Z");
   printf("\nE e => muda o alvo x");
   printf("\nB b => muda o alvo y");
   printf("\nF f => muda o alvo z");
   printf("\nK k => muda início da profundidade de visualização");
   printf("\nL l => muda fim da profundidade de visualizaçaõ");
   printf("\nA a => muda o ângulo de abertura da lente");
   printf("\nESC => sai do programa");
   printf("\n\n--------------------------OBJETOS---------------------------");
   printf("\n0 : GELADEIRA");
   printf("\n1 : PORTA");
   printf("\n\n--------------------------JANELA----------------------------");

   // função simples que inicializa os parãmetros da câmera e da projeção a ser utilizada
   Inicializa();
   // executa
   glutMainLoop();
}


