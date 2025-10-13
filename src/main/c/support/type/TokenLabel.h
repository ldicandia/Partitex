//#ifndef TOKEN_LABEL_HEADER
//#define TOKEN_LABEL_HEADER

/**
 * The type of a Bison token label, that is, an identifier of a token, that
 * transports the lexeme and semantic value as a whole.
 */
//typedef signed int TokenLabel;

//#endif 

#ifndef TOKEN_LABEL_HEADER
#define TOKEN_LABEL_HEADER

/**
 * Definición del alfabeto Σ del lenguaje Partitext.
 * Cada elemento representa un token (símbolo terminal)
 * que el analizador léxico puede emitir hacia el parser.
 */

typedef enum TokenLabel {
	/* --- Literales básicos --- */
	INTEGER = 256,    // números enteros (octavas, compases, tempos)
	ID,                // identificadores genéricos (p. ej. allegro, major)

	/* --- Palabras clave del lenguaje --- */
	KEY, TIME, TEMPO, CLEF, NOTES, REST, CHORD, SECTION, REPEAT, DYNAMIC,

	/* --- Atributos musicales --- */
	DURATION,          // whole, half, quarter, eighth...
	SHARP, FLAT, NATURAL, // #, b, ♮
	OCTAVE,             // número de octava (4, 5, etc.)

	/* --- Símbolos de puntuación y agrupación --- */
	OPEN_BRACE, CLOSE_BRACE,
	OPEN_PARENTHESIS, CLOSE_PARENTHESIS,
	COMMA, SEMICOLON, COLON, SLASH,

	/* --- Tokens de control o auxiliares --- */
	IGNORED, UNKNOWN

} TokenLabel;

#endif
