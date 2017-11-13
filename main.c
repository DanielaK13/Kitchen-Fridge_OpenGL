/*
********************************************************
            TRABALHO 1 DE COMPUTA��O GR�FICA

                      GELADEIRA

            CHRYSTIAN MUNZ,
            DANIELA KUINCHTNER, 152064
            GABRIEL CONSALTER, 152071
            CI�NCIA DA COMPUTA��O - UPF
            PROF. EVANDRO VIAPIANA
********************************************************
*/
#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
#include <locale.h>

#define PASSO    5

#define NUM_OBJETOS 2
#define CENA        0
#define GELADEIRA   1


#define NUM_TEX   1
#define TEXTURA1  1000
#define TEXTURA2  1001
#define TEXTURA3  1002


struct tipo_camera {
   GLfloat posx;               // posi��o x da camera
   GLfloat posy;               // posi��o y da camera
   GLfloat posz;               // posi��o z da camera
   GLfloat alvox;              // alvo x da visualiza��o
   GLfloat alvoy;              // alvo y da visualiza��o
   GLfloat alvoz;              // alvo z da visualiza��o
   GLfloat inicio;             // in�cio da �rea de visualiza��o em profundidade
   GLfloat fim;                // fim da �rea de visualiza��o em profundidade
   GLfloat ang;                // abertura da 'lente' - efeito de zoom
};

struct tipo_luz {
   GLfloat posicao[ 4 ];
   GLfloat ambiente[ 4 ];
   GLfloat difusa[ 4  ];
   GLfloat especular[ 4 ];
   GLfloat especularidade[ 4 ];
};

GLfloat qaAmbientLight[] = {0.1,0.1,0.1,1.0};
GLfloat qaDiffuseLight[] = {1,1,1,1.0};
GLfloat qaSpecularLight[] = {1.0,1.0,1.0,1.0};
GLfloat emitLight[] = {0.9,0.9,0.9,0.01};
GLfloat Noemit[] = {0.0,0.0,0.0,1.0};

GLfloat qaLightPosition[] = {0,-5,0,0};
GLfloat qaLightDirection[] = {1,1,1,0};
GLfloat dirVector[] = {0.0,1.0,0.0,0.0};


// Posi��o de cada luz
GLfloat posLuz[1][4] = {
	{  40, 10,  -50, 1 }
};

// Dire��o de cada luz
GLfloat dirLuz[1][3] = {
	{ 0,-1,0 }
};

// Cor difusa de cada luz
GLfloat luzDifusa[1][4] = {
	{ 1,1,0,0 }
};

// Cor especular de cada luz
GLfloat luzEspecular[1][4] = {
	{ 1,1,0,1 }
};

GLfloat xispa[ 3 ] = { 10.0 , 10.0 , 10.0 };

typedef struct tipo_transformacao_{
    GLfloat dx, dy, dz;
    GLfloat sx, sy, sz;
    GLfloat angx, angy, angz, ang;
} tipo_transformacao;

tipo_transformacao transf[ NUM_OBJETOS ];

// camera vai conter as defini��es da camera sint�tica
struct tipo_camera camera;
// luz vai conter as informa��es da ilumina��o
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

    // Recupera a informa��o dos atributos de
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

    // Faz a gera�ao da textura na mem�ria
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
   glBindTexture ( GL_TEXTURE_2D, texture_id[0] );//armazena na posi��o 0 do vetor
   LoadBMP ( "textPorta.bmp" ); // l� a textura

   texture_id[ 1 ] = TEXTURA2; // define um numero (identificacao) para a textura
   glBindTexture ( GL_TEXTURE_2D, texture_id[1] );//armazena na posi��o 0 do vetor
   LoadBMP ( "laranja.bmp" ); // l� a textura

   texture_id[ 2 ] = TEXTURA3; // define um numero (identificacao) para a textura
   glBindTexture ( GL_TEXTURE_2D, texture_id[2] );//armazena na posi��o 0 do vetor
   LoadBMP ( "maca.bmp" ); // l� a textura


   glTexGeni( GL_S , GL_TEXTURE_GEN_MODE , GL_SPHERE_MAP );
   glTexGeni( GL_T , GL_TEXTURE_GEN_MODE , GL_SPHERE_MAP );
}

