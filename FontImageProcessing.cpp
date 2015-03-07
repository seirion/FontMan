// FontImageProcessing.cpp: implementation of the CFontImageProcessing class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FontImageProcessing.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


////////////  Morphology Operations  /////////////////
void regionfilling( unsigned char img[][A_WIDTH], int px, int py )	// region ������ ������ ��ǥ
{
	int x,y, iteration=0;
	unsigned char complement[A_HEIGHT][A_WIDTH];
	unsigned char filled[A_HEIGHT][A_WIDTH];
	unsigned char old[A_HEIGHT][A_WIDTH];
	
	for (x=0; x<A_HEIGHT; x++)
		for (y=0; y<A_WIDTH; y++)
		{
			// �Է� �̹����� Complement �̹���
			complement[x][y] = BACKGROUND;			
			if (img[x][y] == BACKGROUND)
				complement[x][y] = FOREGROUND;
			
			// �̹��� �ʱ�ȭ
			filled[x][y] = BACKGROUND;
			old[x][y] = BACKGROUND;
		}

	// ������ �� ���������� ����
	filled[px][py] = FOREGROUND;

/*	for (x=0; x<A_HEIGHT; x++){
		TRACE0("\n");
		for (y=0; y<A_WIDTH; y++)
			if (img[x][y]==1)
				TRACE1("%d", img[x][y]);		
	}
*/
	// ������ �� ���������� dilation����, ���̻��� ��ȭ�� ���� ������ ����
	bool changed = true;
	for (iteration=0; iteration<1000; iteration++)
	{
		//���� �̹������� ������ üũ (temp-img)
		if (changed)	//��ȭ�� �ִٸ� 
		{
			for (x=0; x<A_HEIGHT; x++)
				for (y=0; y<A_WIDTH; y++)
					old[x][y] = filled[x][y];	// old: ���� iteration������ �̹���
		}
		else break;	

		dilation( filled );
		
		changed = false;
		for (x=0; x<A_HEIGHT; x++)
		{
			for (y=0; y<A_WIDTH; y++)
			{
				if ( filled[x][y]==FOREGROUND && complement[x][y]==FOREGROUND )
					filled[x][y] = FOREGROUND;
				else
					filled[x][y] = BACKGROUND;

				if (filled[x][y] != old[x][y])
					changed = true;
			}
		}
	}

	// (1,1)�� ���η� ���� ä�����Ƿ� �װ��� complement�� ���Ѵ�.
	for (x=0; x<A_HEIGHT; x++)
		for (y=0; y<A_WIDTH; y++)
		{
			img[x][y] = BACKGROUND;
			if (filled[x][y] == BACKGROUND)
				img[x][y] = FOREGROUND;
		}


	// img Union filled 
/*	for (x=0; x<A_HEIGHT; x++)
	{
		for (y=0; y<A_WIDTH; y++)
		{
			if (filled[x][y] == FOREGROUND)
				img[x][y] = FOREGROUND;
		}
	}
*/
}

void thinning( unsigned char img[][WIDTH] )
{
	int n=2; //don't care 
	
	unsigned char B[8][3][3]={
							{0,0,0, n,1,n, 1,1,1}, {n,0,0, 1,1,0, 1,1,n},
							{1,n,0, 1,1,0, 1,n,0}, {n,1,n, 1,1,0, n,0,0}, 
							{1,1,1, n,1,n, 0,0,0}, {n,1,n, 0,1,1, 0,0,n}, 
							{0,n,1,	0,1,1, 0,n,1}, {0,0,n, 0,1,1, n,1,n} };

	unsigned char hitmiss_set[HEIGHT][WIDTH];

	int x=0,y=0, iteration=0, unchanged_num=1;
	
	// 8���� ����ũ�� ���̻��� ��ȭ�� ���� ������ ����
	for (iteration=0; iteration<1000; iteration++)
	{
		//���� ����ũ�� hit-or-miss transformation set ����
		hit_or_miss(B[iteration%8], img, hitmiss_set);

		if	(unchanged_num % 8 == 0)
			break;

		//img���� hitmiss set ����
		if (difference(img, hitmiss_set)==1)	//��ȭ�� �ִٸ� 
			unchanged_num=1;
		else 
			unchanged_num++;				
	}
}

