#include <stdint.h>

#define COLOR_BLACK   0
#define COLOR_BLUE    1
#define COLOR_GREEN   2
#define COLOR_CYAN    3
#define COLOR_RED     4
#define COLOR_MAGENTA 5
#define COLOR_BROWN   6
#define COLOR_LGRAY   7
#define COLOR_DGRAY   8
#define COLOR_LBLUE   9
#define COLOR_LGREEN  10
#define COLOR_LCYAN   11
#define COLOR_LRED    12
#define COLOR_LMAGENTA 13
#define COLOR_YELLOW  14
#define COLOR_WHITE   15

static const uint32_t g_palette[16]
{
	0x00000000u,
	0x00800000u,
	0x00008000u,
	0x00808000u,
	0x00000080u,
	0x00800080u,
	0x00008080u,
	0x00808080u,

	0x00404040u,
	0x00FF0000u,
	0x0000FF00u,
	0x00FFFF00u,
	0x000000FFu,
	0x00FF00FFu,
	0x0000FFFFu,
	0x00FFFFFFu,
};

const int32_t g_screen_size_x= 80;
const int32_t g_screen_size_y= 64;
const int32_t g_game_field_size_x= g_screen_size_x - 2;
const int32_t g_game_field_size_y= g_screen_size_y - 2;


#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

int rand()
{
	// TODO - implement.
	return 0;
}

struct
{
	uint32_t* pixels;
	uint32_t width;
	uint32_t height;
} screen;

void SetCursor( int x, int y )
{
}

void HexNop( int hex_nop_n )
{
}

void ShowSymbol( unsigned char x, unsigned char y, char symbol, char b_color, char f_color )
{
	const uint32_t c_cell_size_x= 4;
	const uint32_t c_cell_size_y= 4;
	for( uint32_t xx= 0; xx < c_cell_size_x; ++xx )
	for( uint32_t yy= 0; yy < c_cell_size_y; ++yy )
		screen.pixels[ (xx + x * c_cell_size_x) + (yy + y * c_cell_size_y) * screen.width ]=
			g_palette[ ( ((xx^yy)&1) == 0 || symbol == ' ' ) ? b_color : f_color ];
}

void ClearScreen(void)
{
	for( uint32_t x= 0; x < screen.width ; ++x )
	for( uint32_t y= 0; y < screen.height; ++y )
		screen.pixels[ x + y * screen.width ]= 0;
}

void Intro()
{
	int i, j;
	char game_name[]=   "DOS-Snake by Panzerschrek";
	char copyright[]=   "(c) 2013 Artjom Kunz";

	ClearScreen();
	for( i=0; i< sizeof(game_name); i++ )
		ShowSymbol( 30+i,10, game_name[i], COLOR_BLACK, COLOR_LRED );
	for( i=0; i< sizeof(copyright); i++ )
		ShowSymbol( 30+i, 11, copyright[i], COLOR_BLACK, COLOR_DGRAY );

	for( i=0; i<4; i++ )
		HexNop(65535);
}

void PrintField()
{
	int x, y;
	for( x=0; x< g_screen_size_x; x++ )
	{
		ShowSymbol( x, 0, '#', COLOR_BLACK, COLOR_LGRAY );
		ShowSymbol( x, g_screen_size_y - 1, '#', COLOR_BLACK, COLOR_LGRAY );
	}
	for( y=1; y< g_screen_size_y; y++ )
	{
		ShowSymbol( 0, y, '#', COLOR_BLACK, COLOR_LGRAY );
		ShowSymbol( g_screen_size_x - 1,y, '#', COLOR_BLACK, COLOR_LGRAY );
	}
}

int snake_segments[64][3];//3-d coordinate - rotation
int num_segments;
int head_rotation= LEFT;

int apple[2]= { g_screen_size_x / 2 - 3, g_screen_size_y / 2 };
int lifes=2, level= 1, score= 0;

void ShowScoreAndLevel()
{
	SetCursor( 1, 24 );
	printf( "Score: %d", score );
	printf( "  Level: %d", level );
	printf( "  Lifes: %d", lifes );
	SetCursor( 54, 24 );
	printf( "controls - WASD, Q - quit" );
	return;
}


void InitSnake()
{
	snake_segments[0][0]= g_screen_size_x / 2;
	snake_segments[0][1]= g_screen_size_y / 2;
	snake_segments[1][0]= g_screen_size_x / 2 + 1;
	snake_segments[1][1]= g_screen_size_y / 2;
	snake_segments[2][0]= g_screen_size_x / 2 + 2;
	snake_segments[2][1]= g_screen_size_y / 2;
	snake_segments[0][2]= snake_segments[1][2]= snake_segments[2][2]='<';

	num_segments= 3;
	head_rotation= LEFT;
}

void DrawApple()
{
	ShowSymbol( apple[0], apple[1], '*', COLOR_BLACK, COLOR_LRED );
}

void SpawnApple()
{
	int new_apple[2];
	int i;
	int in_snake= 1;

	while( in_snake )
	{
		new_apple[0]= ( rand() % g_game_field_size_x ) + 1;
		new_apple[1]= ( rand() % g_game_field_size_y ) + 1;
		in_snake= 0;
		for( i=0; i< num_segments; i++ )
		{
			if ( new_apple[0] == snake_segments[i][0] && new_apple[1] == snake_segments[i][1]  )
				in_snake= 1;
		}

	}
	apple[0]= new_apple[0];
	apple[1]= new_apple[1];
}


