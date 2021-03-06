#include "movegen.h"





//--------------------------------
Move pv[max_ply][max_ply + 1];          // the 'flg' property of first element in a row is length of PV at that depth
Move killers[max_ply][2];
unsigned history[2][6][128];
unsigned min_history, max_history;





//--------------------------------
void InitMoveGen()
{
    InitEval();
}





//--------------------------------
void PushMove(Move *move_array, int *movCr, short_list<UC, lst_sz>::iterator it, UC to, UC flg)
{
    Move    m;
    m.pc    = it;
    m.to    = to;
    m.flg   = flg;

    move_array[(*movCr)++] = m;
}





//--------------------------------
int GenMoves(Move *move_array, int apprice, Move *best_move)
{
    int i, to, ray;
    int moveCr = 0;

    GenCastles(move_array, &moveCr);

    auto it = coords[wtm].begin();
    for(;it != coords[wtm].end(); ++it)
    {
        UC fr = *it;
        UC at = b[fr]/2;
        if(at == _p/2)
        {
            GenPawn(move_array, &moveCr, it);
            continue;
        }

        if(!slider[at])
        {
            for(ray = 0; ray < rays[at]; ray++)
            {
                to = fr + shifts[at][ray];
                if(ONBRD(to) && !LIGHT(b[to], wtm))
                    PushMove(move_array, &moveCr, it, to, b[to] ? mCAPT : 0);
            }
            continue;
        }

        for(ray = 0; ray < rays[at]; ray++)
        {
            to = fr;
            for(i = 0; i < 8; i++)
            {
                to += shifts[at][ray];
                if(!ONBRD(to))
                    break;
                UC tt = b[to];
                if(!tt)
                {
                    PushMove(move_array, &moveCr, it, to, 0);
                    continue;
                }
                if(DARK(b[to], wtm))
                    PushMove(move_array, &moveCr, it, to, mCAPT);
                break;
            }// for(i
        }// for(ray
    }// for(it
    if(apprice == APPRICE_ALL)
        AppriceMoves(move_array, moveCr, best_move);
    else if(apprice == APPRICE_CAPT)
        AppriceQuiesceMoves(move_array, moveCr);

    return moveCr;
}





//--------------------------------
void GenPawn(Move *move_array,
             int *moveCr,
             short_list<UC, lst_sz>::iterator it)
{
    unsigned to, pBeg, pEnd;
    UC fr = *it;
    if((wtm && ROW(fr) == 6) || (!wtm && ROW(fr) == 1))
    {
        pBeg = mPR_Q;
        pEnd = mPR_B;
    }
    else
    {
        pBeg = 0;
        pEnd = 0;
    }
    for(unsigned i = pBeg; i <= pEnd; ++i)
        if(wtm)
        {
            to = fr + 17;
            if(ONBRD(to) && DARK(b[to], wtm))
                PushMove(move_array, moveCr, it, to, mCAPT | i);
            to = fr + 15;
            if(ONBRD(to) && DARK(b[to], wtm))
                PushMove(move_array, moveCr, it, to, mCAPT | i);
            to = fr + 16;
            if(ONBRD(to) && !b[to])                 // ONBRD not needed
                PushMove(move_array, moveCr, it, to, i);
            if(ROW(fr) == 1 && !b[fr + 16] && !b[fr + 32])
                PushMove(move_array, moveCr, it, fr + 32, 0);
            int ep = b_state[prev_states + ply].ep;
            int delta = ep - 1 - COL(fr);
            if(ep && ABSI(delta) == 1 && ROW(fr) == 4)
                PushMove(move_array, moveCr, it, fr + 16 + delta, mCAPT | mENPS);
        }
        else
        {
            to = fr - 17;
            if(ONBRD(to) && DARK(b[to], wtm))
                PushMove(move_array, moveCr, it, to, mCAPT | i);
            to = fr - 15;
            if(ONBRD(to) && DARK(b[to], wtm))
                PushMove(move_array, moveCr, it, to, mCAPT | i);
            to = fr - 16;
            if(ONBRD(to) && !b[to])                  // ONBRD not needed
                PushMove(move_array, moveCr, it, to, i);
            if(ROW(fr) == 6 && !b[fr - 16] && !b[fr - 32])
                PushMove(move_array, moveCr, it, fr - 32, 0);
            int ep = b_state[prev_states + ply].ep;
            int delta = ep - 1 - COL(fr);
            if(ep && ABSI(delta) == 1 && ROW(fr) == 3)
                PushMove(move_array, moveCr, it, fr - 16 + delta, mCAPT | mENPS);
        }
}





