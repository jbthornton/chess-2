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
	{.number = C64(9007267974742048), .mask = C64(565157600297596), .maxIndex = 2047},
	{.number = C64(4512395787632656), .mask = C64(1130315200595066), .maxIndex = 2049},
	{.number = C64(9009433176769560), .mask = C64(2260630401190006), .maxIndex = 2207},
	{.number = C64(18018797629865989), .mask = C64(4521260802379886), .maxIndex = 2051},
	{.number = C64(9008402382488321), .mask = C64(9042521604759646), .maxIndex = 2615},
	{.number = C64(18015498029498432), .mask = C64(18085043209519166), .maxIndex = 2047},
	{.number = C64(108086529854800000), .mask = C64(36170086419038334), .maxIndex = 4095},
	{.number = C64(4611756387442098372), .mask = C64(282578800180736), .maxIndex = 2311},
	{.number = C64(2305860601534218256), .mask = C64(565157600328704), .maxIndex = 1023},
	{.number = C64(4719781222774079496), .mask = C64(1130315200625152), .maxIndex = 1024},
	{.number = C64(288265577703800865), .mask = C64(2260630401218048), .maxIndex = 1023},
	{.number = C64(1152956697570967808), .mask = C64(4521260802403840), .maxIndex = 1023},
	{.number = C64(35186521669888), .mask = C64(9042521604775424), .maxIndex = 1023},
	{.number = C64(180214354912772097), .mask = C64(18085043209518592), .maxIndex = 1023},
	{.number = C64(36063990518005890), .mask = C64(36170086419037696), .maxIndex = 2099},
	{.number = C64(1170936040556331040), .mask = C64(282578808340736), .maxIndex = 2047},
	{.number = C64(1125968626843664), .mask = C64(565157608292864), .maxIndex = 1023},
	{.number = C64(19140332792971272), .mask = C64(1130315208328192), .maxIndex = 1023},
	{.number = C64(4612249105886879746), .mask = C64(2260630408398848), .maxIndex = 1023},
	{.number = C64(18296079694987280), .mask = C64(4521260808540160), .maxIndex = 1501},
	{.number = C64(36310340723544128), .mask = C64(9042521608822784), .maxIndex = 1023},
	{.number = C64(18014674461163521), .mask = C64(18085043209388032), .maxIndex = 1023},
	{.number = C64(140874931503136), .mask = C64(36170086418907136), .maxIndex = 2048},
	{.number = C64(306279959302242320), .mask = C64(282580897300736), .maxIndex = 2049},
	{.number = C64(9263908831681970232), .mask = C64(565159647117824), .maxIndex = 1029},
	{.number = C64(1128099064381448), .mask = C64(1130317180306432), .maxIndex = 1023},
	{.number = C64(7502999178761470976), .mask = C64(2260632246683648), .maxIndex = 1023},
	{.number = C64(405326166025568512), .mask = C64(4521262379438080), .maxIndex = 1023},
	{.number = C64(87821292784320640), .mask = C64(9042522644946944), .maxIndex = 1023},
	{.number = C64(360288520484421696), .mask = C64(18085043175964672), .maxIndex = 1023},
	{.number = C64(1152956689515823234), .mask = C64(36170086385483776), .maxIndex = 2067},
	{.number = C64(9241386504085831720), .mask = C64(283115671060736), .maxIndex = 2047},
	{.number = C64(73183528305557520), .mask = C64(565681586307584), .maxIndex = 1023},
	{.number = C64(1729452660018381968), .mask = C64(1130822006735872), .maxIndex = 1311},
	{.number = C64(9223407229818897408), .mask = C64(2261102847592448), .maxIndex = 1023},
	{.number = C64(35188669153800), .mask = C64(4521664529305600), .maxIndex = 1031},
	{.number = C64(2308129997696532608), .mask = C64(9042787892731904), .maxIndex = 1023},
	{.number = C64(329889222464913409), .mask = C64(18085034619584512), .maxIndex = 1023},
	{.number = C64(5764748397963640896), .mask = C64(36170077829103616), .maxIndex = 2048},
	{.number = C64(585557080786601984), .mask = C64(420017753620736), .maxIndex = 2325},
	{.number = C64(567352697554944), .mask = C64(699298018886144), .maxIndex = 1313},
	{.number = C64(4684871711546411008), .mask = C64(1260057572672512), .maxIndex = 1535},
	{.number = C64(2199090372640), .mask = C64(2381576680245248), .maxIndex = 1023},
	{.number = C64(1099545190432), .mask = C64(4624614895390720), .maxIndex = 1023},
	{.number = C64(144396800499916801), .mask = C64(9110691325681664), .maxIndex = 1023},
	{.number = C64(18084769401159681), .mask = C64(18082844186263552), .maxIndex = 1023},
	{.number = C64(9368621921467437060), .mask = C64(36167887395782656), .maxIndex = 2503},
	{.number = C64(9529616880237543456), .mask = C64(35466950888980736), .maxIndex = 2047},
	{.number = C64(8813273939984), .mask = C64(34905104758997504), .maxIndex = 1023},
	{.number = C64(36310306422652936), .mask = C64(34344362452452352), .maxIndex = 1024},
	{.number = C64(288234774735224864), .mask = C64(33222877839362048), .maxIndex = 1023},
	{.number = C64(2305878202175782944), .mask = C64(30979908613181440), .maxIndex = 1023},
	{.number = C64(1100048531488), .mask = C64(26493970160820224), .maxIndex = 1023},
	{.number = C64(7075190200044912672), .mask = C64(17522093256097792), .maxIndex = 1023},
	{.number = C64(648665681975312416), .mask = C64(35607136465616896), .maxIndex = 2096},
	{.number = C64(6998875399535067202), .mask = C64(9079539427579068672), .maxIndex = 4095},
	{.number = C64(8869115863233), .mask = C64(8935706818303361536), .maxIndex = 3855},
	{.number = C64(144119621564239906), .mask = C64(8792156787827803136), .maxIndex = 3967},
	{.number = C64(576469618189934594), .mask = C64(8505056726876686336), .maxIndex = 3839},
	{.number = C64(2233384043521), .mask = C64(7930856604974452736), .maxIndex = 3839},
	{.number = C64(1155173871356348417), .mask = C64(6782456361169985536), .maxIndex = 3839},
	{.number = C64(36031013259837569), .mask = C64(4485655873561051136), .maxIndex = 3831},
	{.number = C64(1170956811088519298), .mask = C64(9115426935197958144), .maxIndex = 4095},
};

