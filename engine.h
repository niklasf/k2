#include "eval.h"
#include "extern.h"
#include <iomanip>
#include <ctime>
#include "Timer.h"

//--------------------------------
#define UNUSED(x) (void)(x)

#define MAX_MOVES       256
#define RESIGN_VALUE    850
#define RESIGN_MOVES    3

#define BETA_CUTOFF 1000
#define MOVE_IS_NULL 0xFF
#define HASH_ENTRIES_PER_MB 15625

#ifndef DONT_USE_HASH_TABLE
    #include <unordered_map>
    enum {hNONE, hEXACT, hUPPER, hLOWER};
    struct hashEntryStruct
    {
        short       value;
        Move        best_move;
        unsigned    depth           : 7;
        unsigned    bound_type      : 2;
        bool        avoid_null_move : 1;
        bool        only_move       : 1;
        bool        in_check        : 1;
    };
#endif // DONT_USE_HASH_TABLE
//--------------------------------

void InitEngine();
void Perft(int depth);
short Search(int depth, short alpha, short beta, int lmr);
short Quiesce(short alpha, short beta);
void StorePV(Move m);
void UpdateStatistics(Move m, int dpt, unsigned i);
short RootSearch(int depth, short alpha, short beta);
void RootMoveGen(bool ic);
void MainSearch();
void InitSearch();
void PrintSearchResult();
void PlyOutput(short sc);
void InitTime();
bool ShowPV(int _ply);
void Ambiguous(Move m);
bool MakeMoveFinaly(char *mov);
void EvalAllMaterialAndPST();
bool FenStringToEngine(char *fen);
bool DrawDetect();
bool SimpleDrawByRepetition();
void CheckForInterrupt();
void MkMove(Move m);
void UnMove(Move m);
bool DrawByRepetitionInRoot(Move lastMove);
void MakeNullMove();
void UnMakeNullMove();
bool NullMove(int depth, short beta, bool ic, int lmr_);
bool Futility(int depth, short beta);
bool DrawByRepetition();
void ShowFen();
void ReHash(int size_MB);
#ifndef DONT_USE_HASH_TABLE
bool HashProbe(int depth, short alpha, short beta,
               hashEntryStruct *entry, bool *best_move_hashed);
bool PseudoLegal(Move m, bool stm);
#endif // DONT_USE_HASH_TABLE
