

#ifndef _TESTMESSAGEFORMAT
#define _TESTMESSAGEFORMAT

#include "unicode/utypes.h"

#if !UCONFIG_NO_FORMATTING

#include "unicode/unistr.h"
#include "unicode/fmtable.h"
#include "intltest.h"

class TestMessageFormat: public IntlTest {
public:
    void runIndexedTest( int32_t index, UBool exec, const char* &name, char* par = NULL );

    /**
     * regression test for a specific bug regarding ChoiceFormat boundaries
     **/
    void testBug1(void);
    /**
     * regression test for a specific bug regarding MessageFormat using ChoiceFormat
     **/
    void testBug2(void);
    /**
     * regression test for a specific bug involving NumberFormat and Locales
     **/
    void testBug3(void);
    /** 
     * test MessageFormat with various given patterns
     **/
    void PatternTest(void);
    /** 
     * test MesageFormat formatting functionality in a simple example
     **/
    void sample(void);

    /** 
    * tests the static MessageFormat::format method
     **/
    void testStaticFormat(/* char* par */);
    /** 
     * tests MesageFormat functionality with a simple format
     **/
    void testSimpleFormat(/* char* par */);
    /** 
     * tests MesageFormat functionality with a format including a ChoiceFormat
     **/
    void testMsgFormatChoice(/* char* par */);
    /** 
     * tests MesageFormat functionality with a PluralFormat.
     **/
    void testMsgFormatPlural(/* char* par */);

    /**
     * Verify that MessageFormat accomodates more than 10 arguments
     * and more than 10 subformats.
     */
    void TestUnlimitedArgsAndSubformats();

    /**
     * Test RBNF extensions to MessageFormat.
     */
    void TestRBNF();

    //
    /** 
     * ------------ API tests ----------
     * These routines test various API functionality.
     * In addition to the methods their name suggests,
     * they often test other methods as well.
     **/
    void testCopyConstructor(void);
    void testCopyConstructor2(void);
    void testAssignment(void);
    void testClone(void);
    void testEquals(void);
    void testNotEquals(void);
    void testSetLocale(void);
    void testFormat(void);
    void testParse(void);
    void testAdopt(void);
    void TestTurkishCasing(void);
    void testAutoQuoteApostrophe(void);

private:
};

#endif /* #if !UCONFIG_NO_FORMATTING */

#endif
