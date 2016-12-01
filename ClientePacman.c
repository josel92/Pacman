#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <curses.h>
#include <stdbool.h>
#include <time.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>



// se definen las constantes
#define KEY_ESCAPE           27    // ASCII tecla ESCAPE
#define NFIL                 29    // filas mapa
#define NCOL                 43    // columnas mapa

// colores
#define COL_PARED             1
#define COL_MURO              5
#define COL_PASILLO           3
#define COL_PACMAN_NORMAL     4
#define COL_PACMAN_SAYAYIN    5

// colores de los fantasmas
#define COL_BLINKY            6
#define COL_PINKY             7
#define COL_INKY              8
#define COL_CLYDE             9

pthread_t th1;
	//para el socket
	int fd;
	int puerto,n;		
	struct sockaddr_in dir_serv;
	struct hostent *server;
	char buf[256];
	char **argv_aux;


pthread_t th1;
 int segundos=0;

struct timeval tic1, toc1;
     struct timeval ti, tf;
     int estado,reloj;
    double tiempo2;
    int i, ch;
    double tiempo,tiempo2;


// se definen las enumeraciones
typedef enum {quieto, derecha, arriba, izquierda, abajo} direccion;

// crear el pacman
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
    int	activaPoderN;			
    char mapa[NFIL][NCOL]; 	
    	
} Fan;

