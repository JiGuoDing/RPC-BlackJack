/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#ifndef _BLACKJACK_H_RPCGEN
#define _BLACKJACK_H_RPCGEN

#include <rpc/rpc.h>


#ifdef __cplusplus
extern "C" {
#endif


struct Card {
	char *face;
	u_short point;
};
typedef struct Card Card;

struct Player {
	u_short id;
	struct {
		u_int cards_len;
		Card *cards_val;
	} cards;
};
typedef struct Player Player;

struct GameStatus {
	Player player;
	Player dealer;
	u_short currentPointsOfPlayer;
	u_short currentPointsOfDealer;
	char *msg;
};
typedef struct GameStatus GameStatus;

struct GameResult {
	u_short finalPointsOfPlayer;
	u_short finalPointsOfDealer;
	char *result;
};
typedef struct GameResult GameResult;

#define BLACKJACKPROGRAM 0x00001234
#define BLACKJACKVERSION 1

#if defined(__STDC__) || defined(__cplusplus)
#define StartGame 1
extern  GameStatus * startgame_1(void *, CLIENT *);
extern  GameStatus * startgame_1_svc(void *, struct svc_req *);
#define HitOneCard 2
extern  GameStatus * hitonecard_1(Player *, CLIENT *);
extern  GameStatus * hitonecard_1_svc(Player *, struct svc_req *);
#define GameOver 3
extern  GameResult * gameover_1(GameStatus *, CLIENT *);
extern  GameResult * gameover_1_svc(GameStatus *, struct svc_req *);
extern int blackjackprogram_1_freeresult (SVCXPRT *, xdrproc_t, caddr_t);

#else /* K&R C */
#define StartGame 1
extern  GameStatus * startgame_1();
extern  GameStatus * startgame_1_svc();
#define HitOneCard 2
extern  GameStatus * hitonecard_1();
extern  GameStatus * hitonecard_1_svc();
#define GameOver 3
extern  GameResult * gameover_1();
extern  GameResult * gameover_1_svc();
extern int blackjackprogram_1_freeresult ();
#endif /* K&R C */

/* the xdr functions */

#if defined(__STDC__) || defined(__cplusplus)
extern  bool_t xdr_Card (XDR *, Card*);
extern  bool_t xdr_Player (XDR *, Player*);
extern  bool_t xdr_GameStatus (XDR *, GameStatus*);
extern  bool_t xdr_GameResult (XDR *, GameResult*);

#else /* K&R C */
extern bool_t xdr_Card ();
extern bool_t xdr_Player ();
extern bool_t xdr_GameStatus ();
extern bool_t xdr_GameResult ();

#endif /* K&R C */

#ifdef __cplusplus
}
#endif

#endif /* !_BLACKJACK_H_RPCGEN */
