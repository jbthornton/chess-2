#include <stdbool.h>
#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <time.h>

#include "magic.h"
#include "board.h"

typedef struct MagicData{
	u64 number;
	u64 mask;
	int maxIndex;
	int tableStart;
}MagicData;

static MagicData rookMagics[64] = {
	{.number = C64(5800636891690696992), .mask = C64(282578800148862), .maxIndex = 4095},
	{.number = C64(9008882882970656), .mask = C64(565157600297596), .maxIndex = 2223},
	{.number = C64(9232383634829213728), .mask = C64(1130315200595066), .maxIndex = 3087},
	{.number = C64(40532603375198336), .mask = C64(2260630401190006), .maxIndex = 3487},
	{.number = C64(18018797629865989), .mask = C64(4521260802379886), .maxIndex = 2051},
	{.number = C64(36031010000896012), .mask = C64(9042521604759646), .maxIndex = 3731},
	{.number = C64(36029965522735106), .mask = C64(18085043209519166), .maxIndex = 2495},
	{.number = C64(108086529854800000), .mask = C64(36170086419038334), .maxIndex = 4095},
	{.number = C64(290271338176516), .mask = C64(282578800180736), .maxIndex = 2978},
	{.number = C64(1315068683520840720), .mask = C64(565157600328704), .maxIndex = 1553},
	{.number = C64(2305913412317888560), .mask = C64(1130315200625152), .maxIndex = 1287},
	{.number = C64(36063992130569368), .mask = C64(2260630401218048), .maxIndex = 1163},
	{.number = C64(9223407230892646656), .mask = C64(4521260802403840), .maxIndex = 1119},
	{.number = C64(35321827885184), .mask = C64(9042521604775424), .maxIndex = 1537},
	{.number = C64(144150510096617536), .mask = C64(18085043209518592), .maxIndex = 1135},
	{.number = C64(144150372984834212), .mask = C64(36170086419037696), .maxIndex = 2404},
	{.number = C64(9368614258725621770), .mask = C64(282578808340736), .maxIndex = 2544},
	{.number = C64(9804477160700903432), .mask = C64(565157608292864), .maxIndex = 1039},
	{.number = C64(1153484489188507656), .mask = C64(1130315208328192), .maxIndex = 1795},
	{.number = C64(360428742104907784), .mask = C64(2260630408398848), .maxIndex = 1793},
	{.number = C64(288371663462990088), .mask = C64(4521260808540160), .maxIndex = 2301},
	{.number = C64(144396800516694021), .mask = C64(9042521608822784), .maxIndex = 1545},
	{.number = C64(10484381100753815584), .mask = C64(18085043209388032), .maxIndex = 1475},
	{.number = C64(281612417762368), .mask = C64(36170086418907136), .maxIndex = 3104},
	{.number = C64(36284991017153), .mask = C64(282580897300736), .maxIndex = 2374},
	{.number = C64(9263908831681970232), .mask = C64(565159647117824), .maxIndex = 1029},
	{.number = C64(2595216877592592521), .mask = C64(1130317180306432), .maxIndex = 1971},
	{.number = C64(11529223843235495938), .mask = C64(2260632246683648), .maxIndex = 2015},
	{.number = C64(1100048630784), .mask = C64(4521262379438080), .maxIndex = 2019},
	{.number = C64(4647715365740150816), .mask = C64(9042522644946944), .maxIndex = 1055},
	{.number = C64(27022148593795073), .mask = C64(18085043175964672), .maxIndex = 1024},
	{.number = C64(216324515255435808), .mask = C64(36170086385483776), .maxIndex = 2160},
	{.number = C64(18018865277568032), .mask = C64(283115671060736), .maxIndex = 2175},
	{.number = C64(37154731286593680), .mask = C64(565681586307584), .maxIndex = 1039},
	{.number = C64(571754645028994), .mask = C64(1130822006735872), .maxIndex = 2065},
	{.number = C64(2305948856537317888), .mask = C64(2261102847592448), .maxIndex = 1151},
	{.number = C64(72200547730456832), .mask = C64(4521664529305600), .maxIndex = 2037},
	{.number = C64(18190324666990720), .mask = C64(9042787892731904), .maxIndex = 1279},
	{.number = C64(7025624764551020545), .mask = C64(18085034619584512), .maxIndex = 1055},
	{.number = C64(4665892079117795400), .mask = C64(36170077829103616), .maxIndex = 2272},
	{.number = C64(17593796665346), .mask = C64(420017753620736), .maxIndex = 2563},
	{.number = C64(2260596242255873), .mask = C64(699298018886144), .maxIndex = 1791},
	{.number = C64(17592320278532), .mask = C64(1260057572672512), .maxIndex = 2031},
	{.number = C64(9305707865622716448), .mask = C64(2381576680245248), .maxIndex = 1599},
	{.number = C64(3298568445984), .mask = C64(4624614895390720), .maxIndex = 1537},
	{.number = C64(70508364185601), .mask = C64(9110691325681664), .maxIndex = 1855},
	{.number = C64(9817917592919097377), .mask = C64(18082844186263552), .maxIndex = 1183},
	{.number = C64(1153502193899276296), .mask = C64(36167887395782656), .maxIndex = 3114},
	{.number = C64(17592727142432), .mask = C64(35466950888980736), .maxIndex = 2059},
	{.number = C64(8813273939984), .mask = C64(34905104758997504), .maxIndex = 1023},
	{.number = C64(1477743696450813968), .mask = C64(34344362452452352), .maxIndex = 1663},
	{.number = C64(288232850592137248), .mask = C64(33222877839362048), .maxIndex = 1135},
	{.number = C64(2305878202175782944), .mask = C64(30979908613181440), .maxIndex = 1023},
	{.number = C64(18015635997491232), .mask = C64(26493970160820224), .maxIndex = 1103},
	{.number = C64(163256586012196929), .mask = C64(17522093256097792), .maxIndex = 1794},
	{.number = C64(648665681975312416), .mask = C64(35607136465616896), .maxIndex = 2096},
	{.number = C64(6998875399535067202), .mask = C64(9079539427579068672), .maxIndex = 4095},
	{.number = C64(576619150856691778), .mask = C64(8935706818303361536), .maxIndex = 4049},
	{.number = C64(10090491437549356042), .mask = C64(8792156787827803136), .maxIndex = 4007},
	{.number = C64(13470226456578), .mask = C64(8505056726876686336), .maxIndex = 3875},
	{.number = C64(2233384043521), .mask = C64(7930856604974452736), .maxIndex = 3839},
	{.number = C64(1154051802568984577), .mask = C64(6782456361169985536), .maxIndex = 3847},
	{.number = C64(10995183419457), .mask = C64(4485655873561051136), .maxIndex = 3871},
	{.number = C64(1170956811088519298), .mask = C64(9115426935197958144), .maxIndex = 4095},
};

