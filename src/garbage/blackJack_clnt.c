/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include <memory.h> /* for memset */
#include "blackJack.h"

/* Default timeout can be changed using clnt_control() */
static struct timeval TIMEOUT = { 25, 0 };

GameStatus *
startgame_1(void *argp, CLIENT *clnt)
{
	static GameStatus clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, StartGame,
		(xdrproc_t) xdr_void, (caddr_t) argp,
		(xdrproc_t) xdr_GameStatus, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}

GameStatus *
hitonecard_1(Player *argp, CLIENT *clnt)
{
	static GameStatus clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, HitOneCard,
		(xdrproc_t) xdr_Player, (caddr_t) argp,
		(xdrproc_t) xdr_GameStatus, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}

GameResult *
gameover_1(GameStatus *argp, CLIENT *clnt)
{
	static GameResult clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, GameOver,
		(xdrproc_t) xdr_GameStatus, (caddr_t) argp,
		(xdrproc_t) xdr_GameResult, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}