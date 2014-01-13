////////////////
// NMPARSER.H //
////////////////

#ifndef NMPARSER_H
#define NMPARSER_H

#include "NmStr.h"

class CNmParser
{
public:
	CNmParser();
	CNmParser(char *buf);
	~CNmParser();

	void    SeekStart();
	void    Seek(char *pos);
	void    BufferCopy(const char *buf);                // copy buffer to start parsing from
	bool    BufferFromFile(const char *filename);
	void    BufferAppend(const char *buf, int buflen);
	void    BufferFree();
	bool    BufferToFile(const char *filename);

	// Get<word>	-> does not increment pointer
	// X<word>		-> does increment pointer

	char *Find(char c);          // Find character
	char *Find(char *s);         // Find string
	char *FindOneOf(const char *set);  // Find a character from a set

	char    GetChar();      // read current char
	char *GetCharPtr();     // read current char pointer
	char *GetBuffer();

	bool    EOP();          // EndOfParser ? ('\0' reached)

	char    XChar();                    // extract current char
	float   XFloat(bool trim = true);   // extract a float from current position (trim ON deletes ' ' from beginning first)
	bool    XString(CNmStr&str, char end); // extract a string from current position to char end
	bool    XString(char **dst, char end); // extract a string from current position to char end
	                                      // m_Position is set over end
	                                      // dst is destroyed without any type of checks

	void    XClear(char c);     // while c is read, increment m_Pos
	void    XClear(const char *setc);

	bool    XGo(char c);        // increment m_Pos until c is reached	[ m_Pos = &c + 1 ]
	bool    XGo(const char *s); //                       s				[ m_Pos = &s + strlen(s) ]

	bool    XGoOn(char c);       // increment m_Pos until c is reached [ m_Pos = &c ]
	bool    XGoOn(const char *s); //						  s            [ m_Pos = &s ]

	bool    XGoOneOf(const char *set);
	bool    XGoOneOfOn(const char *set);

	void    wDelete(int dwCount);
	void    wInsert(const char *szBuf, bool bIncPos = false);

private:
	char *m_Pos;
	char *m_Buf;
};

#endif