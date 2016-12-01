#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <curses.h>
#include <stdbool.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>




// se definen las constantes
#define KEY_ESCAPE           27    // ASCII tecla ESCAPE
#define NFIL                 29    // filas mapa
#define NCOL                 43    // columnas mapa
#define UNO					 49
#define DOS					 50
#define TRES				 51
#define CUATRO				 52

// colores
#define COL_PARED             1
#define COL_MURO              2
#define COL_PASILLO           3
#define COL_PACMAN_NORMAL     4
#define COL_PACMAN_SAYAYIN    5

// colores de los fantasmas
#define COL_BLINKY            6
#define COL_PINKY             7
#define COL_INKY              8
#define COL_CLYDE             9


pthread_t th1;
	pthread_t hilo_p;
 int segundos=0;
 int marcador = 0;
int marcadortotal;
struct timeval tic1, toc1;
     struct timeval ti, tf;
     int estado,reloj;
    double tiempo2;
    int i, ch;
    double tiempo,tiempo2;


// se definen las enumeraciones
typedef enum {quieto, derecha, arriba, izquierda, abajo} direccion;

// estructura del pacman
typedef struct
{
    int fil,col;              // posición actual (fila, columna)
    int fil1,col1,fil2,col2,fil3,col3,fil4,col4;
    int color1,color2,color3,color4;
    direccion dir;            // dirección hacia la que avanza
    int color;                // color del pacman
    bool invencible;          // se comió la pepita de la invencibilidad
    bool boca_abierta;        // tiene abierta 'C' o cerrada 'c' la boca?
    struct timeval tic, toc;  // para el cronometro
    double tiempo_entre_mov;  // tiempo entre movimientos
    int vidas;
    int npod;
    int activaPoderN;
    char mapa[29][43];
} Pac;

typedef struct
{
    int fil,col;              // posición actual (fila, columna)
    direccion dir;            // dirección hacia la que avanza
    int color;                // color del pacman
    bool invencible;          // se comió la pepita de la invencibilidad
    bool boca_abierta;        // tiene abierta 'C' o cerrada 'c' la boca?
    struct timeval tic, toc;  // para el cronometro
    double tiempo_entre_mov;  // tiempo entre movimientos
    int vidas;
    int npod;
    int activaPoderN;
    char mapa[29][43];
} Fan;


Fan fan1,fan2,fan3,fan4;

Pac pacman =
{
    .fil          = 21,
    .col          = 21,
    .dir          = quieto,
    .color        = COL_PACMAN_NORMAL,
    .invencible   = false,
    .boca_abierta = false,
    .tiempo_entre_mov = 1.0/3, // muevase 10 cuadros por segundo
    .vidas        = 3,
    .npod		  = 0,
	.activaPoderN = 0,
    .mapa = {
    "A-------------------BXA-------------------B",
    "|...................|X|...................|",
    "|.A-----B.A-------B.|X|.A-------B.A-----B.|",
    "|*|XXXXX|.|XXXXXXX|.|X|.|XXXXXXX|.|XXXXX|*|",
    "|.D-----C.D-------C.D-C.D-------C.D-----C.|",
    "|.........................................|",
    "|.A-----B.A-B.A-------------B.A-B.A-----B.|",
    "|.D-----C.|X|.D----BXXXA----C.|X|.D-----C.|",
    "|.........|X|......|XXX|......|X|.........|",
    "D-------B.|XD----B |XXX| A----CX|.A-------C",
    "XXXXXXXX|.|XA----C D---C D----BX|.|XXXXXXXX",
    "XXXXXXXX|.|X|                 |X|.|XXXXXXXX",
    "--------C.D-C A----MMMMM----B D-C.D--------",
    "              |             |              ",
    "--------B.A-B D-------------C A-B.A--------",
    "XXXXXXXX|.|X|                 |X|.|XXXXXXXX",
    "XXXXXXXX|.|X| A-------------B |X|.|XXXXXXXX",
    "A-------C.D-C D----BXXXA----C D-C.D-------B",
    "|..................|XXX|..................|",
    "|.A-----B.A------B.|XXX|.A------B.A-----B.|",
    "|.D---BX|.D------C.D---C.D------C.|XA---C.|",
    "|.*...|X|.........................|X|...*.|",
    "D---B.|X|.A-B.A-------------B.A-B.|X|.A---C",
    "A---C.D-C.|X|.D----BXXXA----C.|X|.D-C.D---B",
    "|.........|X|......|XXX|......|X|.........|",
    "|.A-------CXD----B.|XXX|.A----CXD-------B.|",
    "|.D--------------C.D---C.D--------------C.|",
    "|.........................................|",
    "D-----------------------------------------C"
}
};

	/*	Ficheros descriptores*/
	int fd,fd2;
	
	int puerto;
	int n;
	socklen_t longitud_cli;
	/*	Informacion de la direccion del servidor y cliente*/
	struct sockaddr_in dir_serv;
	struct sockaddr_in dir_cli;
	
	
	/*	Ficheros descriptores*/
	int fd3,fd4;
	
	int puerto2;
	int n2;
	socklen_t longitud_cli2;
	/*	Informacion de la direccion del servidor y cliente*/
	struct sockaddr_in dir_serv2;
	struct sockaddr_in dir_cli2;


	/*	Ficheros descriptores*/
	int fd5,fd6;
	
	int puerto3;
	int n3;
	socklen_t longitud_cli3;
	/*	Informacion de la direccion del servidor y cliente*/
	struct sockaddr_in dir_serv3;
	struct sockaddr_in dir_cli3;


	/*	Ficheros descriptores*/
	int fd7,fd8;
	
	int puerto4;
	int n4;
	socklen_t longitud_cli4;
	/*	Informacion de la direccion del servidor y cliente*/
	struct sockaddr_in dir_serv4;
	struct sockaddr_in dir_cli4;


