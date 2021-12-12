#ifndef _BYTECODEINSTRUCTIONS_H
#define _BYTECODEINSTRUCTIONS_H

ByteCode& iconst(signed char i); // [-1 .. 5] (7 possible values)

ByteCode& bipush(char s1);
ByteCode& sipush(short int s2);

ByteCode& ldc(char u1);
ByteCode& ldc_w(short int u2);
ByteCode& iload(char u1);
ByteCode& aload(char u1);

ByteCode& istore(char u1);
ByteCode& astore(char u1);

ByteCode& pop();
ByteCode& dup();
ByteCode& dup2();

ByteCode& iadd();
ByteCode& imul();
ByteCode& isub();
ByteCode& ineg();
ByteCode& idiv();
ByteCode& iinc(char u1, signed char i);

// Control transfer commands ...
//** Условный переход если сравнение целых чисел дает истину.
ByteCode& if_icmpeq(short int s2);
ByteCode& if_icmpne(short int s2);
ByteCode& if_icmplt(short int s2);
ByteCode& if_icmpge(short int s2);
ByteCode& if_icmpgt(short int s2);
ByteCode& if_icmple(short int s2);

//** Перейти, если сравнение целого числа с нулем дает истину
ByteCode& ifeq(short int s2);
ByteCode& ifne(short int s2);
ByteCode& iflt(short int s2);
ByteCode& ifle(short int s2);
ByteCode& ifgt(short int s2);
ByteCode& ifge(short int s2);

//** Перейти, если сравнение ссылок верно
ByteCode& if_acmpeq(short int s2);
ByteCode& if_acmpne(short int s2);

 //** Безусловный  переход
ByteCode& goto_(short int s2);

//Функции команд работы с массивами
ByteCode& newarray(char u1);
ByteCode& anewarray(short int u2);
ByteCode& arraylength();
ByteCode& iaload();
ByteCode& aaload();
ByteCode& iastore();
ByteCode& aastore();

//Функции команд работы с объектами
ByteCode& new_(short int u2);
ByteCode& getfield(short int u2);
ByteCode& putfield(short int u2);

//Функции команд работы с методами
ByteCode& invokevirtual(short int u2, short int argCount, bool isVoid);
ByteCode& invokespecial(short int u2, short int argCount, bool isVoid);
ByteCode& invokestatic(short int u2, short int argCount, bool isVoid);
ByteCode& ireturn();
ByteCode& areturn();
ByteCode& return_();

#endif // _BYTECODEINSTRUCTIONS_H
