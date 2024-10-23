#include "../xdr/blackJack.h"
#include <time.h>

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

// 游戏初始化
GameStatus* startgame_1_svc(void* any, struct svc_req* rep)
{
    // 初始化内存
    GameStatus* gameStatus = (GameStatus*)calloc(1, sizeof(GameStatus));
    if (gameStatus == NULL) {
        // 内存分配失败
        printf("Memory for gameStatus allocation failed\n");
        return NULL;
    }
    gameStatus->msg = (char*)calloc(750, sizeof(char));
    strcat(gameStatus->msg, "\n**********Game starts**********\n");

    // 绑定玩家id
    gameStatus->dealer.id = 0;
    gameStatus->player.id = 1;

    // 初始化随机数种子
    srand(time(NULL));

    // 为玩家和庄家分别发两张牌
    Init2Cards(gameStatus);
    gameStatus->player.cards.cards_len = 2;
    strcat(gameStatus->msg, "\nDealer gets initial 2 cards\n");
    gameStatus->dealer.cards.cards_len = 2;
    strcat(gameStatus->msg, "\nPlayer gets initial 2 cards\n");

    // char numStr[10];
    // snprintf(numStr, sizeof(numStr), "%d", gameStatus->player.cards.cards_len);
    // strcat(gameStatus->msg, "\n");
    // strcat(gameStatus->msg, numStr);
    // strcat(gameStatus->msg, "\n");

    gameStatus->currentPointsOfPlayer = CalPoints(gameStatus->player.cards.cards_val, gameStatus->player.cards.cards_len);
    gameStatus->currentPointsOfDealer = CalPoints(gameStatus->dealer.cards.cards_val, gameStatus->dealer.cards.cards_len);

    return gameStatus;
}

// 发一张牌
GameStatus* hitonecard_1_svc(HitRequest* hreq, struct svc_req* req)
{
    OneMoreCard(hreq->gameStatus, hreq->id);
    switch (hreq->id) {
    case 0: {
        strcat(hreq->gameStatus->msg, "\n Dealer hit one more card.\n");
        break;
    }
    case 1: {
        // strcat(hreq->gameStatus->msg, "\n Player hit one more card.\n");
        break;
    }
    default:
        break;
    }
    return hreq->gameStatus;
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
    free(status);

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
        status->player.cards.cards_val = realloc(status->player.cards.cards_val, 20 * sizeof(Card));
        status->player.cards.cards_val[status->player.cards.cards_len] = CARDS[rand() % 13];
        status->player.cards.cards_len++;
        break;
    }

    case 0: {

        break;
    }

    default:
        break;
    }
}