Pac pacman;
Fan fantasma =
{
    .fil          = 13,
    .col          = 21,
    .dir          = quieto,
    .color        = COL_BLINKY,
    .invencible   = false,
    .boca_abierta = false,
    .tiempo_entre_mov = 1.0/2, // muevase 10 cuadros por segundo
    .vidas        = 3,
    .activaPoderN = 0,	
    .mapa         ={
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



void hilo_cliente(void *);
void pintar_laberinto(void);
void mover_fantasma(direccion);
bool puede_moverse_fantasma(int, int, direccion);
inline double intervalo_tiempo_seg(struct timeval, struct timeval);
void inicializaciones(void);
void gestion_mov_fantasma();
void contar_segundos_i();

int main(int argc, char **argv){

	pthread_t hilo_cli;
	int valor_hilo;
	if (argc < 3){
		fprintf(stderr,"Error ingrese puerto");
		return 1;
	}
	
	argv_aux=&argv[1];
	puerto = atoi(argv[2]);
	valor_hilo = pthread_create(&hilo_cli,NULL,(void *)hilo_cliente, NULL);
	if(valor_hilo !=0){
		perror("NO SE PUDO CREAR HILO: hilo_cli");
		exit(0);
	}
	
	
	initscr();                  // empiece el modo NCURSES
    inicializaciones();         // inicializaciones de ncurses y de la semilla
    pintar_laberinto();         // pintar el laberinto
    
// Inicializar el cronómetro del pacman 
    gettimeofday(&fantasma.tic, NULL);
    gestion_mov_fantasma();   
    endwin();    // finalice el modo NCURSES



	pthread_join(hilo_cli, NULL);
	return EXIT_SUCCESS;
}

void contar_segundos_i() {
	
	while(1){ 
		segundos++;
        sleep(1);
		if (segundos ==10){
	        fantasma.activaPoderN=0;
			pthread_exit(NULL);
			segundos =0;
        }
    }

}

void hilo_cliente(void *arg){

	fd = socket(AF_INET, SOCK_STREAM, 0);
	
	if(fd < 0){
		fprintf(stderr,"Error en socket()");
		exit(1);
	}
	
	server = gethostbyname(*argv_aux);
	
	if (server == NULL){
		fprintf(stderr,"Error en gethostbyname()");
		exit(1);
	}
	
	
	
	bzero((char *)&dir_serv, sizeof(dir_serv));
	dir_serv.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&dir_serv.sin_addr.s_addr, server->h_length);
	dir_serv.sin_port = htons(puerto);
	
	if (connect(fd, (struct sockaddr *)&dir_serv, sizeof(dir_serv)) < 0){
		fprintf(stderr,"Error en connect()");
		exit(1);
	}
		
	while(1){		

		//bzero((Fan *)&pacman,sizeof(pacman));
		n = send(fd,(Fan *)&fantasma,sizeof(fantasma),0);
		if(n < 0){
			fprintf(stderr,"Error en send()");
			exit(1);
		}

		n = recv(fd,(Pac *)&pacman, sizeof(pacman), 0);
		if(n < 0){
			fprintf(stderr,"Error en recv()");
			exit(1);
		}
		
		strcpy(fantasma.mapa[0],pacman.mapa[0]);
		strcpy(fantasma.mapa[1],pacman.mapa[1]);
		strcpy(fantasma.mapa[2],pacman.mapa[2]);
		strcpy(fantasma.mapa[3],pacman.mapa[3]);
		strcpy(fantasma.mapa[4],pacman.mapa[4]);
		strcpy(fantasma.mapa[5],pacman.mapa[5]);
		strcpy(fantasma.mapa[6],pacman.mapa[6]);
		strcpy(fantasma.mapa[7],pacman.mapa[7]);
		strcpy(fantasma.mapa[8],pacman.mapa[8]);
		strcpy(fantasma.mapa[9],pacman.mapa[9]);
		strcpy(fantasma.mapa[10],pacman.mapa[10]);
		strcpy(fantasma.mapa[11],pacman.mapa[11]);
		strcpy(fantasma.mapa[12],pacman.mapa[12]);
		strcpy(fantasma.mapa[13],pacman.mapa[13]);
		strcpy(fantasma.mapa[14],pacman.mapa[14]);
		strcpy(fantasma.mapa[15],pacman.mapa[15]);
		strcpy(fantasma.mapa[16],pacman.mapa[16]);
		strcpy(fantasma.mapa[17],pacman.mapa[17]);
		strcpy(fantasma.mapa[18],pacman.mapa[18]);
		strcpy(fantasma.mapa[19],pacman.mapa[19]);
		strcpy(fantasma.mapa[20],pacman.mapa[20]);
		strcpy(fantasma.mapa[21],pacman.mapa[21]);
		strcpy(fantasma.mapa[22],pacman.mapa[22]);
		strcpy(fantasma.mapa[23],pacman.mapa[23]);
		strcpy(fantasma.mapa[24],pacman.mapa[24]);
		strcpy(fantasma.mapa[25],pacman.mapa[25]);
		strcpy(fantasma.mapa[26],pacman.mapa[26]);
		strcpy(fantasma.mapa[27],pacman.mapa[27]);
		strcpy(fantasma.mapa[28],pacman.mapa[28]);
		if (pacman.activaPoderN==1){
		    fantasma.activaPoderN=1;
		   // pthread_create(&th1, NULL, (void *)contar_segundos_i, NULL);	
		}else
		    fantasma.activaPoderN=0;		
		if (fantasma.fil==pacman.fil && fantasma.col==pacman.col && pacman.invencible){
		    fantasma.fil=13;
		    fantasma.col=21;
		    //sleep(3);	 				
		}
	}
	
	close(fd);
}


void pintar_laberinto(void)
{
    int f, c, letra, color;

    for (f = 0; f < NFIL; f++)
        for (c = 0; c < NCOL; c++)
        {
            switch(fantasma.mapa[f][c])
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
                color = COL_PASILLO;
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
            // imprima el caracter + color
            mvaddch(f, c, letra | COLOR_PAIR(color));
        }
    refresh();               // imprimir en la pantalla física
}


//=====================================================================================


void mover_fantasma(direccion dir){
    int fil, col;
    int fil_ant = fil = fantasma.fil;
    int col_ant = col = fantasma.col;
    int temp1,temp2;
    char temp;
     
   //---------------------------------------------------------------------//
    // incrementar las posiciones
    switch(dir)
    {
    case quieto:
    	break; // para hacer feliz al gcc
    case derecha:
	    temp=fantasma.mapa[fil][col];
	    if (fantasma.col==42 && fantasma.fil==13){
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
	    if (fantasma.col==0 && fantasma.fil==13){
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
    
  
   switch(fantasma.mapa[fantasma.fil][fantasma.col])
    
      {
    case 'M':
        mvaddch(fantasma.fil, fantasma.col, '=' | COLOR_PAIR(COL_PARED));
        break;
    case '.':
        mvaddch(fantasma.fil, fantasma.col, '.' | COLOR_PAIR(COL_PASILLO));
        break;
    case ' ':
        mvaddch(fantasma.fil, fantasma.col, ' ' | COLOR_PAIR(COL_PASILLO));
	
        break;
    case '*':
        mvaddch(fantasma.fil, fantasma.col, '*' | COLOR_PAIR(COL_PASILLO));
        break;
    }


    fantasma.fil=fil;
    fantasma.col=col;
	pintar_laberinto();
	mvaddch(pacman.fil1, pacman.col1, 'F' | COLOR_PAIR(pacman.color1) | A_BOLD);			//Fantasma Rojos
	mvaddch(pacman.fil2, pacman.col2, 'F' | COLOR_PAIR(pacman.color2) | A_BOLD);			//Fantasma Magenta
	mvaddch(pacman.fil3, pacman.col3, 'F' | COLOR_PAIR(pacman.color3) | A_BOLD);			//Fantasma Cyan
    mvaddch(pacman.fil4, pacman.col4, 'F' | COLOR_PAIR(pacman.color4) | A_BOLD);			//Fantasma Green
	mvaddch(pacman.fil, pacman.col, (pacman.boca_abierta ? 'C' : 'c') | COLOR_PAIR(pacman.color) | A_BOLD);			//Pacman

    refresh();                                    // imprimir en pantalla
  

}
//=====================================================================================

bool puede_moverse_fantasma(int fil, int col, direccion dir)
{

     	

 
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

    	
  
        
    if ((fantasma.mapa[fil][col] == '.') || (fantasma.mapa[fil][col] == ' ') || (fantasma.mapa[fil][col] == '*') || (fantasma.mapa[fil][col] == 'M')  )
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

 void gestion_mov_fantasma(){
 	bool salirse = false;             // ¿salirse del juego?
	direccion proxima_dir = quieto;   // quédese quieto al principio del juego
       
    while ((fantasma.vidas > 0) && (!salirse)){
    	gettimeofday(&fantasma.toc, NULL);
        tiempo = intervalo_tiempo_seg(fantasma.toc, fantasma.tic);

        if (tiempo > fantasma.tiempo_entre_mov) {
	 
            // si se puede mover en la proxima dirección hágalo, de lo contrario
            // siga la inercia
        	if(puede_moverse_fantasma(fantasma.fil, fantasma.col, proxima_dir)){
             	mover_fantasma(proxima_dir);	    
            }
            else{
             	mover_fantasma(fantasma.dir);
            }
            gettimeofday(&fantasma.tic, NULL);
        }

   if (fantasma.activaPoderN==0){
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
   
	if (fantasma.activaPoderN==1){
	ch = getch();
	switch(ch)
        {
        case KEY_RIGHT:
            proxima_dir = izquierda;
            break; // flecha invertida derecha
        case KEY_UP:
            proxima_dir = abajo;
            break; // flecha invertida arriba
        case KEY_LEFT:
            proxima_dir = derecha;
            break; // flecha invertida izquierda
        case KEY_DOWN:
            proxima_dir = arriba;
            break; // flecha invertida abajo
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
}



