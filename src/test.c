#include "test.h"
#include <stdio.h>
#include "search.h"

void tst_movegen(){
	//perft results from https://www.chessprogramming.org/Perft_Results
	char positions[6][MAX_FEN_SIZE] = {
		STARTPOS_FEN,//startpos
		"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -",//p2
		"8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1",//p3
		"r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1",//p4
		"rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8",//p5
		"r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10"//p6
	};

	int results[18] = {
		197281, 4865609, 119060324,//startpos
		97862,  4085603, 193690690,//p2
		674624, 11030083,178633661,//p3
		9467,   422333,  15833292,//p4
		62379,  2103487, 89941194,//p5
		89890,  3894594, 164075551//p6
	};

	int depths[18] = {
		4,5,6,//startpos
		3,4,5,//p2
		5,6,7,//p3
		3,4,5,//p4
		3,4,5,//p5
		3,4,5,//p6
	};

	float time = 0;
	for(int i = 0; i<18; i++){
		printf("position %d \"%s\":\n", i, positions[i]);
		printf("  ");
		time += perft(positions[i/3], depths[i], results[i], false);
	}
	printf("Total time: %fs", time);
}
