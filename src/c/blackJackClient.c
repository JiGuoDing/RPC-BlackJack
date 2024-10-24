#include "../xdr/blackJack.h"
#include <ctype.h>
#include <rpc/clnt.h>
#include <rpc/rpc.h>
#include <rpc/types.h>
#include <string.h>

char* _2Lower(char*);

int main(int argc, char* argv[])
{
    if (argc != 2) {
        printf("不接受 %d 个参数，请只输入服务器地址(xxx.xxx.xxx.xxx)!\n", argc - 1);
        exit(1);
    }
    char* host = argv[1];
    CLIENT* clnt = (CLIENT*)calloc(1, sizeof(CLIENT));

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
    GameStatus* statusPtr = (GameStatus*)calloc(1, sizeof(GameStatus));
    // 游戏初始化失败
    if (statusPtr == NULL) {
        clnt_perror(clnt, "game starting failed");
        free(clnt);
        exit(10086);
    }
    statusPtr = startgame_1(NULL, clnt);
    // 游戏状态变量
    GameStatus status = *statusPtr;

    printf("\n**********game starts**********\n");
    printf("\nDealer's first card is %s\n", status.dealer.cards.cards_val[0].face);
    printf("\nPlayer's first card is %s\n", status.player.cards.cards_val[0].face);
    printf("\nPlayer's second card is %s\n", status.player.cards.cards_val[1].face);

    // 定义一个发牌请求（包括游戏状态和玩家id）
    HitRequest hreq;
    hreq.gameStatus = &status;
    hreq.id = 1;
    char* choice = (char*)calloc(10, sizeof(char));
    printf("\nNow it's your turn!\n");
    while (TRUE) {
        // 玩家回合
        printf("\nPlease choose to hit one more card or stand(h/s): ");
        scanf("%s", choice);
        // printf(status->msg);
        // 选择再发一张牌
        if (strcmp(_2Lower(choice), "h") == 0) {
            statusPtr = hitonecard_1(&hreq, clnt);
            if (statusPtr == NULL) {
                printf("\nSERVER ERROR! failed to hit new card.\n");
                free(choice);
                break;
            }
            status = *statusPtr;
            printf("\nyour new card is: %s\n", status.player.cards.cards_val[status.player.cards.cards_len - 1].face);
            printf("\nNow your current points is: %d\n", status.currentPointsOfPlayer);
        } else if (strcmp(_2Lower(choice), "s") == 0) {
            free(choice);
            break;
        }
        // printf("\n%d\n", status.player.cards.cards_len);
        // 如果手牌数 >= 20，强制进入庄家回合
        if (status.player.cards.cards_len >= 20) {
            free(choice);
            printf("\nYour cards have reached the limit number!\n");
            break;
        }
    }

    printf("\nYour turn is over!\n");
    printf("\nNow it's dealer's turn!\n");
    printf("\ndealer's another card is: %s\n", status.dealer.cards.cards_val[status.dealer.cards.cards_len - 1].face);
    hreq.gameStatus = &status;
    hreq.id = 0;
    while (TRUE) {
        // 庄家回合
        // 手牌点数小于17，强制发牌
        if (status.currentPointsOfDealer < 17) {
            statusPtr = hitonecard_1(&hreq, clnt);
            if (statusPtr == NULL) {
                printf("\nSERVER ERROR! failed to hit new card.\n");
                break;
            }
            status = *statusPtr;
            printf("\ndealer hit a new card: %s\n", status.dealer.cards.cards_val[status.dealer.cards.cards_len - 1].face);
        } else {
            // TODO 庄家的逻辑
            // break;
        }
    }

    // 释放分配的内存
    free(clnt);
    free(statusPtr);
    printf("\n**********Game Over**********\n");

    return 0;
}

char* _2Lower(char* str)
{
    for (int i = 0; str[i] != '\0'; i++)
        str[i] = tolower(str[i]);
    return str;
}