//Declaracion de funciones
void funcion_hijo(void *);
void funcion_hijo2(void *);
void funcion_hijo3(void *);
void funcion_hijo4(void *);

void hilo_poder(void *);

void contar_segundos();
void pintar_laberinto(void);
void mover_pacman(direccion);
bool puede_moverse_pacman(int, int, direccion);
inline double intervalo_tiempo_seg(struct timeval, struct timeval);
void inicializaciones(void);
void gestion_mov_pac();

int main (int argc, char **argv){
	
	pthread_t hilo_cli1;	
	pthread_t hilo_cli2;
	pthread_t hilo_cli3;
	pthread_t hilo_cli4;
	
	if(argc < 5){
		fprintf(stderr,"No ingreso el puerto\n");
		return 1;
	}
	int valor_hilo;
	puerto = atoi(argv[1]);	
	puerto2 = atoi(argv[2]);
	puerto3 = atoi(argv[3]);
	puerto4 = atoi(argv[4]);
	
	valor_hilo = pthread_create(&hilo_cli1,NULL,(void *)funcion_hijo, NULL);
	if(valor_hilo !=0){
		perror("NO SE PUDO CREAR HILO: hilo_cli1");
		exit(0);
	}

	valor_hilo = pthread_create(&hilo_cli2,NULL,(void *)funcion_hijo2, NULL);
	if(valor_hilo !=0){
		perror("NO SE PUDO CREAR HILO: hilo_cli2");
		exit(0);
	}
	
	valor_hilo = pthread_create(&hilo_cli3,NULL,(void *)funcion_hijo3, NULL);
	if(valor_hilo !=0){
		perror("NO SE PUDO CREAR HILO: hilo_cli3");
		exit(0);
	}
	
	valor_hilo = pthread_create(&hilo_cli4,NULL,(void *)funcion_hijo4, NULL);
	if(valor_hilo !=0){
		perror("NO SE PUDO CREAR HILO: hilo_cli4");
		exit(0);
	}

	initscr();                  // empiece el modo NCURSES
	inicializaciones();         // inicializaciones de ncurses y de la semilla
	pintar_laberinto();         // pintar el laberinto
    
	// Inicializar el cronómetro del pacman 
    gettimeofday(&pacman.tic, NULL);
    gestion_mov_pac();   
    endwin();    // finalice el modo NCURSES
    		
	pthread_join(hilo_cli1, NULL);
	pthread_join(hilo_cli2, NULL);	
	pthread_join(hilo_cli3, NULL);
	pthread_join(hilo_cli4, NULL);	
	return EXIT_SUCCESS;
}