//--------------------------------
void GenPawnCap(Move *move_array,
                int *moveCr,
                short_list<UC, lst_sz>::iterator it)
{
    unsigned to, pBeg, pEnd;
    UC fr = *it;
    if((wtm && ROW(fr) == 6) || (!wtm && ROW(fr) == 1))
    {
        pBeg = mPR_Q;
        pEnd = mPR_Q;
    }
    else
    {
        pBeg = 0;
        pEnd = 0;
    }
    for(unsigned i = pBeg; i <= pEnd; ++i)
        if(wtm)
        {
            to = fr + 17;
            if(ONBRD(to) && DARK(b[to], wtm))
                PushMove(move_array, moveCr, it, to, mCAPT | i);
            to = fr + 15;
            if(ONBRD(to) && DARK(b[to], wtm))
                PushMove(move_array, moveCr, it, to, mCAPT | i);
            to = fr + 16;
            if(pBeg && !b[to])
                PushMove(move_array, moveCr, it, to, i);
            int ep = b_state[prev_states + ply].ep;
            int delta = ep - 1 - COL(fr);
            if(ep && ABSI(delta) == 1 && ROW(fr) == 4)
                PushMove(move_array, moveCr, it, fr + 16 + delta, mCAPT | mENPS);
        }
        else
        {
            to = fr - 17;
            if(ONBRD(to) && DARK(b[to], wtm))
                PushMove(move_array, moveCr, it, to, mCAPT | i);
            to = fr - 15;
            if(ONBRD(to) && DARK(b[to], wtm))
                PushMove(move_array, moveCr, it, to, mCAPT | i);
            to = fr - 16;
            if(pBeg && !b[to])
                PushMove(move_array, moveCr, it, to, i);
            int ep = b_state[prev_states + ply].ep;
            int delta = ep - 1 - COL(fr);
            if(ep && ABSI(delta) == 1 && ROW(fr) == 3)
                PushMove(move_array, moveCr, it, fr - 16 + delta, mCAPT | mENPS);
        }
}





//--------------------------------
void GenCastles(Move *move_array, int *moveCr)
{
    UC msk = wtm ? 0x03 : 0x0C;
    UC cst = b_state[prev_states + ply].cstl & msk;
    int check = -1;
    if(!cst)
        return;
    if(wtm)
    {
        if((cst & 1) && !b[0x05] && !b[0x06])
        {
            check = Attack(0x04, black);
            if(!check && !Attack(0x05, black) && !Attack(0x06, black))
                PushMove(move_array, moveCr,
                         king_coord[white], 0x06, mCS_K);
        }
        if((cst & 2) && !b[0x03] && !b[0x02] && !b[0x01])
        {
            if(check == -1)
                check = Attack(0x04, black);
            if(!check && !Attack(0x03, black) && !Attack(0x02, black))
                PushMove(move_array, moveCr,
                         king_coord[white], 0x02, mCS_Q);
        }
    }
    else
    {
        if((cst & 4) && !b[0x75] && !b[0x76])
        {
            check = Attack(0x74, white);
            if(!check && !Attack(0x75, white) && !Attack(0x76, white))
                PushMove(move_array, moveCr,
                         king_coord[black], 0x76, mCS_K);
        }
        if((cst & 8) && !b[0x73] && !b[0x72] && !b[0x71])
        {
            if(check == -1)
                check = Attack(0x74, white);
            if(!check && !Attack(0x73, white) && !Attack(0x72, white))
                PushMove(move_array, moveCr,
                         king_coord[black], 0x72, mCS_Q);
        }
    }
}





//--------------------------------
int GenCaptures(Move *move_array)
{
    int i, to, ray;
    int moveCr = 0;
    auto it = coords[wtm].begin();
    for(; it != coords[wtm].end(); ++it)
    {
        UC fr = *it;
        UC at = b[fr]/2;
        if(at == _p/2)
        {
            GenPawnCap(move_array, &moveCr, it);
            continue;
        }
        if(!slider[at])
        {
            for(ray = 0; ray < rays[at]; ray++)
            {
                to = fr + shifts[at][ray];
                if(ONBRD(to) && DARK(b[to], wtm))
                    PushMove(move_array, &moveCr, it, to, mCAPT);
            }
            continue;
        }

        for(ray = 0; ray < rays[at]; ray++)
        {
            to = fr;
            for(i = 0; i < 8; i++)
            {
                to += shifts[at][ray];
                if(!ONBRD(to))
                    break;
                UC tt = b[to];
                if(!tt)
                    continue;
                if(DARK(tt, wtm))
                    PushMove(move_array, &moveCr, it, to, mCAPT);
                break;
            }// for(i
        }// for(ray
    }// for(pc
    AppriceQuiesceMoves(move_array, moveCr);
    SortQuiesceMoves(move_array, moveCr);
    return moveCr;
}