void hit_or_miss(unsigned char mask[][3], 
							  unsigned char in_img[][WIDTH], unsigned char hitmiss_set[][WIDTH])
{
	int x,y,i,j, matched=0;
	
	for (x=0; x<HEIGHT; x++)
		for (y=0; y<WIDTH; y++)
			hitmiss_set[x][y] = 0;

	//�� �ȼ��� ���Ͽ� 
	for( x=0;x<HEIGHT-1;x++)
	{
		for( y=0;y<WIDTH-1;y++)
		{	
			if (in_img[1+x][1+y] == FOREGROUND)
			{
				//����ũ�� match���� �˾Ƴ��� 
				matched = 0;
				for(i=0;i<3;i++)
				{
					for(j=0;j<3;j++)
					{
						if (mask[i][j]==2)	matched++;	//dont' care
						if (mask[i][j]==in_img[i+x][j+y])
							matched++;	
					}
				}
				if (matched == 9)	//���ŵ� �κ�
					hitmiss_set[1+x][1+y]=FOREGROUND;	
				else
					hitmiss_set[1+x][1+y]=BACKGROUND;
			}
		}	
	}
}

void boundary_extraction(unsigned char img[][WIDTH])
{
	unsigned char mask[3][3] = { 1,1,1, 1,1,1, 1,1,1 };
	unsigned char in_img[HEIGHT][WIDTH];

	int x, y;

	for (x=0; x<HEIGHT; x++)
		for (y=0; y<WIDTH; y++)
			in_img[x][y] = img[x][y];
	
	erosion(img);	
	
	difference(in_img, img);

	//image_copy(in_img, img);
	int i,j;
	for(i=0;i<HEIGHT;i++)
		for(j=0;j<WIDTH;j++)
			img[i][j] = in_img[i][j];
}

void erosion( unsigned char img[][WIDTH] )
{
	int n=2;
	unsigned char mask[3][3]={1,1,1,	//����ũ
							  1,1,1,
							  1,1,1};	
	unsigned char in_img[HEIGHT][WIDTH];
	int x=0,y=0,matched=0;
	int i,j; 

	for( x=0;x<HEIGHT;x++){
		for( y=0;y<WIDTH;y++)
		{
			in_img[x][y] = img[x][y];	
			img[x][y] = BACKGROUND;	//��� �̹��� �ʱ�ȭ
		}
	}
	
	//�� �ȼ�(1+x,1+y)�� origin���� ��� mask ����
	for( x=0;x<HEIGHT-1;x++)
	{
		for( y=0;y<WIDTH-1;y++)
		{
			matched=0;		
			for(i=0;i<3;i++)
			{
				for(j=0;j<3;j++)
				{
//					if (mask[i][j]==n)	matched++;
					if (mask[i][j]==in_img[i+x][j+y])
						matched++;
				}
			}
		
			if (matched==9)
				img[1+x][1+y]=FOREGROUND;
			else
				img[1+x][1+y]=BACKGROUND;
		
		}		
	}
	
}

void dilation( unsigned char img[][A_WIDTH] )
{
	int n=2;
	unsigned char mask[3][3]={0,1,0,	//����ũ
							  1,1,1,
							  0,1,0};	
	unsigned char in_img[A_HEIGHT][A_WIDTH];
	int x=0,y=0,matched=0;
	int i,j; 

	for( x=0;x<A_HEIGHT;x++){
		for( y=0;y<A_WIDTH;y++)
		{
			in_img[x][y] = img[x][y];	
		}
	}
	
	//�� �ȼ�(1+x,1+y)�� origin���� ��� mask ����
	for( x=0;x<A_HEIGHT-1;x++)
	{
		for( y=0;y<A_WIDTH-1;y++)
		{
			matched=0;		
			for(i=0;i<3;i++)
			{
				for(j=0;j<3;j++)
				{
					//if (mask[i][j]==in_img[i+x][j+y])
					if (i!=j && mask[i][j]==1 && in_img[i+x][j+y]==1)
							matched++;
				}
			}
		
			if (matched==0)
				img[1+x][1+y]=BACKGROUND;
			else
				img[1+x][1+y]=FOREGROUND;
		
		}		
	}		
}

// A - B 
int 
difference(unsigned char img1[][WIDTH], unsigned char img2[][WIDTH])
{
	int x,y;
	int changed = 0;
	for( x=0;x<HEIGHT;x++)
		for( y=0;y<WIDTH;y++)
		{
			if (img2[x][y]==FOREGROUND)
			{
				img1[x][y] = BACKGROUND;
				changed = 1;
			}
		}
	
	return changed;
}