// Func�o que define a ilumina��o da cena
void Define_Iluminacao( void ){

    GLfloat luzAmbiente[4]={0.2,0.2,0.2,1.0};

	// Capacidade de brilho do material
	GLfloat especularidade[4]={0.5,0.5,0.5,1.0};
	GLint especMaterial = 90;

	// Define a reflet�ncia do material
	glMaterialfv(GL_FRONT,GL_SPECULAR, especularidade);
	// Define a concentra��o do brilho
	glMateriali(GL_FRONT,GL_SHININESS,especMaterial);

	// Ativa o uso da luz ambiente
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, luzAmbiente);

    glEnable ( GL_LIGHT0 );
	// Define os par�metros das fontes de luz
	int i=0;
	for(i=0;i<1;++i)
	{
		glLightfv(GL_LIGHT0+i, GL_AMBIENT, luzAmbiente);
		glLightfv(GL_LIGHT0+i, GL_DIFFUSE, luzDifusa[i] );
		glLightfv(GL_LIGHT0+i, GL_SPECULAR, luzEspecular[i] );
		glLightfv(GL_LIGHT0+i, GL_POSITION, posLuz[i] );
		glLightfv(GL_LIGHT0+i,GL_SPOT_DIRECTION,dirLuz[i]);
		glLightf(GL_LIGHT0+i,GL_SPOT_CUTOFF,200.0);
		glLightf(GL_LIGHT0+i,GL_SPOT_EXPONENT,xispa[ i ]);
	}

    glShadeModel( GL_SMOOTH );
    //glShadeModel( GL_FLAT );

    // habilita ilumina��o
    glEnable( GL_LIGHTING );
    glEnable ( GL_LIGHT3 );
//    // Ativa o uso da luz ambiente
//    glLightModelfv( GL_LIGHT_MODEL_AMBIENT , luz.ambiente );
//    // pois��o da luz no universo
//    glLightfv( GL_LIGHT3 , GL_POSITION , luz.posicao );
//    // configura a luz ambiente
//    glLightfv( GL_LIGHT3 , GL_AMBIENT  , luz.ambiente  );
//    // configura a luz difusa
//    glLightfv( GL_LIGHT3 , GL_DIFFUSE  , luz.difusa );
//    // configura a luz especular
//    glLightfv( GL_LIGHT3 , GL_SPECULAR , luz.especular );

//
//    glEnable ( GL_LIGHT1 );
//    glLightModelfv( GL_LIGHT_MODEL_AMBIENT , luz.ambiente );
//    glLightfv( GL_LIGHT1 , GL_POSITION , luz.posicao );
//    glLightfv( GL_LIGHT1 , GL_AMBIENT  , luz.ambiente  );
//    glLightfv( GL_LIGHT1 , GL_DIFFUSE  , luz.difusa );
//    glLightfv( GL_LIGHT1 , GL_SPECULAR , luz.especular );


//    glEnable ( GL_LIGHT2 );
//    glLightfv(GL_LIGHT2, GL_AMBIENT, qaAmbientLight );
//    glLightfv(GL_LIGHT2, GL_DIFFUSE, qaDiffuseLight );
//    glLightfv(GL_LIGHT2, GL_POSITION, qaLightPosition );
//    glLightfv(GL_LIGHT2, GL_SPECULAR, qaSpecularLight );
//    glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 30.0);
//    glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, dirVector);
//    glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, 1);

    glEnable(GL_COLOR_MATERIAL);
}

