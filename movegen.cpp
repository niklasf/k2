#include "movegen.h"

//--------------------------------
Move pv[max_ply][max_ply + 1];          // first element in a row is length of PV at that depth
Move kil[max_ply][2];
unsigned history [2][6][128];
unsigned maxHistory;

//--------------------------------
void InitMoveGen()
{
    InitHashTable();
}

//--------------------------------
void PushMove(Move *list, int *movCr, short_list<UC, lst_sz>::iterator it, UC to, UC flg)
{
    Move    m;
    m.pc    = it;
    m.to    = to;
    m.flg   = flg;
#ifndef NDEBUG
    if(to == 0x77)
        ply = ply;
#endif

    list[(*movCr)++] = m;
}

//--------------------------------
int GenMoves(Move *list, int apprice, Move *best_move)
{
    int i, to, ray;
    int moveCr = 0;

    GenCastles(list, &moveCr);

    auto it = coords[wtm].begin();
    for(;it != coords[wtm].end(); ++it)
    {
        UC fr = *it;
        UC at = b[fr]/2;
        if(at == _p/2)
        {
            GenPawn(list, &moveCr, it);
            continue;
        }

        if(!slider[at])
        {
            for(ray = 0; ray < rays[at]; ray++)
            {
                to = fr + shifts[at][ray];
                if(ONBRD(to) && !LIGHT(b[to], wtm))
                    PushMove(list, &moveCr, it, to, b[to] ? mCAPT : 0);
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
                    PushMove(list, &moveCr, it, to, 0);
                    continue;
                }
                if(DARK(b[to], wtm))
                    PushMove(list, &moveCr, it, to, mCAPT);
                break;
            }// for(i
        }// for(ray
    }// for(it
    if(apprice == APPRICE_ALL)
        AppriceMoves(list, moveCr, best_move);
    else if(apprice == APPRICE_CAPT)
        AppriceQuiesceMoves(list, moveCr);
#ifndef DONT_USE_HISTORY
   AppriceHistory(list, moveCr);
#endif // DONT_USE_HISTORY
    return moveCr;
}

//--------------------------------
void GenPawn(Move *list,
             int *moveCr,
             short_list<UC, lst_sz>::iterator it)
{
    unsigned to, pBeg, pEnd;
    UC fr = *it;
    if((wtm && ROW(fr) == 6) || (!wtm && ROW(fr) == 1))
    {
        pBeg = mPR_Q;
        pEnd = mPR_B;   //>> ply <= 2 ? mPR_B : mPR_Q;
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
                PushMove(list, moveCr, it, to, mCAPT | i);
            to = fr + 15;
            if(ONBRD(to) && DARK(b[to], wtm))
                PushMove(list, moveCr, it, to, mCAPT | i);
            to = fr + 16;
            if(ONBRD(to) && !b[to])                 // ONBRD not needed
                PushMove(list, moveCr, it, to, i);
            if(ROW(fr) == 1 && !b[fr + 16] && !b[fr + 32])
                PushMove(list, moveCr, it, fr + 32, 0);
            int ep = boardState[prev_states + ply].ep;
            int delta = ep - 1 - COL(fr);
            if(ep && ABSI(delta) == 1 && ROW(fr) == 4)
                PushMove(list, moveCr, it, fr + 16 + delta, mCAPT | mENPS);
        }
        else
        {
            to = fr - 17;
            if(ONBRD(to) && DARK(b[to], wtm))
                PushMove(list, moveCr, it, to, mCAPT | i);
            to = fr - 15;
            if(ONBRD(to) && DARK(b[to], wtm))
                PushMove(list, moveCr, it, to, mCAPT | i);
            to = fr - 16;
            if(ONBRD(to) && !b[to])                  // ONBRD not needed
                PushMove(list, moveCr, it, to, i);
            if(ROW(fr) == 6 && !b[fr - 16] && !b[fr - 32])
                PushMove(list, moveCr, it, fr - 32, 0);
            int ep = boardState[prev_states + ply].ep;
            int delta = ep - 1 - COL(fr);
            if(ep && ABSI(delta) == 1 && ROW(fr) == 3)
                PushMove(list, moveCr, it, fr - 16 + delta, mCAPT | mENPS);
        }
}

