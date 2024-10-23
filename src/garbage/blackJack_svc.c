/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "blackJack.h"
#include <memory.h>
#include <netinet/in.h>
#include <rpc/pmap_clnt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>

#ifndef SIG_PF
#define SIG_PF void (*)(int)
#endif

Card* Init2Cards();
u_short CalPoints(Card*, u_int);

// 13张牌型
const Card CARDS[] = {
    { "2", 2 }, { "3", 3 }, { "4", 4 },
    { "5", 5 }, { "6", 6 }, { "7", 7 },
    { "8", 8 }, { "9", 9 }, { "10", 10 },
    { "J", 10 }, { "Q", 10 }, { "K", 10 },
    { "A", 1 }
};

static void
blackjackprogram_1(struct svc_req* rqstp, register SVCXPRT* transp)
{
    union {
        Player hitonecard_1_arg;
        GameStatus gameover_1_arg;
    } argument;
    char* result;
    xdrproc_t _xdr_argument, _xdr_result;
    char* (*local)(char*, struct svc_req*);

    switch (rqstp->rq_proc) {
    case NULLPROC:
        (void)svc_sendreply(transp, (xdrproc_t)xdr_void, (char*)NULL);
        return;

    case StartGame:
        _xdr_argument = (xdrproc_t)xdr_void;
        _xdr_result = (xdrproc_t)xdr_GameStatus;
        local = (char* (*)(char*, struct svc_req*))startgame_1_svc;
        break;

    case HitOneCard:
        _xdr_argument = (xdrproc_t)xdr_Player;
        _xdr_result = (xdrproc_t)xdr_GameStatus;
        local = (char* (*)(char*, struct svc_req*))hitonecard_1_svc;
        break;

    case GameOver:
        _xdr_argument = (xdrproc_t)xdr_GameStatus;
        _xdr_result = (xdrproc_t)xdr_GameResult;
        local = (char* (*)(char*, struct svc_req*))gameover_1_svc;
        break;

    default:
        svcerr_noproc(transp);
        return;
    }
    memset((char*)&argument, 0, sizeof(argument));
    if (!svc_getargs(transp, (xdrproc_t)_xdr_argument, (caddr_t)&argument)) {
        svcerr_decode(transp);
        return;
    }
    result = (*local)((char*)&argument, rqstp);
    if (result != NULL && !svc_sendreply(transp, (xdrproc_t)_xdr_result, result)) {
        svcerr_systemerr(transp);
    }
    if (!svc_freeargs(transp, (xdrproc_t)_xdr_argument, (caddr_t)&argument)) {
        fprintf(stderr, "%s", "unable to free arguments");
        exit(1);
    }
    return;
}

int main(int argc, char** argv)
{
    register SVCXPRT* transp;

    pmap_unset(BLACKJACKPROGRAM, BLACKJACKVERSION);

    transp = svcudp_create(RPC_ANYSOCK);
    if (transp == NULL) {
        fprintf(stderr, "%s", "cannot create udp service.");
        exit(1);
    }
    if (!svc_register(transp, BLACKJACKPROGRAM, BLACKJACKVERSION, blackjackprogram_1, IPPROTO_UDP)) {
        fprintf(stderr, "%s", "unable to register (BLACKJACKPROGRAM, BLACKJACKVERSION, udp).");
        exit(1);
    }

    transp = svctcp_create(RPC_ANYSOCK, 0, 0);
    if (transp == NULL) {
        fprintf(stderr, "%s", "cannot create tcp service.");
        exit(1);
    }
    if (!svc_register(transp, BLACKJACKPROGRAM, BLACKJACKVERSION, blackjackprogram_1, IPPROTO_TCP)) {
        fprintf(stderr, "%s", "unable to register (BLACKJACKPROGRAM, BLACKJACKVERSION, tcp).");
        exit(1);
    }

    svc_run();
    fprintf(stderr, "%s", "svc_run returned");
    exit(1);
    /* NOTREACHED */
}

// 游戏初始化
GameStatus* startgame_1_svc(void* any, struct svc_req* rep)
{
    GameStatus* gameStatus = (GameStatus*)calloc(1, sizeof(GameStatus));
    if (gameStatus == NULL) {
        // 内存分配失败
        printf("Memory for gameStatus allocation failed\n");
        return NULL;
    }

    // 为玩家和庄家分别发两张牌
    gameStatus->player.cards.cards_val = Init2Cards();
    gameStatus->player.cards.cards_len = 2;
    gameStatus->dealer.cards.cards_val = Init2Cards();
    gameStatus->dealer.cards.cards_len = 2;

    gameStatus->currentPointsOfPlayer = CalPoints(gameStatus->player.cards.cards_val, gameStatus->player.cards.cards_len);
    gameStatus->currentPointsOfDealer = CalPoints(gameStatus->dealer.cards.cards_val, gameStatus->dealer.cards.cards_len);

    return gameStatus;
}

// 发一张牌
GameStatus* hitonecard_1_svc(Player* player, struct svc_req* req)
{
}

// 初始发两张牌
Card* Init2Cards()
{
    // 分配两张牌的内存
    Card* twoCards = (Card*)calloc(2, sizeof(Card));
    if (twoCards == NULL) {
        // 内存分配失败
        printf("Memory for initial cards allocation failed\n");
        return NULL;
    }
    // 初始化随机数种子
    srand(time(NULL));
    // 随机生成两张牌
    twoCards[0] = CARDS[rand() % 13];
    twoCards[1] = CARDS[rand() % 13];

    return twoCards;
}

Card Gen1Card()
{
    srand(time(NULL));
    Card card = CARDS[rand() % 13];
    return card;
}

// 计算手牌点数
u_short CalPoints(Card* cards, u_int cards_len)
{
    u_short points = 0;
    for (int i = 0; i < cards_len; i++) {
        points += cards[i].point;
    }
    return points;
}