//--------------------------------
void AppriceMoves(Move *move_array, int moveCr, Move *bestMove)
{
#ifndef DONT_USE_HISTORY
    min_history = UINT_MAX;
    max_history = 0;
#endif

    auto it = coords[wtm].begin();
    Move bm = *move_array;
    if(bestMove == nullptr)
        bm.flg = 0xFF;
    else
        bm = *bestMove;
    for(int i = 0; i < moveCr; i++)
    {
        Move m = move_array[i];

        it = m.pc;
        UC fr_pc = b[*it];
        UC to_pc = b[m.to];

        if(m == bm)
            move_array[i].scr = PV_FOLLOW;
        else
        if(to_pc == __ && !(m.flg & mPROM))
        {
            if(m == killers[ply][0])
                move_array[i].scr = FIRST_KILLER;
            else if(m == killers[ply][1])
                move_array[i].scr = SECOND_KILLER;
            else
            {
#ifndef DONT_USE_HISTORY
                UC fr = *it;
                unsigned h = history[wtm][b[fr]/2 - 1][m.to];
                if(h > max_history)
                    max_history = h;
                if(h < min_history)
                    min_history = h;
#endif // DONT_USE_HISTORY
                int y   = ROW(m.to);
                int x   = COL(m.to);
                int y0  = ROW(*it);
                int x0  = COL(*it);
                if(wtm)
                {
                    y   = 7 - y;
                    y0  = 7 - y0;
                }
                int pstVal  = pst[fr_pc/2 - 1][0][y][x] - pst[fr_pc/2 - 1][0][y0][x0];
                pstVal      = 96 + pstVal/2;
                move_array[i].scr = pstVal;
            } // else (ordinary move)
        }// if(to_pc == __ &&
        else
        {
            int ans;
            int src = streng[fr_pc/2];
            int dst = (m.flg & mCAPT) ? streng[to_pc/2] : 0;

#ifndef DONT_USE_SEE_SORTING
            if(dst && dst - src <= 0)
            {
                short tmp = SEE_main(m);
                dst = tmp;
                src = 0;
            }
#else
            if(src > 120)
            {
                move_array[i].scr = EQUAL_CAPTURE;
                continue;
            }
            else
#endif // DONT_USE_SEE_SORTING
            if(dst > 120)
            {
                move_array[i].scr = 0;
                continue;
            }

            short prms[] = {0, 120, 40, 60, 40};
            if(dst <= 120 && (m.flg & mPROM))
                dst += prms[m.flg & mPROM];

            if(dst >= src)
                ans = dst - src/16;
            else
                ans = dst - src;

            if(dst - src >= 0)
            {
                assert(200 + ans/8 > FIRST_KILLER);
                assert(200 + ans/8 <= 250);
                move_array[i].scr = (200 + ans/8);
            }
            else
            {
                if(b[*it]/2 != _k/2)
                {
                    assert(-ans/2 >= 0);
                    assert(-ans/2 <= BAD_CAPTURES);
                    move_array[i].scr = -ans/2;
                }
                else
                {
                    assert(dst/10 >= 0);
                    assert(dst/10 <= BAD_CAPTURES);
                    move_array[i].scr = dst/10;
                }
            }
       }// else on captures
    }// for(int i

#ifndef DONT_USE_HISTORY
    for(int i = 0; i < moveCr; i++)
    {
        Move m = move_array[i];
        if(m.scr >= std::min(MOVE_FROM_PV, SECOND_KILLER)
        || (m.flg & mCAPT))
            continue;
        it      = m.pc;
        UC fr   = *it;
        unsigned h = history[wtm][b[fr]/2 - 1][m.to];
        if(h > 3)
        {
            h -= min_history;
            h = 64*h / (max_history - min_history + 1);
            h += 128;
            move_array[i].scr = h;
            continue;
        }
    }// for(int i
#endif
}