static MagicData bishopMagics[64] = {
	{.number = C64(9223442681554812931), .mask = C64(18049651735527936), .maxIndex = 260},
	{.number = C64(13835093377632174176), .mask = C64(70506452091904), .maxIndex = 121},
	{.number = C64(17729759240197), .mask = C64(275415828992), .maxIndex = 59},
	{.number = C64(5908863482966573360), .mask = C64(1075975168), .maxIndex = 179},
	{.number = C64(720580340078350352), .mask = C64(38021120), .maxIndex = 279},
	{.number = C64(2305844110873886720), .mask = C64(8657588224), .maxIndex = 167},
	{.number = C64(1188950853537501568), .mask = C64(2216338399232), .maxIndex = 227},
	{.number = C64(308505372723970080), .mask = C64(567382630219776), .maxIndex = 423},
	{.number = C64(1468736705503707137), .mask = C64(9024825867763712), .maxIndex = 122},
	{.number = C64(1751970900818789377), .mask = C64(18049651735527424), .maxIndex = 124},
	{.number = C64(2814818756067394), .mask = C64(70506452221952), .maxIndex = 59},
	{.number = C64(4651656015010366496), .mask = C64(275449643008), .maxIndex = 179},
	{.number = C64(11259016254619976), .mask = C64(9733406720), .maxIndex = 279},
	{.number = C64(126100793869733904), .mask = C64(2216342585344), .maxIndex = 167},
	{.number = C64(9826872117084422160), .mask = C64(567382630203392), .maxIndex = 227},
	{.number = C64(1226105032919777288), .mask = C64(1134765260406784), .maxIndex = 227},
	{.number = C64(36046390287147009), .mask = C64(4512412933816832), .maxIndex = 59},
	{.number = C64(9223376435979225090), .mask = C64(9024825867633664), .maxIndex = 61},
	{.number = C64(26389625964546), .mask = C64(18049651768822272), .maxIndex = 161},
	{.number = C64(9223380835095806210), .mask = C64(70515108615168), .maxIndex = 191},
	{.number = C64(6917533426761605252), .mask = C64(2491752130560), .maxIndex = 319},
	{.number = C64(5197156169025650817), .mask = C64(567383701868544), .maxIndex = 223},
	{.number = C64(2199291756552), .mask = C64(1134765256220672), .maxIndex = 167},
	{.number = C64(4648841832178941956), .mask = C64(2269530512441344), .maxIndex = 167},
	{.number = C64(1153062315118043138), .mask = C64(2256206450263040), .maxIndex = 59},
	{.number = C64(144185625541615617), .mask = C64(4512412900526080), .maxIndex = 61},
	{.number = C64(140776147259393), .mask = C64(9024834391117824), .maxIndex = 191},
	{.number = C64(576531138244251652), .mask = C64(18051867805491712), .maxIndex = 511},
	{.number = C64(140746078553092), .mask = C64(637888545440768), .maxIndex = 960},
	{.number = C64(72092812769820680), .mask = C64(1135039602493440), .maxIndex = 191},
	{.number = C64(144123992759877637), .mask = C64(2269529440784384), .maxIndex = 111},
	{.number = C64(576465152498470913), .mask = C64(4539058881568768), .maxIndex = 107},
	{.number = C64(9223513324241567762), .mask = C64(1128098963916800), .maxIndex = 109},
	{.number = C64(5188182504992940034), .mask = C64(2256197927833600), .maxIndex = 107},
	{.number = C64(1161929254171066370), .mask = C64(4514594912477184), .maxIndex = 191},
	{.number = C64(18014673655889936), .mask = C64(9592139778506752), .maxIndex = 511},
	{.number = C64(141287313408005), .mask = C64(19184279556981248), .maxIndex = 911},
	{.number = C64(36169671981924608), .mask = C64(2339762086609920), .maxIndex = 191},
	{.number = C64(35218748608513), .mask = C64(4538784537380864), .maxIndex = 59},
	{.number = C64(5188164380100921345), .mask = C64(9077569074761728), .maxIndex = 59},
	{.number = C64(36063989983477776), .mask = C64(562958610993152), .maxIndex = 167},
	{.number = C64(12150712345475743760), .mask = C64(1125917221986304), .maxIndex = 167},
	{.number = C64(8798241554464), .mask = C64(2814792987328512), .maxIndex = 191},
	{.number = C64(5773614723365339152), .mask = C64(5629586008178688), .maxIndex = 207},
	{.number = C64(9007749548736514), .mask = C64(11259172008099840), .maxIndex = 207},
	{.number = C64(563500246175746), .mask = C64(22518341868716544), .maxIndex = 127},
	{.number = C64(576601558779723777), .mask = C64(9007336962655232), .maxIndex = 59},
	{.number = C64(1152939165780836355), .mask = C64(18014673925310464), .maxIndex = 70},
	{.number = C64(576478348801216576), .mask = C64(2216338399232), .maxIndex = 227},
	{.number = C64(13907133243657816096), .mask = C64(4432676798464), .maxIndex = 227},
	{.number = C64(3459331896188604418), .mask = C64(11064376819712), .maxIndex = 171},
	{.number = C64(9070975126848), .mask = C64(22137335185408), .maxIndex = 153},
	{.number = C64(4400196092416), .mask = C64(44272556441600), .maxIndex = 157},
	{.number = C64(2305843561118040322), .mask = C64(87995357200384), .maxIndex = 61},
	{.number = C64(5765170611501400192), .mask = C64(35253226045952), .maxIndex = 121},
	{.number = C64(216314069492695104), .mask = C64(70506452091904), .maxIndex = 121},
	{.number = C64(90072267962449956), .mask = C64(567382630219776), .maxIndex = 423},
	{.number = C64(2380152437433466888), .mask = C64(1134765260406784), .maxIndex = 227},
	{.number = C64(1441151915252547601), .mask = C64(2832480465846272), .maxIndex = 171},
	{.number = C64(13477607391245), .mask = C64(5667157807464448), .maxIndex = 153},
	{.number = C64(9367630749861158914), .mask = C64(11333774449049600), .maxIndex = 157},
	{.number = C64(144273519899852801), .mask = C64(22526811443298304), .maxIndex = 61},
	{.number = C64(74593068926377985), .mask = C64(9024825867763712), .maxIndex = 122},
	{.number = C64(648802020878721025), .mask = C64(18049651735527936), .maxIndex = 252},
};