static MagicData bishopMagics[64] = {
	{.number = C64(845120716933129), .mask = C64(18049651735527936), .maxIndex = 476},
	{.number = C64(281750123315616), .mask = C64(70506452091904), .maxIndex = 126},
	{.number = C64(35321878249984), .mask = C64(275415828992), .maxIndex = 66},
	{.number = C64(5908863482966573360), .mask = C64(1075975168), .maxIndex = 179},
	{.number = C64(720580340078350352), .mask = C64(38021120), .maxIndex = 279},
	{.number = C64(2305844110873886720), .mask = C64(8657588224), .maxIndex = 167},
	{.number = C64(1188950853537501568), .mask = C64(2216338399232), .maxIndex = 227},
	{.number = C64(3458764789235580978), .mask = C64(567382630219776), .maxIndex = 425},
	{.number = C64(11529356059516870658), .mask = C64(9024825867763712), .maxIndex = 124},
	{.number = C64(2252349838997505), .mask = C64(18049651735527424), .maxIndex = 172},
	{.number = C64(1459166348257263904), .mask = C64(70506452221952), .maxIndex = 70},
	{.number = C64(4651656015010366496), .mask = C64(275449643008), .maxIndex = 179},
	{.number = C64(11259016254619976), .mask = C64(9733406720), .maxIndex = 279},
	{.number = C64(126100793869733904), .mask = C64(2216342585344), .maxIndex = 167},
	{.number = C64(9826872117084422160), .mask = C64(567382630203392), .maxIndex = 227},
	{.number = C64(18014433406156816), .mask = C64(1134765260406784), .maxIndex = 229},
	{.number = C64(4899996663224999940), .mask = C64(4512412933816832), .maxIndex = 69},
	{.number = C64(2341876481306658819), .mask = C64(9024825867633664), .maxIndex = 63},
	{.number = C64(1729531790776865796), .mask = C64(18049651768822272), .maxIndex = 407},
	{.number = C64(576471749567713280), .mask = C64(70515108615168), .maxIndex = 253},
	{.number = C64(8796160283648), .mask = C64(2491752130560), .maxIndex = 370},
	{.number = C64(9009398294839680), .mask = C64(567383701868544), .maxIndex = 255},
	{.number = C64(2199291756552), .mask = C64(1134765256220672), .maxIndex = 167},
	{.number = C64(1152922741561630736), .mask = C64(2269530512441344), .maxIndex = 173},
	{.number = C64(684687919536947208), .mask = C64(2256206450263040), .maxIndex = 117},
	{.number = C64(2305983819724890120), .mask = C64(4512412900526080), .maxIndex = 124},
	{.number = C64(4629701552957833217), .mask = C64(9024834391117824), .maxIndex = 252},
	{.number = C64(9223935536564537347), .mask = C64(18051867805491712), .maxIndex = 1544},
	{.number = C64(108156776985137155), .mask = C64(637888545440768), .maxIndex = 1022},
	{.number = C64(17660905586697), .mask = C64(1135039602493440), .maxIndex = 223},
	{.number = C64(2305860610124955650), .mask = C64(2269529440784384), .maxIndex = 117},
	{.number = C64(6917537832325091346), .mask = C64(4539058881568768), .maxIndex = 138},
	{.number = C64(2305914478006403080), .mask = C64(1128098963916800), .maxIndex = 118},
	{.number = C64(1441187615222013987), .mask = C64(2256197927833600), .maxIndex = 127},
	{.number = C64(1441152431068024898), .mask = C64(4514594912477184), .maxIndex = 225},
	{.number = C64(1188950851918626848), .mask = C64(9592139778506752), .maxIndex = 1022},
	{.number = C64(612630424283283584), .mask = C64(19184279556981248), .maxIndex = 1022},
	{.number = C64(141287278772480), .mask = C64(2339762086609920), .maxIndex = 239},
	{.number = C64(2449976064370282501), .mask = C64(4538784537380864), .maxIndex = 111},
	{.number = C64(17660914176004), .mask = C64(9077569074761728), .maxIndex = 122},
	{.number = C64(35192965562384), .mask = C64(562958610993152), .maxIndex = 169},
	{.number = C64(4620693768512208928), .mask = C64(1125917221986304), .maxIndex = 171},
	{.number = C64(8798241685634), .mask = C64(2814792987328512), .maxIndex = 253},
	{.number = C64(1188967894886678601), .mask = C64(5629586008178688), .maxIndex = 249},
	{.number = C64(622905223509344288), .mask = C64(11259172008099840), .maxIndex = 254},
	{.number = C64(4611968868330635277), .mask = C64(22518341868716544), .maxIndex = 203},
	{.number = C64(281612550045698), .mask = C64(9007336962655232), .maxIndex = 97},
	{.number = C64(45071215743833089), .mask = C64(18014673925310464), .maxIndex = 122},
	{.number = C64(576478348801216576), .mask = C64(2216338399232), .maxIndex = 227},
	{.number = C64(13907133243657816096), .mask = C64(4432676798464), .maxIndex = 227},
	{.number = C64(3459331896188604418), .mask = C64(11064376819712), .maxIndex = 171},
	{.number = C64(9070975126848), .mask = C64(22137335185408), .maxIndex = 153},
	{.number = C64(4400196092416), .mask = C64(44272556441600), .maxIndex = 157},
	{.number = C64(4504974556922112), .mask = C64(87995357200384), .maxIndex = 77},
	{.number = C64(5765170611501400192), .mask = C64(35253226045952), .maxIndex = 121},
	{.number = C64(1747713452744638722), .mask = C64(70506452091904), .maxIndex = 132},
	{.number = C64(90072267962449956), .mask = C64(567382630219776), .maxIndex = 423},
	{.number = C64(2380152437433466888), .mask = C64(1134765260406784), .maxIndex = 227},
	{.number = C64(1441151915252547601), .mask = C64(2832480465846272), .maxIndex = 171},
	{.number = C64(13477607391245), .mask = C64(5667157807464448), .maxIndex = 153},
	{.number = C64(9367630749861158914), .mask = C64(11333774449049600), .maxIndex = 157},
	{.number = C64(144273519899852801), .mask = C64(22526811443298304), .maxIndex = 61},
	{.number = C64(2905525997392646145), .mask = C64(9024825867763712), .maxIndex = 138},
	{.number = C64(2342505406252712961), .mask = C64(18049651735527936), .maxIndex = 324},
};

