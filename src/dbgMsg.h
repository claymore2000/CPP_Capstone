#ifndef _DBGMSG_H_
#define _DBGMSG_H_


std::ostream DbgMsg(NULL);
typedef enum DblLvl{
  DBGMSG_NONE = 0,
  DBGMSG_VERBOSE,
  DBGMSG_DEFAULT
} DbgLvl;

DbgLvl DbgSetTo = DBGMSG_VERBOSE;

std::ostream &DbgMsgDefault(std::ostream &stream) {
  return (DbgSetTo==DBGMSG_DEFAULT) ? std::cout : stream;
}

std::ostream &DbgMsgVerbose(std::ostream &stream) {
  return (DbgSetTo>=DBGMSG_VERBOSE) ? std::cout : stream;
}

std::ostream &DbgMsgNone(std::ostream &stream) {
  return (DbgSetTo>=DBGMSG_NONE) ? stream : stream;
}

#endif