// Inicializa par�metros de rendering
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

    //inicializa��o
    objeto = CENA;
    transformacao = 'R';
    eixo = 'Y';

    // posi��o x da c�mera no universo
    camera.posx   = 0;
    // posi��o y da c�mera no universo
    camera.posy   = 0;
    // posi��o z da c�mera no universo
    camera.posz   = 400;
    // posi��o x do alvo da c�mera no universo - para onde a c�mera 'olha'
    camera.alvox  = 0;
    // posi��o y do alvo da c�mera no universo - para onde a c�mera 'olha'
    camera.alvoy  = 0;
    // posi��o z do alvo da c�mera no universo - para onde a c�mera 'olha'
    camera.alvoz  = 0;
    // a partir de que dist�ncia da c�mera ela come�a a 'enxergar' os objetos
    camera.inicio = 0.1;
    // at� que dist�ncia da c�mera ela consegue 'enxergar' os objetos
    camera.fim    = 5000.0;
    // �ngulo da c�mera - define o zoom
    camera.ang    = 45;

    // posi��o da fonte de luz
    luz.posicao[ 0 ] = 100.0;
    luz.posicao[ 1 ] = -50.0;
    luz.posicao[ 2 ] = 100.0;
    luz.posicao[ 3 ] = 1.0;

    // cor e intensidade da luz ambiente
    luz.ambiente[ 0 ] = 0.2;
    luz.ambiente[ 1 ] = 0.2;
    luz.ambiente[ 2 ] = 0.2;
    luz.ambiente[ 3 ] = 1.0;

    // cor e intensidade da luz difusa
    luz.difusa[ 0 ] = 2.0;
    luz.difusa[ 1 ] = 0.5;
    luz.difusa[ 2 ] = 0.5;
    luz.difusa[ 3 ] = 1.0;

    // cor e intensidade da luz especular
    luz.especular[ 0 ] = 2.0;
    luz.especular[ 1 ] = 0.8;
    luz.especular[ 2 ] = 0.8;
    luz.especular[ 3 ] = 1.0;

    // cor e intensidade da especularidade
    luz.especularidade[ 0 ] = 0.8;
    luz.especularidade[ 1 ] = 0.8;
    luz.especularidade[ 2 ] = 0.8;
    luz.especularidade[ 3 ] = 1.0;

    // ativa a possibilidade de transpar�ncia dos objetos - canal alfa
    glEnable( GL_BLEND );

    // define a forma de c�lculo da transpar�ncia
    glBlendFunc( GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA );

    // ativa a remo��o das faces ocultas
    glEnable( GL_CULL_FACE );

    // ativa o c�lculo de profundidade z-buffer
    glEnable( GL_DEPTH_TEST );

    Define_Iluminacao();

    Texturizacao();
}

void desenhacubo(int x, int y, int z){

   glPushMatrix(); // face frontal
        glBegin(GL_QUADS);
        glNormal3f(   0.0 ,   0.0 ,  1.0 );	// normal da face
           glTexCoord2f(  1.0 , 1.0 ); glVertex3d(-x,-y,z);
           glTexCoord2f(  0.0 , 1.0 );glVertex3d(x,-y,z);
           glTexCoord2f(  0.0 , 0.0 ); glVertex3d(x,y,z);
           glTexCoord2f(  1.0 , 0.0 );glVertex3d(-x,y,z);
        glEnd();
    glPopMatrix();

    glPushMatrix(); // face traseira
        glColor3ub(255,255,255);
        glTranslated(0,0,-4);
        glRotated(180,0,1,0);
        glBegin(GL_QUADS);
        glNormal3f(   0.0 ,   0.0 ,  -1.0 );
            glVertex3d(-x,-y,-z);
            glVertex3d(x,-y,-z);
            glVertex3d(x,y,-z);
            glVertex3d(-x,y,-z);
        glEnd();
    glPopMatrix();

    glPushMatrix(); // face direita
        glColor3ub(255,255,255);
        glBegin(GL_QUADS);
        glNormal3f(   1.0 ,   0.0 ,  0.0 );
           glVertex3d(x,-y,z);
           glVertex3d(x,-y,-z);
           glVertex3d(x,y,-z);
           glVertex3d(x,y,z);
        glEnd();
    glPopMatrix();

    glPushMatrix(); // face esquerda
        glColor3ub(255,255,255);
        glTranslatef(-100,0,0);
        glRotated(180,0,1,0);
        glBegin(GL_QUADS);
        glNormal3f(   -1.0 ,   0.0 ,  0.0 );
           glVertex3d(-x,-y,z);
           glVertex3d(-x,-y,-z);
           glVertex3d(-x,y,-z);
           glVertex3d(-x,y,z);
        glEnd();
    glPopMatrix();

    glPushMatrix(); // face superior
        glColor3ub(255,255,255);
        glBegin(GL_QUADS);
        glNormal3f(   0.0 ,   1.0 ,  0.0 );
           glVertex3d(-x,y,z);
           glVertex3d(x,y,z);
           glVertex3d(x,y,-z);
           glVertex3d(-x,y,-z);
        glEnd();
    glPopMatrix();

    glPushMatrix(); // face inferior
        glColor3ub(255,255,255);
        glTranslatef(0,-100,0);
        glRotated(180,1,0,0);
        glBegin(GL_QUADS);
        glNormal3f(   0.0 ,   -1.0 ,  0.0 );
            glVertex3d(-x,-y,z);
            glVertex3d(x,-y,z);
            glVertex3d(x,-y,-z);
            glVertex3d(-x,-y,-z);
        glEnd();
    glPopMatrix();
}

