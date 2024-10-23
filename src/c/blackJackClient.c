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

    char* ready = (char*)malloc(10 * sizeof(char));
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

    HitRequest* hreq = NULL;
    while (TRUE) {
        // 玩家回合
        // 定义一个发牌请求（包括游戏状态和玩家id）
        if (hreq == NULL)
            hreq = (HitRequest*)calloc(1, sizeof(HitRequest));
        hreq->gameStatus = status;
        char* choice = (char*)calloc(10, sizeof(char));
        printf("\nPlease choose to hit one more card or stand(h/s): ");
        scanf("%s", choice);
        // 选择再发一张牌
        if (strcmp(_2Lower(choice), "h") == 0) {
            hreq->id = 1;
            status = hitonecard_1(hreq, clnt);
            if (status == NULL) {
                printf("\nSERVER ERROR! failed to hit new card.\n");
                free(hreq);
                break;
            }
            // free(hreq);
            printf("\nyour new card is: %s\n", status->player.cards.cards_val[status->player.cards.cards_len - 1].face);
        } else if (strcmp(_2Lower(choice), "s") == 0)
            break;
        else
            continue;
    }

    // 庄家回合

    // 释放客户端内存
    free(clnt);

    return 0;
}

char* _2Lower(char* str)
{
    for (int i = 0; str[i] != '\0'; i++)
        str[i] = tolower(str[i]);
    return str;
}