void Death()
{
	int i, game_over= 0;
	InitSnake();
	SpawnApple();
	score= 0;
	if( lifes > 0 )
		lifes--;
	else
		level= 1, score= 0, lifes=2, game_over= 1;

	ClearScreen();
	SetCursor( 35, 11 );
	printf( "You die" );
	if( game_over )
	{
		 SetCursor( 34, 12 );
		 printf( "Game over" );
	}

	for( i=0; i< 3; i++ )
	HexNop( 65535 );
	ClearScreen();

	PrintField();
	ShowScoreAndLevel();

}

void NextLevel()
{
	int i;
	InitSnake();


	level++;
	SpawnApple();
	lifes++;
	ClearScreen();

	SetCursor( 35, 11 );
	printf( "Level %d", level );
	for( i=0; i< 4; i++ )
	HexNop( 65535 );
	ClearScreen();
	PrintField();
	ShowScoreAndLevel();

}

//snake_segments[0] - head
//snake_segments[ num_segments - 1 ] - tail
void MoveSnake()
{
	int move_vector[2];
	int new_head[2];
	int i;
	char rotation_char;
	if( head_rotation == UP )
	{
		move_vector[0]= 0;
		move_vector[1]= -1;
		rotation_char= '^';
	}
	else if( head_rotation == DOWN )
	{
		move_vector[0]= 0;
		move_vector[1]= 1;
		rotation_char= 'V';
	}
	else if( head_rotation == LEFT )
	{
		move_vector[0]= -1;
		move_vector[1]= 0;
		rotation_char= '<';
	}
	else if( head_rotation == RIGHT )
	{
		move_vector[0]= 1;
		move_vector[1]= 0;
		rotation_char= '>';
	}


	new_head[0]= move_vector[0] + snake_segments[0][0];
	new_head[1]= move_vector[1] + snake_segments[0][1];
	if( new_head[0] < 1 || new_head[0] > g_game_field_size_x || new_head[1] < 1 || new_head[1] > g_game_field_size_y )
	{
		Death();
		return;
	}
	else
	{
		ShowSymbol( snake_segments[ num_segments -1][0], snake_segments[ num_segments -1][1], ' ', COLOR_BLACK, COLOR_LGRAY );//clear tail

		//picked up apple
		if( new_head[0] == apple[0] && new_head[1] == apple[1] )
		{
			snake_segments[ num_segments ][0]= snake_segments[ num_segments - 1 ][0];
			snake_segments[ num_segments ][1]= snake_segments[ num_segments - 1 ][1];
			snake_segments[ num_segments ][2]= snake_segments[ num_segments - 1 ][2];
			num_segments++;
			SpawnApple();
			score+= level;
			ShowScoreAndLevel();
			if( num_segments > level * 2 + 3 || num_segments > 63 )
			 { NextLevel(); return;}

		}

		for( i= num_segments-1; i>=0; i-- )
		{
			snake_segments[i][0]= snake_segments[i-1][0];
			snake_segments[i][1]= snake_segments[i-1][1];
			snake_segments[i][2]= snake_segments[i-1][2];
		}//move snake

		snake_segments[0][0]= new_head[0];
		snake_segments[0][1]= new_head[1];
		snake_segments[0][2]= rotation_char;

		for( i= 1; i< num_segments; i++ )
		{
			if( snake_segments[i][0] == new_head[0] && snake_segments[i][1] == new_head[1] )
			{
				Death();//snake eat himself
				return;
			}
		}
	}
}

char GetKey()
{
	char result;
	return result;
}

void DrawSnake()
{
	int i;

	ShowSymbol( snake_segments[0][0], snake_segments[0][1], snake_segments[0][2], COLOR_BLACK, COLOR_LGREEN );
	for( i= 1; i< num_segments; i++ )
	{
		ShowSymbol( snake_segments[i][0], snake_segments[i][1], snake_segments[i][2], COLOR_BLACK, COLOR_GREEN );
	}
}

kernel void start(
	global uint32_t* const frame_buffer,
	const uint32_t width,
	const uint32_t height)
{
	screen.pixels= frame_buffer;
	screen.width= width;
	screen.height= height;

	Intro();
	ClearScreen();
	PrintField();
	ShowScoreAndLevel();
	InitSnake();
}

kernel void frame_step( const float time_s )
{
	/*
	c= GetKey();
	while( c != 0 )
	{
		if( c == 'w' )
		{
			if( head_rotation != DOWN ) head_rotation= UP;
		}
		else if( c == 's' )
		{
			if( head_rotation != UP ) head_rotation= DOWN;
		}
		else if ( c == 'a' )
		{
			if(head_rotation !=RIGHT)   head_rotation= LEFT;
		}
		else if( c == 'd' )
		{
			if( head_rotation != LEFT )   head_rotation= RIGHT;
		}
		else if ( c == 'q' || c == 'Q' )
			return;
	}
	*/

	static float prev_time_s= 0.0f;
	const float step_duration_s= 1.0f;
	if( time_s - prev_time_s >= step_duration_s )
	{
		printf( "run tick\n" );
		MoveSnake();
		DrawSnake();
		DrawApple();

		prev_time_s+= step_duration_s;
	}
}