void funcion_hijo(void *arg){
	
	fd = socket(AF_INET, SOCK_STREAM,0);
	if(fd < 0){
		fprintf(stderr,"Error abriendo socket\n");
		exit(1);
	}
	
	bzero( (char*)&dir_serv, sizeof(dir_serv) );
	dir_serv.sin_family = AF_INET;
	dir_serv.sin_addr.s_addr = INADDR_ANY;
	dir_serv.sin_port = htons(puerto);
	
	if( bind(fd, (struct sockaddr *)&dir_serv, sizeof(dir_serv)) < 0){
		fprintf(stderr,"Error en bind() no se puede elazar el socket en funcion_hijo\n");
		exit(1);
	}

	listen(fd,5);	
	longitud_cli = sizeof(dir_cli);	
	fd2 = accept(fd, (struct sockaddr *)&dir_cli, &longitud_cli);
	if(fd2 < 0){
		fprintf(stderr,"accept() NO SE PUEDE ACEPTAR EL ENLACE\n");
		exit(1);
	}

	if(n < 0){
		fprintf(stderr,"read()  NO SE PUEDE OBTENER NOMBRE DEL CLIENTE");
		exit(1);
	}
	
	srand(time(NULL));
	int x,y;
	pacman.color1 = COL_BLINKY;
	while(1){	
		n = recv(fd2,(Fan *)&fan1,sizeof(fan1),0);
		n = send(fd2,(Pac *)&pacman,sizeof(pacman),0);
			
		if(n < 0){
			fprintf(stderr,"Error escribiendo el mensaje\n");
		exit(1);
		}
		
		if((fan1.fil == pacman.fil) && (fan1.col == pacman.col) && !pacman.invencible){
			x=rand()%29;
			y=rand()%43;
			while(pacman.mapa[x][y]!=' ' && pacman.mapa[x][y]!='.'){
				x=rand()%29;
				y=rand()%43;
			}
			pacman.vidas--;
			pacman.fil = x;
			pacman.col = y;
		}
		pacman.fil1=fan1.fil;
		pacman.col1=fan1.col;	
	}

	close(fd2);
	close(fd);

}


void funcion_hijo2(void *arg){

	fd3 = socket(AF_INET, SOCK_STREAM,0);
	if(fd3 < 0){
		fprintf(stderr,"Error abriendo socket\n");
		exit(1);
	}
	
	bzero( (char*)&dir_serv2, sizeof(dir_serv2) );
	dir_serv2.sin_family = AF_INET;
	dir_serv2.sin_addr.s_addr = INADDR_ANY;	
	dir_serv2.sin_port = htons(puerto2);
	
	if( bind(fd3, (struct sockaddr *)&dir_serv2, sizeof(dir_serv2)) < 0){
		fprintf(stderr,"Error en bind() no se puede elazar el socket funcion_hijo2\n");
		exit(1);
	}

	listen(fd3,5);	
	longitud_cli2 = sizeof(dir_cli2);	
	fd4 = accept(fd3, (struct sockaddr *)&dir_cli2, &longitud_cli2);
	if(fd4 < 0){
		fprintf(stderr,"accept() NO SE PUEDE ACEPTAR EL ENLACE\n");
		exit(1);
	}
	
	if(n2 < 0){
		fprintf(stderr,"read()  NO SE PUEDE OBTENER NOMBRE DEL CLIENTE");
		exit(1);
	}
	
	srand(time(NULL));
	int x,y;
	pacman.color2 = COL_PINKY;
	
	while(1){	
		n2 = recv(fd4,(Fan *)&fan2,sizeof(fan2),0);
		n2 = send(fd4,(Pac *)&pacman,sizeof(pacman),0);
	
		if(n2 < 0){
			fprintf(stderr,"Error escribiendo el mensaje\n");
		exit(1);
		}
		if((fan2.fil == pacman.fil) && (fan2.col == pacman.col) && !pacman.invencible){
			x=rand()%29;
			y=rand()%43;
			while(pacman.mapa[x][y]!=' ' && pacman.mapa[x][y]!='.'){
				x=rand()%29;
				y=rand()%43;
			}
			pacman.fil = x;
			pacman.col = y;
			pacman.vidas--;
		}
		pacman.fil2 = fan2.fil;
		pacman.col2 = fan2.col;	
	}
	close(fd4);
	close(fd3);

}




