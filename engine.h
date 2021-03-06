#include "hash.h"
#include "extern.h"
#include <iomanip>
#include <ctime>
#include <algorithm>                // for sorting of vectors
#include "Timer.h"





//--------------------------------
#define ENGINE_VERSION "083"

//#define DONT_SHOW_STATISTICS
//#define DONT_USE_FUTILITY
//#define DONT_USE_NULL_MOVE
//#define DONT_USE_DELTA_PRUNING
//#define DONT_USE_HISTORY
//#define DONT_USE_LMR
//#define DONT_USE_MATE_DISTANCE_PRUNING
//#define DONT_USE_PVS_IN_ROOT
//#define DONT_USE_RECAPTURE_EXTENSION
//#define DONT_USE_RANDOMNESS
//#define DONT_USE_ASPIRATION_WINDOWS
//#define CLEAR_HASH_TABLE_AFTER_EACH_MOVE
#define DONT_USE_ONE_REPLY_EXTENSION
#define DONT_USE_IID





//--------------------------------
#define UNUSED(x) (void)(x)

#define MAX_MOVES       256
#define RESIGN_VALUE    850
#define RESIGN_MOVES    3
#define MOVE_IS_NULL 0xFF





const int mate_score = K_VAL - (short)max_ply;
const bool all_moves = false;
const bool captures_only = true;
const int no_lmr = 0;
const unsigned nodes_to_check_stop = 511;
#ifdef TUNE_PARAMETERS
extern std::vector <float> param;
#endif





enum {all_node = -1, pv_node = 0, cut_node = 1};





//--------------------------------
void InitEngine();
void Perft(int depth);
short Search(int depth, short alpha, short beta, signed char node_type, int lmr_parent);
short Quiesce(short alpha, short beta);
void StorePV(Move m);
void UpdateStatistics(Move m, int dpt, unsigned i);
short RootSearch(int depth, short alpha, short beta);
void RootMoveGen(bool ic);
void MainSearch();
void InitSearch();
void PrintSearchResult();
void PlyOutput(short sc, char exclimation);
void InitTime();
bool ShowPV(int _ply);
void Ambiguous(Move m);
bool MakeMoveFinaly(char *mov);
bool FenStringToEngine(char *fen);
bool DrawDetect();
bool SimpleDrawByRepetition();
void CheckForInterrupt();
bool DrawByRepetitionInRoot(Move lastMove);
void MakeNullMove();
void UnMakeNullMove();
bool NullMove(int depth, short beta, bool ic);
bool Futility(int depth, short beta);
bool DrawByRepetition();
void ShowFen();
void ReHash(int size_mb);
bool HashProbe(int depth, short *alpha, short beta,
               tt_entry **entry, bool *best_move_hashed);
bool PseudoLegal(Move &m, bool stm);
Move Next(Move *move_array, unsigned cur, unsigned *top,
          bool *best_move_hashed, tt_entry *entry,
          UC stm, bool captures_only, bool in_check, Move prev_move);
void StoreResultInHash(int depth, short _alpha, short alpha, short beta,
                       unsigned legals,
                       bool beta_cutoff, Move best_move, bool one_reply);
bool DetectOnlyMove(bool beta_cutoff, bool in_check,
                    unsigned move_cr, unsigned max_moves,
                    Move *move_array);
void ShowCurrentUciInfo();
void MoveToStr(Move m, bool stm, char *out);
void PonderHit();
void ShowPVfailHighOrLow(Move m, short x, char exclimation);