void desenhacubo2(int x, int y, int z){ // p�s

   glPushMatrix(); // face frontal
        glColor3ub(255,255,255);
        glBegin(GL_QUADS);
            glVertex3d(-x,-y,z);
            glVertex3d(x,-y,z);
            glVertex3d(x,y,z);
            glVertex3d(-x,y,z);
        glEnd();
    glPopMatrix();

    glPushMatrix(); // face traseira
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
        glColor3ub(255,255,255);

        glBegin(GL_QUADS);
            glVertex3d(x,-y,z);
            glVertex3d(x,-y,-z);
            glVertex3d(x,y,-z);
            glVertex3d(x,y,z);
        glEnd();
    glPopMatrix();

    glPushMatrix(); // face esquerda
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
        glColor3ub(255,255,255);
        glBegin(GL_QUADS);
            glVertex3d(-x,y,z);
            glVertex3d(x,y,z);
            glVertex3d(x,y,-z);
            glVertex3d(-x,y,-z);
        glEnd();
    glPopMatrix();

    glPushMatrix(); // face inferior
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
        glBegin(GL_QUADS);
        glNormal3f(0.0 , 0.0 ,1.0 );	// normal da face
           glVertex3d(-x,-y,z);
           glVertex3d(x,-y,z);
           glVertex3d(x,y,z);
           glVertex3d(-x,y,z);
        glEnd();
    glPopMatrix();

    glPushMatrix(); // face traseira
        glColor3ub(255,255,255);
        glTranslated(0,0,-2);
        glRotated(180,0,1,0);
        glBegin(GL_QUADS);
        glNormal3f(   0.0 ,   0.0 ,  -1.0 );
           glVertex3d(-x,-y,-z);
           glVertex3d(x,-y,-z);
           glVertex3d(x,y,-z);
           glVertex3d(-x,y,-z);
        glEnd();
    glPopMatrix();

    glPushMatrix(); // face direita
        glColor3ub(255,255,255);
        glBegin(GL_QUADS);
        glNormal3f(   1.0 ,   0.0 ,  0.0 );
           glVertex3d(x,-y,z);
           glVertex3d(x,-y,-z);
           glVertex3d(x,y,-z);
           glVertex3d(x,y,z);
        glEnd();
    glPopMatrix();

    glPushMatrix(); // face esquerda
        glColor3ub(255,255,255);
        glTranslatef(-0,0,0);
        glRotated(180,0,1,0);
        glBegin(GL_QUADS);
        glNormal3f(   -1.0 ,   0.0 ,  0.0 );
           glVertex3d(-x,-y,z);
           glVertex3d(-x,-y,-z);
           glVertex3d(-x,y,-z);
           glVertex3d(-x,y,z);
        glEnd();
    glPopMatrix();

    glPushMatrix(); // face superior
        glColor3ub(255,255,255);
        glBegin(GL_QUADS);
        glNormal3f(   0.0 ,   1.0 ,  0.0 );
           glVertex3d(-x,y,z);
           glVertex3d(x,y,z);
           glVertex3d(x,y,-z);
           glVertex3d(-x,y,-z);
        glEnd();
    glPopMatrix();

    glPushMatrix(); // face inferior
        glColor3ub(255,255,255);
        glTranslatef(0,-0,0);
        glRotated(180,1,0,0);
        glBegin(GL_QUADS);
        glNormal3f(   0.0 ,   -1.0 ,  0.0 );
            glVertex3d(-x,-y,z);
            glVertex3d(x,-y,z);
            glVertex3d(x,-y,-z);
            glVertex3d(-x,-y,-z);
        glEnd();
    glPopMatrix();



}
// Fun��o callback chamada para fazer o desenho
void Desenha(void){

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Chama a fun��o que especifica os par�metros de ilumina��o
	Define_Iluminacao();

	// Desabilita a ilumina��o para desenhar as esferas
	glDisable(GL_LIGHTING);
	int i=0;
	for(i=0;i<3;++i)
	{
		// Desenha "esferas" nas posi��es das fontes de luz
		glPushMatrix();
		glTranslatef(posLuz[i][0],posLuz[i][1],posLuz[i][2]);
		glColor3f(luzDifusa[i][0],luzDifusa[i][1],luzDifusa[i][2]);
		//glColor3f(luzEspecular[i][0],luzEspecular[i][1],luzEspecular[i][2]);
		glutSolidSphere(5,20,20);
		glPopMatrix();
	}
	// Habilita ilumina��o novamente
	glEnable(GL_LIGHTING);


    GLUquadricObj *quadObj; // um objeto � criado
    quadObj = gluNewQuadric();
    gluQuadricTexture(quadObj, GL_TRUE);
    gluQuadricDrawStyle(quadObj, GLU_FILL);

   // glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

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
            glRotatef(180,0,1,0);
            glRotatef( transf[GELADEIRA].angx, 1,0,0);
            glRotatef( transf[GELADEIRA].angy, 0,1,0);
            glRotatef( transf[GELADEIRA].angz, 0,0,1);
            glTranslatef(50, 0, 0);
            glRotatef(180,1,0,0);
            desenhacubo(50,50,2);
        glPopMatrix();

        glDisable(GL_TEXTURE_2D);

        glPushMatrix(); //CUBO TRASEIRO
            glTranslatef( 0 , 0 , -50 );
            glRotatef(180,1,0,0);
            desenhacubo(50,50,2);
        glPopMatrix();

        glPushMatrix(); //CUBO ESQUERDO
            glTranslatef( -50 , 0 , 0 );
            glRotated(90, 0, 1, 0);
            glRotatef(180,1,0,0);
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
            glRotatef(180,1,0,0);
            desenhacubo(50,50,2);
        glPopMatrix();

        glPushMatrix(); //CUBO INFERIOR
            glTranslatef( 0 , -50 , 0 );
            glRotatef( 90, 1, 0, 0);
            desenhacubo(50,50,2);
        glPopMatrix();

        glPushMatrix(); // p� direito frente
            glTranslatef(30,-54,30);
            desenhacubo2(6,4,4);
        glPopMatrix();

        glPushMatrix(); // p� direito tr�s
            glTranslatef(30,-54,-30);
            desenhacubo2(6,4,4);
        glPopMatrix();

        glPushMatrix(); // p� esquerdo frente
            glTranslatef(-30,-54,30);
            desenhacubo2(6,4,4);
        glPopMatrix();

        glPushMatrix(); // p� esquerdo tr�s
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
            glRotatef(90,0,1,0);
            gluSphere(quadObj,10,20,20);
        glPopMatrix();

        glDisable(GL_TEXTURE_2D);
        glEnable(GL_TEXTURE_2D);
        glBindTexture ( GL_TEXTURE_2D, TEXTURA3 );

        glPushMatrix(); // ma��
            glTranslatef(-30,-10,-10);
            glRotatef(90,0,1,0);
            gluSphere(quadObj,10,20,20);
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
    if(transf[GELADEIRA].angy > 180) transf[GELADEIRA].angy = 180;
    if(transf[GELADEIRA].angy < 0) transf[GELADEIRA].angy = 0;
}

