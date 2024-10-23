#include "../xdr/blackJack.h"
#include <ctype.h>
#include <rpc/clnt.h>
#include <rpc/rpc.h>
#include <rpc/types.h>
#include <string.h>

char* _2Lower(char*);

int main()
{
    CLIENT* clnt = (CLIENT*)calloc(1, sizeof(CLIENT));
    char* host = "localhost";

    // 创建客户端句柄(Server地址， 程序号， 版本号， 传输协议)
    clnt = clnt_create(host, BLACKJACKPROGRAM, BLACKJACKVERSION, "tcp");
    if (clnt == NULL) {
        clnt_pcreateerror("localhost");
        exit(1);
    }

    char* ready = (char*)calloc(10, sizeof(char));
    printf("Ready?(y/n):");
    scanf("%s", ready);
    printf("\n");

    if (strcmp(ready, "y") != 0 && strcmp(ready, "Y") != 0)
        return 0;
    free(ready);

    // rpc startGame 开始游戏
    GameStatus* status = startgame_1(NULL, clnt);
    // 游戏初始化失败
    if (status == NULL)
        clnt_perror(clnt, "game starting failed");

    printf("\n**********game starts**********\n");
    printf("\nDealer's first card is %s\n", status->dealer.cards.cards_val[0].face);
    printf("\nPlayer's first card is %s\n", status->player.cards.cards_val[0].face);
    printf("\nPlayer's second card is %s\n", status->player.cards.cards_val[1].face);

    HitRequest hreq;
    hreq.gameStatus = status;
    hreq.id = 1;
    while (TRUE) {
        // 玩家回合
        // 定义一个发牌请求（包括游戏状态和玩家id）
        char* choice = (char*)calloc(10, sizeof(char));
        printf("\nPlease choose to hit one more card or stand(h/s): ");
        scanf("%s", choice);
        // printf(status->msg);
        // 选择再发一张牌
        if (strcmp(_2Lower(choice), "h") == 0) {
            status = hitonecard_1(&hreq, clnt);
            if (status == NULL) {
                printf("\nSERVER ERROR! failed to hit new card.\n");
                free(choice);
                break;
            }
            printf("\nyour new card is: %s\n", status->player.cards.cards_val[status->player.cards.cards_len - 1].face);
        } else if (strcmp(_2Lower(choice), "s") == 0) {
            free(choice);
            break;
        } else
            free(choice);
        // 如果手牌数 >= 20，强制进入庄家回合
        if (status->dealer.cards.cards_len >= 20)
            break;
    }

    hreq.gameStatus = status;
    hreq.id = 0;
    while (TRUE) {
        // 庄家回合
        // 手牌点数小于17，强制发牌
        if (status->currentPointsOfDealer < 17)
            status = hitonecard_1(&hreq, clnt);
        if (status == NULL) {
            printf("\nSERVER ERROR! failed to hit new card.\n");
            break;
        }
    }

    // 释放客户端内存
    free(clnt);
    printf("\n**********Game Over**********\n");

    return 0;
}

char* _2Lower(char* str)
{
    for (int i = 0; str[i] != '\0'; i++)
        str[i] = tolower(str[i]);
    return str;
}