void funcion_hijo3(void *arg){


	fd5 = socket(AF_INET, SOCK_STREAM,0);
	if(fd5 < 0){
		fprintf(stderr,"Error abriendo socket\n");
		exit(1);
	}
	
	bzero( (char*)&dir_serv3, sizeof(dir_serv3) );
	dir_serv3.sin_family = AF_INET;
	dir_serv3.sin_addr.s_addr = INADDR_ANY;
	
	dir_serv3.sin_port = htons(puerto3);
	
	if( bind(fd5, (struct sockaddr *)&dir_serv3, sizeof(dir_serv3)) < 0){
		fprintf(stderr,"Error en bind() no se puede elazar el socket funcion_hijo2\n");
		exit(1);
	}

	listen(fd5,5);	
	longitud_cli3 = sizeof(dir_cli3);	
	fd6 = accept(fd5, (struct sockaddr *)&dir_cli3, &longitud_cli3);
	if(fd6 < 0){
		fprintf(stderr,"accept() NO SE PUEDE ACEPTAR EL ENLACE\n");
		exit(1);
	}
	
	if(n3 < 0){
		fprintf(stderr,"read()  NO SE PUEDE OBTENER NOMBRE DEL CLIENTE");
		exit(1);
	}
	
	srand(time(NULL));
	int x,y;
	pacman.color3 = COL_INKY;
	
	while(1){	
		n3 = recv(fd6,(Fan *)&fan3,sizeof(fan3),0);
		n3 = send(fd6,(Pac *)&pacman,sizeof(pacman),0);
	
		if(n3 < 0){
			fprintf(stderr,"Error escribiendo el mensaje\n");
		exit(1);
		}
		
		if((fan3.fil == pacman.fil) && (fan3.col == pacman.col) && !pacman.invencible){
			x=rand()%29;
			y=rand()%43;
			while(pacman.mapa[x][y]!=' ' && pacman.mapa[x][y]!='.'){
				x=rand()%29;
				y=rand()%43;
			}
			pacman.fil = x;
			pacman.col = y;
			pacman.vidas--;
		}
		pacman.fil3 = fan3.fil;
		pacman.col3 = fan3.col;
	}
	close(fd6);
	close(fd5);
}


void funcion_hijo4(void *arg){

	fd7 = socket(AF_INET, SOCK_STREAM,0);
	if(fd7 < 0){
		fprintf(stderr,"Error abriendo socket\n");
		exit(1);
	}
	
	bzero( (char*)&dir_serv4, sizeof(dir_serv4) );
	dir_serv4.sin_family = AF_INET;
	dir_serv4.sin_addr.s_addr = INADDR_ANY;	
	dir_serv4.sin_port = htons(puerto4);
	
	if( bind(fd7, (struct sockaddr *)&dir_serv4, sizeof(dir_serv4)) < 0){
		fprintf(stderr,"Error en bind() no se puede elazar el socket funcion_hijo2\n");
		exit(1);
	}

	listen(fd7,5);	
	longitud_cli4 = sizeof(dir_cli4);
	fd8 = accept(fd7, (struct sockaddr *)&dir_cli4, &longitud_cli4);
	if(fd8 < 0){
		fprintf(stderr,"accept() NO SE PUEDE ACEPTAR EL ENLACE\n");
		exit(1);
	}
	if(n4 < 0){
		fprintf(stderr,"read()  NO SE PUEDE OBTENER NOMBRE DEL CLIENTE");
		exit(1);
	}
	
	srand(time(NULL));
	int x,y;
	pacman.color4 = COL_CLYDE;
	
	while(1){	
		n4 = recv(fd8,(Fan *)&fan4,sizeof(fan4),0);
		n4 = send(fd8,(Pac *)&pacman,sizeof(pacman),0);
	
		if(n4 < 0){
			fprintf(stderr,"Error escribiendo el mensaje\n");
		exit(1);
		}
		
		if((fan4.fil == pacman.fil) && (fan4.col == pacman.col) && !pacman.invencible){
			x=rand()%29;
			y=rand()%43;
			while(pacman.mapa[x][y]!=' ' && pacman.mapa[x][y]!='.'){
				x=rand()%29;
				y=rand()%43;
			}
			pacman.fil = x;
			pacman.col = y;
			pacman.vidas--;
		}
		pacman.fil4 = fan4.fil;
		pacman.col4 = fan4.col;
	}
	close(fd8);
	close(fd7);
}