static u64* rookTable = NULL;
static u64* bishopTable = NULL;

static u64 u64_rand();
static u64 u64_rand_fewbits();
static u64 make_attack_mask(int square, bool bishop);
static u64 blockers_from_mask(u64 mask, int x); // shift the bits of x into the attack mask
static bool try_magic(int square, u64* testTable, bool bishop);//testTable needs enough space
static u64 make_destinations_bitboard(int square, u64 blockers, bool bishop);
static void print_magics();

u64 magic_rook_lookup(int square, u64 occupancy){
	u64 blockers = occupancy & rookMagics[square].mask;
	u64 index = blockers*rookMagics[square].number;
	index >>= 64-12;
	return rookTable[rookMagics[square].tableStart+index];
}

u64 magic_bishop_lookup(int square, u64 occupancy){
	u64 blockers = occupancy & bishopMagics[square].mask;
	u64 index = blockers*bishopMagics[square].number;
	index >>= 64-12;
	return bishopTable[bishopMagics[square].tableStart+index];
}

void fill_rook_table(){
	int tableSize = 0;
	for(int i = 0; i<64; i++){
		rookMagics[i].tableStart = tableSize;
		tableSize += rookMagics[i].maxIndex+1;
	}
	tableSize *= sizeof(u64);
	rookTable = malloc(tableSize);
	printf("Rook table size %dKb\n", tableSize/1024);

	for(int square = 0; square<64; square++){
		for(int i = 0; i<(1<<count_bits(rookMagics[square].mask)); i++){
			u64 blockers = blockers_from_mask(rookMagics[square].mask, i);
			u64 index = blockers*rookMagics[square].number;
			index >>= 64-12;
			rookTable[rookMagics[square].tableStart+index] = make_destinations_bitboard(square, blockers, false);
		}
	}
}

