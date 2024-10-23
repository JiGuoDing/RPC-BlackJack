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

void Init2Cards(GameStatus*);
u_short CalPoints(Card*, u_int);
Card* Gen1Card();
void OneMoreCard(GameStatus*, int);

// 13张牌型
const Card CARDS[] = {
    { "2", 2 }, { "3", 3 }, { "4", 4 },
    { "5", 5 }, { "6", 6 }, { "7", 7 },
    { "8", 8 }, { "9", 9 }, { "10", 10 },
    { "J", 10 }, { "Q", 10 }, { "K", 10 },
    { "A", 1 }
};

static GameStatus gameStatus;

static void
blackjackprogram_1(struct svc_req* rqstp, register SVCXPRT* transp)
{
    union {
        HitRequest hitonecard_1_arg;
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
        _xdr_argument = (xdrproc_t)xdr_HitRequest;
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
    printf("udp_svc registed");

    transp = svctcp_create(RPC_ANYSOCK, 1, 1);
    if (transp == NULL) {
        fprintf(stderr, "%s", "cannot create tcp service.");
        exit(1);
    }
    if (!svc_register(transp, BLACKJACKPROGRAM, BLACKJACKVERSION, blackjackprogram_1, IPPROTO_TCP)) {
        fprintf(stderr, "%s", "unable to register (BLACKJACKPROGRAM, BLACKJACKVERSION, tcp).");
        exit(1);
    }
    printf("tcp_svc registed");

    printf("server booting...");

    svc_run();
    fprintf(stderr, "%s", "svc_run returned");
    exit(1);
    /* NOTREACHED */
}

// 游戏初始化
GameStatus* startgame_1_svc(void* any, struct svc_req* rep)
{
    // 初始化内存
    // GameStatus gameStatus = (GameStatus*)calloc(1, sizeof(GameStatus));

    // if (gameStatus == NULL) {
    //     // 内存分配失败
    //     printf("Memory for gameStatus allocation failed\n");
    //     return NULL;
    // }
    gameStatus.msg = (char*)calloc(750, sizeof(char));
    strcat(gameStatus.msg, "\n**********Game starts**********\n");

    // 绑定玩家id
    gameStatus.dealer.id = 0;
    gameStatus.player.id = 1;

    // 初始化随机数种子
    srand(time(NULL));

    // 为玩家和庄家分别发两张牌
    Init2Cards(&gameStatus);
    gameStatus.player.cards.cards_len = 2;
    strcat(gameStatus.msg, "\nDealer gets initial 2 cards\n");
    gameStatus.dealer.cards.cards_len = 2;
    strcat(gameStatus.msg, "\nPlayer gets initial 2 cards\n");

    // char numStr[10];
    // snprintf(numStr, sizeof(numStr), "%d", gameStatus->player.cards.cards_len);
    // strcat(gameStatus->msg, "\n");
    // strcat(gameStatus->msg, numStr);
    // strcat(gameStatus->msg, "\n");

    gameStatus.currentPointsOfPlayer = CalPoints(gameStatus.player.cards.cards_val, gameStatus.player.cards.cards_len);
    gameStatus.currentPointsOfDealer = CalPoints(gameStatus.dealer.cards.cards_val, gameStatus.dealer.cards.cards_len);

    return &gameStatus;
}

// 发一张牌
GameStatus* hitonecard_1_svc(HitRequest* hreq, struct svc_req* req)
{
    OneMoreCard(&gameStatus, hreq->id);
    switch (hreq->id) {
    case 0: {
        strcat(gameStatus.msg, "\n Dealer hit one more card.\n");
        break;
    }
    case 1: {
        strcat(gameStatus.msg, "\n Player hit one more card.\n");
        break;
    }
    default:
        break;
    }
    return &gameStatus;
}

// 结算游戏
GameResult* gameover_1_svc(GameStatus* status, struct svc_req* req)
{
    GameResult* result = (GameResult*)calloc(1, sizeof(GameResult));
    result->finalPointsOfDealer = status->currentPointsOfDealer;
    result->finalPointsOfPlayer = status->currentPointsOfPlayer;
    // 打印游戏状态记录的日志
    printf("\nGame logs:\n%s", status->msg);
    // 释放日志内存
    free(status->msg);
    // 释放手牌内存
    free(status->dealer.cards.cards_val);
    free(status->player.cards.cards_val);
    // 释放游戏状态内存
    // free(status);

    if (result->finalPointsOfDealer > 21) {
        result->result = "Player wins";
        return result;
    }
    if (result->finalPointsOfPlayer > 21) {
        result->result = "Dealer wins";
        return result;
    }
    if (result->finalPointsOfDealer > result->finalPointsOfPlayer)
        result->result = "Dealer wins";
    else if (result->finalPointsOfDealer < result->finalPointsOfPlayer)
        result->result = "Player wins";
    else
        result->result = "Tie game";
    return result;
}

// 初始发两张牌
void Init2Cards(GameStatus* gameStatus)
{
    // 分配20张牌的内存，最多20张牌
    gameStatus->player.cards.cards_val = (Card*)calloc(20, sizeof(Card));
    gameStatus->dealer.cards.cards_val = (Card*)calloc(20, sizeof(Card));
    if (gameStatus->player.cards.cards_val == NULL || gameStatus->dealer.cards.cards_val == NULL) {
        // 内存分配失败
        printf("Memory for initial cards allocation failed\n");
        strcat(gameStatus->msg, "\nMemory for initial cards allocation failed\n");
    }
    // 随机生成两张牌
    gameStatus->dealer.cards.cards_val[0] = CARDS[rand() % 13];
    gameStatus->dealer.cards.cards_val[1] = CARDS[rand() % 13];
    gameStatus->player.cards.cards_val[0] = CARDS[rand() % 13];
    gameStatus->player.cards.cards_val[1] = CARDS[rand() % 13];
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

// 再发一张牌
void OneMoreCard(GameStatus* status, int id)
{
    switch (id) {
    case 1: {
        // 手牌添加一张
        status->player.cards.cards_val[status->player.cards.cards_len++] = CARDS[rand() % 13];
        // 更新点数
        status->currentPointsOfPlayer += status->player.cards.cards_val[status->player.cards.cards_len - 1].point;
        break;
    }

    case 0: {
        // 手牌添加一张
        status->dealer.cards.cards_val[status->player.cards.cards_len++] = CARDS[rand() % 13];
        // 更新点数
        status->currentPointsOfDealer += status->dealer.cards.cards_val[status->dealer.cards.cards_len - 1].point;
        break;
    }

    default:
        break;
    }
}