static u64* rookTable = NULL;
static u64* bishopTable = NULL;

u64 getRookDestinations(int square, u64 occupancy){
	u64 blockers = occupancy & rookMagics[square].mask;
	u64 index = blockers*rookMagics[square].number;
	index >>= 64-12;
	return rookTable[rookMagics[square].tableStart+index];
}

u64 getBishopDestinations(int square, u64 occupancy){
	u64 blockers = occupancy & bishopMagics[square].mask;
	u64 index = blockers*bishopMagics[square].number;
	index >>= 64-12;
	return bishopTable[bishopMagics[square].tableStart+index];
}

static u64 u64Rand(){
	u64 x = 0;
	x |= (random()&(0xFFFF));
	x |= (random()&(0xFFFF))<<16;
	x |= (random()&(0xFFFF))<<32;
	x |= (random()&(0xFFFF))<<48;
	return x;
};

static u64 u64RandFewbits(){
	return u64Rand()&u64Rand()&u64Rand();
}

static u64 attackMask(int square, bool bishop){
	u64 mask = 0;
	const int bishopDirections[8] =  { -1,-1, -1,1, 1,-1, 1,1};
	const int rookDirections[8] =  { 1 ,0, -1,0, 0,1, 0,-1};
	for(int d = 0; d<8; d+=2){
		int dx = bishop ? bishopDirections[d] : rookDirections[d];
		int dy = bishop ? bishopDirections[d+1] : rookDirections[d+1];
		int x = square%8;
		int y = square/8;
		for(int i = 0; i<8; i++){
			x+=dx;
			y+=dy;
			if((x+dx)<0 || (x+dx)>=8 || (y+dy)<0 || (y+dy)>=8)
				break;
			SET_BIT64(mask, BOARD_INDEX(x,y));
		}
	}
	return mask;
}

