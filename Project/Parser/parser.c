/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdlib.h>

#include "reader.h"
#include "scanner.h"
#include "parser.h"
#include "error.h"

Token *currentToken;
Token *lookAhead;

void scan(void)
{
  Token *tmp = currentToken;
  currentToken = lookAhead;
  lookAhead = getValidToken();
  free(tmp);
}

void eat(TokenType tokenType)
{
  if (lookAhead->tokenType == tokenType)
  {
    printToken(lookAhead);
    scan();
  }
  else
    missingToken(tokenType, lookAhead->lineNo, lookAhead->colNo);
}

void compileProgram(void)
{
  assert("Parsing a Program ....");
  eat(KW_PROGRAM);
  eat(TK_IDENT);
  eat(SB_SEMICOLON);
  compileBlock();
  eat(SB_PERIOD);
  assert("Program parsed!");
}

void compileBlock(void)
{
  assert("Parsing a Block ....");
  if (lookAhead->tokenType == KW_CONST)
  {
    eat(KW_CONST);
    compileConstDecl();
    compileConstDecls();
    compileBlock2();
  }
  else
    compileBlock2();
  assert("Block parsed!");
}

void compileBlock2(void)
{
  if (lookAhead->tokenType == KW_TYPE)
  {
    eat(KW_TYPE);
    compileTypeDecl();
    compileTypeDecls();
    compileBlock3();
  }
  else
    compileBlock3();
}

void compileBlock3(void)
{
  if (lookAhead->tokenType == KW_VAR)
  {
    eat(KW_VAR);
    compileVarDecl();
    compileVarDecls();
    compileBlock4();
  }
  else
    compileBlock4();
}

void compileBlock4(void)
{
  compileSubDecls();
  compileBlock5();
}

void compileBlock5(void)
{
  eat(KW_BEGIN);
  compileStatements();
  eat(KW_END);
}
void compileConstDecls(void)
{
  assert("Parsing subconstants...");
  while (lookAhead->tokenType == TK_IDENT)
    compileConstDecl();
  assert("Subconstants parsed!");
}

void compileConstDecl(void)
{
  assert("Parsing constant...");
  eat(TK_IDENT);
  eat(SB_EQ);
  compileConstant();
  eat(SB_SEMICOLON);
  assert("Constant parsed!");
}

void compileTypeDecls(void)
{
  assert("Parsing subtypes...");
  while (lookAhead->tokenType == TK_IDENT)
    compileTypeDecl();
  assert("Subtypes parsed!");
}

void compileTypeDecl(void)
{
  assert("Parsing a type...");
  eat(TK_IDENT);
  eat(SB_EQ);
  compileType();
  eat(SB_SEMICOLON);
  assert("Type parsed!");
}

//X??t tr?????ng h???p khai b??o nhi???u bi???n
// v?? d??? var a:integer; b:long;
void compileVarDecls(void)
{
  //  assert("Parsing variables...");
  while (lookAhead->tokenType == TK_IDENT)
    compileVarDecl();
  //  assert("Variables parsed");
}

void compileVarDecl(void)
{
  //  assert("Parsing a variable...");
  eat(TK_IDENT);
  eat(SB_COLON);
  compileType();
  eat(SB_SEMICOLON);
  //  assert("Variable parsed!");
}

void compileSubDecls(void)
{
  assert("Parsing subtoutines ....");
  if (lookAhead->tokenType == KW_FUNCTION)
  {
    compileFuncDecl();
    compileSubDecls();
  }
  else if (lookAhead->tokenType == KW_PROCEDURE)
  {
    compileProcDecl();
    compileSubDecls();
  }
  assert("Subtoutines parsed ....");
}

void compileFuncDecl(void)
{
  assert("Parsing a function ....");
  eat(KW_FUNCTION);
  eat(TK_IDENT);
  compileParams(); // ?????c c??c bi???n trong function EX: Function tinhtong(a :integer)
  eat(SB_COLON);
  compileBasicType(); // Lo???i return c???a function n??y v?? d??? tr??? v??? 1 Integer
  eat(SB_SEMICOLON);
  compileBlock(); // Compile ph???n b??n trong function
  eat(SB_SEMICOLON);
  assert("Function parsed ....");
}

