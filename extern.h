#if !defined(EXTERN)
#define EXTERN





extern UC b[137];
extern short_list<UC, lst_sz> coords[2];
extern UC       slider[7];
extern UC       rays[7];
extern UC       attacks[240];
extern SC       shifts[6][8];
extern UC       pc_streng[6];
extern int      material[2];
extern int      pieces[2];
extern unsigned wtm;
extern unsigned ply;
extern UQ       nodes;
extern Move     pv[max_ply][max_ply + 1];
extern Move     killers[max_ply][2];
extern char     *cv;
extern unsigned history [2][6][128];
extern SC       pst[6][2][8][8];
extern short    val_opn;
extern short    val_end;
extern UC       unused1[24];
extern BrdState b_state[prev_states + max_ply];
extern US       reversible_moves;
extern char     cur_moves[5*max_ply];
extern UC      pawn_max[10][2];
extern UC      pawn_min[10][2];
extern UQ       hash_key;
extern bool     uci;
extern bool     xboard;
extern bool     infinite_analyze;
extern short    streng[7];
extern UQ       tmpCr;
extern short_list<UC, lst_sz>::iterator king_coord[2];
extern bool     pondering_in_process;
extern char     king_dist[120];
extern float    pst_gain[6][2];
extern short    sort_streng[7];
extern UC       quantity[2][6 + 1];
extern UQ       doneHashKeys[101 + max_ply];

#endif