static u64 blockersFromMask(u64 mask, int x){//x should be in the range 0-2^(num bits in mask)
	u64 blockers = 0;
	int i = 0;
	while(mask){
		if(x&(1<<i))
			SET_BIT64(blockers, bitScanForward(mask));
		mask &= mask-1;
		i++;
	}
	return blockers;
}

//return true if a valid magic was found that was better than the last one
static bool tryMagic(int square, u64* testTable, bool bishop){
	u64 magic = u64RandFewbits();
	u64 mask = bishop ? bishopMagics[square].mask : rookMagics[square].mask;
	MagicData* oldMagic = (bishop)? &bishopMagics[square] : &rookMagics[square];
	int maxIndex = 0;
	for(int i = 0; i<(1<<countBits(mask)); i++){
		u64 blockers = blockersFromMask(mask, i);
		int tableIndex = (blockers*magic)>>(64-12);
		if(testTable[tableIndex] == magic) return false;
		testTable[tableIndex] = magic;
		if(tableIndex>maxIndex) maxIndex = tableIndex;
		if(maxIndex>=oldMagic->maxIndex) return false;
	}
	oldMagic->number = magic;
	oldMagic->maxIndex = maxIndex;

	return true;
}

static void printMagics(){
	printf("static MagicData rookMagics[64] = {\n");
	for(int i = 0; i<64; i++){
		printf(
				"\t{.number = C64(%lu), .mask = C64(%lu), .maxIndex = %d},\n",//table start is set when table is filled
				rookMagics[i].number,
				rookMagics[i].mask,
				rookMagics[i].maxIndex
		);
	}
	printf("};\n\n");
	
	printf("static MagicData bishopMagics[64] = {\n");
	for(int i = 0; i<64; i++){
		printf(
				"\t{.number = C64(%lu), .mask = C64(%lu), .maxIndex = %d},\n",
				bishopMagics[i].number,
				bishopMagics[i].mask,
				bishopMagics[i].maxIndex
		);
	}
	printf("};\n\n");
}