// Fun��o usada para especificar o volume de visualiza��o
void EspecificaParametrosVisualizacao( void ){
    // seleciona o tipo de matriz para a proje��o
    glMatrixMode( GL_PROJECTION );

    // limpa (zera) as matrizes
    glLoadIdentity();

    // Especifica e configura a proje��o perspectiva
    gluPerspective( camera.ang , aspecto , camera.inicio , camera.fim );

    // Especifica sistema de coordenadas do modelo
    glMatrixMode( GL_MODELVIEW );

    // Inicializa sistema de coordenadas do modelo
    glLoadIdentity();

    // rotaciona a camera
    glRotatef( rotacao , 0 , 1 , 0 );

    // Especifica posi��o da camera (o observador) e do alvo
    gluLookAt( camera.posx , camera.posy , camera.posz , camera.alvox , camera.alvoy , camera.alvoz , 0 , 1 , 0 );
}

// Fun��o callback chamada quando o tamanho da janela � alterado
void AlteraTamanhoJanela( GLsizei largura , GLsizei altura ){
    // Para previnir uma divis�o por zero
    if ( altura == 0 ) altura = 1;

    // Especifica as dimens�es da viewport
    glViewport( 0 , 0 , largura , altura );

    // Calcula a corre��o de aspecto
    aspecto = ( GLfloat )largura / ( GLfloat )altura;

    printf("\nAspecto (rela��o entre a altura e a largura) atual : %f", aspecto );

    EspecificaParametrosVisualizacao();
}

