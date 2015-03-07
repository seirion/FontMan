// FontImageProcessing.h: interface for the CFontImageProcessing class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FONTIMAGEPROCESSING_H__F039C0F6_9202_4674_A421_9DF0590E6D5B__INCLUDED_)
#define AFX_FONTIMAGEPROCESSING_H__F039C0F6_9202_4674_A421_9DF0590E6D5B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define FOREGROUND 1
#define BACKGROUND 0

	void boundary_extraction(unsigned char img[][WIDTH]);
	void hit_or_miss(unsigned char mask[3][3],unsigned char in_img[][WIDTH], unsigned char hitmiss_set[][WIDTH]);
	void thinning(unsigned char img[][WIDTH]);
	int difference(unsigned char img1[][WIDTH], unsigned char img2[][WIDTH]);
	void erosion( unsigned char img[][WIDTH] );

	// 애니메이션 용
	void dilation( unsigned char img[][A_WIDTH] );
	void regionfilling( unsigned char img[][A_WIDTH], int px, int py );


#endif // !defined(AFX_FONTIMAGEPROCESSING_H__F039C0F6_9202_4674_A421_9DF0590E6D5B__INCLUDED_)