void fill_bishop_table(){
	int tableSize = 0;
	for(int i = 0; i<64; i++){
		bishopMagics[i].tableStart = tableSize;
		tableSize += bishopMagics[i].maxIndex+1;
	}
	tableSize *= sizeof(u64);
	bishopTable = malloc(tableSize);
	printf("Bishop table size %dKb\n", tableSize/1024);

	for(int square = 0; square<64; square++){
		for(int i = 0; i<(1<<count_bits(bishopMagics[square].mask)); i++){
			u64 blockers = blockers_from_mask(bishopMagics[square].mask, i);
			u64 index = blockers*bishopMagics[square].number;
			index >>= 64-12;
			bishopTable[bishopMagics[square].tableStart+index] = make_destinations_bitboard(square, blockers, true);
		}
	}
}

static u64 u64_rand(){
	u64 x = 0;
	x |= (random()&(0xFFFF));
	x |= (random()&(0xFFFF))<<16;
	x |= (random()&(0xFFFF))<<32;
	x |= (random()&(0xFFFF))<<48;
	return x;
};

static u64 u64_rand_fewbits(){
	return u64_rand()&u64_rand()&u64_rand();
}

static u64 make_attack_mask(int square, bool bishop){
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

static u64 blockers_from_mask(u64 mask, int x){//x should be in the range 0-2^(num bits in mask)
	u64 blockers = 0;
	int i = 0;
	while(mask){
		if(x&(1<<i))
			SET_BIT64(blockers, bitscan_forward(mask));
		mask &= mask-1;
		i++;
	}
	return blockers;
}

//return true if a valid magic was found that was better than the last one
static bool try_magic(int square, u64* testTable, bool bishop){
	u64 magic = u64_rand_fewbits();
	u64 mask = bishop ? bishopMagics[square].mask : rookMagics[square].mask;
	MagicData* oldMagic = (bishop)? &bishopMagics[square] : &rookMagics[square];
	int maxIndex = 0;
	for(int i = 0; i<(1<<count_bits(mask)); i++){
		u64 blockers = blockers_from_mask(mask, i);
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

static void print_magics(){
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
		rookMagics[i].mask = make_attack_mask(i, false);
		bishopMagics[i].mask = make_attack_mask(i, true);
		rookMagics[i].maxIndex = INT_MAX;
		bishopMagics[i].maxIndex = INT_MAX;
	}
	
	u64* testTable = malloc(sizeof(u64)*(1<<12));
	
	int square = 0;
	while(square<64){
		if(try_magic(square, testTable, false))
			square++;
	}
	
	square = 0;
	while(square<64){
		if(try_magic(square, testTable, true))
			square++;
	}
	free(testTable);
	print_magics();

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

static u64 make_destinations_bitboard(int square, u64 blockers, bool bishop){
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

