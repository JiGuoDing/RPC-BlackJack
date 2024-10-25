#include "../xdr/blackJack.h"
#include <ctype.h>
#include <rpc/clnt.h>
#include <rpc/rpc.h>
#include <rpc/types.h>
#include <stdarg.h>
#include <string.h>

// 定义颜色代码
const char* red = "\033[31m";
const char* green = "\033[32m";
const char* yellow = "\033[33m";
const char* blue = "\033[34m";
const char* reset = "\033[0m";

const char* COLORSET[] = { "\033[31m", "\033[32m", "\033[33m", "\033[34m", "\033[0m" };

// 字母转小写
char* _2Lower(char*);
// 展示手牌
void showCards(Player);
// 输出黄色文本
void printfY(const char*, ...);
// 输出黄色文本
void printfR(const char*, ...);
// 输出黄色文本
void printfG(const char*, ...);
// 输出黄色文本
void printfB(const char*, ...);

int main(int argc, char* argv[])
{
    if (argc != 2) {
        printfR("不接受 %d 个参数，请只输入服务器地址(xxx.xxx.xxx.xxx)!\n", argc - 1);
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
    printfY("Ready?(y/n):");
    scanf("%s", ready);
    printf("\n");

    if (strcmp(ready, "y") != 0 && strcmp(ready, "Y") != 0)
        return 0;
    free(ready);

    // rpc startGame 开始游戏
    // GameStatus* statusPtr = (GameStatus*)calloc(1, sizeof(GameStatus));
    GameStatus* statusPtr = NULL;
    // 记录游戏是否结束
    bool_t gameIsOver = FALSE;
    // 游戏初始化失败
    statusPtr = startgame_1(NULL, clnt);
    if (statusPtr == NULL) {
        clnt_perror(clnt, "game starting failed");
        free(clnt);
        exit(10086);
    }
    // 游戏状态变量
    GameStatus status = *statusPtr;

    printfY("\n**********game starts**********\n");
    printfB("\nDealer's first card is %s\n", status.dealer.cards.cards_val[0].face);
    printfG("\nPlayer's first card is %s\n", status.player.cards.cards_val[0].face);
    printfG("\nPlayer's second card is %s%s\n", status.player.cards.cards_val[1].face);

    // 定义一个发牌请求（包括游戏状态和玩家id）
    HitRequest hreq;
    hreq.gameStatus = &status;
    hreq.id = 1;
    char* choice = (char*)calloc(10, sizeof(char));
    printfG("\nNow it's your turn!\n");
    while (!gameIsOver) {

        // 展示玩家手牌
        showCards(status.player);
        printfG("\nNow your current points is: %d\n", status.currentPointsOfPlayer);

        // 玩家回合
        printfY("\nPlease choose to hit one more card or stand(h/s): ");
        scanf("%s", choice);

        // 选择再发一张牌
        if (strcmp(_2Lower(choice), "h") == 0) {
            statusPtr = hitonecard_1(&hreq, clnt);
            if (statusPtr == NULL) {
                printfR("\nSERVER ERROR! failed to hit new card.\n");
                // 服务器崩溃，游戏结束
                gameIsOver = TRUE;
                free(choice);
                break;
            }
            status = *statusPtr;
            printfG("\nyour new card is: %s\n", status.player.cards.cards_val[status.player.cards.cards_len - 1].face);
        } else if (strcmp(_2Lower(choice), "s") == 0) {
            free(choice);
            break;
        }

        // 展示玩家的手牌
        showCards(status.player);
        printfG("\nNow your current points is: %d\n", status.currentPointsOfPlayer);

        // 如果手牌数 >= 20，强制进入庄家回合
        if (status.player.cards.cards_len >= 20) {
            free(choice);
            printfY("\nYour cards have reached the limit number!\n");
            break;
        }

        // 玩家手牌点数大于21，玩家输
        if (status.currentPointsOfPlayer > 21) {
            gameIsOver = TRUE;
            free(choice);
            printfY("\nYou bust with points of: %d\n", status.currentPointsOfPlayer);
            printfY("\nYou have *LOST* the game\n");
            break;
        }
    }

    if (!gameIsOver) {
        printfY("\nYour turn is over!\n");
        printfY("\nNow it's dealer's turn!\n");
        printfB("\ndealer's another card is: %s\n", status.dealer.cards.cards_val[status.dealer.cards.cards_len - 1].face);
        hreq.gameStatus = &status;
        hreq.id = 0;
    }

    while (!gameIsOver) {
        // 庄家回合
        // 手牌点数小于17，强制发牌
        if (status.currentPointsOfDealer < 17) {
            statusPtr = hitonecard_1(&hreq, clnt);
            if (statusPtr == NULL) {
                printfR("\nSERVER ERROR! failed to hit new card.\n");
                // 服务器崩溃，游戏结束
                break;
            }
            status = *statusPtr;
            printfB("\ndealer hit a new card: %s\n", status.dealer.cards.cards_val[status.dealer.cards.cards_len - 1].face);
        } else if (status.currentPointsOfDealer >= 17 && status.currentPointsOfDealer <= 21) {
            // TODO 庄家的逻辑
            break;
        } else {
        }

        // 展示庄家的手牌
        printfB("\nNow dealer's cards are: ");
        for (int i = 0; i < status.dealer.cards.cards_len; i++)
            printfB("%s ", status.dealer.cards.cards_val[i].face);
        printf("\n");

        // 玩家手牌点数大于21，玩家输
        if (status.currentPointsOfDealer > 21) {
            gameIsOver = TRUE;
            printfY("\nDealer busts with points of: %d\n", status.currentPointsOfDealer);
            printfY("\nYou have *WON* the game with points of %d\n", status.currentPointsOfPlayer);
            break;
        }
    }

    // 释放分配的内存
    free(clnt);
    // free(statusPtr);
    printfY("\n**********Game Over**********\n");

    return 0;
}

char* _2Lower(char* str)
{
    for (int i = 0; str[i] != '\0'; i++)
        str[i] = tolower(str[i]);
    return str;
}

void showCards(Player player)
{
    printf("\nNow your cards are: ");
    for (int i = 0; i < player.cards.cards_len; i++)
        printf("%s ", player.cards.cards_val[i].face);
    printf("\n");
}

// 输出黄色文本
void printfY(const char* text, ...)
{
    // 初始化可变参数列表
    va_list args;
    va_start(args, text);

    // 输出黄色文本
    printf("%s", yellow);
    vprintf(text, args);
    printf("%s", reset);

    // 结束可变参数列表
    va_end(args);
}
// 输出红色文本
void printfR(const char* text, ...)
{
    // 初始化可变参数列表
    va_list args;
    va_start(args, text);

    // 输出黄色文本
    printf("%s", red);
    vprintf(text, args);
    printf("%s", reset);

    // 结束可变参数列表
    va_end(args);
}
// 输出绿色文本
void printfG(const char* text, ...)
{
    // 初始化可变参数列表
    va_list args;
    va_start(args, text);

    // 输出黄色文本
    printf("%s", green);
    vprintf(text, args);
    printf("%s", reset);

    // 结束可变参数列表
    va_end(args);
}
// 输出蓝色文本
void printfB(const char* text, ...)
{
    // 初始化可变参数列表
    va_list args;
    va_start(args, text);

    // 输出黄色文本
    printf("%s", blue);
    vprintf(text, args);
    printf("%s", reset);

    // 结束可变参数列表
    va_end(args);
}