void magicSearch(){
	srandom(time(0));
	//init masks
	for(int i = 0; i<64; i++){
		rookMagics[i].mask = attackMask(i, false);
		bishopMagics[i].mask = attackMask(i, true);
		rookMagics[i].maxIndex = INT_MAX;
		bishopMagics[i].maxIndex = INT_MAX;
	}
	
	u64* testTable = malloc(sizeof(u64)*(1<<12));
	
	int square = 0;
	while(square<64){
		if(tryMagic(square, testTable, false))
			square++;
	}
	
	square = 0;
	while(square<64){
		if(tryMagic(square, testTable, true))
			square++;
	}
	free(testTable);
	printMagics();

	printf("\n");
	int tableSize = 0;
	for(int i = 0; i<64; i++){
		tableSize += rookMagics[i].maxIndex+1;
	}
	tableSize *= sizeof(u64);
	printf("Rook table size %dKb\n", tableSize/1024);
	tableSize = 0;
	for(int i = 0; i<64; i++){
		tableSize += bishopMagics[i].maxIndex+1;
	}
	tableSize *= sizeof(u64);
	printf("Bishop table size %dKb\n", tableSize/1024);
}

void refineMagics(int iterations){
	srandom(time(0));
	u64* testTable = malloc(sizeof(u64)*(1<<12));
	for(int i = 0; i<iterations; i++){
		for(int square = 0; square<64; square++){
			tryMagic(square, testTable, true);
			tryMagic(square, testTable, false);
		}
	}

	free(testTable);
	printMagics();

	int tableSize = 0;
	for(int i = 0; i<64; i++){
		tableSize += rookMagics[i].maxIndex+1;
	}
	tableSize *= sizeof(u64);
	printf("Rook table size %dKb\n", tableSize/1024);
	tableSize = 0;
	for(int i = 0; i<64; i++){
		tableSize += bishopMagics[i].maxIndex+1;
	}
	tableSize *= sizeof(u64);
	printf("Bishop table size %dKb\n", tableSize/1024);
}

static u64 destinationsBitboard(int square, u64 blockers, bool bishop){
	u64 destinations = 0;
	const int bishopDirections[8] =  { -1,-1, -1,1, 1,-1, 1,1};
	const int rookDirections[8] =  { 1 ,0, -1,0, 0,1, 0,-1};
	for(int d = 0; d<8; d+=2){
		int dx = bishop ? bishopDirections[d] : rookDirections[d];
		int dy = bishop ? bishopDirections[d+1] : rookDirections[d+1];
		int x = square%8;
		int y = square/8;
		for(int i = 0; i<8; i++){
			x+=dx;
			y+=dy;
			if( (x)<0 || (x)>=8 || (y)<0 || (y)>=8)	
				break;
			SET_BIT64(destinations, BOARD_INDEX(x,y));
			if(GET_BIT64(blockers, BOARD_INDEX(x,y)))
				break;
		}
	}
	return destinations;
}

static void fillRookTable(){
	int tableSize = 0;
	for(int i = 0; i<64; i++){
		rookMagics[i].tableStart = tableSize;
		tableSize += rookMagics[i].maxIndex+1;
	}
	tableSize *= sizeof(u64);
	rookTable = malloc(tableSize);
	printf("Rook table size %dKb\n", tableSize/1024);

	for(int square = 0; square<64; square++){
		for(int i = 0; i<(1<<countBits(rookMagics[square].mask)); i++){
			u64 blockers = blockersFromMask(rookMagics[square].mask, i);
			u64 index = blockers*rookMagics[square].number;
			index >>= 64-12;
			rookTable[rookMagics[square].tableStart+index] = destinationsBitboard(square, blockers, false);
		}
	}
}

static void fillBishopTable(){
	int tableSize = 0;
	for(int i = 0; i<64; i++){
		bishopMagics[i].tableStart = tableSize;
		tableSize += bishopMagics[i].maxIndex+1;
	}
	tableSize *= sizeof(u64);
	bishopTable = malloc(tableSize);
	printf("Bishop table size %dKb\n", tableSize/1024);

	for(int square = 0; square<64; square++){
		for(int i = 0; i<(1<<countBits(bishopMagics[square].mask)); i++){
			u64 blockers = blockersFromMask(bishopMagics[square].mask, i);
			u64 index = blockers*bishopMagics[square].number;
			index >>= 64-12;
			bishopTable[bishopMagics[square].tableStart+index] = destinationsBitboard(square, blockers, true);
		}
	}
}

void fillMagicTables(){
	fillRookTable();
	fillBishopTable();
}