//--------------------------------
void AppriceQuiesceMoves(Move *move_array, int moveCr)
{
    auto it = coords[wtm].begin();
    for(int i = 0; i < moveCr; i++)
    {
        Move m = move_array[i];

        it = m.pc;
        UC fr = b[*it];
        UC pt = b[m.to];

        int src = sort_streng[fr/2];
        int dst = (m.flg & mCAPT) ? sort_streng[pt/2] : 0;

        if(dst > 120)
        {
            move_array[i].scr = KING_CAPTURE;
            return;
        }

        short prms[] = {0, 120, 40, 60, 40};
        if(dst <= 120 && (m.flg & mPROM))
            dst += prms[m.flg & mPROM];

        int ans;
        if(dst > src)
            ans = dst - src/16;
        else
        {
            dst = SEE_main(move_array[i]);
            src = 0;
            ans = dst;

        }
        if(dst > 120)
        {
            move_array[i].scr = 0;
            continue;
        }

        if(dst >= src)
        {
            assert(200 + ans/8 > FIRST_KILLER);
            assert(200 + ans/8 <= 250);
            move_array[i].scr = (200 + ans/8);
        }
        else
        {
            if(fr/2 != _k/2)
            {
                assert(-ans/2 >= 0);
                assert(-ans/2 <= BAD_CAPTURES);
                move_array[i].scr = -ans/2;
            }
            else
            {
                assert(dst/10 >= 0);
                assert(dst/10 <= BAD_CAPTURES);
                move_array[i].scr = dst/10;
            }
        }
    }
}





//-----------------------------
short SEE(UC to, short frStreng, short val, bool stm)
{
    auto it = SeeMinAttacker(to);
    if(it == coords[!wtm].end())
        return -val;
    if(frStreng == 15000)
        return -15000;

    val -= frStreng;
    short tmp1 = -val;
    if(wtm != stm && tmp1 < -2)
        return tmp1;

    auto storeMen = it;
    UC storeBrd = b[*storeMen];
    coords[!wtm].erase(it);
    b[*storeMen] = __;
    wtm = !wtm;

    short tmp2 = -SEE(to, streng[storeBrd/2], -val, stm);

    wtm = !wtm;
    val = std::min(tmp1, tmp2);

    it = storeMen;
    coords[!wtm].restore(it);
    b[*storeMen] = storeBrd;
    return val;
}





//-----------------------------
short_list<UC, lst_sz>::iterator SeeMinAttacker(UC to)
{
    int shft_l[] = {15, -17};
    int shft_r[] = {17, -15};
    UC  pw[] = {_p, _P};

    if(b[to + shft_l[!wtm]] == pw[!wtm])
        for(auto it = coords[!wtm].begin();
            it != coords[!wtm].end();
            ++it)
            if(*it == to + shft_l[!wtm])
                return it;

    if(b[to + shft_r[!wtm]] == pw[!wtm])
        for(auto it = coords[!wtm].begin();
            it != coords[!wtm].end();
            ++it)
            if(*it == to + shft_r[!wtm])
                return it;

    auto it = coords[!wtm].begin();
    for(; it != coords[!wtm].end(); ++it)
    {
        UC fr = *it;
        int pt  = b[fr]/2;
        if(pt == _p/2)
            continue;
        UC att = attacks[120 + to - fr] & (1 << pt);
        if(!att)
            continue;
        if(!slider[pt])
            return it;
        if(SliderAttack(to, fr))
             return it;
    }// for (menCr

    return it;
}





//-----------------------------
short SEE_main(Move m)
{
    auto it = coords[wtm].begin();
    it = m.pc;
    UC fr_pc = b[*it];
    UC to_pc = b[m.to];
    short src = streng[fr_pc/2];
    short dst = (m.flg & mCAPT) ? streng[to_pc/2] : 0;
    auto storeMen = coords[wtm].begin();
    storeMen = m.pc;
    UC storeBrd = b[*storeMen];
    coords[wtm].erase(storeMen);
    b[*storeMen] = __;

    short see_score = -SEE(m.to, src, dst, wtm);
    see_score = std::min(dst, see_score);

    coords[wtm].restore(storeMen);
    b[*storeMen] = storeBrd;
    return see_score;
}





//--------------------------------
void SortQuiesceMoves(Move *move_array, int moveCr)
{
    if(moveCr <= 1)
        return;
    for(int i = 0; i < moveCr; ++i)
    {
        bool swoped_around = false;
        for(int j = 0; j < moveCr - i - 1; ++j)
        {
            if(move_array[j + 1].scr > move_array[j].scr)
            {
                std::swap(move_array[j], move_array[j + 1]);
                swoped_around = true;
            }
        }
        if(!swoped_around)
            break;
    }
}