void hilo_poder(void *arg){
int seg =0;
	while(1){
	
	
		seg++;
		sleep(1);
		if(seg==8){
			mvprintw(1, 50, "                                     ");
			mvprintw(2, 50, "Poder No Disponible               ");
 			mvprintw(3, 50, "                                  ");
 			mvprintw(4, 50, "                                  ");
 			mvprintw(5, 50, "                                  ");
 			mvprintw(6, 50, "                                   ");
 			if(pacman.activaPoderN ==2 )
 				pacman.tiempo_entre_mov =1.0/3;
 			
			pacman.activaPoderN =0;
			pacman.npod = 0;
			marcador=-1;
    	  	seg =0;
 			pthread_exit(NULL);  
 		}
	}
}

void contar_segundos() {
    
     while(1){ 
	segundos++;
        sleep(1);
	if (segundos ==15){
	pacman.color=COL_PACMAN_NORMAL;
	pacman.invencible = false;
	pthread_exit(NULL);
	segundos =0;
        }
    }
}


//=====================================================================================

void pintar_laberinto(void)
{
    int f, c, letra, color;

    for (f = 0; f < NFIL; f++)
        for (c = 0; c < NCOL; c++)
        {
            switch(pacman.mapa[f][c])
            {
            case '|':
                color = COL_PARED;
                letra = ACS_VLINE | A_BOLD;
                break;
            case '-':
                color = COL_PARED;
                letra = ACS_HLINE | A_BOLD;
                break;
            case 'M':               // muro que solo pueden atravesar fantasmas
                color = COL_PARED | A_BOLD;
                letra = '=';
                break;
            case 'A':
                color = COL_PARED;
                letra = ACS_ULCORNER | A_BOLD;
                break;
            case 'B':
                color = COL_PARED;
                letra = ACS_URCORNER | A_BOLD;
                break;
            case 'C':
                color = COL_PARED;
                letra = ACS_LRCORNER | A_BOLD;
                break;
            case 'D':
                color = COL_PARED;
                letra = ACS_LLCORNER | A_BOLD;
                break;
            case ' ':
                color = COL_PASILLO;
                letra = ' ';
                break;
            case 'X':
                color = COL_MURO;
                letra = ' ' | A_BOLD;
                break;
            case '.':
                color = COL_PASILLO;
                letra = '.';
                break;
	    case '*':
                color = COL_PASILLO;
                letra = '*';
                break;	
            default:
                getch();     // presione una tecla para terminar
                endwin();    // finalice el modo NCURSES

                perror("Error en la especificación del laberinto");
                exit(EXIT_FAILURE);
            }
            mvaddch(f, c, letra | COLOR_PAIR(color));
        }
    refresh();               // imprimir en la pantalla física
}


//=====================================================================================