//--------------------------------
void GenPawnCap(Move *list,
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
                PushMove(list, moveCr, it, to, mCAPT | i);
            to = fr + 15;
            if(ONBRD(to) && DARK(b[to], wtm))
                PushMove(list, moveCr, it, to, mCAPT | i);
            to = fr + 16;
            if(pBeg && !b[to])
                PushMove(list, moveCr, it, to, i);
            int ep = boardState[prev_states + ply].ep;
            int delta = ep - 1 - COL(fr);
            if(ep && ABSI(delta) == 1 && ROW(fr) == 4)
                PushMove(list, moveCr, it, fr + 16 + delta, mCAPT | mENPS);
        }
        else
        {
            to = fr - 17;
            if(ONBRD(to) && DARK(b[to], wtm))
                PushMove(list, moveCr, it, to, mCAPT | i);
            to = fr - 15;
            if(ONBRD(to) && DARK(b[to], wtm))
                PushMove(list, moveCr, it, to, mCAPT | i);
            to = fr - 16;
            if(pBeg && !b[to])
                PushMove(list, moveCr, it, to, i);
            int ep = boardState[prev_states + ply].ep;
            int delta = ep - 1 - COL(fr);
            if(ep && ABSI(delta) == 1 && ROW(fr) == 3)
                PushMove(list, moveCr, it, fr - 16 + delta, mCAPT | mENPS);
        }
}

//--------------------------------
void GenCastles(Move *list, int *moveCr)
{
    UC msk = wtm ? 0x03 : 0x0C;
    UC cst = boardState[prev_states + ply].cstl & msk;
    int check = -1;
    if(!cst)
        return;
    if(wtm)
    {
        if((cst & 1) && !b[0x05] && !b[0x06])
        {
            check = Attack(0x04, black);
            if(!check && !Attack(0x05, black) && !Attack(0x06, black))
                PushMove(list, moveCr,
                         king_coord[white], 0x06, mCS_K);
        }
        if((cst & 2) && !b[0x03] && !b[0x02] && !b[0x01])
        {
            if(check == -1)
                check = Attack(0x04, black);
            if(!check && !Attack(0x03, black) && !Attack(0x02, black))
                PushMove(list, moveCr,
                         king_coord[white], 0x02, mCS_Q);
        }
    }
    else
    {
        if((cst & 4) && !b[0x75] && !b[0x76])
        {
            check = Attack(0x74, white);
            if(!check && !Attack(0x75, white) && !Attack(0x76, white))
                PushMove(list, moveCr,
                         king_coord[black], 0x76, mCS_K);
        }
        if((cst & 8) && !b[0x73] && !b[0x72] && !b[0x71])
        {
            if(check == -1)
                check = Attack(0x74, white);
            if(!check && !Attack(0x73, white) && !Attack(0x72, white))
                PushMove(list, moveCr,
                         king_coord[black], 0x72, mCS_Q);
        }
    }
}

//--------------------------------
int GenCaptures(Move *list)
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
            GenPawnCap(list, &moveCr, it);
            continue;
        }
        if(!slider[at])
        {
            for(ray = 0; ray < rays[at]; ray++)
            {
                to = fr + shifts[at][ray];
                if(ONBRD(to) && DARK(b[to], wtm))
                    PushMove(list, &moveCr, it, to, mCAPT);
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
                    PushMove(list, &moveCr, it, to, mCAPT);
                break;
            }// for(i
        }// for(ray
    }// for(pc
    AppriceQuiesceMoves(list, moveCr);
    return moveCr;
}

