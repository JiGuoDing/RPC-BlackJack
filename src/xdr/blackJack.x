/*
牌结构体
*/
struct Card{
    /*
    face表示牌的字面量
    point表示牌的点数
    */
    string face<>;
    unsigned short point;
};

/*
玩家/庄家手中牌
struct cardsInHand{
    Card cards<>;
};
*/

/*
玩家结构体
*/
struct Player{
    /*玩家id*/
    unsigned short id;
    /*玩家手牌*/
    Card cards<>;
};


/*游戏状态*/
struct GameStatus{
    /*玩家和庄家*/
    Player player;
    Player dealer;
    /*玩家和庄家的手牌点数（庄家的明牌的点数）*/
    unsigned short currentPointsOfPlayer;
    unsigned short currentPointsOfDealer;
    /*提示信息*/
    string msg<>;
};


/*请求发牌结构体*/
struct HitRequest{
    /*游戏状态*/
    GameStatus *gameStatus;
    /*玩家id*/
    unsigned short id;
};

/*游戏结果*/
struct GameResult{
    /*最后的总点数*/
    unsigned short finalPointsOfPlayer;
    unsigned short finalPointsOfDealer;
    /*游戏结果*/
    string result<>;
};

program BLACKJACKPROGRAM{
    version BLACKJACKVERSION{
        /*开始游戏，每人发两张牌*/
        GameStatus StartGame() = 1;
        /*请求发一张牌*/
        GameStatus HitOneCard(HitRequest) = 2;
        /*游戏结束*/
        GameResult GameOver(GameStatus) = 3;
    } = 1;
} = 0x00001234;