void mover_pacman(direccion dir){
    int fil, col;
    

    // guardar las posiciones anteriores
    int fil_ant = fil = pacman.fil;
    int col_ant = col = pacman.col;

   //---------------------------------------------------------------------//
    // incrementar las posiciones
    switch(dir)
    {
    case quieto:
                 break; // para hacer feliz al gcc
    case derecha:
	
	         if (pacman.col==42 && pacman.fil==13){
          	    fil=13;
                    col=-1;	
	         }

                 if (col<NCOL-1) 
 	            col++;
                 break;

    case arriba:
                if (fil>0)      
		   fil--;
                break;
    case izquierda:
	        if (pacman.col==0 && pacman.fil==13){
		   fil=13;
		   col=43;	
	         }
		if (col>0) 
		   col--;
                break;
    case abajo:
               if (fil<NFIL-1) {
	          fil++;
               break;
                }
   }
  //----------------------------------------------------------//
    switch(pacman.mapa[fil][col])
    {
    case '.':
    	marcadortotal++;
    	if(pacman.npod==0){
	        marcador++;         
	        mvprintw(5, 50, "marcador       %d",marcador);
	      //  pacman.activaPoderN = 0;
	    }
        if(marcador==20 ){
        	mvprintw(1, 50, "                                       ");
 			mvprintw(2, 50, "Poder Disponible                  ");
 			mvprintw(3, 50, "[1]: Invertir fantasmas         ");
 			mvprintw(4, 50, "[2]: Nitro                      ");
 			mvprintw(5, 50, "[3]: Aleatorio                  ");
 			mvprintw(6, 50, "                                  ");
        	pacman.npod=1;
        }
        pacman.mapa[fil][col] = ' ';
    case '*':{
    	marcadortotal++;
	pacman.mapa[fil][col] = ' '; 
	}
	
    case ' ':{
        pacman.mapa[fil][col] = ' ';
        pacman.col = col;
        pacman.fil = fil;
        pacman.dir = dir;

        mvaddch(fil_ant, col_ant, ' ' | COLOR_PAIR(COL_PASILLO));
        break;
       }

	
 	gettimeofday(&tf, NULL);   // Instante final
 	tiempo2= tiempo2+(((tf.tv_sec - ti.tv_sec)*1000 + (tf.tv_usec - ti.tv_usec)/1000.0)/10);
	mvprintw(0, 50, "Vidas    = %d", tiempo2);
   }

     if (tiempo2>3 ){
        mvprintw(0, 50, "Vidas    = %d", tiempo2);
            pacman.color=COL_PACMAN_NORMAL;
	    estado=0;
	    //break;
	 }
    // }
	pintar_laberinto();
    // se escribe el pacman en la nueva posición
    pacman.boca_abierta = !pacman.boca_abierta;   // abrir y cerrar boca
    mvaddch(pacman.fil, pacman.col, (pacman.boca_abierta ? 'C' : 'c') | COLOR_PAIR(pacman.color) | A_BOLD);
     mvaddch(fan1.fil, fan1.col, 'F' | COLOR_PAIR(COL_BLINKY) | A_BOLD);
     mvaddch(fan2.fil, fan2.col, 'F' | COLOR_PAIR(COL_PINKY) | A_BOLD);
     mvaddch(fan3.fil, fan3.col, 'F' | COLOR_PAIR(COL_INKY) | A_BOLD);
     mvaddch(fan4.fil, fan4.col, 'F' | COLOR_PAIR(COL_CLYDE) | A_BOLD);
	mvprintw(0, 50, "Vida del Pacman   %d",pacman.vidas);
	mvprintw(10, 50, "marcadortotal       %d",marcadortotal);


    refresh();                                    // imprimir en pantalla
  
}
//=====================================================================================

bool puede_moverse_pacman(int fil, int col, direccion dir)
{

     	

   //reloj=reloj++;
    // incrementar las posiciones
    switch(dir)
    {
    case quieto:
        break; // para hacer feliz al gcc
    case derecha:
        if (col<NCOL-1) col++;
        break;
    case arriba:
        if (fil>0)      fil--;
        break;
    case izquierda:
        if (col>0)      col--;
        break;
    case abajo:
        if (fil<NFIL-1) fil++;
        break;
    }

    
     if (pacman.mapa[fil][col] == '*'){
	//gettimeofday(&ti, NULL);   // Instante inicial	
	pacman.color=COL_PACMAN_SAYAYIN;
	pacman.invencible = true;
        segundos=0;
	pthread_create(&th1, NULL, (void *)contar_segundos, NULL);
        
	//reloj=0;
     }
     if (reloj==10)
	pacman.color=COL_PACMAN_NORMAL;
		
     
    if ((pacman.mapa[fil][col] == '.') || (pacman.mapa[fil][col] == ' ') || (pacman.mapa[fil][col] == '*'))
        return true;
    else
        return false;



}

inline double intervalo_tiempo_seg(struct timeval toc, struct timeval tic)
{
    return (toc.tv_sec - tic.tv_sec) + (toc.tv_usec - tic.tv_usec)/1000000.0;
}

//=====================================================================================