// Fun��o callback chamada para gerenciar eventos do mouse
void GerenciaMouse( GLint button , GLint state , GLint x , GLint y ){
   // bot�o esquerdo do mouse
    if ( button == GLUT_LEFT_BUTTON )
        if ( state == GLUT_DOWN ) // Zoom-in
            if ( camera.ang >= 6 ) camera.ang -= 5;

    // bot�o direito do mouse
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

    if ( key == 'o' || key == 'O' ){
        transf[ GELADEIRA ].angy += PASSO;
        maxTrans();
    }

    if ( key == 'c' || key == 'C' ){
        transf[ GELADEIRA ].angy -= PASSO;
        maxTrans();
    }

    if ( key == 'v' || key == 'V' )
        visual_eixo =! visual_eixo;

    if ( toupper( key ) == 'R' || toupper( key ) == 'T' )
        transformacao = toupper( key );

    if ( toupper( key ) == 'X' || toupper( key ) == 'Y' || toupper(key)=='Z')
        eixo = toupper( key );

    if ( key == '+' || key == '=' ){
        switch( transformacao ){
            case 'R':
                if(objeto != 1){
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
                if(objeto != 1){
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

   // leva a c�mera para a direita
   if ( key == 'D' ){
       camera.posx += PASSO;
       camera.alvox += PASSO; //leva o alvo junto
   }

   // leva a c�mera para a esquerda
   if ( key == 'd' ){
       camera.posx -= PASSO;
       camera.alvox -= PASSO;//leva o alvo junto
   }

   // leva a c�mera para cima
   if ( key == 'H' ){
       camera.posy += PASSO;
       camera.alvoy += PASSO;//leva o alvo junto
   }

   // leva a c�mera para baixo
   if ( key == 'h' ){
       camera.posy -= PASSO;
       camera.alvoy -= PASSO;//leva o alvo junto
   }

    //leva a c�mera para tr�s
   if ( key == 'P' ){
       camera.posz += PASSO;
       camera.alvoz += PASSO;//leva o alvo junto
   }

   // leva a c�mera para frente
   if ( key == 'p' ){
       camera.posz -= PASSO;
       camera.alvoz -= PASSO;//leva o alvo junto
   }

   // leva o ponto para onde a c�mera 'olha' para a direita
   if ( key == 'E' )
       camera.alvox += PASSO;

   // leva o ponto para onde a c�mera 'olha' para a esquerda
   if ( key == 'e' )
       camera.alvox -= PASSO;

   // leva o ponto para onde a c�mera 'olha' para cima
   if ( key == 'B' )
       camera.alvoy += PASSO;

   // leva o ponto para onde a c�mera 'olha' para baixo
   if ( key == 'b' )
       camera.alvoy -= PASSO;

   // leva o ponto para onde a c�mera 'olha' para a frente
   if ( key == 'f' )
       camera.alvoz += PASSO;

   // leva o ponto para onde a c�mera 'olha' para tr�s
   if ( key == 'F' )
       camera.alvoz -= PASSO;

   // aumenta o ponto inicial de visualiza��o do objetos pela c�metra
   if ( key == 'K' && camera.inicio + PASSO < camera.fim )
       camera.inicio += PASSO;

   // diminui o ponto inicial de visualiza��o do objetos pela c�metra
   if ( key == 'k' && camera.inicio - PASSO > 0 )
       camera.inicio -= PASSO;

   // aumenta o ponto final de visualiza��o do objetos pela c�metra
   if ( key == 'L' )
       camera.fim += PASSO;

   // diminiu o ponto final de visualiza��o do objetos pela c�metra
   if ( key == 'l' && camera.fim - PASSO > camera.inicio )
       camera.fim -= PASSO;

   // aumenta o angulo de vis�o da c�mera (zoom-out)
   if ( key == 'A' && camera.ang + PASSO < 180 )
       camera.ang += PASSO;

   // diminui o angulo de vis�o da c�mera (zoom-in)
   if ( key == 'a' && camera.ang - PASSO > 0 )
       camera.ang -= PASSO;

   // inicializa tudo
   if ( key == 'I'  || key == 'i' )
      Inicializa();

   EspecificaParametrosVisualizacao();

   // obriga redesenhar
   glutPostRedisplay();

}

// tratamento das teclas especiais (teclas de fun��o e de navega��o). os par�metros que recebe s�o a tecla pressionada e a posi��o x e y
void TeclasEspeciais( GLint key , GLint x , GLint y ){
}

// Programa Principal
int main( int argc , char *argv[] ){
   setlocale(LC_ALL, "Portuguese");

   glutInit( &argc , argv );

   // especifica a utiliza��o dos buffers duplos e indica o sistema de cores
   glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH  );

   // especifica as dimens�es da janela na tela, em pixels
   glutInitWindowSize( 500 , 500 );

   // especifica a coordenada superior esquerda da janela na tela de v�deo
   glutInitWindowPosition( 0 , 0 );

   // cria a janela com o titulo especificado
   glutCreateWindow( "Visualizacao 3D com Textura" );

   // determina qual a fun��o do programa que desenhar� os objetos
   glutDisplayFunc( Desenha );

   // determina qual a fun��o do programa que ser� executada a cada altera��o do tamanho da janela no video
   glutReshapeFunc( AlteraTamanhoJanela );

   // determina qual a fun��o do programa tratar� o uso do mouse
   glutMouseFunc( GerenciaMouse );

   // determina qual a fun��o do programa tratar� o teclado
   glutKeyboardFunc( GerenciaTeclado );

   // Determina qual a fun��o do programa que tratar� as teclas especiais digitadas ( teclas de fun��o e de navega��o)
   glutSpecialFunc( TeclasEspeciais );

   // mostra na console um help sobre as teclas que o programa usa e o que cada uma faz

   printf("\n----------------------COMANDOS B�SICOS----------------------");
   printf("\nI i => Inicializa��o");
   printf("\nR r => seleciona transforma��o ROTA��O");
   printf("\nT t => seleciona transforma��o TRANSLA��O");
   printf("\nX x => seleciona eixo X");
   printf("\nY y => seleciona eixo Y");
   printf("\nZ z => seleciona eixo Z");
   printf("\nO o => abre a porta");
   printf("\nC c => fecha a porta");
   printf("\n+ - => dire��o da transforma��o");
   printf("\n\n----------------------OUTROS COMANDOS-----------------------");
   printf("\nG g => rotaciona a c�mera");
   printf("\nD d => muda a posi��o da c�mera no eixo X");
   printf("\nH h => muda a posi��o da c�mera no eixo Y");
   printf("\nP p => muda a posi��o da c�mera no eixo Z");
   printf("\nE e => muda o alvo x");
   printf("\nB b => muda o alvo y");
   printf("\nF f => muda o alvo z");
   printf("\nK k => muda in�cio da profundidade de visualiza��o");
   printf("\nL l => muda fim da profundidade de visualiza�a�");
   printf("\nA a => muda o �ngulo de abertura da lente");
   printf("\nESC => sai do programa");
   printf("\n\n--------------------------OBJETOS---------------------------");
   printf("\n0 : GELADEIRA");
   printf("\n1 : PORTA");
   printf("\n\n--------------------------JANELA----------------------------");

   // fun��o simples que inicializa os par�metros da c�mera e da proje��o a ser utilizada
   Inicializa();
   // executa
   glutMainLoop();
}