void compileProcDecl(void)
{
  // V??? c?? b???n procedure gi???ng function tuy nhi??n procedure gi???ng nh?? 1 h??m void v???y
  assert("Parsing a procedure ....");
  eat(KW_PROCEDURE);
  eat(TK_IDENT);
  compileParams();
  eat(SB_SEMICOLON);
  compileBlock();
  eat(SB_SEMICOLON);
  assert("Procedure parsed ....");
}

void compileUnsignedConstant(void)
{
  // H??m n??y s??? ?????c c??c bi???n nh?? s???, bi???n v?? char
  assert("Parsing a unsigned constant...");
  switch (lookAhead->tokenType)
  {
  case TK_NUMBER:
    eat(TK_NUMBER);
    break;
  case TK_IDENT:
    eat(TK_IDENT);
    break;
  case TK_CHAR:
    eat(TK_CHAR);
    break;
  default:
    error(ERR_INVALIDCONSTANT, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
  assert("Unsigned constant parsed!");
}

void compileConstant(void)
{
  // Gi???ng compile Unsigned constant tuy nhi??n n?? x??? l?? c??c tr?????ng h???p nh?? a +b ,c - d
  assert("Parsing a constant...");
  switch (lookAhead->tokenType)
  {
  case SB_PLUS:
    eat(SB_PLUS);
    compileConstant2();
    break;
  case SB_MINUS:
    eat(SB_MINUS);
    compileConstant2();
    break;
  // l?? do ?????t TK_CHAR ??? ????y l?? KPL kh??ng x??? l?? c???ng tr??? 2 char
  case TK_CHAR:
    eat(TK_CHAR);
    break;
  default:
    compileConstant2();
    break;
  }
  assert("Constant parsed!");
}

void compileConstant2(void)
{
  switch (lookAhead->tokenType)
  {
  case TK_IDENT:
    eat(TK_IDENT);
    break;
  case TK_NUMBER:
    eat(TK_NUMBER);
    break;
  default:
    error(ERR_INVALIDCONSTANT, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}

void compileType(void)
{
  // NH???n c??c t?????ng h???p integer, char, array, bi???n
  switch (lookAhead->tokenType)
  {
  case KW_INTEGER:
    eat(KW_INTEGER);
    break;
  case KW_CHAR:
    eat(KW_CHAR);
    break;
  case TK_IDENT:
    eat(TK_IDENT);
    break;
  case KW_ARRAY: // A(.1.)
    eat(KW_ARRAY);
    eat(SB_LSEL);
    eat(TK_NUMBER);
    eat(SB_RSEL);
    eat(KW_OF);
    compileType();
    break;
  default:
    error(ERR_INVALIDTYPE, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}
// ?????c xem n?? l?? lo???i g?? integer hay char n???u k ph???i 2 lo???i n??y th?? b??o l???i
void compileBasicType(void)
{
  // C??i n??y ch??? c???n v???y b???i v?? function ch??? return 2 gi?? tr??? l?? char v?? integer
  switch (lookAhead->tokenType)
  {
  case KW_INTEGER:
    eat(KW_INTEGER);
    break;
  case KW_CHAR:
    eat(KW_CHAR);
    break;
  default:
    error(ERR_INVALIDBASICTYPE, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}

// ?????c c??c params trong function v?? procedure
void compileParams(void)
{
  switch (lookAhead->tokenType)
  {
  case SB_LPAR:
    eat(SB_LPAR);
    compileParam();   // ?????c (n: integer)
    compileParams2(); // ?????c xem sau c?? ph???i l?? d???u ; hay )
    eat(SB_RPAR);
    break;
  // n???u trong l??c khai b??o bi???n c?? : hay ; ?????ng kh??ng th?? m???c k??? kh??ng quan t??m
  case SB_COLON:
  case SB_SEMICOLON:
    break;
  default:
    error(ERR_INVALIDPARAM, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}

void compileParams2(void)
{
  switch (lookAhead->tokenType)
  {
  // N???u l?? semicolon ch???ng t??? l?? c??n param n??n ?????c ti???p ?????n ch???t
  case SB_SEMICOLON:
    eat(SB_SEMICOLON);
    compileParam();
    compileParams2();
    break;
  case SB_RPAR:
    break;
  default:
    error(ERR_INVALIDPARAM, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}

// D??ng ????? ?????c 1 param
void compileParam(void)
{
  switch (lookAhead->tokenType)
  {
  // Tr?????ng h???p : a: integer
  case TK_IDENT:
    eat(TK_IDENT);
    eat(SB_COLON);
    compileBasicType();
    break;
  // tr?????ng h???p ?????t l?? var b: integer
  case KW_VAR:
    eat(KW_VAR);
    eat(TK_IDENT);
    eat(SB_COLON);
    compileBasicType();
    break;
  default:
    error(ERR_INVALIDPARAM, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}

// ?????c t???ng d??ng 1 b??n trong begin v?? end
void compileStatements(void)
{
  compileStatement();
  compileStatements2();
}

void compileStatements2(void)
{
  switch (lookAhead->tokenType)
  {
  case SB_SEMICOLON:
    eat(SB_SEMICOLON);
    compileStatement();
    compileStatements2();
    break;
  // Follow
  case KW_END:
    break;
  // Error
  default:
    error(ERR_INVALIDSTATEMENT, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}

void compileStatement(void)
{
  switch (lookAhead->tokenType)
  {
  case TK_IDENT:
    compileAssignSt(); // G??n
    break;
  case KW_CALL:
    compileCallSt(); // In
    break;
  case KW_BEGIN:
    compileGroupSt();
    break;
  case KW_IF:
    compileIfSt();
    break;
  case KW_WHILE:
    compileWhileSt();
    break;
  case KW_FOR:
    compileForSt();
    break;
    // EmptySt needs to check FOLLOW tokens
  case SB_SEMICOLON:
  case KW_END:
  case KW_ELSE:
    break;
    // Error occurs
  default:
    error(ERR_INVALIDSTATEMENT, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}

void compileAssignSt(void)
{
  assert("Parsing an assign statement ....");
  eat(TK_IDENT);
  // Check xem c?? ph???i l?? 1 ph???n t??? c???a Array k
  if (lookAhead->tokenType == SB_LSEL)
  {
    compileIndexes();
  }
  eat(SB_ASSIGN); // C??i assign n??y thay cho := th?????ng s??? k ?????c d???u := n??y ????u
  compileExpression();
  assert("Assign statement parsed ....");
}
// x??? l?? h??m print v?? d??? call x
void compileCallSt(void)
{
  assert("Parsing a call statement ....");
  eat(KW_CALL);
  eat(TK_IDENT); // ?????c indent l?? writeln ho???c writeI ph???n ph??n t??ch ng??? ngh??a s??? x??? l?? c??i n??y (i guess)
  compileArguments();
  assert("Call statement parsed ....");
}
// x??? l?? b??n trong begin v?? end
void compileGroupSt(void)
{
  assert("Parsing a group statement ....");
  eat(KW_BEGIN);
  compileStatements();
  eat(KW_END);
  assert("Group statement parsed ....");
}
// x??? l?? if
void compileIfSt(void)
{
  assert("Parsing an if statement ....");
  eat(KW_IF);
  compileCondition(); // h??m n??y ????? x??? l?? ??i???u ki???n v?? d??? a <b
  eat(KW_THEN);
  compileStatement(); // h??m n??y ????? x??? l?? ph??p to??n ki???u a := a + b
  if (lookAhead->tokenType == KW_ELSE)
    compileElseSt();
  assert("If statement parsed ....");
}
// x??? l?? else c???a if
void compileElseSt(void)
{
  eat(KW_ELSE);
  compileStatement();
}

void compileWhileSt(void)
{
  assert("Parsing a while statement ....");
  eat(KW_WHILE);
  compileCondition();
  eat(KW_DO);
  compileStatement();
  assert("While statement pased ....");
}

void compileForSt(void)
{
  assert("Parsing a for statement ....");
  eat(KW_FOR);
  eat(TK_IDENT);
  eat(SB_ASSIGN);
  compileExpression();
  eat(KW_TO);
  compileExpression();
  eat(KW_DO);
  compileStatement();
  assert("For statement parsed ....");
}

void compileArguments(void)
{
  // lu??n ph???i c?? () v?? d??ng writeln(f(1)) n???u call x + b l?? h???ng lu??n n??y
  switch (lookAhead->tokenType)
  {
  case SB_LPAR:
    eat(SB_LPAR);
    compileExpression();
    compileArguments2();
    eat(SB_RPAR);
    break;
  // Follow - same as call statement as statement:
  case SB_SEMICOLON:
  case KW_END:
  case KW_ELSE:
  // Follow - term2
  case SB_TIMES:
  case SB_SLASH:
  // Follow - expression3
  // Follow (For statement)
  case KW_TO:
  case KW_DO:
  // Follow (arguments2)
  case SB_COMMA:
  // Follow (condition2)
  case SB_EQ:
  case SB_NEQ:
  case SB_LE:
  case SB_LT:
  case SB_GE:
  case SB_GT:
  // Follow (factor)
  case SB_RPAR:
  // Follow (indexes)
  case SB_RSEL:
  // Follow (if statement)
  case KW_THEN:
    break;
  // Error
  default:
    error(ERR_INVALIDARGUMENTS, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}

// H??m n??y ????? xem v?? d??? nh?? ??? h??m call th?? ????? xem c?? in nhi???u bi???n ko v?? d??? writeln(a + b, c +d)
void compileArguments2(void)
{
  switch (lookAhead->tokenType)
  {
  case SB_COMMA:
    eat(SB_COMMA);
    compileExpression();
    compileArguments2();
    break;
  // Follow
  case SB_RPAR:
    break;
  // Error:
  default:
    error(ERR_INVALIDARGUMENTS, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}

// x??? l?? ki???u so s??nh a > b
void compileCondition(void)
{
  compileExpression(); // x??? l?? v??? ?????u
  compileCondition2(); // x??? l?? c??i so s??nh v??? sau n?? n???u k c?? th?? l?? sai
}

void compileCondition2(void)
{
  switch (lookAhead->tokenType)
  {
  case SB_EQ:
    eat(SB_EQ);
    compileExpression();
    break;
  case SB_NEQ:
    eat(SB_NEQ);
    compileExpression();
    break;
  case SB_LE:
    eat(SB_LE);
    compileExpression();
    break;
  case SB_LT:
    eat(SB_LT);
    compileExpression();
    break;
  case SB_GE:
    eat(SB_GE);
    compileExpression();
    break;
  case SB_GT:
    eat(SB_GT);
    compileExpression();
    break;
  default:
    error(ERR_INVALIDCOMPARATOR, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}

// ph???n m??? r???ng express2 x??? l?? th??m tr?????ng h???p c?? d???u + ho???c tr??? ?????ng tr?????c v???n cho pass
void compileExpression(void)
{
  assert("Parsing an expression");
  switch (lookAhead->tokenType)
  {
  // d???u c???ng v???i tr??? x??t tr?????ng h???p th??ch g??n x = 1 + 2 ch??? h???n hay 1-2
  case SB_PLUS:
    eat(SB_PLUS);
    compileExpression2();
    break;
  case SB_MINUS:
    eat(SB_MINUS);
    compileExpression2();
    break;
  default:
    compileExpression2();
    break;
  }
  assert("Expression parsed");
}

// x??? l?? v?? d??? nh?? ph??p a * (b + c)
void compileExpression2(void)
{
  // x??? l?? ph???n a *
  compileTerm();
  // x??? l?? ph???n b + c
  compileExpression3();
}

void compileExpression3(void)
{
  // x??? l?? tr?????ng h???p c???ng tr???
  switch (lookAhead->tokenType)
  {
  case SB_PLUS:
    eat(SB_PLUS);
    compileTerm();
    compileExpression3();
    break;
  case SB_MINUS:
    eat(SB_MINUS);
    compileTerm();
    compileExpression3();
    break;
  // Follow (statement)
  case SB_SEMICOLON:
  case KW_END:
  case KW_ELSE:
  // Follow (For statement)
  case KW_TO:
  case KW_DO:
  // Follow (arguments2)
  case SB_COMMA:
  // Follow (condition2)
  case SB_EQ:
  case SB_NEQ:
  case SB_LE:
  case SB_LT:
  case SB_GE:
  case SB_GT:
  // Follow (factor)
  case SB_RPAR:
  // Follow (indexes)
  case SB_RSEL:
  // Follow (if statement)
  case KW_THEN:
    break;
  // Error
  default:
    error(ERR_INVALIDEXPRESSION, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}

// x??? l?? v?? d??? tr?????ng h???p a * b
void compileTerm(void)
{
  compileFactor(); // x??? l?? bi???n a
  compileTerm2();  // x??? l?? ph???n * b
}

void compileTerm2(void)
{
  // ?????c xem ?????ng sau c?? x??? l?? tr?????ng h???p nh??n chia g?? k
  switch (lookAhead->tokenType)
  {
  case SB_TIMES:
    eat(SB_TIMES);
    compileFactor();
    compileTerm2();
    break;
  case SB_SLASH:
    eat(SB_SLASH);
    compileFactor();
    compileTerm2();
    break;
  // Follow - same as expression3
  case SB_PLUS:
  case SB_MINUS:
  // Follow (statement)
  case SB_SEMICOLON:
  case KW_END:
  case KW_ELSE:
  // Follow (For statement)
  case KW_TO:
  case KW_DO:
  // Follow (arguments2)
  case SB_COMMA:
  // Follow (condition2)
  case SB_EQ:
  case SB_NEQ:
  case SB_LE:
  case SB_LT:
  case SB_GE:
  case SB_GT:
  // Follow (factor)
  case SB_RPAR:
  // Follow (indexes)
  case SB_RSEL:
  // Follow (if statement)
  case KW_THEN:
    break;
  default:
    error(ERR_INVALIDTERM, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}

void compileFactor(void)
{
  switch (lookAhead->tokenType)
  {
  // ?????c ch??? v?? s???
  case TK_NUMBER:
  case TK_CHAR:
    compileUnsignedConstant();
    break;
  // ?????c tr?????ng h???p c?? d???u ngo???c (c??i n??y ki???u 3 * (1 + 2))
  case SB_LPAR:
    eat(SB_LPAR);
    compileExpression();
    eat(SB_RPAR);
    break;
  // ?????c bi???n
  case TK_IDENT:
    eat(TK_IDENT);
    switch (lookAhead->tokenType)
    {
    // x??t tr?????ng h???p n?? l?? m???t ph???n t??? c???a array
    case SB_LSEL:
      compileIndexes();
      break;
    // x??t tr?????ng h???p n?? l?? function v?? d??? F(1)
    case SB_LPAR:
      compileArguments();
      break;
    default:
      break;
    }
    break;
  default:
    error(ERR_INVALIDFACTOR, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}

// ?????c ph???n t??? b??n trong array
void compileIndexes(void)
{
  if (lookAhead->tokenType == SB_LSEL)
  {
    eat(SB_LSEL);
    compileExpression(); // c??i n??y auto l?? number
    eat(SB_RSEL);
    compileIndexes(); // check tr?????ng h???p n?? l?? m???ng 2 chi???u
  }
}

int compile(char *fileName)
{
  if (openInputStream(fileName) == IO_ERROR)
    return IO_ERROR;

  currentToken = NULL;
  lookAhead = getValidToken();

  compileProgram();

  free(currentToken);
  free(lookAhead);
  closeInputStream();
  return IO_SUCCESS;
}