void inicializaciones(void)
{
    // el monitor soporta colores?
    if (has_colors() == FALSE)
    {
        endwin();
        puts("Su monitor no soporta colores");
        exit(EXIT_FAILURE);
    }

    curs_set(0);         // no se muestra el cursor
    cbreak();            // no line buffering: lee letra a letra, no espera \n
    keypad(stdscr, TRUE);// acepte flechas y otras teclas especiales
    timeout(1);          // getch() espera 1 miliseg. si no hay tecla presionada
    noecho();            // no haga echo() con getch()
    srand(time(NULL));   // inicializar el generador de números aleatorios
    start_color();       // activar modo colores

    // definir colores            letra         fondo
    init_pair(COL_PARED,          COLOR_YELLOW,  COLOR_BLUE);
    init_pair(COL_MURO,           COLOR_BLUE,    COLOR_BLUE);
    init_pair(COL_PASILLO,        COLOR_WHITE,   COLOR_BLACK);
    init_pair(COL_PACMAN_NORMAL,  COLOR_YELLOW,  COLOR_BLACK);
    init_pair(COL_PACMAN_SAYAYIN, COLOR_RED,     COLOR_BLACK);
    init_pair(COL_BLINKY,         COLOR_RED,     COLOR_BLACK);
    init_pair(COL_PINKY,          COLOR_MAGENTA, COLOR_BLACK);
    init_pair(COL_INKY,           COLOR_CYAN,    COLOR_BLACK);
    init_pair(COL_CLYDE,          COLOR_GREEN,   COLOR_BLACK);
}



//=====================================================================================

 void gestion_mov_pac(){
 int x,y;
	 bool salirse = false;             // ¿salirse del juego?
	direccion proxima_dir = quieto;   // quédese quieto al principio del juego
       while ((pacman.vidas > 0) && (!salirse))
    {

	//struct timeval tic1, toc1;
        // mover el pacman
        gettimeofday(&pacman.toc, NULL);
        tiempo = intervalo_tiempo_seg(pacman.toc, pacman.tic);

        if (tiempo > pacman.tiempo_entre_mov) {
	 
            // si se puede mover en la proxima dirección hágalo, de lo contrario
            // siga la inercia
           if(puede_moverse_pacman(pacman.fil, pacman.col, proxima_dir)){
              mover_pacman(proxima_dir);	    
            }
            else{
              mover_pacman(pacman.dir);
             }
            gettimeofday(&pacman.tic, NULL);
        }

        ch = getch();
        switch(ch)
        {
        case KEY_RIGHT:
            proxima_dir = derecha;
            break; // flecha derecha
        case KEY_UP:
            proxima_dir = arriba;
            break; // flecha arriba
        case KEY_LEFT:
            proxima_dir = izquierda;
            break; // flecha izquierda
        case UNO:
        	if(pacman.npod==1){
        		pacman.activaPoderN = 1;
        		mvprintw(6, 50, "poder 1 activado    = %d         ", pacman.activaPoderN);
        		pthread_create(&hilo_p,NULL,(void *)hilo_poder, NULL);
        	}
        	break;
        case DOS:
        	if(pacman.npod==1){
        		pacman.activaPoderN = 2;
        		pacman.tiempo_entre_mov =1.0/6;
        		mvprintw(6, 50, "poder 2 activado    = %d         ", pacman.activaPoderN);
        		pthread_create(&hilo_p,NULL,(void *)hilo_poder, NULL);
        	}
        	break;
        case TRES:
        	srand(time(NULL));
        	if(pacman.npod==1){
        		pacman.activaPoderN = 3;
        		x=rand()%29;
				y=rand()%43;
				while(pacman.mapa[x][y]!=' ' && pacman.mapa[x][y]!='.'){
					x=rand()%29;
					y=rand()%43;
				}
				pacman.fil = x;
				pacman.col = y;
        		mvprintw(6, 50, "poder 3 activado    = %d         ", pacman.activaPoderN);
        	}
        	break;
        case KEY_DOWN:
            proxima_dir = abajo;
            break; // flecha abajo
        case ' ':              // pausa
            mvprintw(3, 50, "Juego pausado");
            nodelay(stdscr, FALSE);// getch() espera si no hay tecla presionada
            getch();
            nodelay(stdscr, TRUE); // getch() no espera si no hay tecla presionada
            mvprintw(3, 50, "             ");
            break;
        case KEY_ESCAPE:           // salirse del juego
            salirse = true;
            break;
        } 
    
     }

}