//--------------------------------
void AppriceMoves(Move *list, int moveCr, Move *bestMove)
{
#ifndef DONT_USE_HISTORY
    maxHistory = 0;
#endif
    auto it = coords[wtm].begin();
    Move bm = *list;
    if(bestMove == nullptr)
        bm.flg = 0xFF;
    else
        bm = *bestMove;
    for(int i = 0; i < moveCr; i++)
    {
        Move m = list[i];

#ifndef NDEBUG
        if(m.to == 0x77)
            ply = ply;
#endif

        it = m.pc;
        UC fr_pc = b[*it];
        UC to_pc = b[m.to];

        if(m == bm)
            list[i].scr = PV_FOLLOW;
        else
        if(to_pc == __ && !(m.flg & mPROM))
        {
            if(m == kil[ply][0])
                list[i].scr = FIRST_KILLER;
            else if(m == kil[ply][1])
                list[i].scr = SECOND_KILLER;
            else if(m == pv[ply][1])
                list[i].scr = MOVE_FROM_PV;
#ifdef DONT_USE_HISTORY
            else
            {
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
                pstVal      = pstVal/2 + 48;
                list[i].scr = pstVal;
            }
#else
            else
            {
                list[i].scr = 0;
                UC fr = *it;
                unsigned h = history[wtm][b[fr]/2 - 1][m.to];
                if(h > maxHistory)
                    maxHistory = h;
            }
#endif // DONT_USE_HISTORY
        }
        else
        {
            int ans;
            int src = streng[fr_pc/2];
            int dst = (m.flg & mCAPT) ? streng[to_pc/2] : 0;

#ifndef DONT_USE_SEE_SORTING
            if(dst && dst - src < -2)
            {
                auto storeMen = coords[wtm].begin();
                storeMen = m.pc;
                UC storeBrd = b[*storeMen];
                coords[wtm].erase(storeMen);
                b[*storeMen] = __;
                short tmp = -SEE(m.to, src, dst, wtm);

                dst = tmp;
                src = 0;
                coords[wtm].restore(storeMen);
                b[*storeMen] = storeBrd;
            }
#endif // DONT_USE_SEE_SORTING

            if(src > 120)
            {
                list[i].scr = EQUAL_CAPTURE;
                continue;
            }
            else if(dst > 120)
            {
                list[i].scr = 0;
                continue;
            }

            ans = dst - src/16;
            short prms[] = {0, 1200, 400, 600, 400};
            if(m.flg & mPROM)
                ans += prms[m.flg & mPROM];

            if(ans > 0)
                list[i].scr = (0x80 + ans/10);
            else if(ans < 0)
            {
                if(fr_pc/2 != _k/2)
                    list[i].scr = (11 + ans/10);
            }
            else
                list[i].scr = EQUAL_CAPTURE;
        }
    }
}

//--------------------------------
void AppriceQuiesceMoves(Move *list, int moveCr)
{
    auto it = coords[wtm].begin();
    for(int i = 0; i < moveCr; i++)
    {
        Move m = list[i];

        it = m.pc;
        UC fr = b[*it];
        UC pt = b[m.to];

        int ans;
        int src = streng[fr/2];
        int dst = (m.flg & mCAPT) ? streng[pt/2] : 0;
/*        if(src > 120)
            continue;
        else */if(dst > 120)
        {
            list[i].scr = KING_CAPTURE;
            return;
        }
#ifndef DONT_USE_SEE_SORTING
        if(dst && dst - src < -2)
        {
            auto storeMen = coords[wtm].begin();
            storeMen = m.pc;
            UC storeBrd = b[*storeMen];
            coords[wtm].erase(storeMen);
            b[*storeMen] = __;
            short tmp = -SEE(m.to, src, dst, wtm);
            dst = tmp;
            src = 0;
            coords[wtm].restore(storeMen);
            b[*storeMen] = storeBrd;
        }
        ans = dst - src/16;
#else
        ans = dst - src;
#endif // DONT_USE_SEE_SORTING
        short prms[] = {0, 1200, 400, 600, 400};
        if(dst <= 1200 && (m.flg & mPROM))
            ans += prms[m.flg & mPROM];

        if(ans > 0)
            list[i].scr = (0x80 + ans/10);
        else if(ans < 0)
        {
            if(fr/2 != _k/2)
                list[i].scr = (11 + ans/10);
        }
        else
            list[i].scr = EQUAL_CAPTURE;
    }
}

//--------------------------------
void Next(Move *list, int cur, int top, Move *ans)
{
    int max = -32000, imx = cur;

    for(int i = cur; i < top; i++)
    {
        UC sc = list[i].scr;
        if(sc > max)
        {
            max = sc;
            imx = i;
        }
    }
    *ans = list[imx];
    if(imx != cur)
    {
        list[imx] = list[cur];
        list[cur] = *ans;
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
void AppriceHistory(Move *list, int moveCr)
{
    if(maxHistory == 0)
        maxHistory = 1;
    auto it = coords[wtm].begin();
    for(int i = 0; i < moveCr; ++i)
    {
        if(list[i].scr > 0)
            continue;
        it = list[i].pc;
        UC fr = *it;
        unsigned h = history[wtm][b[fr]/2 - 1][list[i].to];
        list[i].scr = 100*h/maxHistory + 15;
    }
}
